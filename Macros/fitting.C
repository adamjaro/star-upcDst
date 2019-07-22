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

void fitting(TString input = "StRP_production_0000.root") {
	bool text = false;
	bool bckground = false;
    cout<<"Opening input file: "<<input<<endl;
    TFile* data = new TFile(input ,"r");
    if(bckground)
    	TFile* background = new TFile("outputBackground.root" ,"r");
////////////// Insert histograms

TH1D* hMissingPtTPC = (TH1D*)data -> Get("All/MissingPt_TPC2t_Combi");
TH1D* hMissingPtTOF = (TH1D*)data -> Get("All/MissingPt_TOF2trk_Combi");
TH1D* hMissingPtQ0 = (TH1D*)data -> Get("All/MissingPt_Q0_Combi");
TH1D* hMissingPtExc = (TH1D*)data -> Get("All/MissingPt_Excl_Combi");
TH1F* hCuts = (TH1F*)data -> Get("AnalysisFlow");

TH1D* hInvMassCombi = (TH1D*)data -> Get("All/InvMass_Excl_Combi");
if(bckground)
	TH1D* hInvMassBack = (TH1D*)background -> Get("All/InvMass_Excl_Combi");

TH1D* hInvMassIn = (TH1D*)data -> Get("Inelastic/InvMass_Excl_InEl");
if(bckground)
	TH1D* hInvMassBackIn = (TH1D*)background -> Get("Inelastic/InvMass_Excl_InEl");

TH1D* hInvMassEl = (TH1D*)data -> Get("Elastic/InvMass_Excl_El");
if(bckground)
	TH1D* hInvMassBackEl = (TH1D*)background -> Get("Elastic/InvMass_Excl_El");

const int Nbins = 1000;
Double_t binSize = 5/100;
double mm[Nbins],ym[Nbins],yme[Nbins],ym1[Nbins], mme[Nbins];
for (int ib=0; ib<Nbins; ib++) {
  mme[ib] = binSize/2;
  mm[ib]  = hInvMassCombi -> GetBinCenter(ib);
  ym[ib]  = hInvMassCombi -> GetBinContent(ib);
  yme[ib] = hInvMassCombi -> GetBinError(ib);
}
TGraphErrors *gm = new TGraphErrors(Nbins,mm,ym,mme  ,yme);
gm->SetMarkerStyle(20);
gm->SetMarkerSize(1);
gm->SetMarkerColor(4);
gm->SetLineColor(4);
gm->SetTitle("");

for (int ib=0; ib<Nbins; ib++) {
  mm[ib]  = hInvMassIn -> GetBinCenter(ib);
  ym[ib]  = hInvMassIn -> GetBinContent(ib);
  yme[ib] = hInvMassIn -> GetBinError(ib);
}
TGraphErrors *gIn = new TGraphErrors(Nbins,mm,ym,mme  ,yme);
gIn->SetMarkerStyle(20);
gIn->SetMarkerSize(1);
gIn->SetMarkerColor(4);
gIn->SetLineColor(4);
gIn->SetTitle("");

for (int ib=0; ib<Nbins; ib++) {
  mm[ib]  = hInvMassEl -> GetBinCenter(ib);
  ym[ib]  = hInvMassEl -> GetBinContent(ib);
  yme[ib] = hInvMassEl -> GetBinError(ib);
}
TGraphErrors *gEl = new TGraphErrors(Nbins,mm,ym,mme  ,yme);
gEl->SetMarkerStyle(20);
gEl->SetMarkerSize(1);
gEl->SetMarkerColor(4);
gEl->SetLineColor(4);
gEl->SetTitle("");

if(bckground){
	for (int ib=0; ib<Nbins; ib++) {
	  mm[ib]  = hInvMassBack -> GetBinCenter(ib);
	  ym[ib]  = hInvMassBack -> GetBinContent(ib);
	  yme[ib] = hInvMassBack -> GetBinError(ib);
	}
	TGraphErrors *gBack = new TGraphErrors(Nbins,mm,ym,mme  ,yme);
	gBack->SetMarkerStyle(33);
	gBack->SetMarkerSize(1);
	gBack->SetMarkerColor(2);
	gBack->SetLineColor(2);
	gBack->SetTitle("");

	for (int ib=0; ib<Nbins; ib++) {
	  mm[ib]  = hInvMassBackIn -> GetBinCenter(ib);
	  ym[ib]  = hInvMassBackIn -> GetBinContent(ib);
	  yme[ib] = hInvMassBackIn -> GetBinError(ib);
	}
	TGraphErrors *gBackIn = new TGraphErrors(Nbins,mm,ym,mme  ,yme);
	gBackIn->SetMarkerStyle(33);
	gBackIn->SetMarkerSize(1);
	gBackIn->SetMarkerColor(2);
	gBackIn->SetLineColor(2);
	gBackIn->SetTitle("");

	for (int ib=0; ib<Nbins; ib++) {
	  mm[ib]  = hInvMassBackEl -> GetBinCenter(ib);
	  ym[ib]  = hInvMassBackEl -> GetBinContent(ib);
	  yme[ib] = hInvMassBackEl -> GetBinError(ib);
	}
	TGraphErrors *gBackEl = new TGraphErrors(Nbins,mm,ym,mme  ,yme);
	gBackEl->SetMarkerStyle(33);
	gBackEl->SetMarkerSize(1);
	gBackEl->SetMarkerColor(2);
	gBackEl->SetLineColor(2);
	gBackEl->SetTitle("");
}
// //////////////////////////////////////////////////////////
    TCanvas *cCuts = new TCanvas("cCuts","cCuts",1200,800);
    gPad->SetMargin(0.1,0.02,0.15,0.02);
    gPad->SetLogy();

    hCuts->SetStats(0);
	 hCuts->SetTitle("; ; Counts");
	 hCuts->SetLineWidth(2);
    hCuts->GetXaxis()->SetBinLabel(1, "All");
    hCuts->GetXaxis()->SetBinLabel(8, "MissingPt < 0.1 GeV");
	 hCuts->GetXaxis()->SetLabelSize(.055);
    hCuts->GetYaxis()->SetTitleFont(42);
    hCuts->GetXaxis()->SetLabelFont(42);
    hCuts->GetYaxis()->SetLabelFont(42);
    hCuts->GetYaxis()->SetTitleSize(.055);
	 hCuts->GetYaxis()->SetTitleOffset(0.8);
    hCuts->Draw("same");


    TPaveText *textPub = new TPaveText(0.75,0.7,0.85,0.91,"brNDC");
    textPub -> SetTextSize(0.05);
    textPub -> SetFillColor(0);
    textPub -> SetTextFont(42);
    textPub -> AddText("p + p #rightarrow p + #pi^{+} + #pi^{-} + p");
    textPub -> AddText("#sqrt{s} = 510 GeV");
    textPub -> AddText("Cuts flow");
    textPub -> Draw("same");

    cCuts->Update();
////////////// 
	TCanvas *cMissingPt = new TCanvas("cMissingPt","cMissingPt",1200,800);
	gPad->SetMargin(0.9,0.02,0.14,0.02);
	gPad->SetLogy();
	hMissingPtTPC->SetStats(0);
	hMissingPtTPC->SetTitle(" ; p_{T}^{miss} [GeV/c];Number of events");
	hMissingPtTPC->GetXaxis()->SetTitleFont(42);
	hMissingPtTPC->GetYaxis()->SetTitleFont(42);
	hMissingPtTPC->GetXaxis()->SetLabelFont(42);
	hMissingPtTPC->GetYaxis()->SetLabelFont(42);
	hMissingPtTPC->SetMinimum(10);
	hMissingPtTPC->GetXaxis()->SetTitleSize(.055);
	hMissingPtTPC->GetYaxis()->SetTitleSize(.055);
	hMissingPtTPC->GetXaxis()->SetTitleOffset(1);
	hMissingPtTPC->GetYaxis()->SetTitleOffset(0.8);

	hMissingPtTPC->Draw("same");	
	hMissingPtQ0->SetFillColor(4);
	hMissingPtQ0->SetLineColor(4);
	hMissingPtQ0->SetFillStyle(1001);
	hMissingPtQ0->Draw("same");
	hMissingPtExc->SetFillColor(2);
	hMissingPtExc->SetFillStyle(3134);
	hMissingPtExc->Draw("same");
	cMissingPt->cd();


	TPaveText *textPub1 = new TPaveText(0.25,0.75,0.35,0.75,"brNDC");
	textPub1 -> SetTextSize(0.05);
	textPub1 -> SetFillColor(0);
	textPub1 -> SetTextFont(42);
	if(text)
		textPub1 -> AddText("Exclusive peak");
	textPub1 -> Draw("same");

	TPaveText *textPub = new TPaveText(0.75,0.85,0.85,0.96,"brNDC");
	textPub -> SetTextSize(0.05);
	textPub -> SetFillColor(0);
	textPub -> SetTextFont(42);
	textPub -> AddText("p + p #rightarrow p + #pi^{+} + #pi^{-} + p");
	textPub -> AddText("#sqrt{s} = 510 GeV");
	textPub -> Draw("same");

	TLegend *legend = new TLegend(0.72,0.6,0.85,0.85,"","brNDC");
	legend -> AddEntry(hMissingPtTPC, "2 TPC tracks", "l");
	legend -> AddEntry(hMissingPtQ0, "Total charge 0", "fl");
	legend -> AddEntry(hMissingPtExc, "Exclusive", "fl");
	legend -> SetTextFont(42);
	legend -> SetFillStyle(0);
	legend -> SetLineColor(0);
	legend -> SetTextSize(0.04);
	legend -> Draw("same");

	TLine *left = new TLine(0.1,0,0.1,1000);
	left->SetLineStyle(4);
	left->SetLineColor(2);
	left->SetLineWidth(4);
   left->Draw("same");
	cMissingPt->Update();

 ////////////////////////////

 
	TCanvas *cInvMass = new TCanvas("cInvMass","cInvMass",1200,800);
	gPad->SetMargin(0.9,0.02,0.14,0.02);

	gm->SetTitle(" ; Inv. mass m(#pi^{+}#pi^{-}) [GeV/c^{2}];Number of track pairs");
	gm->GetXaxis()->SetTitleFont(42);
	gm->GetYaxis()->SetTitleFont(42);
	gm->GetXaxis()->SetLabelFont(42);
	gm->GetYaxis()->SetLabelFont(42);
	gm->GetXaxis()->SetTitleSize(.055);
	gm->GetYaxis()->SetTitleSize(.055);
	gm->GetXaxis()->SetLabelSize(.045);
	gm->GetYaxis()->SetLabelSize(.045);
	gm->GetXaxis()->SetTitleOffset(1);
	gm->GetYaxis()->SetTitleOffset(0.8);
	gm->GetXaxis()->SetRangeUser(0,2.5);
	gm->Draw("apz");

	if(bckground){
		gBack->GetXaxis()->SetRangeUser(0,2.5);
		gBack->Draw("psame");
	}
	TPaveText *textPub1 = new TPaveText(0.45,0.7,0.55,0.8,"brNDC");
	textPub1 -> SetTextSize(0.05);
	textPub1 -> SetFillColor(0);
	textPub1 -> SetTextFont(42);
	if(text)
		textPub1 -> AddText("Drop at about 1 GeV");
	textPub1 -> Draw("same");

	TPaveText *textPub2 = new TPaveText(0.75,0.5,0.8,0.6,"brNDC");
	textPub2 -> SetTextSize(0.05);
	textPub2 -> SetFillColor(0);
	textPub2 -> SetTextFont(42);
	if(text)
		textPub2 -> AddText("Peak at about 1270 MeV");
	textPub2 -> Draw("same");

	TPaveText *textPub = new TPaveText(0.75,0.75,0.85,0.97,"brNDC");
	textPub -> SetTextSize(0.05);
	textPub -> SetFillColor(0);
	textPub -> SetTextFont(42);
	textPub -> AddText("p + p #rightarrow p + #pi^{+} + #pi^{-} + p");
	textPub -> AddText("#sqrt{s} = 510 GeV");
	int Nentries = hInvMassCombi->GetEntries();
	TString tileIdStr; tileIdStr.Form("%i Exclusive events",Nentries);
	textPub -> AddText(tileIdStr);
	textPub -> Draw("same");

	TLegend *legend = new TLegend(0.65,0.65,0.81,0.75,"","brNDC"); 
	legend -> AddEntry(gm, "In+El (opposite-sign pairs)", "pl");
	if(bckground)
		legend -> AddEntry(gBack, "In+El (same-sign pairs)", "pl"); 
	legend -> SetTextFont(42);
	legend -> SetFillStyle(0);
	legend -> SetLineColor(0);
	legend -> SetTextSize(0.04);
	legend -> Draw("same");

 cInvMass->Update();

 ////////////////////////////
	TCanvas *cInvMassIn = new TCanvas("cInvMassIn","cInvMassIn",1200,800);
	gPad->SetMargin(0.9,0.02,0.14,0.02);

	gIn->SetTitle(" ; Inv. mass m(#pi^{+}#pi^{-}) [GeV/c^{2}];Number of track pairs");
	gIn->GetXaxis()->SetTitleFont(42);
	gIn->GetYaxis()->SetTitleFont(42);
	gIn->GetXaxis()->SetLabelFont(42);
	gIn->GetYaxis()->SetLabelFont(42);
	gIn->GetXaxis()->SetTitleSize(.055);
	gIn->GetYaxis()->SetTitleSize(.055);
	gIn->GetXaxis()->SetLabelSize(.045);
	gIn->GetYaxis()->SetLabelSize(.045);
	gIn->GetXaxis()->SetTitleOffset(1);
	gIn->GetYaxis()->SetTitleOffset(0.8);
	gIn->GetXaxis()->SetRangeUser(0,2.5);
	gIn->Draw("apz");

	if(bckground){
		gBackIn->GetXaxis()->SetRangeUser(0,2.5);
		gBackIn->Draw("psame");
	}
	TPaveText *textPub1 = new TPaveText(0.45,0.7,0.55,0.8,"brNDC");
	textPub1 -> SetTextSize(0.05);
	textPub1 -> SetFillColor(0);
	textPub1 -> SetTextFont(42);
	if(text)
		textPub1 -> AddText("Drop at about 1 GeV");
	textPub1 -> Draw("same");

	TPaveText *textPub2 = new TPaveText(0.75,0.5,0.8,0.6,"brNDC");
	textPub2 -> SetTextSize(0.05);
	textPub2 -> SetFillColor(0);
	textPub2 -> SetTextFont(42);
	if(text)
		textPub2 -> AddText("Peak at about 1270 MeV");
	textPub2 -> Draw("same");

	TPaveText *textPub = new TPaveText(0.75,0.75,0.85,0.97,"brNDC");
	textPub -> SetTextSize(0.05);
	textPub -> SetFillColor(0);
	textPub -> SetTextFont(42);
	textPub -> AddText("p + p #rightarrow p + #pi^{+} + #pi^{-} + p");
	textPub -> AddText("#sqrt{s} = 510 GeV");
	int NentriesIn = hInvMassIn->GetEntries();
	TString tileIdStrIn; tileIdStrIn.Form("%i Exclusive events",NentriesIn);
	textPub -> AddText(tileIdStrIn);
	textPub -> Draw("same");

	TLegend *legend = new TLegend(0.6,0.65,0.81,0.75,"","brNDC");
	legend -> AddEntry(gIn, "Inelastic (opposite-sign pairs)", "pl");
	if(bckground)
		legend -> AddEntry(gBackIn, "Inelastic (same-sign pairs)", "pl"); 
	legend -> SetTextFont(42);
	legend -> SetFillStyle(0);
	legend -> SetLineColor(0);
	legend -> SetTextSize(0.04);
	legend -> Draw("same");

 cInvMassIn->Update();

 ////////////////////////////
	TCanvas *cInvMassEl = new TCanvas("cInvMassEl","cInvMassEl",1200,800);
	gPad->SetMargin(0.9,0.02,0.14,0.02);

	gEl->SetTitle(" ; Inv. mass m(#pi^{+}#pi^{-}) [GeV/c^{2}];Number of track pairs");
	gEl->GetXaxis()->SetTitleFont(42);
	gEl->GetYaxis()->SetTitleFont(42);
	gEl->GetXaxis()->SetLabelFont(42);
	gEl->GetYaxis()->SetLabelFont(42);
	gEl->GetXaxis()->SetTitleSize(.055);
	gEl->GetYaxis()->SetTitleSize(.055);
	gEl->GetXaxis()->SetLabelSize(.045);
	gEl->GetYaxis()->SetLabelSize(.045);
	gEl->GetXaxis()->SetTitleOffset(1);
	gEl->GetYaxis()->SetTitleOffset(0.8);
	gEl->GetXaxis()->SetRangeUser(0,2.5);
	gEl->Draw("apz");

	if(bckground){
		gBackEl->GetXaxis()->SetRangeUser(0,2.5);
		gBackEl->Draw("psame");
	}

	TPaveText *textPub1 = new TPaveText(0.45,0.7,0.55,0.8,"brNDC");
	textPub1 -> SetTextSize(0.05);
	textPub1 -> SetFillColor(0);
	textPub1 -> SetTextFont(42);
	if(text)
		textPub1 -> AddText("Drop at about 1 GeV");
	textPub1 -> Draw("same");

	TPaveText *textPub2 = new TPaveText(0.75,0.5,0.8,0.6,"brNDC");
	textPub2 -> SetTextSize(0.05);
	textPub2 -> SetFillColor(0);
	textPub2 -> SetTextFont(42);
	if(text)
		textPub2 -> AddText("Peak at about 1270 MeV");
	textPub2 -> Draw("same");

	TPaveText *textPub = new TPaveText(0.75,0.75,0.85,0.97,"brNDC");
	textPub -> SetTextSize(0.05);
	textPub -> SetFillColor(0);
	textPub -> SetTextFont(42);
	textPub -> AddText("p + p #rightarrow p + #pi^{+} + #pi^{-} + p");
	textPub -> AddText("#sqrt{s} = 510 GeV");
	int NentriesEl = hInvMassEl->GetEntries();
	TString tileIdStrEl; tileIdStrEl.Form("%i Exclusive events",NentriesEl);
	textPub -> AddText(tileIdStrEl);
	textPub -> Draw("same");

	TLegend *legend = new TLegend(0.6,0.65,0.81,0.75,"","brNDC");
	legend -> AddEntry(gEl, "Elastic (opposite-sign pairs)", "pl");
	if(bckground)
		legend -> AddEntry(gBackEl, "Elastic (same-sign pairs)", "pl"); 
	legend -> SetTextFont(42);
	legend -> SetFillStyle(0);
	legend -> SetLineColor(0);
	legend -> SetTextSize(0.04);
	legend -> Draw("same");

 cInvMassEl->Update();
//////////////////// Output

	cout<<"Creating output file: "<<"output.root"<<endl;
	TFile *fOut = new TFile("output.root","recreate");

	cMissingPt-> Write("hMissingPt");
	cMissingPt->SaveAs("cMissingPt.png");
	cMissingPt->Close();

	cCuts->Write("hCuts");
	cCuts->SaveAs("Cuts.png");
	cCuts->Close();


	cInvMass->Write("cInvMass");
	cInvMass->SaveAs("cInvMass.png");
	//cInvMass->Close();

	cInvMassIn->Write("cInvMassIn");
	cInvMassIn->SaveAs("cInvMassIn.png");
	cInvMassIn->Close();

	cInvMassEl->Write("cInvMassEl");
	cInvMassEl->SaveAs("cInvMassEl.png");
	cInvMassEl->Close();
    
   fOut->Close();



}



