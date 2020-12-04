 
#include "iostream"
#include "memory"

#include "TApplication.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TH1.h"
#include "TGraph.h"
#include "TPad.h"
#include "TStyle.h"
#include "TLegend.h"

#include "../OfficialMacros/AtlasLabels.C"
#include "../OfficialMacros/AtlasStyle.C"
#include "../OfficialMacros/AtlasUtils.C"

TApplication Runner("gui",0,NULL);

std::vector< TGraph* > getROC( std::unique_ptr< TFile >&,const std::string& );

std::string mcSample = "t#bar{t} and Z'";

void retrieveCurves( std::unique_ptr< TFile >&,
		     const std::string&,
		     const std::string&,
		     std::vector< TGraph* >&,
		     std::vector< TGraph* >& );

int main() {

  SetAtlasStyle();

  std::string fileName_ttbar = "./data/Outcome_HybridPerformance_ttbar.root";
  std::string fileName_Zprime = "./data/Outcome_HybridPerformance_Zprime.root";

  std::unique_ptr< TFile > inFile_ttbar( new TFile( fileName_ttbar.c_str(),"read" ) );
  std::unique_ptr< TFile > inFile_Zprime( new TFile( fileName_Zprime.c_str(),"read" ) );

  std::vector< TGraph* > ROCs_online_MV2c10_ttbar;
  std::vector< TGraph* > ROCs_online_MV2c00_ttbar;
  std::vector< TGraph* > ROCs_online_MV2c10_Zprime;
  std::vector< TGraph* > ROCs_online_MV2c00_Zprime;

  retrieveCurves( inFile_ttbar,
		  "online_MV2c10",
		  "online_MV2c00",
		  ROCs_online_MV2c10_ttbar,
		  ROCs_online_MV2c00_ttbar );
  retrieveCurves( inFile_Zprime,
		  "online_MV2c10",
		  "online_MV2c00",
		  ROCs_online_MV2c10_Zprime,
		  ROCs_online_MV2c00_Zprime );

  // =======================================================

  // tt-bar
  for ( unsigned int i(0); i<ROCs_online_MV2c10_ttbar.size(); i++ ) {
    ROCs_online_MV2c10_ttbar.at( i )->SetLineColor( 4 );
    ROCs_online_MV2c10_ttbar.at( i )->SetMinimum( 0.6 );
    if ( i == 0 ) ROCs_online_MV2c10_ttbar.at( i )->SetMaximum( 5 * 1e5 );
    else ROCs_online_MV2c10_ttbar.at( i )->SetMaximum( 2 * 1e3 );
    ROCs_online_MV2c10_ttbar.at( i )->SetLineWidth( 2 );
    ROCs_online_MV2c10_ttbar.at( i )->SetLineStyle( 2 );
  }

  for ( unsigned int i(0); i<ROCs_online_MV2c00_ttbar.size(); i++ ) {
    ROCs_online_MV2c00_ttbar.at( i )->SetLineColor( 2 );
    ROCs_online_MV2c00_ttbar.at( i )->SetLineWidth( 2 );
    ROCs_online_MV2c00_ttbar.at( i )->SetLineStyle( 2 );
  }

  // Zprime 
  for ( unsigned int i(0); i<ROCs_online_MV2c10_Zprime.size(); i++ ) {
    ROCs_online_MV2c10_Zprime.at( i )->SetLineColor( 4 );
    ROCs_online_MV2c10_Zprime.at( i )->SetMinimum( 0.6 );
    if ( i == 0 ) ROCs_online_MV2c10_Zprime.at( i )->SetMaximum( 5 * 1e5 );
    else ROCs_online_MV2c10_Zprime.at( i )->SetMaximum( 2 * 1e3 );
    ROCs_online_MV2c10_Zprime.at( i )->SetLineWidth( 2 );
  }

  for ( unsigned int i(0); i<ROCs_online_MV2c00_Zprime.size(); i++ ) {
    ROCs_online_MV2c00_Zprime.at( i )->SetLineColor( 2 );
    ROCs_online_MV2c00_Zprime.at( i )->SetLineWidth( 2 );
  }


  // =======================================================

  TCanvas *c0 = new TCanvas("c0","c0",0.,0.,800,800);
  gPad->SetLogy();
  gPad->SetTicks(1,1);

  ROCs_online_MV2c10_ttbar.at( 0 )->Draw( "AL" );
  ROCs_online_MV2c10_Zprime.at( 0 )->Draw( "LSAME" );

  ROCs_online_MV2c00_ttbar.at( 0 )->Draw( "LSAME" );
  ROCs_online_MV2c00_Zprime.at( 0 )->Draw( "LSAME" );


  ATLASLabel(   0.20, 0.88,"#scale[1.]{Simulation Preliminary}");
  myText(       0.225, 0.83, 1, "#scale[1.]{t#bar{t}, #sqrt{s} = 13 TeV}" );
  myText(       0.225, 0.78, 1, "#scale[1.]{Jet E_{T} > 55 GeV, |#eta| < 2.5}");   
  
  TLegend *legenda_bl = new TLegend( 0.547619,
				     0.609032,
				     0.91604,
				     0.748387 );
  
  legenda_bl->SetFillColor(0);
  legenda_bl->SetLineColor(0);

  legenda_bl->AddEntry( ROCs_online_MV2c10_ttbar.at( 0 ),"","lb");
  legenda_bl->AddEntry( ROCs_online_MV2c00_ttbar.at( 0 ),"","lb");
  legenda_bl->AddEntry( ROCs_online_MV2c10_Zprime.at( 0 ),"","lb");
  legenda_bl->AddEntry( ROCs_online_MV2c00_Zprime.at( 0 ),"","lb");

  legenda_bl->Draw( "SAME" );

  myText(    0.65, 0.72, 1, "#scale[0.7]{t#bar{t}, 2018 tuning}");
  myText(    0.65, 0.685, 1, "#scale[0.7]{t#bar{t}, Hybrid tuning}");
  myText(    0.65, 0.65, 1, "#scale[0.7]{Z' #rightarrow t#bar{t}, 2018 tuning}");
  myText(    0.65, 0.615, 1, "#scale[0.7]{Z' #rightarrow t#bar{t}, Hybrid tuning}");

  c0->Draw();
  c0->Update();
  c0->SaveAs( "paperPlot_ROC_bl.pdf" );
  c0->SaveAs( "paperPlot_ROC_bl.C" );

  // =======================================================  

  TCanvas *c1 = new TCanvas("c1","c1",0.,0.,800,800);
  gPad->SetLogy();
  gPad->SetTicks(1,1);

  ROCs_online_MV2c10_ttbar.at( 1 )->Draw( "AL" );
  ROCs_online_MV2c10_Zprime.at( 1 )->Draw( "LSAME" );

  ROCs_online_MV2c00_ttbar.at( 1 )->Draw( "LSAME" );
  ROCs_online_MV2c00_Zprime.at( 1 )->Draw( "LSAME" );

  ATLASLabel(   0.20, 0.88,"#scale[1.]{Simulation Preliminary}");
  myText(       0.225, 0.83, 1, "#scale[1.]{t#bar{t}, #sqrt{s} = 13 TeV}" );
  myText(       0.225, 0.78, 1, "#scale[1.]{Jet E_{T} > 55 GeV, |#eta| < 2.5}");

  legenda_bl->Draw( "SAME" );

  myText(    0.65, 0.72, 1, "#scale[0.7]{t#bar{t}, 2018 tuning}");
  myText(    0.65, 0.685, 1, "#scale[0.7]{t#bar{t}, Hybrid tuning}");
  myText(    0.65, 0.65, 1, "#scale[0.7]{Z' #rightarrow t#bar{t}, 2018 tuning}");
  myText(    0.65, 0.615, 1, "#scale[0.7]{Z' #rightarrow t#bar{t}, Hybrid tuning}");
  
  // myText(    0.65, 0.875, 1, "#scale[0.7]{t#bar{t}, 2018 tuning}");
  // myText(    0.65, 0.84, 1, "#scale[0.7]{t#bar{t}, Hybrid tuning}");
  // myText(    0.65, 0.805, 1, "#scale[0.7]{Z' #rightarrow t#bar{t}, 2018 tuning}");
  // myText(    0.65, 0.77, 1, "#scale[0.7]{Z' #rightarrow t#bar{t}, Hybrid tuning}");

  c1->Draw();
  c1->Update();
  c1->SaveAs( "paperPlot_ROC_bc.pdf" );
  c1->SaveAs( "paperPlot_ROC_bc.C" );
  // =======================================================   

  Runner.Run( true );

  inFile_ttbar->Close();
  inFile_Zprime->Close();
}

std::vector< TGraph* > getROC( std::unique_ptr< TFile >& inFile,const std::string& variableName ) {
  
  TH1D *btag_u = (TH1D*) inFile->Get( Form("btag_%s_u",variableName.c_str()) );
  TH1D *btag_b = (TH1D*) inFile->Get( Form("btag_%s_b",variableName.c_str()) );
  TH1D *btag_c = (TH1D*) inFile->Get( Form("btag_%s_c",variableName.c_str()) );

  btag_u->Scale( 1./btag_u->Integral() );
  btag_b->Scale( 1./btag_b->Integral() );
  btag_c->Scale( 1./btag_c->Integral() );

  TGraph *gr_lb = new TGraph();
  TGraph *gr_cb = new TGraph();

  int nPoints_lb = 0;
  int nPoints_cb = 0;

  for ( int i(1); i<=btag_u->GetNbinsX(); i++ ) {
    double ueff = btag_u->Integral( i,btag_u->GetNbinsX() );
    double ceff = btag_c->Integral( i,btag_c->GetNbinsX() );
    double beff = btag_b->Integral( i,btag_b->GetNbinsX() );

    if ( ceff != 0 ) ceff = 1./ceff;
    if ( ueff != 0 ) ueff = 1./ueff;

    gr_lb->SetPoint( nPoints_lb++,beff * 100,ueff );
    gr_cb->SetPoint( nPoints_cb++,beff * 100,ceff );
  }
  /*
  btag_u->Rebin(8*5);
  btag_c->Rebin(8*5);
  btag_b->Rebin(8*5);
  */
  btag_u->SetLineColor(2);
  btag_c->SetLineColor(3);
  btag_b->SetLineColor(4);

  btag_u->GetXaxis()->SetTitle("MV2c10");
  btag_u->GetYaxis()->SetTitle("A.U.");

  btag_u->SetMaximum(1);
  btag_u->SetMinimum(1e-4);

  /*
  TCanvas *c0 = new TCanvas(Form("c0_%s",variableName.c_str()),
			    Form("c0_%s",variableName.c_str()),
			    700,450);
  gPad->SetLogy();
  gPad->SetGridx();
  gPad->SetGridy();
  btag_u->GetXaxis()->SetNdivisions(505);
  btag_u->GetYaxis()->SetNdivisions(10);

  btag_u->Draw("HIST");
  btag_c->Draw("HISTSAME");
  btag_b->Draw("HISTSAME");

  ATLASLabel(   0.20, 0.45,"Preliminary");
  myText(       0.20, 0.40, 1, Form("#scale[0.8]{%s Monte Carlo, #sqrt{s}= 13 TeV}",mcSample.c_str()));
  myText(       0.20, 0.35, 1, "#scale[0.8]{Jet p_{T} > 55 GeV, |#eta_{Jet}| < 2.5}");
  myBoxText(    0.25, 0.30, 0.03, 2, "#scale[0.7]{Light quarks}");
  myBoxText(    0.25, 0.25, 0.03, 3, "#scale[0.7]{Charm quarks}");
  myBoxText(    0.25, 0.20, 0.03, 4, "#scale[0.7]{Bottom quarks}");

  c0->Draw();
  c0->Update();
  c0->SaveAs( Form("btag_%s.pdf",variableName.c_str()) );
  c0->SaveAs( Form("btag_%s.C",variableName.c_str()) );
  */
  // ====================================================== //

  gr_lb->GetXaxis()->SetTitle("b-jet efficiency [%]");
  gr_lb->GetYaxis()->SetTitle("Light jet rejection");
  gr_lb->GetXaxis()->SetRangeUser(38,102);

  gr_cb->GetXaxis()->SetTitle("b-jet efficiency [%]");
  gr_cb->GetYaxis()->SetTitle("c-jet rejection");
  gr_cb->GetXaxis()->SetRangeUser(38,102);

  std::vector< TGraph* > output;
  output.push_back( gr_lb );
  output.push_back( gr_cb );
  return output;
}


void retrieveCurves( std::unique_ptr< TFile >& inFile,
		     const std::string& variableNameA,
		     const std::string& variableNameB,
                     std::vector< TGraph* >& roc_mv2c10,
                     std::vector< TGraph* >& roc_mv2c00 ) {

  roc_mv2c10 = getROC( inFile,variableNameA );
  roc_mv2c00 = getROC( inFile,variableNameB );


  if ( roc_mv2c10.size() != 2 )
    std::cout<<"roc_mv2c10 has size "<<roc_mv2c10.size()<<std::endl; 
  if ( roc_mv2c10.size() != 2 )
    std::cout<<"roc_mv2c00 has size "<<roc_mv2c10.size()<<std::endl; 

  for ( unsigned int i(0); i<roc_mv2c10.size(); i++ )
    roc_mv2c10.at( i )->SetLineColor(4);

  for ( unsigned int i(0); i<roc_mv2c00.size(); i++ )
    roc_mv2c00.at( i )->SetLineColor(2);

}


