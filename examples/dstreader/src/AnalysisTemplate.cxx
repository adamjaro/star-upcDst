
// C++ headers
#include <iostream>

// ROOT headers
#include "TFile.h"
#include "TTree.h"

// picoDst headers
#include "StUPCEvent.h"
#include "StUPCTrack.h"
#include "StUPCVertex.h"

using namespace std;

struct MyEvent {

public:

  //event info
  Int_t mRunNum_mini; // number of current run
  Int_t mEvtNum_mini; // event number
  Int_t mFillNum_mini; // beam fill number
  UShort_t mTofMult_mini;
  Int_t mNumberOfTracks_mini;
  Bool_t isTrigger_mini[3];

  //vertex info
  Int_t mNvertex_mini;
  Float_t mPosX_mini[30]; // position x
  Float_t mPosY_mini[30]; // position y
  Float_t mPosZ_mini[30]; // position z
  Int_t mNPrimaryTracks_mini[30];
  

  //ZDC and SMD
  UShort_t mZdcEastUA_mini; // ZDC unattenuated signal, east
  UShort_t mZdcWestUA_mini; // ZDC unattenuated signal, west
  static const Int_t mNZdcPmt = 3;
  UShort_t mZdcEastADC_mini[mNZdcPmt]; // ZDC 3 PMT ADCs, east
  UShort_t mZdcWestADC_mini[mNZdcPmt]; // ZDC 3 PMT ADCs, west
  UShort_t mZdcEastTDC_mini; // ZDC TDC, east
  UShort_t mZdcWestTDC_mini; // ZDC TDC, west
  UShort_t mZdcTimeDiff_mini; // ZDC time difference, east-west
  Float_t mZdcVertexZ_mini; // ZDC vertex z position, mm
  static const Int_t mNZdcSmd = 16; // number of ZDC SMD channels, east and west
  UShort_t mZdcSmdEast_mini[mNZdcSmd]; // ZDC SMD data east
  UShort_t mZdcSmdWest_mini[mNZdcSmd]; // ZDC SMD data west

  //track info
  static const Int_t MAXTracks = 500;
  Bool_t mFlagBemc_mini[MAXTracks];
  Bool_t mFlagTof_mini[MAXTracks];
  Double32_t mPt_mini[MAXTracks]; // pT at point of dca to primary vertex
  Double32_t mEta_mini[MAXTracks]; // pseudorapidity at point of dca to primary vertex
  Double32_t mPhi_mini[MAXTracks]; // phi at point of dca to primary vertex

  Float_t mDcaXY_mini[MAXTracks]; // perpendicular dca to primary vertex of associated global track
  Float_t mDcaZ_mini[MAXTracks]; // longitudinal dca to primary vertex of associated global track

  Short_t mCharge_mini[MAXTracks]; // track electrical charge

  UShort_t mNhits_mini[MAXTracks]; // total number of hits on track
  UShort_t mNhitsFit_mini[MAXTracks]; // number of hits used in fit

  Double32_t mChi2_mini[MAXTracks]; // chi2 of fit

  UShort_t mNhitsDEdx_mini[MAXTracks]; // number of hits used for dE/dx measurement
  Double32_t mDEdxSignal_mini[MAXTracks]; // measured dE/dx value

  Float16_t mNSigmasTPCElectron_mini[MAXTracks]; // dE/dx n sigmas for particle species
  Float16_t mNSigmasTPCPion_mini[MAXTracks]; // dE/dx n sigmas for particle species

  Double32_t mBemcPt_mini[MAXTracks]; // pT at BEMC radius
  Double32_t mBemcEta_mini[MAXTracks]; // pseudorapidity at BEMC radius
  Double32_t mBemcPhi_mini[MAXTracks]; // phi at BEMC radius
  Double32_t mBemcPmag_mini[MAXTracks]; // phi at BEMC radius
  UInt_t mBemcClsId_mini[MAXTracks]; // id of BEMC cluster which track is matched to
  Float_t mBemcHitE_mini[MAXTracks]; // energy of matched BEMC cluster

  Float_t mTofTime_mini[MAXTracks]; // time of flight by TOF
  Float_t mTofPathLength_mini[MAXTracks]; // path lenght from TOF

  UInt_t mVtxId_mini[MAXTracks]; // ID of primary vertex associated with track

};

//_____________________________________________________________________________
int main(void) {

  //open input file
  TFile *infile = TFile::Open("/Users/kong/google_drive/BNL_folder/Work/STAR/STAR_UPCpicoMaker/star-upcDst/upcDst_files/upcDst_dAu200.root", "read");

  //get picoDst tree in file
  TTree *upcTree = dynamic_cast<TTree*>( infile->Get("mUPCTree") );

  //open output file
  TFile *outfile = TFile::Open("output.root", "recreate");
  //create output tree
  TTree *myTree = new TTree("myTree", "myTree");
  MyEvent myEvent;

  myTree->Branch("mRunNum_mini", &myEvent.mRunNum_mini, "mRunNum_mini/I");
  myTree->Branch("mEvtNum_mini", &myEvent.mEvtNum_mini, "mEvtNum_mini/I");
  myTree->Branch("mFillNum_mini", &myEvent.mFillNum_mini, "mFillNum_mini/I");
  myTree->Branch("mTofMult_mini", &myEvent.mTofMult_mini, "mTofMult_mini/S");
  myTree->Branch("mNumberOfTracks_mini", &myEvent.mNumberOfTracks_mini, "mNumberOfTracks_mini/I");
  myTree->Branch("isTrigger_mini", myEvent.isTrigger_mini, "isTrigger_mini[3]/B");
  
  myTree->Branch("mNvertex_mini", &myEvent.mNvertex_mini, "mNvertex_mini/I");
  myTree->Branch("mPosX_mini", myEvent.mPosX_mini, "mPosX_mini[mNvertex_mini]/F");
  myTree->Branch("mPosY_mini", myEvent.mPosY_mini, "mPosY_mini[mNvertex_mini]/F");
  myTree->Branch("mPosZ_mini", myEvent.mPosZ_mini, "mPosZ_mini[mNvertex_mini]/F");
  myTree->Branch("mNPrimaryTracks_mini", myEvent.mNPrimaryTracks_mini, "mNPrimaryTracks_mini[mNvertex_mini]/I");

  myTree->Branch("mZdcEastUA_mini", &myEvent.mZdcEastUA_mini, "mZdcEastUA_mini/S");
  myTree->Branch("mZdcWestUA_mini", &myEvent.mZdcWestUA_mini, "mZdcWestUA_mini/S");
  myTree->Branch("mZdcEastADC_mini", myEvent.mZdcEastADC_mini, "mZdcEastADC_mini[3]/S");
  myTree->Branch("mZdcWestADC_mini", myEvent.mZdcWestADC_mini, "mZdcWestADC_mini[3]/S");
  myTree->Branch("mZdcEastTDC_mini", &myEvent.mZdcEastTDC_mini, "mZdcEastTDC_mini/S");
  myTree->Branch("mZdcWestTDC_mini", &myEvent.mZdcWestTDC_mini, "mZdcWestTDC_mini/S");
  myTree->Branch("mZdcTimeDiff_mini", &myEvent.mZdcTimeDiff_mini, "mZdcTimeDiff_mini/S");
  myTree->Branch("mZdcVertexZ_mini", &myEvent.mZdcVertexZ_mini, "mZdcVertexZ_mini/F");
  myTree->Branch("mZdcSmdEast_mini", myEvent.mZdcSmdEast_mini, "mZdcSmdEast_mini[16]/S");
  myTree->Branch("mZdcSmdWest_mini", myEvent.mZdcSmdWest_mini, "mZdcSmdWest_mini[16]/S");
  
  myTree->Branch("mFlagBemc_mini", myEvent.mFlagBemc_mini, "mFlagBemc_mini[mNumberOfTracks_mini]/D");
  myTree->Branch("mFlagTof_mini", myEvent.mFlagTof_mini, "mFlagTof_mini[mNumberOfTracks_mini]/D");
  myTree->Branch("mPt_mini", myEvent.mPt_mini, "mPt_mini[mNumberOfTracks_mini]/D");
  myTree->Branch("mEta_mini", myEvent.mEta_mini, "mEta_mini[mNumberOfTracks_mini]/D");
  myTree->Branch("mPhi_mini", myEvent.mPhi_mini, "mPhi_mini[mNumberOfTracks_mini]/D");
  myTree->Branch("mDcaXY_mini", myEvent.mDcaXY_mini, "mDcaXY_mini[mNumberOfTracks_mini]/F");
  myTree->Branch("mDcaZ_mini", myEvent.mDcaZ_mini, "mDcaZ_mini[mNumberOfTracks_mini]/F");
  myTree->Branch("mCharge_mini", myEvent.mCharge_mini, "mCharge_mini[mNumberOfTracks_mini]/S");
  myTree->Branch("mNhits_mini", myEvent.mNhits_mini, "mNhits_mini[mNumberOfTracks_mini]/S");
  myTree->Branch("mNhitsFit_mini", myEvent.mNhitsFit_mini, "mNhitsFit_mini[mNumberOfTracks_mini]/S");
  myTree->Branch("mChi2_mini", myEvent.mChi2_mini, "mChi2_mini[mNumberOfTracks_mini]/D");
  myTree->Branch("mNhitsDEdx_mini", myEvent.mNhitsDEdx_mini, "mNhitsDEdx_mini[mNumberOfTracks_mini]/S");
  myTree->Branch("mDEdxSignal_mini", myEvent.mDEdxSignal_mini, "mDEdxSignal_mini[mNumberOfTracks_mini]/D");
  myTree->Branch("mNSigmasTPCElectron_mini", myEvent.mNSigmasTPCElectron_mini, "mNSigmasTPCElectron_mini[mNumberOfTracks_mini]/F");
  myTree->Branch("mNSigmasTPCPion_mini", myEvent.mNSigmasTPCPion_mini, "mNSigmasTPCPion_mini[mNumberOfTracks_mini]/F");
  myTree->Branch("mBemcPt_mini", myEvent.mBemcPt_mini, "mBemcPt_mini[mNumberOfTracks_mini]/D");
  myTree->Branch("mBemcEta_mini", myEvent.mBemcEta_mini, "mBemcEta_mini[mNumberOfTracks_mini]/D");
  myTree->Branch("mBemcPhi_mini", myEvent.mBemcPhi_mini, "mBemcPhi_mini[mNumberOfTracks_mini]/D");
  myTree->Branch("mBemcPmag_mini", myEvent.mBemcPmag_mini, "mBemcPmag_mini[mNumberOfTracks_mini]/D");
  myTree->Branch("mBemcClsId_mini", myEvent.mBemcClsId_mini, "mBemcClsId_mini[mNumberOfTracks_mini]/I");
  myTree->Branch("mBemcHitE_mini", myEvent.mBemcHitE_mini, "mBemcHitE_mini[mNumberOfTracks_mini]/F");
  myTree->Branch("mTofTime_mini", myEvent.mTofTime_mini, "mTofTime_mini[mNumberOfTracks_mini]/F");
  myTree->Branch("mTofPathLength_mini", myEvent.mTofPathLength_mini, "mTofPathLength_mini[mNumberOfTracks_mini]/F");
  myTree->Branch("mVtxId_mini", myEvent.mVtxId_mini, "mVtxId_mini[mNumberOfTracks_mini]/I");

  //connect upc event to the tree
  static StUPCEvent *upcEvt = 0x0;
  upcTree->SetBranchAddress("mUPCEvent", &upcEvt);
  
  //ask for number of events
  Long64_t nev = upcTree->GetEntries();
  cout << "Number of events: " << nev << endl;
  //event loop
  for(Long64_t iev=0; iev<nev; iev++) {

    //get the event
    upcTree->GetEntry(iev);

    //save event info in minitrees:
    myEvent.mRunNum_mini = upcEvt->getRunNumber();
    myEvent.mEvtNum_mini = upcEvt->getEventNumber(); 
    myEvent.mFillNum_mini = upcEvt->getFillNumber();
    myEvent.mTofMult_mini = upcEvt->getTOFMultiplicity();
    myEvent.mNumberOfTracks_mini = upcEvt->getNumberOfTracks();
    myEvent.isTrigger_mini[0] = upcEvt->isTrigger(530701);
    myEvent.isTrigger_mini[1] = upcEvt->isTrigger(530702);
    myEvent.isTrigger_mini[2] = upcEvt->isTrigger(530703);

    //vertex
    int nVertex = upcEvt->getNumberOfVertices();
    
    bool hasValidVertex = false;
    for(int v=0;v<nVertex;v++){
      StUPCVertex *vtx = upcEvt->getVertex(v);
      myEvent.mPosX_mini[v] = vtx->getPosX();
      myEvent.mPosY_mini[v] = vtx->getPosY();
      myEvent.mPosZ_mini[v] = vtx->getPosZ();
      myEvent.mNPrimaryTracks_mini[v] = vtx->getNPrimaryTracks();
      
      int numMatchedTracks = 0;
      for(Int_t i=0; i<upcEvt->getNumberOfTracks(); i++) {
        StUPCTrack *trk = upcEvt->getTrack(i);
        if( v != trk->getVertexId() ) continue;
        if( !trk->getFlag(StUPCTrack::kBemc) && !trk->getFlag(StUPCTrack::kTof) ) continue;
          numMatchedTracks++;
      }
      if( numMatchedTracks > 0 && numMatchedTracks < 6 ){
        hasValidVertex = true;
      }

    }

    if( !hasValidVertex ) continue;
    if( iev%10000 == 0 ) cout << "Number of events = " << iev << endl;

    myEvent.mNvertex_mini = upcEvt->getNumberOfVertices();
    myEvent.mZdcEastUA_mini = upcEvt->getZDCUnAttEast(); 
    myEvent.mZdcWestUA_mini = upcEvt->getZDCUnAttWest(); 
    for(int i=0;i<3;i++){
      myEvent.mZdcEastADC_mini[i] = upcEvt->getZDCEastADC(i+1); 
      myEvent.mZdcWestADC_mini[i] = upcEvt->getZDCWestADC(i+1);
    }
    myEvent.mZdcEastTDC_mini = upcEvt->getZDCEastTDC(); 
    myEvent.mZdcWestTDC_mini = upcEvt->getZDCWestTDC(); 
    myEvent.mZdcTimeDiff_mini = upcEvt->getZDCTimeDiff(); 
    myEvent.mZdcVertexZ_mini = upcEvt->getZdcVertexZ(); 
    for(int i=0;i<8;i++){
      myEvent.mZdcSmdEast_mini[i] = upcEvt->getZdcSMDEast(0,i+1); 
      myEvent.mZdcSmdEast_mini[i+8] = upcEvt->getZdcSMDEast(1,i+1); 

      myEvent.mZdcSmdWest_mini[i] = upcEvt->getZdcSMDWest(0,i+1);
      myEvent.mZdcSmdWest_mini[i+8] = upcEvt->getZdcSMDWest(1,i+1);
    }
    //tracks loop
    for(Int_t i=0; i<upcEvt->getNumberOfTracks(); i++) {
      StUPCTrack *trk = upcEvt->getTrack(i);

      myEvent.mFlagBemc_mini[i] = trk->getFlag(StUPCTrack::kBemc);
      myEvent.mFlagTof_mini[i] = trk->getFlag(StUPCTrack::kTof);
      myEvent.mPt_mini[i] = trk->getPt();
      myEvent.mEta_mini[i] = trk->getEta();
      myEvent.mPhi_mini[i] = trk->getPhi();
      myEvent.mDcaXY_mini[i] = trk->getDcaXY();
      myEvent.mDcaZ_mini[i] = trk->getDcaZ();
      myEvent.mCharge_mini[i] = trk->getCharge();
      myEvent.mNhits_mini[i] = trk->getNhits();
      myEvent.mNhitsFit_mini[i] = trk->getNhitsFit();
      myEvent.mChi2_mini[i] = trk->getChi2();
      myEvent.mNhitsDEdx_mini[i] = trk->getNhitsDEdx();
      myEvent.mDEdxSignal_mini[i] = trk->getDEdxSignal();
      myEvent.mNSigmasTPCElectron_mini[i] = trk->getNSigmasTPCElectron();
      myEvent.mNSigmasTPCPion_mini[i] = trk->getNSigmasTPCPion();
      myEvent.mBemcPt_mini[i] = trk->getBemcPt();
      myEvent.mBemcEta_mini[i] = trk->getBemcEta();
      myEvent.mBemcPhi_mini[i] = trk->getBemcPhi();
      myEvent.mBemcPmag_mini[i] = trk->getBemcPmag();
      myEvent.mBemcClsId_mini[i] = trk->getBemcClusterId();
      myEvent.mBemcHitE_mini[i] = trk->getBemcHitE();
      
      myEvent.mTofTime_mini[i] = trk->getTofTime();
      myEvent.mTofPathLength_mini[i] = trk->getTofPathLength();
      myEvent.mVtxId_mini[i] = trk->getVertexId();

    }

    myTree->Fill();

  }

  //close the outputs
  myTree->Write();
  outfile->Close();

  return 0;

}//main

