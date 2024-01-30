
//_____________________________________________________________________________
//    Class for upcDst production from two input upcDst
//
//    Fills structure of StUPCEvent
//_____________________________________________________________________________

//C++
#include <iostream>

//ROOT
#include "TFile.h"
#include "TTree.h"
#include "TH1I.h"

//upcDst
#include "StUPCEvent.h"
#include "StUPCTrack.h"

//local classes
#include "StUPCMakerFrom2upcDst.h"

ClassImp(StUPCMakerFrom2upcDst);

using namespace std;

//_____________________________________________________________________________
StUPCMakerFrom2upcDst::StUPCMakerFrom2upcDst(const string& in1_name, const string& in2_name, const string& out_name):
  mInpMain(0x0), mInpIdx(0x0), mTreeMain(0x0), mTreeIdx(0x0), mEvtMain(0x0), mEvtIdx(0x0),
  mOutFile(0x0), mTreeOut(0x0), mEvtOut(0x0), mHistList(0x0), mCounter(0x0) {

  cout << "StUPCMakerFrom2upcDst::StUPCMakerFrom2upcDst" << endl;

  //output upcDst event, created first for static TClonesArrays
  mEvtOut = new StUPCEvent();

  //inputs
  mInpMain = TFile::Open(in1_name.c_str(), "read"); // main event loop
  mInpIdx = TFile::Open(in2_name.c_str(), "read"); // indexed secondary input

  //input trees
  mTreeMain = dynamic_cast<TTree*>( mInpMain->Get("mUPCTree") ); // main tree
  mTreeIdx = dynamic_cast<TTree*>( mInpIdx->Get("mUPCTree") ); // secondary indexed tree
  mTreeIdx->BuildIndex("mUPCEvent.mEvtNum");

  //input events
  mTreeMain->SetBranchAddress("mUPCEvent", &mEvtMain); // main input events
  mTreeIdx->SetBranchAddress("mUPCEvent", &mEvtIdx); // input secondary indexed events

  //output file and tree
  mOutFile = new TFile(out_name.c_str(), "recreate");
  mTreeOut = new TTree("mUPCTree", "mUPCTree");

  //output upcDst event
  mTreeOut->Branch("mUPCEvent", &mEvtOut);

  //output histograms
  mHistList = new TList();
  mHistList->SetOwner();

  //counter of processed events
  mCounter = new TH1I("mCounter", "mCounter", kMaxCnt-1, 1, kMaxCnt);
  mHistList->Add(mCounter);

}//StUPCMakerFrom2upcDst

//_____________________________________________________________________________
Int_t StUPCMakerFrom2upcDst::EventLoop() {

  Long64_t nev = mTreeMain->GetEntries();
  cout << " Number of events: " << nev << endl;

  //nev = 6;

  //main event loop
  for(Long64_t iev=0; iev<nev; iev++) {

    //clear the output UPC event
    mEvtOut->clearEvent();

    //main input event
    mTreeMain->GetEntry(iev);

    mCounter->Fill( kAna ); // analyzed events

    //cout << "event: " << iev << ", tracks: " << mEvtMain->getNumberOfTracks() << endl;
    cout << "main event: " << iev << ", b-cross id: " << mEvtMain->getBunchCrossId() << endl;

    //secondary input by the index
    Int_t stat = mTreeIdx->GetEntryWithIndex( mEvtMain->getEventNumber() );
    if( stat <= 0 ) continue;

    cout << "  secondary event, stat: " << stat << " b-cross id: " << mEvtIdx->getBunchCrossId() << endl;

    //set the output ucpDst event here
    mEvtOut->setRunNumber( mEvtMain->getRunNumber() );

    mCounter->Fill( kWritten ); // written events to output tree

    //write the output tree
    mTreeOut->Fill();

  }//main event loop

  return 0;

}//EventLoop

//_____________________________________________________________________________
Int_t StUPCMakerFrom2upcDst::Finish()
{
  //called at the end

  //write the output file
  mOutFile->cd();

  mTreeOut->Write();
  mHistList->Write("HistList", TObject::kSingleKey);

  mOutFile->Close();

  return 0;

}//Finish

