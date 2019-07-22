// Run: ./Analysis input index
// e.g. ./Analysis /gpfs01/star/pwg/truhlar/Final/CPtrig/merge_files/StUPCRP_production.list -1
// or you can open just root file
// ./Analysis /gpfs01/star/pwg/jaroslav/test/star-upcDst/trees/RP_central_elastic_17_v0/merge_files/StUPCRP_central_elastic_17_v0_0000.root


// c++ headers
#include <iostream>
#include <utility>
#include <sstream> 
#include <algorithm> 
#include <stdio.h> 
#include <stdlib.h> 
#include <vector> 
#include <fstream> 
#include <cmath> 
#include <cstdlib>

// ROOT headers
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TH1D.h"
#include <TH2.h> 
#include <TF1.h> 
#include <TF2.h> 
#include <THStack.h> 
#include <TStyle.h> 
#include <TGraph.h> 
#include <TGraph2D.h> 
#include <TGraphErrors.h> 
#include <TCanvas.h> 
#include <TLegend.h> 
#include <TGaxis.h> 
#include <TString.h> 
#include <TColor.h> 
#include <TLine.h> 
#include <TExec.h> 
#include <TFitResultPtr.h> 
#include <TFitResult.h> 
#include <TLatex.h> 
#include <TMath.h>
#include <TLorentzVector.h>

// picoDst headers
#include "StRPEvent.h"
#include "StUPCRpsTrack.h"
#include "StUPCRpsTrackPoint.h"

#include "StUPCEvent.h"
#include "StUPCTrack.h"
#include "StUPCBemcCluster.h"
#include "StUPCVertex.h"

using namespace std;

enum {kCPtrig=1, kEl, kInEl, kTOF2t, kSameVrtx, kTotCH0, kMissPt, kMaxCount};
enum SIDE {E=0, East=0, W=1, West=1, nSides};
enum XY_COORDINATE {X, Y, nCoordinates};
enum RP_ID {E1U, E1D, E2U, E2D, W1U, W1D, W2U, W2D, nRomanPots};
enum BRANCH_ID { EU, ED, WU, WD, nBranches };
enum ARM_ID { EU_WU, ED_WD, nArms };
enum STATION_ID { E1, E2, W1, W2, nStations };
enum RP_CONFIGURATION {EUD, EDU, IUU, IDD, nConfiguration}; 
// EUD means elastic combination from east up to west down
// EDU means elastic combination from east down to west up
// IUU means inelastic combination from east up to west up
// IDD means inelastic combination from east down to west down

// Labels, names etc. (defined as TString to gain higher functionality than const char*, e.g. defined "+" operator)

const double pionMass = 0.13957; // GeV /c^2
const double speedOfLight = 299792458; // m/s
const int nTriggers = 21;
const int triggerID[] = {1,2,3,4,5,8,9,570701,570702,570703,
570704,570705,570709,570711,570712,570719,590701,590703,590705,590708,590709};


TString triggerName[nTriggers] = {  TString("CPT2_test"), // 1
                        TString("CPT2noBBCL_test"), // 2
                        TString("SDT_test"), // 3
                        TString("ET_test"), // 4
                        TString("CPTnoBBCL_test"), // 5
                        TString("IT_PREPOST_test"), // 8
                        TString("ET_test"), // 9
                        TString("CPT2"),  // 570701
                        TString("UPC"), // 570702
                        TString("SDT"), // 570703
                        TString("Zerobias"), // 570704
                        TString("CPT2noBBCL"), // 570705
                        TString("ET"), // 570709
                        TString("CPT2"), // 570711
                        TString("UPC"), // 570712
                        TString("ET"), // 570719
                        TString("CPT2"), // 590701
                        TString("SDT"), // 590703
                        TString("CPT2noBBCL"), // 590705
                        TString("CPTnoBBCL"), // 590708
                        TString("ET")}; // 590709


TString branchName[nBranches] = { TString("EU"), TString("ED"), TString("WU"), TString("WD") };
TString summaryLabels[10] = { TString("All"), TString("Elastic"), TString("Inelastic"), TString("2 TPC-TOF tracks"), 
                              TString("Same vertex"), TString("TotCharge 0"), TString("MissingPt < 0.1 GeV"), 
                              TString(""), TString(""), TString("")};
TString systemID[3] = { TString("Combi"), TString("InEl"), TString("El")};
TString systemState[4] = { TString("TPC2t"), TString("TOF2trk"), TString("Q0"), TString("Excl")};
TString configLabels[4] = { TString("EUD"), TString("EDU"), TString("IUU"), TString("IDD")};


// Histograms declarations
TH1I* hAnalysisFlow; // control plot
TH1F* hTriggerBits; // control plot
TH1F* hConfiguration;
TH1F* hNumberTrackPerBranch[nBranches]; // number of track per each brunch (west up, west down, east up, east down)
TH1F* hNumberTrack;

TH1F* hMissingPt[12]; 
TH1F* hInvMass[12];

TFile *infile;
TFile *outfile;
TChain *upcChain;
TChain *rpChain;
StUPCEvent *upcEvt;
StRPEvent *rpEvt;

TTree *upcTree;
TTree *rpTree;

TTree *recTree;
Int_t nTracks; //triggerBits,
Int_t totalCharge;
Int_t nTofTrks;
Double_t nSigPPion,nSigPKaon,nSigPProton;
Double_t missingPt; 
Double_t invMass;
Double_t deltaTOF, deltaDeltaTOF, mSquared;

Double_t dEdx1, dEdx2;
Double_t momentum1, momentum2;
Double_t charge1, charge2;
Double_t TOFlength1, TOFlength2;
Double_t TOFtime1, TOFtime2;

Double_t vertexZ;

Double_t DcaXY1, DcaZ1, DcaXY2, DcaZ2;
Double_t Nhits1, NhitsDEdx1, Nhits2, NhitsDEdx2;
Double_t Eta1, Eta2;

Bool_t elastic;

Double_t xCorrelationsWest, xCorrelationsEast;
Double_t yCorrelationsWest, yCorrelationsEast;

vector<Double_t> dEdx;
vector<Double_t> momentum;
vector<Double_t> TOFtime;
vector<Double_t> TOFlength;
vector<Double_t> charge;
vector<Double_t> nSigmaTPCPion;
vector<Double_t> nSigmaTPCKaon;
vector<Double_t> nSigmaTPCProton;
vector<Double_t> vexterId;
vector<Double_t> vertexesZ;
vector<Double_t> DcaXY;
vector<Double_t> DcaZ;
vector<Double_t> Nhits;
vector<Double_t> NhitsDEdx;
vector<Double_t> Eta;


void Init();
void Make();
void FillPlots(int state, int configuration );
TFile *CreateOutputTree(const string& out);
bool ConnectInput(const string& in, int fileId);

//_____________________________________________________________________________
int main(int argc, char** argv) {
  //open output file
  outfile = CreateOutputTree("AnalysisOutput.root"); 
  if(!outfile) {cout << "Can not open output file." << endl; return -1;}

  Init();
  int fileId = 0;
  const string& input = argv[1];
  if( input.find(".root") == string::npos )
    fileId = atoi(argv[2]);

  if(!ConnectInput(argv[1], fileId)){
  //if(!ConnectInput("/gpfs01/star/pwg/truhlar/Final/CPtrig/merge_files/StUPCRP_production_0000.root")){
    cout << "No input." << endl; 
    return 1;
  }
  //ask for number of events
  Long64_t nev = upcTree->GetEntries();
  cout<<"Proccesing "<<nev<<" events"<<endl;
  //event loop
  //nev = 1000;
  for(Long64_t iev=0; iev<nev; ++iev) { //get the event
    upcTree->GetEntry(iev); 
    Make();
  } 

  //close the outputs
  outfile->Write();
  outfile->Close();
  cout<<"Ending Analysis... GOOD BYE!"<<endl;
  return 0;
}//main

void Init(){
  hAnalysisFlow = new TH1I("AnalysisFlow", "CutsFlow", kMaxCount-1, 1, kMaxCount);
  //hAnalysisFlow = new TH1F("AnalysisFlow", "AnalysisFlow", 10, -0.5, 9.5);
  for(int tb=1; tb<kMaxCount; ++tb) hAnalysisFlow->GetXaxis()->SetBinLabel(tb, summaryLabels[tb-1]);

  hTriggerBits = new TH1F("TriggerBits", "TriggerBits", nTriggers, -0.5, 20.5);
  for(int tb=0; tb<nTriggers; ++tb) hTriggerBits->GetXaxis()->SetBinLabel(tb+1, triggerName[tb]);

  hConfiguration = new TH1F("Configuration", "Track Configuration", 4, -0.5, 3.5);
  for(int tb=0; tb<4; ++tb) hConfiguration->GetXaxis()->SetBinLabel(tb+1, configLabels[tb]);

  for(int i=0; i<nBranches; ++i)
    hNumberTrackPerBranch[i] = new TH1F("NumberTracksPerBranch,"+branchName[i],"Number of tracks in branch "+branchName[i], 8, -0.5, 7.5);

  hNumberTrack = new TH1F("NumberTracks", "Number of Tracks in RP", 40, -0.5, 39.5); 

  for(int i=0; i<12;++i){
    if(i==0){
      outfile->mkdir("All")->cd();
    }else if(i==4){
      outfile->cd();
      outfile->mkdir("Inelastic")->cd();
    }else if(i==8){
      outfile->cd();
      outfile->mkdir("Elastic")->cd();
    }
    
    hMissingPt[i] = new TH1F( "MissingPt_"+systemState[i%4]+"_"+systemID[i/4], "p^{miss}_{T} [GeV/c]", 200, 0, 2 );
    hInvMass[i] = new TH1F( "InvMass_"+systemState[i%4]+"_"+systemID[i/4],"M_{#pi#pi} [GeV/c^{2}]", 100, 0, 5 );
  }
  outfile->cd();
}

void Make(){
  hAnalysisFlow->Fill(kCPtrig);
  for(int var = 0; var < nTriggers; ++var){
    if(upcEvt->isTrigger(triggerID[var])){
      hTriggerBits->Fill(var);
      //triggerBits = var; // not 100% correct, some events have more than 1 trigger
    }
  }

  nTracks = (Int_t) rpEvt->getNumberOfTracks();

  // Vector below will be filled with indices of good-quality tracks
  vector<int> rpTrackIdVec_perBranch[nBranches];
  vector<int> rpTrackIdVec_perSide[nSides];
  int numberOfTracks = 0;
  int numberOfTracksPerBranch[nBranches] = {0, 0, 0, 0};

  // Loop over all tracks reconstructed in Roman Pots  
  for(int k=0; k<rpEvt->getNumberOfTracks(); ++k){
  // Get pointer to k-th track in Roman Pot data collection
    StUPCRpsTrack *trk = rpEvt->getTrack(k);
    trk->setEvent(rpEvt);
  // Get ID of a branch in which this k-th track was reconstructed
    int j = trk->branch();
    int side = j<2 ? E : W;

    ++numberOfTracks;
    ++numberOfTracksPerBranch[j];

  // If track is global (made of track-points in 1st and 2nd station)
  // and all 8 planes were used in reconstruction - store its ID
    if( trk->type()==StUPCRpsTrack::rpsGlobal && trk->planesUsed()==8) rpTrackIdVec_perBranch[j].push_back( k );
  // a bit looser selection
    if( (trk->getTrackPoint(0) ? trk->getTrackPoint(0)->planesUsed()>=3 : true) &&
      (trk->getTrackPoint(1) ? trk->getTrackPoint(1)->planesUsed()>=3 : true) ) rpTrackIdVec_perSide[side].push_back( k );
  }

  hNumberTrack->Fill(numberOfTracks);
  for(int i=0; i<nBranches; ++i) 
    hNumberTrackPerBranch[i]->Fill(numberOfTracksPerBranch[i]);

// Loop over arms - check if have good-quality tracks, selecting branch combination
  for(int i=0; i<nConfiguration; ++i){ 
// Define IDs of branches based on known ID of the arm
    int branch[nSides];
    switch(i) {
        case EUD :{
          branch[E] = EU;
          branch[W] = WD;
        }break;
        case EDU :{
          branch[E] = ED;
          branch[W] = WU;
        }break; 
        case IUU :{
          branch[E] = EU;
          branch[W] = WU;
        }break; 
        case IDD :{
          branch[E] = ED;
          branch[W] = WD;
        }break;        
    }

// If exactly one good-quality track in each branch in the arm and there is no track in the other RP do some staff
    if( rpTrackIdVec_perBranch[ branch[E] ].size()==1 
      && rpTrackIdVec_perBranch[ branch[W] ].size()==1
      && rpTrackIdVec_perSide[E].size()==1 && rpTrackIdVec_perSide[W].size()==1){
  // Get pointers to good-quality tracks
      if(i==EUD || i==EDU){
        hAnalysisFlow->Fill(kEl);
        elastic = true;
      }else{
        hAnalysisFlow->Fill(kInEl);
        elastic = false;
      }
      hConfiguration->Fill(i);

      totalCharge = missingPt = invMass = 0;
      nTofTrks = nSigPPion = 0;
      dEdx.clear();
      momentum.clear();
      TOFtime.clear();
      TOFlength.clear();
      charge.clear();
      nSigmaTPCPion.clear();
      nSigmaTPCKaon.clear();
      nSigmaTPCProton.clear();
      vexterId.clear();
      vertexesZ.clear();
      DcaXY.clear();
      DcaZ.clear();
      Nhits.clear();
      NhitsDEdx.clear();
      Eta.clear();
      double nSigmaPairPion2 = 0;
      double nSigmaPairKaon2 = 0;
      double nSigmaPairProton2 = 0;
      TLorentzVector centralTracksTotalFourMomentum;

      xCorrelationsWest = rpEvt->getTrack(rpTrackIdVec_perSide[West][0])->pVec().X();
      xCorrelationsEast = rpEvt->getTrack(rpTrackIdVec_perSide[East][0])->pVec().X();
      yCorrelationsWest = rpEvt->getTrack(rpTrackIdVec_perSide[West][0])->pVec().Y();
      yCorrelationsEast = rpEvt->getTrack(rpTrackIdVec_perSide[East][0])->pVec().Y();

    // loop over all TPC tracks
      for(int j=0; j<upcEvt->getNumberOfTracks(); ++j){
    // get TPC track object
        const StUPCTrack* trk = upcEvt->getTrack(j);
        TLorentzVector trkLVector;
        trk->getLorentzVector(trkLVector,pionMass);

        if( !trk->getFlag(StUPCTrack::kPrimary) || !trk->getFlag(StUPCTrack::kTof) ) continue;
        // read basic information about the track     
 
        dEdx.push_back(trk->getDEdxSignal()*1000000);
        momentum.push_back(trkLVector.P());
        charge.push_back(trk->getCharge());
        TOFtime.push_back(trk->getTofTime());
        TOFlength.push_back(trk->getTofPathLength());

        DcaXY.push_back(trk->getDcaXY());
        DcaZ.push_back(trk->getDcaZ());
        Nhits.push_back(trk->getNhits());
        NhitsDEdx.push_back(trk->getNhitsDEdx());
        Eta.push_back(trk->getEta());

        nSigmaTPCPion.push_back(trk->getNSigmasTPCPion());
        nSigmaTPCKaon.push_back(trk->getNSigmasTPCKaon());
        nSigmaTPCProton.push_back(trk->getNSigmasTPCProton());

        vexterId.push_back(trk->getVertexId());
        vertexesZ.push_back(trk->getVertex()->getPosZ());

        totalCharge += static_cast<int>( trk->getCharge() );
        centralTracksTotalFourMomentum += trkLVector;

        nSigmaPairPion2 += pow(trk->getNSigmasTPCPion(),2);
        nSigmaPairKaon2 += pow(trk->getNSigmasTPCKaon(),2);
        nSigmaPairProton2 += pow(trk->getNSigmasTPCProton(),2);

        ++nTofTrks;

      }

      missingPt = (centralTracksTotalFourMomentum.Vect() + rpEvt->getTrack( rpTrackIdVec_perSide[E][0] )->pVec() + rpEvt->getTrack( rpTrackIdVec_perSide[W][0] )->pVec() ).Pt();
      invMass = centralTracksTotalFourMomentum.M();
      
      FillPlots(0,i);
      if( nTofTrks!=2)
        return;
      hAnalysisFlow->Fill(kTOF2t);

      if( vexterId[0] != vexterId[1])
        return;
      hAnalysisFlow->Fill(kSameVrtx);

      FillPlots(1,i);

      if(totalCharge != 0) 
        return;
      hAnalysisFlow->Fill(kTotCH0);

      FillPlots(2,i);      

      nSigPPion = sqrt(nSigmaPairPion2);
      nSigPKaon= sqrt(nSigmaPairKaon2);
      nSigPProton = sqrt(nSigmaPairProton2);

      if( missingPt > 0.1 )
        return;
      hAnalysisFlow->Fill(kMissPt);

      FillPlots(3,i);
      
      dEdx1 = dEdx[0];
      dEdx2 = dEdx[1];
      momentum1 = momentum[0];
      momentum2 = momentum[1];
      charge1 = charge[0];
      charge2 = charge[1];
      TOFtime1 = TOFtime[0];
      TOFtime2 = TOFtime[1];
      TOFlength1 = TOFlength[0];
      TOFlength2 = TOFlength[1];
      vertexZ = vertexesZ[0];
      DcaXY1 = DcaXY[0];
      DcaXY2 = DcaXY[1];
      DcaZ1 = DcaZ[0];
      DcaZ2 = DcaZ[1];
      Nhits1 = Nhits[0];
      Nhits2 = Nhits[1];
      NhitsDEdx1 = NhitsDEdx[0];
      NhitsDEdx2 = NhitsDEdx[1];
      Eta1 = Eta[0];
      Eta2 = Eta[1];

      recTree->Fill();

    }
  } // end of loop over arms

}

//_____________________________________________________________________________
TFile *CreateOutputTree(const string& out) {

  TFile *outputFile = TFile::Open(out.c_str(), "recreate");
  if(!outputFile) return 0x0;

  //standard reconstructed tree
  recTree = new TTree("recTree", "recTree");

  recTree ->Branch("nSigPPion", &nSigPPion, "nSigPPion/D");
  recTree ->Branch("nSigPKaon", &nSigPKaon, "nSigPKaon/D");
  recTree ->Branch("nSigPProton", &nSigPProton, "nSigPProton/D");
  recTree ->Branch("missingPt", &missingPt, "missingPt/D");
  recTree ->Branch("invMass", &invMass, "invMass/D");
  recTree ->Branch("dEdx1", &dEdx1, "dEdx1/D");
  recTree ->Branch("dEdx2", &dEdx2, "dEdx2/D");
  recTree ->Branch("momentum1", &momentum1, "momentum1/D");
  recTree ->Branch("momentum2", &momentum2, "momentum2/D");
  recTree ->Branch("charge1", &charge1, "charge1/D");
  recTree ->Branch("charge2", &charge2, "charge2/D");
  recTree ->Branch("TOFtime1", &TOFtime1, "TOFtime1/D");
  recTree ->Branch("TOFtime2", &TOFtime2, "TOFtime2/D");
  recTree ->Branch("TOFlength1", &TOFlength1, "TOFlength1/D");
  recTree ->Branch("TOFlength2", &TOFlength2, "TOFlength2/D");
  recTree ->Branch("deltaDeltaTOF", &deltaDeltaTOF, "deltaDeltaTOF/D");
  recTree ->Branch("mSquared", &mSquared, "mSquared/D"); 
  recTree ->Branch("deltaTOF", &deltaTOF, "deltaTOF/D");
  recTree ->Branch("elastic", &elastic, "elastic/B");
  recTree ->Branch("vertexZ", &vertexZ, "vertexZ/D");
  recTree ->Branch("DcaXY1", &DcaXY1, "DcaXY1/D");
  recTree ->Branch("DcaXY2", &DcaXY2, "DcaXY2/D");
  recTree ->Branch("DcaZ1", &DcaZ1, "DcaZ1/D");
  recTree ->Branch("DcaZ2", &DcaZ2, "DcaZ2/D");
  recTree ->Branch("Nhits1", &Nhits1, "Nhits1/D");
  recTree ->Branch("Nhits2", &Nhits2, "Nhits2/D");
  recTree ->Branch("NhitsDEdx1", &NhitsDEdx1, "NhitsDEdx1/D");
  recTree ->Branch("NhitsDEdx2", &NhitsDEdx2, "NhitsDEdx2/D");
  recTree ->Branch("Eta1", &Eta1, "Eta1/D");
  recTree ->Branch("Eta2", &Eta2, "Eta2/D");
  recTree ->Branch("xCorrelationsWest", &xCorrelationsWest, "xCorrelationsWest/D");
  recTree ->Branch("xCorrelationsEast", &xCorrelationsEast, "xCorrelationsEast/D");
  recTree ->Branch("yCorrelationsWest", &yCorrelationsWest, "yCorrelationsWest/D");
  recTree ->Branch("yCorrelationsEast", &yCorrelationsEast, "yCorrelationsEast/D");

  //recTree ->Branch("", &, "/D");

  return outputFile;

}//CreateOutputTree

bool ConnectInput(const string& in, int fileId) {
  //input from file or chain
  upcTree = 0x0;
  if( in.find(".root") != string::npos ) {
    cout << "Input from root file" << endl;
    infile = TFile::Open(in.c_str(), "read");
    if(!infile) return false;
    upcTree = dynamic_cast<TTree*>( infile->Get("mUPCTree") );
  } else {
    cout << "Input from chain" << endl;
    upcChain = new TChain("mUPCTree");
    ifstream instr(in.c_str());
    string line;
    int lineId=0;
    while(getline(instr, line)) {
      if(fileId==lineId || fileId== -1){
        upcChain->AddFile(line.c_str());
      }
      lineId++;
    }
    instr.close();
    upcTree = dynamic_cast<TTree*>( upcChain );
  }

  if(!upcTree) return false;


  upcTree->SetBranchAddress("mUPCEvent", &upcEvt);
  upcTree->SetBranchAddress("mRPEvent", &rpEvt); 

  return true;

}//ConnectInput

void FillPlots(int state, int configuration){
  for(int tmp =0; tmp<3;++tmp){
    if(tmp == 1 && (configuration!=IUU && configuration!=IDD))
      continue;
    if(tmp == 2 && (configuration!=EUD && configuration!=EDU))
      continue;

    hMissingPt[4*tmp+state]->Fill(missingPt); 
    hInvMass[4*tmp+state]->Fill(invMass);
    
  }
}

