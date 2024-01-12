
//_____________________________________________________________________________
//    Class for upcDst production from picoDst
//
//    Fills structure of StUPCEvent
//_____________________________________________________________________________

//C++
#include <vector>

//ROOT
#include "TFile.h"
#include "TTree.h"
#include "TH1I.h"

//StRoot
#include "StPicoEvent/StPicoDst.h"
#include "StPicoDstMaker/StPicoDstMaker.h"
#include "StPicoEvent/StPicoEvent.h"
#include "StPicoEvent/StPicoTrack.h"

//upcDst
#include "StUPCEvent.h"
#include "StUPCSelectV0.h"
#include "StUPCTrack.h"

//local classes
#include "StUPCMakerFromPicoDst.h"

ClassImp(StUPCMakerFromPicoDst);

using namespace std;

//_____________________________________________________________________________
StUPCMakerFromPicoDst::StUPCMakerFromPicoDst(StPicoDstMaker *pm, string outnam) : StMaker("StReadPico"),
  mPicoDstMaker(pm), mPicoDst(0x0), mOutName(outnam), mOutFile(0x0),
  mHistList(0x0), mCounter(0x0), mUPCEvent(0x0), mUPCTree(0x0),
  mSelectV0(0x0) {

  LOG_INFO << "StUPCMakerFromPicoDst::StUPCMakerFromPicoDst called" << endm;

}//StUPCMakerFromPicoDst

//_____________________________________________________________________________
Int_t StUPCMakerFromPicoDst::Init() {

  LOG_INFO << "StUPCMakerFromPicoDst::Init" << endm;

  //create the output file
  mOutFile = new TFile(mOutName.c_str(), "recreate");

  //output UPC event
  mUPCEvent = new StUPCEvent();

  //create the tree
  mUPCTree = new TTree("mUPCTree", "mUPCTree");
  //add branch with event object
  mUPCTree->Branch("mUPCEvent", &mUPCEvent);

  //counter histograms
  mHistList = new TList();
  mHistList->SetOwner();

  //counter of processed events
  mCounter = new TH1I("mCounter", "mCounter", kMaxCnt-1, 1, kMaxCnt);
  mHistList->Add(mCounter);

  //selector for tracks from V0
  mSelectV0 = new StUPCSelectV0();

  return kStOk;

}//Init

//_____________________________________________________________________________
Int_t StUPCMakerFromPicoDst::Make() {

  //cout << "StUPCMakerFromPicoDst::Make" << endl;

  mUPCEvent->clearEvent(); //clear the output UPC event

  mPicoDst = mPicoDstMaker->picoDst(); // get input picoDst event

  mCounter->Fill( kAna ); // analyzed events

  //run number and event number
  mUPCEvent->setRunNumber( mPicoDst->event()->runId() );
  mUPCEvent->setEventNumber( mPicoDst->event()->eventId() );

  //tracks in event
  UInt_t nTracks = mPicoDst->numberOfTracks();

  //flag to filter picoDst tracks for writing to upcDst output
  vector<bool> trackFilter(nTracks);

  //initialize the flags with false (true is to write the track to the output)
  for(size_t i=0; i<trackFilter.size(); i++) trackFilter[i] = false;

  //cout << "Tracks: " << nTracks << " " << trackFilter.size() << endl;

  //select tracks from V0 candidates
  int nsel = mSelectV0->selectTracks(mPicoDst, trackFilter);

  //other selections for J/psi and CEP to go here

  //cout << "nsel: " << nsel << endl;

  //return when nothing was selected
  if( nsel <= 0 ) return kStOk;

  //write upcDst tracks from selected picoDst tracks

  //tracks loop
  for(UInt_t itrk=0; itrk<nTracks; itrk++) {

    if( !trackFilter[itrk] ) continue;

    //cout << "selected track: " << itrk << endl;

    //create new upc track
    StUPCTrack *upcTrack = mUPCEvent->addTrack();

    //set the upc track from the current pico track
    setUpcTrackFromPicoTrack(upcTrack, mPicoDst->track(itrk));

  }//tracks loop

  mCounter->Fill( kWritten ); // events with written upcDst output

  mUPCTree->Fill();

  return kStOk;

}//Make

//_____________________________________________________________________________
Int_t StUPCMakerFromPicoDst::Finish() {

  //called at the end

  LOG_INFO << "StUPCMakerFromPicoDst::Finish()" << endm;

  //write the output file
  mOutFile->cd();

  mUPCTree->Write();
  mHistList->Write("HistList", TObject::kSingleKey);

  mOutFile->Close();

  return kStOk;

}//Finish

//_____________________________________________________________________________
void StUPCMakerFromPicoDst::setUpcTrackFromPicoTrack(StUPCTrack *utrk, StPicoTrack *ptrk) {

  TVector3 trackMom = ptrk->gMom();
  utrk->setPtEtaPhi(ptrk->gPt(), trackMom.Eta(), trackMom.Phi());

  Float_t bField = mPicoDst->event()->bField();
  utrk->setCurvatureDipAnglePhase( ptrk->helix(bField).curvature(), ptrk->helix(bField).dipAngle(), ptrk->helix(bField).phase() );

  utrk->setOrigin(ptrk->origin());
  utrk->setDcaXY( -999 ); // there is no DCA, store -999?
  utrk->setDcaZ( -999 ); // there is no DCA, store -999?
  utrk->setCharge( ptrk->charge() );
  utrk->setNhits( ptrk->nHits() );
  utrk->setNhitsFit( ptrk->nHitsFit() );
  utrk->setChi2( ptrk->chi2() );
  utrk->setNhitsDEdx( ptrk->nHitsDedx() );
  utrk->setDEdxSignal( ptrk->dEdx() );
  utrk->setNSigmasTPC( StUPCTrack::kElectron, ptrk->nSigmaElectron() );
  utrk->setNSigmasTPC( StUPCTrack::kPion, ptrk->nSigmaPion() );
  utrk->setNSigmasTPC( StUPCTrack::kKaon, ptrk->nSigmaKaon() );
  utrk->setNSigmasTPC( StUPCTrack::kProton, ptrk->nSigmaProton() );

  //BEMC to go here

  //TOF
  if( ptrk->isTofTrack() ) utrk->setFlag( StUPCTrack::kTof );

}//setUpcTrackFromPicoTrack













