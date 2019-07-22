#include "TStyle.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TPaveText.h"
#include "TLegend.h"
#include "TMath.h"
#include "TLatex.h"
#include "TF1.h"
#include "TFile.h"
#include "TColor.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include <iostream>
#include <ctime>
#include <cstdio>
#include <fstream>



const double speedOfLight = 299792458; // m/s
const double pionMass = 0.13957; // GeV /c^2

TString particleID[3] = { TString("Pion"), TString("Kaon"), TString("Proton")};
TString doubleParticleID[3] = { TString("PionVsKaon"), TString("PionsVsProton"), TString("KaonVsProton")};

TTree* recTree;
TFile* infile;

TH2D* hnSigma[3];
TH2D* hnSigmaVsM2[3];
TH2D* hChMomVsnSigma[3];

TH2D* hdEdx;
TH1D* hnM2;
TH1D* hdeltaDeltaTOF; 
TH1D* hdeltaTOF; 
TH1D* hdeltaTOFexpected;

Double_t nSigPPion,nSigPKaon,nSigPProton;
Double_t missingPt; 
Double_t invMass;
Double_t deltaTOF, deltaDeltaTOF, mSquared;
Double_t dEdx1, dEdx2;
Double_t momentum1, momentum2;
Double_t charge1, charge2;
Double_t TOFlength1, TOFlength2;
Double_t TOFtime1, TOFtime2;
Char_t elastic;

Int_t counter;

TCanvas *nM2;
TCanvas *cdEdx;

TCanvas *cDeltaTOF;
TCanvas *cDeltaDeltaTOF; 

TCanvas *nSigmaParticleVsOther[3];
TCanvas *nSigmaParticleVsM2[3];
TCanvas *cChMomVsnSigma[3];

void Init();
void ConnectInput();
void Make();
void Plot();

void PID(TString input = "StRP_production_0000.root") {
	cout<<"Opening input file: "<<input<<endl;
	infile = TFile::Open(input, "read");
	if (!infile){
      cout<<"Error: cannot open "<<input<<endl;
      return;
   }

	Init(); // Preparing histograms 
	ConnectInput(); // Connecting input

////////////// Making histograms

	Long64_t nev = recTree->GetEntries();
	cout<<"Proccesing "<<nev<<" events"<<endl;
	//event loop
	//nev = 1000;
	counter=0;
	for(Long64_t iev=0; iev<nev; ++iev) { //get the event
		recTree->GetEntry(iev); 
		Make();
	} 
	cout<<"counter: "<<counter<<endl;
	Plot();
//////////////////// Output

	cout<<"Creating output file: "<<"output.root"<<endl;
	TFile *fOut = new TFile("output.root","recreate");

	nM2->Write("nM2");
	nM2->SaveAs("nM2.png");
	nM2->Close();

	cdEdx->Write("cdEdx");
	cdEdx->SaveAs("cdEdx.png");
	cdEdx->Close();

	cDeltaTOF->Write("cDeltaTOF");
	cDeltaTOF->SaveAs("cDeltaTOF.png");
	cDeltaTOF->Close();

	cDeltaDeltaTOF->Write("cDeltaDeltaTOF");
	cDeltaDeltaTOF->SaveAs("cDeltaDeltaTOF.png");
	cDeltaDeltaTOF->Close();




	for(int i = 0; i < 3; ++i){

		nSigmaParticleVsM2[i]->Write("nSigma" + particleID[i] + "VsM2");
		nSigmaParticleVsM2[i]->SaveAs("nSigma" + particleID[i] + "VsM2.png");
		nSigmaParticleVsM2[i]->Close();

		int i1 = i;
		int i2 = i+1;
		if(i==2){
			i1 = 0;
			i2 = 2;
		}
		nSigmaParticleVsOther[i]->Write("nSigma" + particleID[i1] + "Vs" + particleID[i2]);
		nSigmaParticleVsOther[i]->SaveAs("nSigma" + particleID[i1] + "Vs" + particleID[i2] + ".png");
		nSigmaParticleVsOther[i]->Close();

		cChMomVsnSigma[i]->Write("cChMomVsnSigma"+particleID[i]);
		cChMomVsnSigma[i]->SaveAs("cChMomVsnSigma"+particleID[i] + ".png");
		cChMomVsnSigma[i]->Close();

	}

   fOut->Close();

}

void Make(){
	hnSigma[0]->Fill(nSigPPion,nSigPKaon);
	hnSigma[1]->Fill(nSigPKaon,nSigPProton);
	hnSigma[2]->Fill(nSigPPion,nSigPProton);

	hdEdx->Fill(charge1*momentum1,dEdx1);
	hdEdx->Fill(charge2*momentum2,dEdx2);

	hChMomVsnSigma[0]->Fill(charge1*momentum1,nSigPPion);
	hChMomVsnSigma[1]->Fill(charge1*momentum1,nSigPKaon);
	hChMomVsnSigma[2]->Fill(charge1*momentum1,nSigPProton);

	hChMomVsnSigma[0]->Fill(charge2*momentum2,nSigPPion);
	hChMomVsnSigma[1]->Fill(charge2*momentum2,nSigPKaon);
	hChMomVsnSigma[2]->Fill(charge2*momentum2,nSigPProton);

// Calculate mSquared
	double speedOfLight2 = speedOfLight*speedOfLight;
	double speedOfLight4 = speedOfLight2*speedOfLight2;
	double length1Squared = TOFlength1*TOFlength1/(100*100*speedOfLight2); // convert from cm to m
	double length2Squared = TOFlength2*TOFlength2/(100*100*speedOfLight2); // convert from cm to m
	double deltaTime2 = (TOFtime2 - TOFtime1)*(TOFtime2 - TOFtime1)/(pow(10.0,18.0)); // convert from ns to s
	double deltaTime4 = deltaTime2*deltaTime2;
	double oneOverMomentum1sq = 1/(momentum1*momentum1);
	double oneOverMomentum2sq = 1/(momentum2*momentum2);
	double cEq = -2*length1Squared*length2Squared + speedOfLight4*deltaTime4 + length2Squared*length2Squared + length1Squared*length1Squared -2*speedOfLight2*deltaTime2*(length2Squared + length1Squared);
	double bEq = -2*length1Squared*length2Squared*speedOfLight2*(oneOverMomentum1sq + oneOverMomentum2sq) + 2*length1Squared*length1Squared*speedOfLight2*oneOverMomentum1sq + 2*length2Squared*length2Squared*speedOfLight2*oneOverMomentum2sq -2*speedOfLight4*deltaTime2*(length1Squared*oneOverMomentum1sq + length2Squared*oneOverMomentum2sq);
	double aEq = -2*length1Squared*length2Squared*speedOfLight4*oneOverMomentum1sq*oneOverMomentum2sq + speedOfLight4*(length1Squared*length1Squared*oneOverMomentum1sq*oneOverMomentum1sq + length2Squared*length2Squared*oneOverMomentum2sq*oneOverMomentum2sq);
	mSquared = (-bEq + sqrt(bEq*bEq-4*aEq*cEq)) / (2*aEq);

	if(TOFtime1 < 0 || TOFtime2 < 0 || TOFlength1 < 0 || TOFlength2 < 0)
		mSquared = -1.0;

//////////////////////

	hnM2->Fill(mSquared);
	hnSigmaVsM2[0]->Fill(nSigPPion,mSquared);
	hnSigmaVsM2[1]->Fill(nSigPKaon,mSquared);
	hnSigmaVsM2[2]->Fill(nSigPProton,mSquared);

/////////////// Calculate deltaDeltaTOF
	double expectedTime1 = (TOFlength1/ speedOfLight) * sqrt(1 + pow(pionMass/ momentum1, 2)) * pow(10,7); // in ns
	double expectedTime2 = (TOFlength2 / speedOfLight) * sqrt(1 + pow(pionMass/ momentum2,2)) * pow(10,7); // in ns
	double deltaTOF = TOFtime2 - TOFtime1;
	double deltaTOFexpected = expectedTime2 - expectedTime1;
	deltaDeltaTOF = deltaTOF - deltaTOFexpected;


   if(TOFtime1 < 0 || TOFtime2 < 0 || TOFlength1 < 0 || TOFlength2 < 0){
		deltaDeltaTOF = -999;
		deltaTOF = -999;   
		deltaTOFexpected = -999;  
   }
////////////////
   //cout<<deltaTOF <<" "<<deltaTOFexpected <<"  "<< deltaDeltaTOF<<endl;
	hdeltaDeltaTOF -> Fill(deltaDeltaTOF);
	hdeltaTOF -> Fill(deltaTOF);
	hdeltaTOFexpected -> Fill(deltaTOFexpected);

}


void Plot(){

	for(int i = 0; i < 3; ++i){

		int i1 = i;
		int i2 = i+1;
		if(i==2){
			i1 = 0;
			i2 = 2;
		}

		nSigmaParticleVsOther[i] = new TCanvas("nSigma" + particleID[i1] + "Vs" + particleID[i2],"nSigma" + particleID[i1] + "Vs" + particleID[i2],1200,800);
		gPad->SetMargin(0.1,0.11,0.14,0.02);
		gStyle->SetPalette(1);
		hnSigma[i]->SetStats(0);
		hnSigma[i]->SetTitle(" ; n#sigma^{pair}_{" + particleID[i1] + "}; n#sigma^{pair}_{" + particleID[i2] + "}");
		hnSigma[i]->GetXaxis()->SetTitleFont(42);
		hnSigma[i]->GetYaxis()->SetTitleFont(42);
		hnSigma[i]->GetXaxis()->SetLabelFont(42);
		hnSigma[i]->GetYaxis()->SetLabelFont(42);
		hnSigma[i]->GetXaxis()->SetTitleSize(.055);
		hnSigma[i]->GetYaxis()->SetTitleSize(.055);
		hnSigma[i]->GetXaxis()->SetTitleOffset(1);
		hnSigma[i]->GetYaxis()->SetTitleOffset(0.8);
		hnSigma[i]->Draw("colz");

		TPaveText *textPub = new TPaveText(0.68,0.75,0.75,0.97,"brNDC");
		textPub -> SetTextSize(0.055);
		textPub -> SetTextFont(42);
		textPub -> SetFillColor(0);	
		textPub -> AddText("p + p #rightarrow p + #pi^{+} + #pi^{-} + p");
		textPub -> AddText("#sqrt{s} = 510 GeV");
		textPub -> Draw("same");

		nSigmaParticleVsOther[i]->Update();

		nSigmaParticleVsM2[i] = new TCanvas("nSigma" + particleID[i] + "VsM2","nSigma" + particleID[i] + "VsM2",1200,800);
		gPad->SetMargin(0.1,0.11,0.14,0.02);
		gStyle->SetPalette(1);
		hnSigmaVsM2[i]->SetStats(0);
		hnSigmaVsM2[i]->SetTitle(" ; n#sigma^{pair}_{" + particleID[i] + "}; m^{2}_{TOF} [GeV^{2}/c^{4}]");
		hnSigmaVsM2[i]->GetXaxis()->SetTitleFont(42);
		hnSigmaVsM2[i]->GetYaxis()->SetTitleFont(42);
		hnSigmaVsM2[i]->GetXaxis()->SetLabelFont(42);
		hnSigmaVsM2[i]->GetYaxis()->SetLabelFont(42);
		hnSigmaVsM2[i]->GetXaxis()->SetTitleSize(.055);
		hnSigmaVsM2[i]->GetYaxis()->SetTitleSize(.055);
		hnSigmaVsM2[i]->GetXaxis()->SetTitleOffset(1);
		hnSigmaVsM2[i]->GetYaxis()->SetTitleOffset(0.8);
		hnSigmaVsM2[i]->Draw("colz");

		TPaveText *textPub = new TPaveText(0.68,0.75,0.75,0.97,"brNDC");
		textPub -> SetTextSize(0.055);
		textPub -> SetTextFont(42);
		textPub -> SetFillColor(0);	
		textPub -> AddText("p + p #rightarrow p + #pi^{+} + #pi^{-} + p");
		textPub -> AddText("#sqrt{s} = 510 GeV");
		textPub -> Draw("same");

		nSigmaParticleVsM2[i]->Update();

		cChMomVsnSigma[i] = new TCanvas("cChMomVsnSigma"+particleID[i],"cChMomVsnSigma"+particleID[i],1200,800);
		gPad->SetMargin(0.1,0.11,0.14,0.02);
		gStyle->SetPalette(1);
		hChMomVsnSigma[i]->SetStats(0);
		hChMomVsnSigma[i]->SetTitle(" ; #frac{q}{e} #times p [GeV/c] ; n#sigma^{pair}_{" + particleID[i] + "}");
		hChMomVsnSigma[i]->GetXaxis()->SetTitleFont(42);
		hChMomVsnSigma[i]->GetYaxis()->SetTitleFont(42);
		hChMomVsnSigma[i]->GetXaxis()->SetLabelFont(42);
		hChMomVsnSigma[i]->GetYaxis()->SetLabelFont(42);
		hChMomVsnSigma[i]->GetXaxis()->SetTitleSize(.055);
		hChMomVsnSigma[i]->GetYaxis()->SetTitleSize(.055);
		hChMomVsnSigma[i]->GetXaxis()->SetTitleOffset(1);
		hChMomVsnSigma[i]->GetYaxis()->SetTitleOffset(0.8);
		hChMomVsnSigma[i]->Draw("colz");

		TPaveText *textPub = new TPaveText(0.23,0.75,0.3,0.97,"brNDC");
		textPub -> SetTextSize(0.055);
		textPub -> SetTextFont(42);
		textPub -> SetFillColor(0);	
		textPub -> AddText("p + p #rightarrow p + #pi^{+} + #pi^{-} + p");
		textPub -> AddText("#sqrt{s} = 510 GeV");
		textPub -> Draw("same");

		cChMomVsnSigma[i]->Update();
	}


	cdEdx = new TCanvas("cdEdx","cdEdx",1200,800);
	gPad->SetMargin(0.1,0.11,0.14,0.02);
	gStyle->SetPalette(1);
	hdEdx->SetStats(0);
	hdEdx->SetTitle(" ; #frac{q}{e} #times p [GeV/c] ;dE/dx [keV/cm]");
	hdEdx->GetXaxis()->SetTitleFont(42);
	hdEdx->GetYaxis()->SetTitleFont(42);
	hdEdx->GetXaxis()->SetLabelFont(42);
	hdEdx->GetYaxis()->SetLabelFont(42);
	hdEdx->GetXaxis()->SetTitleSize(.055);
	hdEdx->GetYaxis()->SetTitleSize(.055);
	hdEdx->GetXaxis()->SetTitleOffset(1);
	hdEdx->GetYaxis()->SetTitleOffset(0.8);
	hdEdx->Draw("colz");

	TPaveText *textPub = new TPaveText(0.24,0.75,0.31,0.97,"brNDC");
	textPub -> SetTextSize(0.055);
	textPub -> SetTextFont(42);
	textPub -> SetFillColor(0);	
	textPub -> AddText("p + p #rightarrow p + #pi^{+} + #pi^{-} + p");
	textPub -> AddText("#sqrt{s} = 510 GeV");
	textPub -> Draw("same");

	cdEdx->Update();

	nM2 = new TCanvas("nM2","nM2",1200,800);
	gPad->SetMargin(0.1,0.11,0.14,0.02);
	gStyle->SetPalette(1);
	hnM2->SetStats(0);
	hnM2->SetTitle(" ; m^{2}_{TOF} [GeV^{2}/c^{4}]; Number of events");
	hnM2->GetXaxis()->SetTitleFont(42);
	hnM2->GetYaxis()->SetTitleFont(42);
	hnM2->GetXaxis()->SetLabelFont(42);
	hnM2->GetYaxis()->SetLabelFont(42);
	hnM2->GetXaxis()->SetTitleSize(.055);
	hnM2->GetYaxis()->SetTitleSize(.055);
	hnM2->GetXaxis()->SetTitleOffset(1);
	hnM2->GetYaxis()->SetTitleOffset(0.8);
	hnM2->Draw("colz");

	TPaveText *textPub = new TPaveText(0.68,0.75,0.75,0.97,"brNDC");
	textPub -> SetTextSize(0.055);
	textPub -> SetTextFont(42);
	textPub -> SetFillColor(0);	
	textPub -> AddText("p + p #rightarrow p + #pi^{+} + #pi^{-} + p");
	textPub -> AddText("#sqrt{s} = 510 GeV");
	textPub -> Draw("same");

	nM2->Update();

	cDeltaDeltaTOF = new TCanvas("DeltaDeltaTOF","DeltaDeltaTOF",1200,800);
	gPad->SetMargin(0.1,0.11,0.14,0.02);
	gStyle->SetPalette(1);
	hdeltaDeltaTOF->SetStats(0);
	hdeltaDeltaTOF->SetTitle(" ; #Delta #Delta TOF [ns]; Number of events");
	hdeltaDeltaTOF->GetXaxis()->SetTitleFont(42);
	hdeltaDeltaTOF->GetYaxis()->SetTitleFont(42);
	hdeltaDeltaTOF->GetXaxis()->SetLabelFont(42);
	hdeltaDeltaTOF->GetYaxis()->SetLabelFont(42);
	hdeltaDeltaTOF->GetXaxis()->SetTitleSize(.055);
	hdeltaDeltaTOF->GetYaxis()->SetTitleSize(.055);
	hdeltaDeltaTOF->GetXaxis()->SetTitleOffset(1);
	hdeltaDeltaTOF->GetYaxis()->SetTitleOffset(0.8);
	hdeltaDeltaTOF->Draw("colz");

	TPaveText *textPub = new TPaveText(0.68,0.75,0.75,0.97,"brNDC");
	textPub -> SetTextSize(0.055);
	textPub -> SetTextFont(42);
	textPub -> SetFillColor(0);	
	textPub -> AddText("p + p #rightarrow p + #pi^{+} + #pi^{-} + p");
	textPub -> AddText("#sqrt{s} = 510 GeV");
	textPub -> Draw("same");

	cDeltaDeltaTOF->Update();

	cDeltaTOF = new TCanvas("DeltaTOF","DeltaTOF",1200,800);
	gPad->SetMargin(0.1,0.11,0.14,0.02);
	gStyle->SetPalette(1);
	hdeltaTOF->SetStats(0);
	hdeltaTOF->SetTitle(" ; #Delta TOF [ns]; Number of events");
	hdeltaTOF->GetXaxis()->SetTitleFont(42);
	hdeltaTOF->GetYaxis()->SetTitleFont(42);
	hdeltaTOF->GetXaxis()->SetLabelFont(42);
	hdeltaTOF->GetYaxis()->SetLabelFont(42);
	hdeltaTOF->GetXaxis()->SetTitleSize(.055);
	hdeltaTOF->GetYaxis()->SetTitleSize(.055);
	hdeltaTOF->GetXaxis()->SetTitleOffset(1);
	hdeltaTOF->GetYaxis()->SetTitleOffset(0.8);
	hdeltaTOF->Draw("same");
	hdeltaTOFexpected->SetLineColor(2);
	hdeltaTOFexpected->Draw("same");


	TPaveText *textPub = new TPaveText(0.68,0.75,0.75,0.97,"brNDC");
	textPub -> SetTextSize(0.055);
	textPub -> SetTextFont(42);
	textPub -> SetFillColor(0);	
	textPub -> AddText("p + p #rightarrow p + #pi^{+} + #pi^{-} + p");
	textPub -> AddText("#sqrt{s} = 510 GeV");
	textPub -> Draw("same");


	TLegend *legend = new TLegend(0.62,0.6,0.85,0.75,"","brNDC");
	legend -> AddEntry(hdeltaTOF, "Data", "l");
	legend -> AddEntry(hdeltaTOFexpected, "Pion assumption", "fl");
	legend -> SetTextFont(42);
	legend -> SetFillStyle(0);
	legend -> SetLineColor(0);
	legend -> SetTextSize(0.04);
	legend -> Draw("same");

	cDeltaTOF->Update();

}

void Init(){

	for(int i = 0; i <3; ++i)
		hnSigma[i] = new TH2D("nSigma"+doubleParticleID[i],"nSigma "+doubleParticleID[i],100,0,50,100,0,50 );

	for(int i = 0; i <3; ++i)
		hnSigmaVsM2[i] = new TH2D("nSigma"+particleID[i]+"VsM2","nSigma "+particleID[i]+" Vs M2",100,0,50,100,0,1 );

	for(int i = 0; i <3; ++i)
		hChMomVsnSigma[i] = new TH2D("ChMomVsnSigma"+particleID[i],"charge momentum Vs nSigma"+particleID[i],80,-4,4,100,0,50 );
	

	hdEdx = new TH2D("dEdx","dEdx",120,-3,3,80,0,10 );
	hnM2 = new TH1D("mSquared", "mSquared", 200, 0, 1.0);

	hdeltaDeltaTOF = new TH1D("deltaDeltaTOF", "deltaDeltaTOF", 1200, -10, 10);
	hdeltaTOF = new TH1D("deltaTOF", "deltaTOF", 200, -10, 10);
	hdeltaTOFexpected = new TH1D("deltaTOFexpected", "deltaTOFexpected", 200, -10, 10);
}

void ConnectInput(){
	recTree = dynamic_cast<TTree*>( infile->Get("recTree") );
	if (!recTree){
      cout<<"Error: cannot get recTree"<<endl;
      return;
   }

	recTree->SetBranchAddress("nSigPPion", &nSigPPion);
	recTree->SetBranchAddress("nSigPKaon", &nSigPKaon);
	recTree->SetBranchAddress("nSigPProton", &nSigPProton);
	recTree->SetBranchAddress("missingPt", &missingPt);
	recTree->SetBranchAddress("invMass", &invMass);
	recTree->SetBranchAddress("dEdx1", &dEdx1);
	recTree->SetBranchAddress("dEdx2", &dEdx2);
	recTree->SetBranchAddress("momentum1", &momentum1);
	recTree->SetBranchAddress("momentum2", &momentum2);
	recTree->SetBranchAddress("charge1", &charge1);
	recTree->SetBranchAddress("charge2", &charge2);
	recTree->SetBranchAddress("deltaDeltaTOF", &deltaDeltaTOF);
	recTree->SetBranchAddress("mSquared", &mSquared); 
	recTree->SetBranchAddress("deltaTOF", &deltaTOF);
	recTree->SetBranchAddress("TOFtime1", &TOFtime1);
	recTree->SetBranchAddress("TOFtime2", &TOFtime2);
	recTree->SetBranchAddress("TOFlength1", &TOFlength1);
	recTree->SetBranchAddress("TOFlength2", &TOFlength2);
	recTree->SetBranchAddress("elastic", &elastic);
}