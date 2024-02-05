
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
#include "StRPEvent.h"
#include "StUPCTrack.h"
#include "StUPCBemcCluster.h"
#include "StUPCVertex.h"
#include "StUPCTofHit.h"

//local classes
#include "StUPCMakerFrom2upcDst.h"

ClassImp(StUPCMakerFrom2upcDst);

using namespace std;


//_____________________________________________________________________________
StUPCMakerFrom2upcDst::StUPCMakerFrom2upcDst(const string& in1_name, const string& in2_name, const string& out_name):
  mInpMain(0x0), mInpIdx(0x0), mTreeMain(0x0), mTreeIdx(0x0), mEvtMain(0x0), mEvtIdx(0x0),
  mOutFile(0x0), mTreeOut(0x0), mEvtOut(0x0) {

  cout << "StUPCMakerFrom2upcDst::StUPCMakerFrom2upcDst called" << endl;
  //output upcDst event, created first for static TClonesArrays
  mEvtOut = new StUPCEvent();

  //inputs
  mInpMain = TFile::Open(in1_name.c_str(), "read"); // main event loop
  mInpIdx = TFile::Open(in2_name.c_str(), "read"); // indexed secondary input

  //input trees
  mTreeMain = dynamic_cast<TTree*>( mInpMain->Get("mUPCTree") ); // main tree
  mTreeIdx = dynamic_cast<TTree*>( mInpIdx->Get("mUPCTree") ); // secondary indexed tree

  mTreeMain->SetBranchAddress("mUPCEvent", &mEvtMain); // main input events
  mTreeIdx->SetBranchAddress("mUPCEvent", &mEvtIdx); // input secondary indexed events

  //input events
  mTreeIdx->SetBranchAddress("mUPCEvent", &mEvtIdx); // input secondary indexed events
  mTreeIdx->BuildIndex("mUPCEvent.mEvtNum");

  //output file and copy the tree from main
  mOutFile = new TFile(out_name.c_str(), "recreate");
  // clone a RP info
  mTreeMain->SetBranchStatus("*", 0); //disable all branches
  enableBranchAndSubbranches(mTreeMain, "mRPEvent"); // set RP branch
  mTreeOut = mTreeMain->CloneTree(0);

  mTreeMain->SetBranchStatus("*", 1); //set on all branches back again

  mTreeOut->Branch("mUPCEvent", &mEvtOut);

}//StUPCMakerFrom2upcDst

//_____________________________________________________________________________
Int_t StUPCMakerFrom2upcDst::EventLoop() {
  Long64_t nev = mTreeMain->GetEntries();
  cout << " Number of events: " << nev << endl;

  //main event loop
  for(Long64_t iev=0; iev<nev; iev++) {

    //main input event
    mTreeMain->GetEntry(iev);
    //copy the main UPC event into output UPC event
    *mEvtOut = *mEvtMain;

    //copy the info from the index upc (Grigory's pico)
    Int_t stat = mTreeIdx->GetEntryWithIndex( mEvtOut->getEventNumber() );
    if( stat > 0 )
      fillPicoInfo();

    //write the output tree
    mTreeOut->Fill();
  }//main event loop

  return 0;

}//EventLoop

//_____________________________________________________________________________
Int_t StUPCMakerFrom2upcDst::Finish()
{
  //called at the end
  cout << "StUPCMakerFrom2upcDst::Finish() called" << endl;

  //write the output file
  
  mOutFile->cd();

  mTreeOut->Write();

  mOutFile->Close();

  return 0;

}//Finish


//_____________________________________________________________________________
void StUPCMakerFrom2upcDst::fillPicoInfo()
{
  //cout << "StUPCMakerFrom2upcDst::fillPicoInfo() called" << endl;
  if( mEvtIdx->getBunchCrossId() != mEvtOut->getBunchCrossId() ){
    cout << "Error in StUPCMakerFrom2upcDst::EventLoop(): bunch crossing ID missmatched" << endl;
    return;
  }

  mEvtOut->setBeamXPosition( mEvtIdx->getBeamXPosition() );
  mEvtOut->setBeamXSlope( mEvtIdx->getBeamXSlope() );
  mEvtOut->setBeamYPosition( mEvtIdx->getBeamYPosition() );
  mEvtOut->setBeamYSlope( mEvtIdx->getBeamYSlope() );

  // Add tracks, tracks loop
  for(Int_t itrk=0; itrk<mEvtIdx->getNumberOfTracks(); itrk++) {
    StUPCTrack *track = mEvtIdx->getTrack(itrk);
    if( !track ) continue;

    //UPC track
    StUPCTrack *upcTrack = mEvtOut->addTrack();
    upcTrack->setPtEtaPhi(track->getPt(), track->getEta(), track->getPhi());
    upcTrack->setCurvatureDipAnglePhase(track->getCurvature(), track->getDipAngle(), track->getPhase() );
    upcTrack->setOrigin( track->getOrigin() );
    upcTrack->setDcaXY( track->getDcaXY() );
    upcTrack->setDcaZ( track->getDcaZ() );
    upcTrack->setCharge( track->getCharge() );
    upcTrack->setNhits( track->getNhits() );
    upcTrack->setNhitsFit( track->getNhitsFit() );
    upcTrack->setChi2( track->getChi2() );
    upcTrack->setNhitsDEdx( track->getNhitsDEdx() );
    upcTrack->setDEdxSignal( track->getDEdxSignal() );
    upcTrack->setNSigmasTPC( StUPCTrack::kElectron, track->getNSigmasTPCElectron() );
    upcTrack->setNSigmasTPC( StUPCTrack::kPion, track->getNSigmasTPCPion() );
    upcTrack->setNSigmasTPC( StUPCTrack::kKaon, track->getNSigmasTPCKaon() );
    upcTrack->setNSigmasTPC( StUPCTrack::kProton, track->getNSigmasTPCProton() );
    upcTrack->setVertexId( track->getVertexId() );

    if( track->getFlag(StUPCTrack::kPrimary) )
      upcTrack->setFlag( StUPCTrack::kPrimary );
    if( track->getFlag(StUPCTrack::kBemcProj) ) {
      upcTrack->setFlag( StUPCTrack::kBemcProj );
      upcTrack->setBemcPtEtaPhi(track->getBemcPt(), track->getBemcEta(), track->getBemcPhi());
    }
    if( track->getFlag(StUPCTrack::kTof) ) {
      upcTrack->setFlag( StUPCTrack::kTof );
      upcTrack->setTofTime( track->getTofTime() );
      upcTrack->setTofPathLength( track->getTofPathLength() );
    }
    if( track->getFlag(StUPCTrack::kV0) )
      upcTrack->setFlag( StUPCTrack::kV0 );
    if( track->getFlag(StUPCTrack::kCEP) )
      upcTrack->setFlag( StUPCTrack::kCEP );
    if( track->getFlag(StUPCTrack::kBemc) ) {
      StUPCBemcCluster *cluster = mEvtIdx->getCluster( track->getBemcClusterId() );
      if( !cluster ) continue;

      StUPCBemcCluster *upcCls = mEvtOut->addCluster();
      upcCls->setEnergy( cluster->getEnergy() );
      upcCls->setEta( cluster->getEta() );
      upcCls->setPhi( cluster->getPhi() );
      upcCls->setId( mEvtOut->getNumberOfClusters() ); // cluster ID by number of clusters
      
      upcTrack->setFlag( StUPCTrack::kBemc );
      upcTrack->setBemcPtEtaPhi(track->getBemcPt(), track->getBemcEta(), track->getBemcPhi());
      upcTrack->setBemcClusterId(mEvtOut->getNumberOfClusters()); // same cluster ID for track
      upcTrack->setBemcHitE(track->getBemcHitE());
    }

  }//tracks loop

  //cout << "StUPCMakerFrom2upcDst::fillPicoInfo() finished" << endl;
}//fillPicoInfo

/*
//_____________________________________________________________________________
void StUPCMakerFrom2upcDst::setUpcDstEvent()
{
  //cout << "StUPCMakerFrom2upcDst::setUpcDstEvent() called" << endl;
  for( UInt_t i = 0; i < mEvtMain->getTriggerArraySize(); i++)
    mEvtOut->addTriggerId( mEvtMain->getTriggerID(i) );

  mEvtOut->setRunNumber( mEvtMain->getRunNumber() );
  mEvtOut->setEventNumber( mEvtMain->getEventNumber() );
  mEvtOut->setFillNumber( mEvtMain->getFillNumber() );
  mEvtOut->setBunchCrossId( mEvtMain->getBunchCrossId() );
  mEvtOut->setBunchCrossId7bit( mEvtMain->getBunchCrossId7bit() );
  mEvtOut->setMagneticField( mEvtMain->getMagneticField() );  
  mEvtOut->setZDCEastRate( mEvtMain->getZDCEastRate() );
  mEvtOut->setZDCWestRate( mEvtMain->getZDCWestRate() );
  mEvtOut->setZDCCoincRate( mEvtMain->getZDCCoincRate() );
  mEvtOut->setLastDSM0( mEvtMain->getLastDSM0() );
  mEvtOut->setLastDSM1( mEvtMain->getLastDSM1() );
  mEvtOut->setLastDSM3( mEvtMain->getLastDSM3() );
  mEvtOut->setZDCUnAttEast( mEvtMain->getZDCUnAttEast() );
  mEvtOut->setZDCUnAttWest( mEvtMain->getZDCUnAttWest() );
  for (Int_t ipmt=1; ipmt<=3; ipmt++) {
    mEvtOut->setZDCEastADC( mEvtMain->getZDCEastADC(ipmt), ipmt );
    mEvtOut->setZDCWestADC( mEvtMain->getZDCWestADC(ipmt), ipmt );
  }
  mEvtOut->setZDCEastTDC( mEvtMain->getZDCEastTDC() );
  mEvtOut->setZDCWestTDC( mEvtMain->getZDCWestTDC() );
  mEvtOut->setZDCTimeDiff( mEvtMain->getZDCTimeDiff() );
  mEvtOut->setZdcVertexZ( mEvtMain->getZdcVertexZ() );
  //ZDC SMD
  //vertical - horizontal loop
  for(Int_t verthori=0; verthori<2; verthori++) {
    //strip loop
    for(Int_t strip=1; strip<9; strip++) {
      mEvtOut->setZdcSMDEast( verthori, strip,  mEvtMain->getZdcSMDEast(verthori, strip) );
      mEvtOut->setZdcSMDWest( verthori, strip,  mEvtMain->getZdcSMDWest(verthori, strip) );
    }//strip loop
  }//vertical - horizontal loop

  mEvtOut->setBBCSmallEast( mEvtMain->getBBCSmallEast() );
  mEvtOut->setBBCSmallWest( mEvtMain->getBBCSmallWest() );
  mEvtOut->setBBCLargeEast( mEvtMain->getBBCLargeEast() );
  mEvtOut->setBBCLargeWest( mEvtMain->getBBCLargeWest() );

  mEvtOut->setVPDSumEast( mEvtMain->getVPDSumEast() );
  mEvtOut->setVPDSumWest( mEvtMain->getVPDSumWest() );
  mEvtOut->setVPDTimeDiff( mEvtMain->getVPDTimeDiff() );

  mEvtOut->setTOFMultiplicity( mEvtMain->getTOFMultiplicity() );
  mEvtOut->setBEMCMultiplicity( mEvtMain->getBEMCMultiplicity() );

  mEvtOut->setNGlobTracks( mEvtMain->getNGlobTracks() );
  mEvtOut->setNPrimTracks( mEvtMain->getNPrimTracks() );
  mEvtOut->setNPrimVertices( mEvtMain->getNPrimVertices() );

  // Add tracks, tracks loop
  for(Int_t itrk=0; itrk< mEvtMain->getNumberOfTracks(); itrk++) {
    StUPCTrack *track = mEvtMain->getTrack(itrk);
    if( !track ) continue;

    //UPC track
    StUPCTrack *upcTrack = mEvtOut->addTrack();
    upcTrack->setPtEtaPhi(track->getPt(), track->getEta(), track->getPhi());
    upcTrack->setCurvatureDipAnglePhase(track->getCurvature(), track->getDipAngle(), track->getPhase() );
    upcTrack->setOrigin( track->getOrigin() );
    upcTrack->setDcaXY( track->getDcaXY() );
    upcTrack->setDcaZ( track->getDcaZ() );
    upcTrack->setCharge( track->getCharge() );
    upcTrack->setNhits( track->getNhits() );
    upcTrack->setNhitsFit( track->getNhitsFit() );
    upcTrack->setChi2( track->getChi2() );
    upcTrack->setNhitsDEdx( track->getNhitsDEdx() );
    upcTrack->setDEdxSignal( track->getDEdxSignal() );
    upcTrack->setNSigmasTPC( StUPCTrack::kElectron, track->getNSigmasTPCElectron() );
    upcTrack->setNSigmasTPC( StUPCTrack::kPion, track->getNSigmasTPCPion() );
    upcTrack->setNSigmasTPC( StUPCTrack::kKaon, track->getNSigmasTPCKaon() );
    upcTrack->setNSigmasTPC( StUPCTrack::kProton, track->getNSigmasTPCProton() );
    upcTrack->setVertexId( track->getVertexId() );
    if( track->getFlag(StUPCTrack::kPrimary) )
      upcTrack->setFlag( StUPCTrack::kPrimary );
    if( track->getFlag(StUPCTrack::kBemcProj) ) {
      upcTrack->setFlag( StUPCTrack::kBemcProj );
      upcTrack->setBemcPtEtaPhi(track->getBemcPt(), track->getBemcEta(), track->getBemcPhi());
    }
    if( track->getFlag(StUPCTrack::kBemc) ) {
      upcTrack->setFlag( StUPCTrack::kBemc );
      upcTrack->setBemcPtEtaPhi(track->getBemcPt(), track->getBemcEta(), track->getBemcPhi());
      upcTrack->setBemcClusterId(track->getBemcClusterId());
      upcTrack->setBemcHitE(track->getBemcHitE());
    }
    if( track->getFlag(StUPCTrack::kTof) ) {
      upcTrack->setFlag( StUPCTrack::kTof );
      upcTrack->setTofTime( track->getTofTime() );
      upcTrack->setTofPathLength( track->getTofPathLength() );
    }
    if( track->getFlag(StUPCTrack::kV0) )
      upcTrack->setFlag( StUPCTrack::kV0 );
    if( track->getFlag(StUPCTrack::kCEP) )
      upcTrack->setFlag( StUPCTrack::kCEP );
  }//tracks loop

  // Add BEMC clusters, clusters vector loop
  for(int iClstr=0; iClstr < mEvtMain->getNumberOfClusters(); iClstr++) {
    StUPCBemcCluster *cluster = mEvtMain->getCluster(iClstr);
    if( !cluster ) continue;

    //put cluster in clones array in UPC event
    StUPCBemcCluster *upcCls = mEvtOut->addCluster();
    upcCls->setEta( cluster->getEta() );
    upcCls->setPhi( cluster->getPhi() );
    upcCls->setSigmaEta( cluster->getSigmaEta() );
    upcCls->setSigmaPhi( cluster->getSigmaPhi() );
    upcCls->setEnergy( cluster->getEnergy() );
    upcCls->setHTEnergy( cluster->getHTEnergy() );
    upcCls->setHTsoftID( cluster->getHTsoftID() );
    //cluster ID as position of cluster in clusters vector
    upcCls->setId( cluster->getId() );

  }//clusters vector loop

  // Add TOF hits, TOF hit loop
  for(int ihit=0; ihit < mEvtMain->getNumberOfHits(); ihit++) {
    // get hit from main upcDst
    StUPCTofHit *tofhit = mEvtMain->getHit( ihit );
    if( !tofhit ) continue;

    //put hit in clones array in UPC event
    StUPCTofHit *upcHit = mEvtOut->addHit();
    upcHit->setTray( tofhit->getTray() );
    upcHit->setModule( tofhit->getModule() );
    upcHit->setCell( tofhit->getCell() );
    upcHit->setLeadingEdgeTime( tofhit->getLeadingEdgeTime() );
    upcHit->setTrailingEdgeTime( tofhit->getTrailingEdgeTime() );
  }

  // Add vertecies
  for (int iVrtx = 0; iVrtx < mEvtMain->getNumberOfVertices(); ++iVrtx) {
    StUPCVertex *mainVertex = mEvtMain->getVertex( iVrtx );
    if( !mainVertex ) continue;

    //write vertex to output UPC event
    StUPCVertex *upcVtx = mEvtOut->addVertex();
    upcVtx->setPosX( mainVertex->getPosX() );
    upcVtx->setPosY( mainVertex->getPosY() );
    upcVtx->setPosZ( mainVertex->getPosZ() );
    upcVtx->setErrX( mainVertex->getErrX() );
    upcVtx->setErrY( mainVertex->getErrY() );
    upcVtx->setErrZ( mainVertex->getErrZ() );
    upcVtx->setNPrimaryTracks( mainVertex->getNPrimaryTracks() );
    upcVtx->setNTracksUsed( mainVertex->getNTracksUsed() );
    upcVtx->setId( mainVertex->getId() );
  }//vertex loop

  // Add MC info
  mEvtOut->setIsMC( mEvtMain->getIsMC() );

  //cout << "StUPCMakerFrom2upcDst::setUpcDstEvent() finished" << endl;
}//setUpcDstEvent
*/


void StUPCMakerFrom2upcDst::enableBranchAndSubbranches(TTree* tree, const char* branchName) {
    TBranch* branch = tree->GetBranch(branchName);
    if (branch) {
        tree->SetBranchStatus(branchName, 1);
        TObjArray* subBranches = branch->GetListOfBranches();
        if (subBranches) {
            for (int i = 0; i < subBranches->GetEntries(); ++i) {
                TBranch* subBranch = dynamic_cast<TBranch*>(subBranches->At(i));
                if (subBranch) {
                    const char* subBranchName = subBranch->GetName();
                    enableBranchAndSubbranches(tree, subBranchName);
                }
            }
        }
    }
}