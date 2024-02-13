
// c++ headers
#include <iostream>

// ROOT headers
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"

// picoDst headers
#include "StUPCEvent.h"
#include "StUPCTrack.h"

using namespace std;

//_____________________________________________________________________________
void make_pT() {

  //open input file
  TFile *infile = TFile::Open("/gpfs01/star/pwg/truhlar/run17PicoOut/testSample/18055040/18055040.pico.root", "read");

  //get picoDst tree in file
  TTree *upcTree = dynamic_cast<TTree*>( infile->Get("mUPCTree") );

  //open output file
  TFile *outfile = TFile::Open("output.root", "recreate");

  //pT histogram
  TH1D *hPt = new TH1D("hPt", "hPt", 100, 0, 10);

  //connect upc event to the tree
  static StUPCEvent *upcEvt = 0x0;
  upcTree->SetBranchAddress("mUPCEvent", &upcEvt);

  //ask for number of events
  Long64_t nev = upcTree->GetEntries();
  cout << "Number of events: " << nev << endl;

  nev = 3;

  //event loop
  for(Long64_t iev=0; iev<nev; iev++) {

    //get the event
    upcTree->GetEntry(iev);

    //tracks loop
    for(Int_t i=0; i<upcEvt->getNumberOfTracks(); i++) {
      StUPCTrack *trk = upcEvt->getTrack(i);

      cout << trk->getPt() << endl;
      hPt->Fill(trk->getPt());
    }

  }

  //close outputs
  hPt->Write();
  outfile->Close();


}


