
#include "iostream"
#include "string"
#include "vector"
#include "map"

#include <TROOT.h>
#include "TLatex.h"
#include "TApplication.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TH1.h"
#include "TStyle.h"
#include "TPad.h"
#include "TLegend.h"
#include "TKey.h"
#include "TMathText.h"

#include "../OfficialMacros/AtlasLabels.C"
#include "../OfficialMacros/AtlasStyle.C"
#include "../OfficialMacros/AtlasUtils.C"

TApplication Runner("gui",0,NULL);

std::vector< std::string > getListOfFolders(TFile&);
void plotResolution(const std::string&,TH1D*,TH1D*);

std::string coordinate = "X";

int main( int narg,char* argv[] ) {
  if ( narg == 1 ) {
    std::cout<<"No argument provided!"<<std::endl;
    return 0;
  }
  coordinate = argv[1];
  std::cout<<"coordinate = "<<coordinate<<std::endl;
  
  SetAtlasStyle();
  
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);

  std::string inputFileName = "./data/Outcome_PrimaryVertexResolution.root";
  std::unique_ptr< TFile > inputFile( new TFile(inputFileName.c_str(),"READ") );

  std::vector< std::string > triggerList = getListOfFolders( *inputFile.get() );
  std::cout <<"### Available Trigger Chains: " << std::endl;
  for ( const std::string& trigger : triggerList ) {
    if ( trigger.find("HLT") == std::string::npos ) continue;
    std::cout<<"   +++ "<< trigger << std::endl;
  }


  std::map< const std::string,TH1D* > map_PV_RES;
  for ( const std::string& trigger : triggerList ) {
    if ( trigger.find("HLT") == std::string::npos ) continue;
    const std::string histoName = trigger + "/" + trigger + "_PV_RES_" + coordinate;
    std::cout << "### Retrieving histogram from " << histoName << std::endl;
    map_PV_RES[ trigger ] = (TH1D*) inputFile->Get( histoName.c_str() );
  }

  std::map< const std::string,TH1D* >::iterator iter = map_PV_RES.begin();
  for ( ; iter != map_PV_RES.end(); iter++ ) {
    if ( iter->first.find("split") == std::string::npos ) continue;

    const std::string splitConfiguration = iter->first;
    const std::string nonSplitConfiguration = splitConfiguration.substr( 0,splitConfiguration.find("_split") );

    if ( map_PV_RES.find( nonSplitConfiguration.c_str() )  == map_PV_RES.end() ) continue;
    std::cout << "### Plotting " << splitConfiguration << "   together with  " << nonSplitConfiguration << std::endl;

    plotResolution( iter->first, iter->second, map_PV_RES.find( nonSplitConfiguration.c_str() )->second );
  }

  Runner.Run( true );
  inputFile->Close();
}

void plotResolution( const std::string& trigger, TH1D* HistoSplit, TH1D* HistoNonSplit ) {

  if ( coordinate != "Z" ) {
    HistoSplit->GetXaxis()->SetRangeUser(-0.05,0.05);
  }
  
  double meanSplit = HistoSplit->GetMean();
  double meanErrorSplit = HistoSplit->GetMeanError();
  double rmsSplit = HistoSplit->GetRMS();
  double rmsErrorSplit = HistoSplit->GetRMSError();

  double meanNonSplit = HistoNonSplit->GetMean();
  double meanErrorNonSplit = HistoNonSplit->GetMeanError();
  double rmsNonSplit = HistoNonSplit->GetRMS();
  double rmsErrorNonSplit = HistoNonSplit->GetRMSError();

  double X_width = HistoSplit->GetBinWidth(1);

  std::string lowerCase = "x";
  if ( coordinate == "Y" ) lowerCase = "y";
  else if ( coordinate == "Z" ) lowerCase = "z";

  std::string xAxisTitle = Form( "\\mathrm{PV} (\\mathrm{%s}_{online} - \\mathrm{%s}_{offline}) \\ \\mathrm{[mm]}",
				 lowerCase.c_str(),
  				 lowerCase.c_str() );
  HistoSplit->GetXaxis()->SetTitle( xAxisTitle.c_str() );
  HistoSplit->GetYaxis()->SetTitle( Form("Fraction of events / %.3f mm",X_width) );
  
  HistoSplit->Scale( 1./HistoSplit->Integral() ); 
  HistoSplit->SetLineColor(2);
  HistoSplit->SetMarkerSize(0);
  HistoSplit->SetMaximum(15);
  HistoSplit->SetMinimum(5e-5);
    
  HistoNonSplit->Scale( 1./HistoNonSplit->Integral() );
  HistoNonSplit->SetMarkerSize(0);

  TLegend *legenda = new TLegend( 0.603152,
				  0.789474,
				  0.896848,
				  0.886316 );

 legenda->SetFillColorAlpha(0,0);
  legenda->SetLineColorAlpha(0,0);

  legenda->AddEntry( HistoSplit,"Offline Algorithm","l" );
  legenda->AddEntry( HistoNonSplit,"Histogram technique","l" );

  TCanvas *c0 = new TCanvas();
  gPad->SetLogy();

  HistoSplit->Draw("HIST");
  HistoNonSplit->Draw("HISTSAME");
  legenda->Draw("SAME");

  ATLASLabel(   0.18, 0.85,"Simulation Internal");
  myText(       0.18, 0.79, 1, "#scale[1.]{t#bar{t}, #sqrt{s} = 13 TeV}" );
  myText(       0.18, 0.73, 1, "#scale[1.]{Jet E_{T} > 55 GeV, |#eta| < 2.5}");
  
  double xPointCoordinate = 0.479943;
  if ( coordinate != "Z" ) xPointCoordinate = 0.0267106;
  
  c0->Draw();
  c0->Update();
  c0->SaveAs( Form("plotPrimaryVertexResolution_%s_%s.pdf",trigger.c_str(),coordinate.c_str()) );
  c0->SaveAs( Form("plotPrimaryVertexResolution_%s_%s.png",trigger.c_str(),coordinate.c_str()) );
}


std::vector< std::string > getListOfFolders( TFile& file ) {
  std::vector< std::string > *vec = new std::vector< std::string >();

  file.cd();
  TIter keyList( gDirectory->GetListOfKeys() );
  TKey *key;

  while ( (key = (TKey*)keyList()) ) {
    TClass *cl = gROOT->GetClass( key->GetClassName() );

    if (!cl->InheritsFrom("TDirectoryFile")) continue;
    vec->push_back( std::string( key->GetName() ) );
  }

  return *vec;

}


