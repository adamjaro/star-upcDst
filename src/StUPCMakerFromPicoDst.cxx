
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
#include "StPicoEvent/StPicoBEmcPidTraits.h"
#include "StEmcUtil/geometry/StEmcGeom.h"
#include "StPicoEvent/StPicoBTofPidTraits.h"
#include "St_db_Maker/St_db_Maker.h"
#include "tables/St_vertexSeed_Table.h"
#include "tables/St_vertexSeedTriggers_Table.h"

//upcDst
#include "StUPCEvent.h"
#include "StUPCSelectV0.h"
#include "StUPCSelectCEP.h"
#include "StUPCTrack.h"
#include "StUPCBemcCluster.h"

//local classes
#include "StUPCMakerFromPicoDst.h"

ClassImp(StUPCMakerFromPicoDst);

using namespace std;

//_____________________________________________________________________________
StUPCMakerFromPicoDst::StUPCMakerFromPicoDst(StPicoDstMaker *pm, string outnam) : StMaker("StReadPico"),
  mPicoDstMaker(pm), mPicoDst(0x0), mOutName(outnam), mOutFile(0x0),
  mHistList(0x0), mCounter(0x0), mUPCEvent(0x0), mUPCTree(0x0), mDbMk(0x0),
  mSelectV0(0x0), mSelectCEP(0x0) {

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
  mSelectCEP = new StUPCSelectCEP();

  //init database to read beam-line parameters
  mDbMk = new St_db_Maker("db", "MySQL:StarDb", "$STAR/StarDb");
  //mDbMk->SetDebug();
  mDbMk->Init();

  //geometry for BEMC
  mBemcGeom = StEmcGeom::getEmcGeom("bemc");

  return kStOk;

}//Init

//_____________________________________________________________________________
Int_t StUPCMakerFromPicoDst::Make() {

  //cout << "StUPCMakerFromPicoDst::Make" << endl;

  mUPCEvent->clearEvent(); //clear the output UPC event

  mPicoDst = mPicoDstMaker->picoDst(); // get input picoDst event

  mCounter->Fill( kAna ); // analyzed events

  //tracks in event
  UInt_t nTracks = mPicoDst->numberOfTracks();

  //flag to filter picoDst tracks for writing to upcDst output
  vector<UChar_t> trackFilter(nTracks);

  //UPC tracks for filtering
  vector<StUPCTrack> upcTracks(nTracks);

  //tracks loop to initialize for filtering
  for(UInt_t itrk=0; itrk<nTracks; itrk++) {

    //initialize the flags with false (true is to write the track to the output)
    trackFilter[itrk] = 0;

    //set the UPC tracks from picoDst tracks
    setUpcTrackFromPicoTrack(&upcTracks[itrk], mPicoDst->track(itrk));

  }//tracks loop to initialize

  //cout << "Tracks: " << nTracks << " " << trackFilter.size() << endl;

  //run number and event number
  // beam line parametrs have to be set before v0 selection
  mUPCEvent->setRunNumber( mPicoDst->event()->runId() );
  mUPCEvent->setEventNumber( mPicoDst->event()->eventId() );
  mUPCEvent->setBunchCrossId( mPicoDst->event()->bunchId() ); // set bunch Id for the merger

  readBeamLine();

  //magnetic field
  mUPCEvent->setMagneticField(mPicoDst->event()->bField());

  //select tracks from V0 candidates
  TVector3 vertex = mPicoDst->event()->primaryVertex();
  //int nsel = mSelectV0->selectTracks(upcTracks, trackFilter, mUPCEvent, vertex);
  int nsel = mSelectCEP->selectTracks(mPicoDst, trackFilter);
  
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
    setUpcTrackFromPicoTrack(upcTrack, mPicoDst->track(itrk), true);
    // set flags from preselection
    if( trackFilter[itrk] & (1 << StUPCTrack::kV0) ) 
      upcTrack->setFlag( StUPCTrack::kV0 );
    if( trackFilter[itrk] & (1 << StUPCTrack::kCEP) ) 
      upcTrack->setFlag( StUPCTrack::kCEP );
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
void StUPCMakerFromPicoDst::setUpcTrackFromPicoTrack(StUPCTrack *utrk, StPicoTrack *ptrk, bool writeBemc) {

  if(!ptrk) return;

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

  //TOF
  if( ptrk->isTofTrack() ) 
  {
    utrk->setFlag( StUPCTrack::kTof );

    StPicoBTofPidTraits* bTof = mPicoDst->btofPidTraits( ptrk->bTofPidTraitsIndex() );
    utrk->setTofTime( bTof->btof() );
    utrk->setTofPathLength( calculateTofPathLength(ptrk->origin(), bTof->btofHitPos(), ptrk->helix(bField).curvature()) );
    // DEBUG
    //LOG_INFO << "PathLength: "<< utrk->getTofPathLength() << endm;
  }
  //write the BEMC only when requested
  if( !writeBemc ) return;

  //BEMC matching
  //cout << "BEMC: " << mPicoDst->numberOfBEmcPidTraits() << endl;
  if( ptrk->bemcPidTraitsIndex() >= 0 ) {
    //track is matched to BEMC
    //cout << "BEMC: " << ptrk->bemcPidTraitsIndex() << endl;

    //BEMC PID
    StPicoBEmcPidTraits *bemcPID = mPicoDst->bemcPidTraits( ptrk->bemcPidTraitsIndex() );

    utrk->setFlag( StUPCTrack::kBemc );
    utrk->setBemcHitE( bemcPID->btowE() );

    //tower ID
    Int_t towId = bemcPID->btowId();

    //cluster for valid tower ID
    if( towId > 0 and towId < 4801 ) {

      Float_t eta=-9e9, phi=-9e9;
      mBemcGeom->getEta(towId, eta);
      mBemcGeom->getPhi(towId, phi);

      StUPCBemcCluster *upcCls = mUPCEvent->addCluster();
      upcCls->setEnergy( bemcPID->bemcE() );
      upcCls->setEta(eta);
      upcCls->setPhi(phi);
      upcCls->setId( mUPCEvent->getNumberOfClusters() ); // cluster ID by number of clusters
      utrk->setBemcClusterId( mUPCEvent->getNumberOfClusters() ); // same cluster ID for track

      //cout << "BEMC cluster:" << mUPCEvent->getNumberOfClusters() << endl;

    }
  }
}//setUpcTrackFromPicoTrack

//_____________________________________________________________________________
void StUPCMakerFromPicoDst::readBeamLine() {

  //mDbMk->SetDateTime(20170305,0); // event or run start time, set to your liking
  //mDbMk->SetFlavor("ofl");

  mDbMk->InitRun(mUPCEvent->getRunNumber());
  mDbMk->Make();

  TDataSet *DB = 0;
  DB = mDbMk->GetDataBase("Calibrations/rhic/vertexSeed");
  if (!DB) {
    LOG_INFO << "StUPCMakerFromPicoDst::readBeamLine: no table found in db, or malformed local db config" << endm;
    return;
  }

  St_vertexSeed *dataset = (St_vertexSeed*) DB->Find("vertexSeed");

  if (!dataset) {
    LOG_INFO << "StUPCMakerFromPicoDst::readBeamLine: dataset does not contain requested table" << endm;
    return;
  } 

  Int_t rows = dataset->GetNRows();
  if (rows != 1) {
    LOG_INFO << "StUPCMakerFromPicoDst::readBeamLine:: found INDEXED table with inconsistent number of rows = " << rows << endm;
    if( rows == 0)
      return;
  }

  //TDatime val[2];
  //mDbMk->GetValidity((TTable*)dataset,val);
  //LOG_INFO << "Dataset validity range: [ " << val[0].GetDate() << "." << val[0].GetTime() << " - " << val[1].GetDate() << "." << val[1].GetTime() << " ] "<< endm;

  vertexSeed_st *table = dataset->GetTable();
  mUPCEvent->setBeamXPosition( table[0].x0 );
  mUPCEvent->setBeamXSlope( table[0].dxdz );
  mUPCEvent->setBeamYPosition( table[0].y0 );
  mUPCEvent->setBeamYSlope( table[0].dydz );
}//readBeamLine

//_____________________________________________________________________________
double StUPCMakerFromPicoDst::calculateTofPathLength(const TVector3 beginPoint, const TVector3 endPoint, const double curvature) {

  double xdif =  endPoint.x() - beginPoint.x();
  double ydif =  endPoint.y() - beginPoint.y();
  
  double C = sqrt(xdif*xdif + ydif*ydif);
  double s_perp = C;
  if (curvature){
    double R = 1/curvature;
    s_perp = 2*R * asin(C/(2*R));
  }

  double s_z = fabs(endPoint.z() - beginPoint.z());
  double value = sqrt(s_perp*s_perp + s_z*s_z);

  return value;
}//calculateTofPathLength










