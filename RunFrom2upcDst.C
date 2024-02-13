
//macro to merge two upcDst files to a single one

#include "TArgumentParser.h"
#include <string>
#include <iostream>

//_____________________________________________________________________________
void RunFrom2upcDst(string filelist, Int_t nFiles, string outfile, string config) {
//void RunFrom2upcDst() {

  //string filelist = "scheduler/test_2upcDst.list";
  //string outfile = "out.root";

  //get the inputs from the list of two files
  ifstream in_list(filelist.c_str());
  string in1, in2;
  const string picoSuffix = "pico.root";

  //main input
  getline(in_list, in1);

  //indexed input
  getline(in_list, in2);


  cout << "First input: " << in1 << endl;
  cout << "Second input: " << in2 << endl;

  if( in1.find(picoSuffix, in1.length() - 13) != string::npos ){
    string tmp = in1;
    in1 = in2;
    in2 = tmp;
  }

  cout << "Main input: " << in1 << endl;
  cout << "Indexed input: " << in2 << endl;

  //load library for upcDst
  gROOT->Macro("loadMuDst.C");
  gSystem->Load("StPicoEvent");
  gSystem->Load("StUPCFilterMaker.so");

  //maker to access the input upcDst data
  StUPCMakerFrom2upcDst anaMaker(in1, in2, outfile);

  anaMaker.EventLoop();
  anaMaker.Finish();  

}//RunFrom2upcDst


