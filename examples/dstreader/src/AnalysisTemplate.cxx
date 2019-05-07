
// C++ headers
#include <iostream>

// ROOT headers
#include "TFile.h"
#include "TTree.h"

// picoDst headers
#include "StUPCEvent.h"
#include "StUPCTrack.h"

using namespace std;

//_____________________________________________________________________________
int main(void) {

  //open input file
  TFile *infile = TFile::Open("/gpfs01/star/pwg/jaroslav/star-upcDst-data/test_productions/mc/StUPC_slight14e1x1.root", "read");

  //get picoDst tree in file
  TTree *upcTree = dynamic_cast<TTree*>( infile->Get("mUPCTree") );

  //open output file
  TFile *outfile = TFile::Open("output.root", "recreate");
  //create output tree
  TTree *myTree = new TTree("myTree", "myTree");
  Double_t tracksPt;
  myTree->Branch("tracksPt", &tracksPt, "tracksPt/D");

  //connect upc event to the tree
  static StUPCEvent *upcEvt = 0x0;
  upcTree->SetBranchAddress("mUPCEvent", &upcEvt);

  //ask for number of events
  Long64_t nev = upcTree->GetEntries();
  cout << "Number of events: " << nev << endl;

  nev = 300;

  //event loop
  for(Long64_t iev=0; iev<nev; iev++) {

    //get the event
    upcTree->GetEntry(iev);

    //tracks loop
    for(Int_t i=0; i<upcEvt->getNumberOfTracks(); i++) {
      StUPCTrack *trk = upcEvt->getTrack(i);

      tracksPt = trk->getPt();
      myTree->Fill();
    }

  }

  //close the outputs
  myTree->Write();
  outfile->Close();




  return 0;

}//main
