#ifndef StUPCEvent_h
#define StUPCEvent_h

//_____________________________________________________________________________
//    Class for UPC data
//    Author: Jaroslav Adam
//_____________________________________________________________________________

class StUPCTrack;
class TClonesArray;
class StUPCBemcCluster;
class TIterator;
class StUPCVertex;
class TParticle;

#include "TArrayI.h"

class StUPCEvent
{

public:

  StUPCEvent();
  virtual ~StUPCEvent();

  void clearEvent();

  //setters
  void addTriggerId(Int_t id);
  void setRunNumber(Int_t run) { mRunNum = run; }
  void setEventNumber(Int_t num) { mEvtNum = num; }
  void setFillNumber(Int_t num) { mFillNum = num; }
  void setBunchCrossId(UInt_t id) { mbCrossId = id; }
  void setBunchCrossId7bit(UInt_t id) { mbCrossId7bit = id; }
  void setMagneticField(Double_t mag) { mMagField = mag; }

  void setLastDSM0(UShort_t dsm) { mLastDSM0 = dsm; }
  void setLastDSM1(UShort_t dsm) { mLastDSM1 = dsm; }
  void setLastDSM3(UShort_t dsm) { mLastDSM3 = dsm; }

  void setZDCUnAttEast(UShort_t signal) { mZdcEastUA = signal; }
  void setZDCUnAttWest(UShort_t signal) { mZdcWestUA = signal; }
  void setZdcVertexZ(Float_t vtx) { mZdcVertexZ = vtx; }

  void setBBCSmallEast(UInt_t sum) { mBBCSmallEast = sum; }
  void setBBCSmallWest(UInt_t sum) { mBBCSmallWest = sum; }
  void setBBCLargeEast(UInt_t sum) { mBBCLargeEast = sum; }
  void setBBCLargeWest(UInt_t sum) { mBBCLargeWest = sum; }

  void setVPDSumEast(UShort_t sum) { mVPDSumEast = sum; }
  void setVPDSumWest(UShort_t sum) { mVPDSumWest = sum; }

  void setTOFMultiplicity(UShort_t tof) { mTofMult = tof; }
  void setNTofHit(UInt_t tof) { mNTofHit = tof; }
  void setBEMCMultiplicity(UInt_t be) { mBemcMult = be; }

  StUPCTrack *addTrack();

  StUPCBemcCluster *addCluster();

  StUPCVertex *addVertex();

  void setIsMC(Bool_t mc=kTRUE);
  TParticle *addMCParticle();

  //getters
  Bool_t isTrigger(Int_t id) const;
  Int_t getRunNumber() const { return mRunNum; }
  Int_t getEventNumber() const { return mEvtNum; }
  Int_t getFillNumber() const { return mFillNum; }
  UInt_t getBunchCrossId() const { return mbCrossId; }
  UInt_t getBunchCrossId7bit() const { return mbCrossId7bit; }
  Double_t getMagneticField() const { return mMagField; }

  UShort_t getLastDSM0() const { return mLastDSM0; }
  UShort_t getLastDSM1() const { return mLastDSM1; }
  UShort_t getLastDSM3() const { return mLastDSM3; }

  UShort_t getZDCUnAttEast() const { return mZdcEastUA; }
  UShort_t getZDCUnAttWest() const { return mZdcWestUA; }
  Float_t getZdcVertexZ() const { return mZdcVertexZ/10.; } // convert from mm to cm

  UInt_t getBBCSmallEast() const { return mBBCSmallEast; }
  UInt_t getBBCSmallWest() const { return mBBCSmallWest; }
  UInt_t getBBCLargeEast() const { return mBBCLargeEast; }
  UInt_t getBBCLargeWest() const { return mBBCLargeWest; }

  UShort_t getVPDSumEast() const { return mVPDSumEast; }
  UShort_t getVPDSumWest() const { return mVPDSumWest; }

  UShort_t getTOFMultiplicity() const { return mTofMult; }
  UInt_t getNTofHit() const { return mNTofHit; }
  UInt_t getBEMCMultiplicity() const { return mBemcMult; }

  Int_t getNumberOfTracks() const;
  StUPCTrack *getTrack(Int_t iTrack) const;

  Int_t getNumberOfClusters() const;
  StUPCBemcCluster *getCluster(Int_t iCls) const;
  StUPCBemcCluster *getClusterId(UInt_t clsId) const;
  TIterator *makeClustersIter() const;

  Int_t getNumberOfVertices() const;
  StUPCVertex *getVertex(Int_t iVtx) const;
  StUPCVertex *getVertexId(UInt_t vtxId) const;
  TIterator *makeVerticesIter() const;

  Bool_t getIsMC() const { return mMCParticles != NULL ? kTRUE : kFALSE; }
  Int_t getNumberOfMCParticles() const;
  TParticle *getMCParticle(Int_t iMC) const;

private:

  StUPCEvent(const StUPCEvent &o); //not implemented
  StUPCEvent &operator=(const StUPCEvent &o); //not implemented

  TArrayI mTrgIDs; // fired trigger IDs

  Int_t mRunNum; // number of current run
  Int_t mEvtNum; // event number
  Int_t mFillNum; // beam fill number
  UInt_t mbCrossId; // bunch crossing ID
  UInt_t mbCrossId7bit; // bunch crossing ID 7bit
  Double32_t mMagField; // magnetic field

  UShort_t mLastDSM0; // TCU bits 0-15 from StTriggerData::lastDSM(0)
  UShort_t mLastDSM1; // TCU bits 16-31
  UShort_t mLastDSM3; // TCU bits 58-62

  UShort_t mZdcEastUA; // ZDC unattenuated signal, east
  UShort_t mZdcWestUA; // ZDC unattenuated signal, west
  Float_t mZdcVertexZ; // ZDC vertex z position, mm

  UInt_t mBBCSmallEast; // BBC truncated sum, small tiles, east
  UInt_t mBBCSmallWest; // BBC truncated sum, small tiles, west
  UInt_t mBBCLargeEast; // BBC truncated sum, large tiles, east
  UInt_t mBBCLargeWest; // BBC truncated sum, large tiles, west

  UShort_t mVPDSumEast; // VPD ADC sum east
  UShort_t mVPDSumWest; // VPD ADC sum west

  UShort_t mTofMult; // TOF multiplicity from StTriggerData
  UInt_t mNTofHit; // number of TOF hits from StMuDst::numberOfTofHit
  UInt_t mBemcMult; // BEMC multiplicity, number of all BEMC clusters in event

  static TClonesArray *mgUPCTracks; // array of upc tracks
  TClonesArray *mUPCTracks; //-> array of upc tracks
  Int_t mNtracks; //! number of upc tracks in event, local use when filling

  static TClonesArray *mgUPCBemcClusters; // array of BEMC clusters
  TClonesArray *mUPCBemcClusters; //-> array of BEMC clusters
  Int_t mNclusters; //! number of BEMC clusters written in event, local use when filling

  static TClonesArray *mgUPCVertices; // array of UPC vertices
  TClonesArray *mUPCVertices; //-> array of UPC vertices
  Int_t mNvertices; //! number of vertices written in event, local use when filling

  static TClonesArray *mgMCParticles; // array of MC particles
  TClonesArray *mMCParticles; // array of MC particles
  Int_t mNmc; //! number of mc particles in event, local use when filling

  ClassDef(StUPCEvent, 1);
};

#endif













