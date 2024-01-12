
#include "TArgumentParser.h"

//_____________________________________________________________________________
void RunFromPicoDst(string filelist, Int_t nFiles, string outfile, string config) {
//void RunFromPicoDst() {

  PrintFilelist(filelist);

  //string filelist = "/star/u/vanekjan/pwg/vanekjan/myLambdaAnalysis/pp/input_local/SL21d/st_physics_18056115_raw_1000009.picoDst.root";
  //string outfile = "out.root";

  //libraries to access the picoDst
  gROOT->Macro("loadMuDst.C");
  gSystem->Load("StPicoEvent");
  gSystem->Load("StPicoDstMaker");

  //load the analysis maker compiled before with cons
  gSystem->Load("StUPCFilterMaker.so");

  //create chain directory-like structure for maker
  //top level
  StChain *chain = new StChain();
  //maker to access picoDst data
  StPicoDstMaker *picoDstMaker = new StPicoDstMaker(StPicoDstMaker::IoRead, filelist.c_str());

  //analysis maker
  StUPCMakerFromPicoDst *anaMaker = new StUPCMakerFromPicoDst(picoDstMaker, outfile);

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


