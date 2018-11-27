
void run_make_pT() {

  gROOT->ProcessLine(".include ../include");

  gSystem->Load("../build/libstar-upc.so");

  gROOT->ProcessLine(".x make_pT.C+g");

}

