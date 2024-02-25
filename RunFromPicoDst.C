
#include "TArgumentParser.h"

class StUPCMakerFromPicoDst;
StUPCMakerFromPicoDst *anaMaker;
void AddTrigger(UInt_t id);

//_____________________________________________________________________________
void RunFromPicoDst(string filelist, Int_t nFiles, string outfile, string config) {
//void RunFromPicoDst() {

  PrintFilelist(filelist);

  //string filelist = "/star/u/jaroslav/test/data/pp/SL21d/st_physics_18056115_raw_1000009.picoDst.root";
  //string outfile = "out.root";

  //libraries to access the picoDst
  gROOT->Macro("loadMuDst.C");
  gSystem->Load("StPicoEvent");
  gSystem->Load("StPicoDstMaker");

  // load libraries to read beam-line position
  // base libraries
  gSystem->Load("St_base");
  gSystem->Load("StChain");
  gSystem->Load("StUtilities");
  gSystem->Load("StIOMaker");
  gSystem->Load("StarClassLibrary");

  // db-related libraries
  gSystem->Load("St_Tables");
  gSystem->Load("StDbLib");
  gSystem->Load("StDbBroker");
  gSystem->Load("St_db_Maker");

  //load the analysis maker compiled before with cons
  gSystem->Load("StUPCFilterMaker.so");


  //create chain directory-like structure for maker
  //top level
  StChain *chain = new StChain();
  //maker to access picoDst data
  StPicoDstMaker *picoDstMaker = new StPicoDstMaker(StPicoDstMaker::IoRead, filelist.c_str());

  //analysis maker
  anaMaker = new StUPCMakerFromPicoDst(picoDstMaker, outfile);


  //load values from config file
  TArgumentParser parser;
  parser.AddFuncInt3("add_trigger", AddTrigger);

  cout << "RunFromPicoDst, using config from: " << config << endl;
  parser.Parse(config);

  //initialize the makers
  chain->Init();

  cout << "Number of events: " << picoDstMaker->chain()->GetEntries() << endl;

  //loop over events
  chain->EventLoop(picoDstMaker->chain()->GetEntries());
  //chain->EventLoop(12);
  chain->Finish();

  //release allocated memory
  delete chain;

}//RunFromPicoDst

//_____________________________________________________________________________
void PrintFilelist(const string& filelist) {

  //print the full list of files including their size

  cout << "------- In RunFromPicoDst.C, the list of files is: -------" << endl;

  ifstream in(filelist.c_str());

  string line;
  while(in.good()) {
    getline(in, line);
    if(line.empty()) continue;

    string cmd = "ls " + line + " -alh";

    gSystem->Exec(cmd.c_str());
  }

  cout << "------------------ End of list of files ------------------" << endl;

  in.close();

}//PrintFilelist


//_____________________________________________________________________________
void AddTrigger(UInt_t id, Int_t rmin, Int_t rmax) {

  //wrapper function to call maker::addTriggerId

  anaMaker->addTriggerId(id);

}//AddTrigger