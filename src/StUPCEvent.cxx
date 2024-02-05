
//_____________________________________________________________________________
//    Class for UPC data
//    Author: Jaroslav Adam
//
//    UPC event
//_____________________________________________________________________________

//root headers
#include "TClonesArray.h"
#include "TIterator.h"
#include "TParticle.h"

//local headers
#include "StUPCTrack.h"
#include "StUPCBemcCluster.h"
#include "StUPCTofHit.h"
#include "StUPCVertex.h"

#include "StUPCEvent.h"

ClassImp(StUPCEvent);

TClonesArray *StUPCEvent::mgUPCTracks = 0;
TClonesArray *StUPCEvent::mgUPCBemcClusters = 0;
TClonesArray *StUPCEvent::mgUPCTOFHits = 0;
TClonesArray *StUPCEvent::mgUPCVertices = 0;
TClonesArray *StUPCEvent::mgMCParticles = 0;

//_____________________________________________________________________________
StUPCEvent::StUPCEvent():
  mRunNum(0), mEvtNum(0), mFillNum(0), mbCrossId(0), mbCrossId7bit(0), mMagField(0),
  mBeamXPosition(0), mBeamXSlope(0), mBeamYPosition(0), mBeamYSlope(0),   
  mZdcEastRate(0), mZdcWestRate(0), mZdcCoincRate(0),
  mLastDSM0(0), mLastDSM1(0), mLastDSM3(0),
  mZdcEastUA(0), mZdcWestUA(0), mZdcEastTDC(0), mZdcWestTDC(0),
  mZdcTimeDiff(0), mZdcVertexZ(0),
  mBBCSmallEast(0), mBBCSmallWest(0), mBBCLargeEast(0), mBBCLargeWest(0),
  mVPDSumEast(0), mVPDSumWest(0), mVPDTimeDiff(0),
  mTofMult(0), mBemcMult(0),
  mNGlobTracks(0), mNPrimTracks(0),mNPrimVertices(0),
  mUPCTracks(0x0), mNtracks(0),
  mUPCBemcClusters(0x0), mNclusters(0),
  mUPCTOFHits(0x0), mNhits(0),
  mUPCVertices(0x0), mNvertices(0),
  mMCParticles(0x0), mNmc(0)
{
  //default constructor

  mTrgIDs.Set(0);

  for (Int_t ipmt=0; ipmt<mNZdcPmt; ipmt++) {
    mZdcEastADC[ipmt] = 0;
    mZdcWestADC[ipmt] = 0;
  }
  for(Int_t i=0; i<mNZdcSmd; i++) {
    mZdcSmdEast[i] = 0;
    mZdcSmdWest[i] = 0;
  }


  if(!mgUPCTracks) {
    mgUPCTracks = new TClonesArray("StUPCTrack");
    mUPCTracks = mgUPCTracks;
    mUPCTracks->SetOwner(kTRUE);
  }

  if(!mgUPCBemcClusters) {
    mgUPCBemcClusters = new TClonesArray("StUPCBemcCluster");
    mUPCBemcClusters = mgUPCBemcClusters;
    mUPCBemcClusters->SetOwner(kTRUE);
  }

  if(!mgUPCTOFHits) {
    mgUPCTOFHits = new TClonesArray("StUPCTofHit");
    mUPCTOFHits = mgUPCTOFHits;
    mUPCTOFHits->SetOwner(kTRUE);
  }

  if(!mgUPCVertices) {
    mgUPCVertices = new TClonesArray("StUPCVertex");
    mUPCVertices = mgUPCVertices;
    mgUPCVertices->SetOwner(kTRUE);
  }

}//StUPCEvent

//_____________________________________________________________________________
StUPCEvent::~StUPCEvent()
{
  //destructor

  if(mUPCTracks) {delete mUPCTracks; mUPCTracks = 0x0;}
  if(mUPCBemcClusters) {delete mUPCBemcClusters; mUPCBemcClusters = 0x0;}
  if(mUPCTOFHits) {delete mUPCTOFHits; mUPCTOFHits = 0x0;}
  if(mUPCVertices) {delete mUPCVertices; mUPCVertices = 0x0;}
  if(mMCParticles) {delete mMCParticles; mMCParticles = 0x0;}

}//~StUPCEvent

//_____________________________________________________________________________
void StUPCEvent::clearEvent()
{
  // clear event variables

  mTrgIDs.Set(0);

  mBemcMult = 0;

  mUPCTracks->Clear("C");
  mNtracks = 0;

  mUPCBemcClusters->Clear("C");
  mNclusters = 0;

  mUPCTOFHits->Clear("C");
  mNhits = 0;

  mUPCVertices->Clear("C");
  mNvertices = 0;

  if(mMCParticles) {
    mMCParticles->Clear("C");
    mNmc = 0;
  }

}//clearEvent

//_____________________________________________________________________________
void StUPCEvent::addTriggerId(Int_t id) {
  //add fired trigger ID

  //position to put the ID
  Int_t pos = mTrgIDs.GetSize();

  //extend the array
  mTrgIDs.Set(pos+1);

  //put the ID
  mTrgIDs.AddAt(id, pos);

}//addTriggerId

//_____________________________________________________________________________
void StUPCEvent::setZDCEastADC(UShort_t signal, Int_t pmt) {

  //set ZDC PMT signal, check for bounds, east side
  Int_t idx = pmt - 1;
  if( idx < 0 or idx > mNZdcPmt-1 ) return;

  mZdcEastADC[idx] = signal;

}//setZDCEastADC

//_____________________________________________________________________________
void StUPCEvent::setZDCWestADC(UShort_t signal, Int_t pmt) {

  //set ZDC PMT signal, check for bounds, west side
  Int_t idx = pmt - 1;
  if( idx < 0 or idx > mNZdcPmt-1 ) return;

  mZdcWestADC[idx] = signal;

}//setZDCWestADC

//_____________________________________________________________________________
void StUPCEvent::setZdcSMDEast(Int_t verthori, Int_t strip, UShort_t smd) {

  //ZDC SMD data, indexing for verthori is 0=vertical, 1=Horizontal and strips go as strip=1, strip<9

  if( verthori < 0 or verthori > 1 ) return;
  if( strip < 1 or strip > 8 ) return;

  mZdcSmdEast[strip-1 + verthori*8] = smd;

}//setZdcSMDEast

//_____________________________________________________________________________
void StUPCEvent::setZdcSMDWest(Int_t verthori, Int_t strip, UShort_t smd) {

  //ZDC SMD data, indexing for verthori is 0=vertical, 1=Horizontal and strips go as strip=1, strip<9

  if( verthori < 0 or verthori > 1 ) return;
  if( strip < 1 or strip > 8 ) return;

  mZdcSmdWest[strip-1 + verthori*8] = smd;

}//setZdcSMDWest

//_____________________________________________________________________________
StUPCTrack *StUPCEvent::addTrack()
{
  // construct new upc track

  return dynamic_cast<StUPCTrack*>( mUPCTracks->ConstructedAt(mNtracks++) );

}//addTrack

//_____________________________________________________________________________
StUPCBemcCluster *StUPCEvent::addCluster()
{
  // construct new BEMC cluster

  return dynamic_cast<StUPCBemcCluster*>( mUPCBemcClusters->ConstructedAt(mNclusters++) );

}//addCluster

//_____________________________________________________________________________
StUPCTofHit *StUPCEvent::addHit()
{
  // construct new TOF hit

  return dynamic_cast<StUPCTofHit*>( mUPCTOFHits->ConstructedAt(mNhits++) );

}//addHit

//_____________________________________________________________________________
StUPCVertex *StUPCEvent::addVertex()
{
  //construct new UPC vertex

  return dynamic_cast<StUPCVertex*>( mUPCVertices->ConstructedAt(mNvertices++) );

}//addVertex

//_____________________________________________________________________________
void StUPCEvent::setIsMC(Bool_t mc) {

  // set the event as MC, initialize mc array

  if(!mc) return;

  if(!mgMCParticles) {
    mgMCParticles = new TClonesArray("TParticle");
    mMCParticles = mgMCParticles;
    mMCParticles->SetOwner(kTRUE);
  }

}//setIsMC

//_____________________________________________________________________________
TParticle *StUPCEvent::addMCParticle()
{
  // construct new mc TParticle

  if(!mMCParticles) return 0x0;

  return dynamic_cast<TParticle*>( mMCParticles->ConstructedAt(mNmc++) );

}//addMCParticle

//_____________________________________________________________________________
Bool_t StUPCEvent::isTrigger(Int_t id) const
{
  //get fired trigger ID
  for(Int_t i=0; i<mTrgIDs.GetSize(); i++) {
    if( mTrgIDs.At(i) == id ) return kTRUE;
  }

  return kFALSE;

}//setTrigger

//_____________________________________________________________________________
Int_t StUPCEvent::getNumberOfTracks() const {

  //number of tracks in event

  if( !mUPCTracks ) return 0;

  return mUPCTracks->GetEntriesFast();

}//getNumberOfTracks

//_____________________________________________________________________________
StUPCTrack *StUPCEvent::getTrack(Int_t iTrack) const
{
  // get upc track

  StUPCTrack *track = dynamic_cast<StUPCTrack*>( mUPCTracks->At(iTrack) );
  if(track) track->setEvent( const_cast<StUPCEvent*>(this) );

  return track;

}//getTrack

//_____________________________________________________________________________
Int_t StUPCEvent::getNumberOfClusters() const {

  //number of BEMC clusters in event

  if( !mUPCBemcClusters ) return 0;

  return mUPCBemcClusters->GetEntriesFast();

}//getNumberOfClusters

//_____________________________________________________________________________
StUPCBemcCluster *StUPCEvent::getCluster(Int_t iCls) const
{
  // get BEMC cluster

  return dynamic_cast<StUPCBemcCluster*>( mUPCBemcClusters->At(iCls) );

}//getCluster

//_____________________________________________________________________________
StUPCBemcCluster *StUPCEvent::getClusterId(UInt_t clsId) const
{
  // get BEMC cluster according to origial ID

  //clusters loop
  StUPCBemcCluster *cls=0x0;
  TIterator *clsIter = makeClustersIter();
  while( (cls = dynamic_cast<StUPCBemcCluster*>( clsIter->Next() )) != NULL ) {

    //get cluster with a given ID
    if( cls->getId() == clsId ) {delete clsIter; return cls;}
  }//clusters loop

  delete clsIter;
  return 0x0;

}//getCluster

//_____________________________________________________________________________
TIterator *StUPCEvent::makeClustersIter() const
{
  // create iterator for BEMC clusters clones array

  return mUPCBemcClusters->MakeIterator();

}//getClustersIter

//_____________________________________________________________________________
Int_t StUPCEvent::getNumberOfHits() const {

  //number of TOF hits in event

  if( !mUPCTOFHits ) return 0;

  return mUPCTOFHits->GetEntriesFast();

}//getNumberOfHits

//_____________________________________________________________________________
StUPCTofHit *StUPCEvent::getHit(Int_t iHit) const
{
  // get TOF hit

  return dynamic_cast<StUPCTofHit*>( mUPCTOFHits->At(iHit) );

}//getHit

//_____________________________________________________________________________
Int_t StUPCEvent::getNumberOfVertices() const {

  //number of primary vertices in event

  if( !mUPCVertices ) return 0;

  return mUPCVertices->GetEntriesFast();

}//getNumberOfVertices

//_____________________________________________________________________________
StUPCVertex *StUPCEvent::getVertex(Int_t iVtx) const
{
  //get UPC vertex from clones array

  return dynamic_cast<StUPCVertex*>( mUPCVertices->At(iVtx) );

}//getVertex

//_____________________________________________________________________________
StUPCVertex *StUPCEvent::getVertexId(UInt_t vtxId) const
{
  // get vertex according to original ID

  //vertex loop
  StUPCVertex *vtx = 0x0;
  TIterator *vtxIter = makeVerticesIter();
  while( (vtx = dynamic_cast<StUPCVertex*>( vtxIter->Next() )) != NULL ) {

    //get vertex with a given ID
    if( vtx->getId() == vtxId ) {delete vtxIter; return vtx;}
  }//vertex loop

  delete vtxIter;
  return 0x0;

}//getVertexId

//_____________________________________________________________________________
TIterator *StUPCEvent::makeVerticesIter() const
{
  // create iterator for vertices clones array

  return mUPCVertices->MakeIterator();

}//makeVerticesIter

//_____________________________________________________________________________
Int_t StUPCEvent::getNumberOfMCParticles() const {

  //number of MC particles in event

  if( !mMCParticles ) return 0;

  return mMCParticles->GetEntriesFast();

}//getNumberOfMCParticles

//_____________________________________________________________________________
TParticle *StUPCEvent::getMCParticle(Int_t iMC) const
{
  // get mc TParticle

  if(!mMCParticles) return 0x0;

  return dynamic_cast<TParticle*>( mMCParticles->At(iMC) );

}//getMCParticle


//_____________________________________________________________________________
StUPCEvent &StUPCEvent::operator=(const StUPCEvent &o)
{
  if (this == &o)
      return *this; //self assignment

  clearEvent();

  for( Int_t i = 0; i < o.getTriggerArraySize(); i++)
    addTriggerId( o.getTriggerID(i) );

  setRunNumber( o.getRunNumber() );
  setEventNumber( o.getEventNumber() );
  setFillNumber( o.getFillNumber() );
  setBunchCrossId( o.getBunchCrossId() );
  setBunchCrossId7bit( o.getBunchCrossId7bit() );
  setMagneticField( o.getMagneticField() );  
  setZDCEastRate( o.getZDCEastRate() );
  setZDCWestRate( o.getZDCWestRate() );
  setZDCCoincRate( o.getZDCCoincRate() );
  setLastDSM0( o.getLastDSM0() );
  setLastDSM1( o.getLastDSM1() );
  setLastDSM3( o.getLastDSM3() );
  setZDCUnAttEast( o.getZDCUnAttEast() );
  setZDCUnAttWest( o.getZDCUnAttWest() );
  for (Int_t ipmt=1; ipmt<=3; ipmt++) {
    setZDCEastADC( o.getZDCEastADC(ipmt), ipmt );
    setZDCWestADC( o.getZDCWestADC(ipmt), ipmt );
  }
  setZDCEastTDC( o.getZDCEastTDC() );
  setZDCWestTDC( o.getZDCWestTDC() );
  setZDCTimeDiff( o.getZDCTimeDiff() );
  setZdcVertexZ( o.getZdcVertexZ() );
  //ZDC SMD
  //vertical - horizontal loop
  for(Int_t verthori=0; verthori<2; verthori++) {
    //strip loop
    for(Int_t strip=1; strip<9; strip++) {
      setZdcSMDEast( verthori, strip,  o.getZdcSMDEast(verthori, strip) );
      setZdcSMDWest( verthori, strip,  o.getZdcSMDWest(verthori, strip) );
    }//strip loop
  }//vertical - horizontal loop

  setBBCSmallEast( o.getBBCSmallEast() );
  setBBCSmallWest( o.getBBCSmallWest() );
  setBBCLargeEast( o.getBBCLargeEast() );
  setBBCLargeWest( o.getBBCLargeWest() );

  setVPDSumEast( o.getVPDSumEast() );
  setVPDSumWest( o.getVPDSumWest() );
  setVPDTimeDiff( o.getVPDTimeDiff() );

  setTOFMultiplicity( o.getTOFMultiplicity() );
  setBEMCMultiplicity( o.getBEMCMultiplicity() );

  setNGlobTracks( o.getNGlobTracks() );
  setNPrimTracks( o.getNPrimTracks() );
  setNPrimVertices( o.getNPrimVertices() );

  // Add tracks, tracks loop
  for(Int_t itrk=0; itrk< o.getNumberOfTracks(); itrk++) {
    StUPCTrack *track = o.getTrack(itrk);
    if( !track ) continue;

    //UPC track
    StUPCTrack *upcTrack = addTrack();
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
  for(int iClstr=0; iClstr < o.getNumberOfClusters(); iClstr++) {
    StUPCBemcCluster *cluster = o.getCluster(iClstr);
    if( !cluster ) continue;

    //put cluster in clones array in UPC event
    StUPCBemcCluster *upcCls = addCluster();
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
  for(int ihit=0; ihit < o.getNumberOfHits(); ihit++) {
    // get hit from main upcDst
    StUPCTofHit *tofhit = o.getHit( ihit );
    if( !tofhit ) continue;

    //put hit in clones array in UPC event
    StUPCTofHit *upcHit = addHit();
    upcHit->setTray( tofhit->getTray() );
    upcHit->setModule( tofhit->getModule() );
    upcHit->setCell( tofhit->getCell() );
    upcHit->setLeadingEdgeTime( tofhit->getLeadingEdgeTime() );
    upcHit->setTrailingEdgeTime( tofhit->getTrailingEdgeTime() );
  }

  // Add vertecies
  for (int iVrtx = 0; iVrtx < o.getNumberOfVertices(); ++iVrtx) {
    StUPCVertex *mainVertex = o.getVertex( iVrtx );
    if( !mainVertex ) continue;

    //write vertex to output UPC event
    StUPCVertex *upcVtx = addVertex();
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
  setIsMC( o.getIsMC() );  

  return *this;
}//operator=(













