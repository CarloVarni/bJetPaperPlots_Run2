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

#include "../OfficialMacros/AtlasLabels.C"
#include "../OfficialMacros/AtlasStyle.C"
#include "../OfficialMacros/AtlasUtils.C"

TApplication Runner("gui",0,NULL);

std::vector< const std::string > getListOfFolders(TFile&);
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

  std::vector< const std::string > triggerList = getListOfFolders( *inputFile.get() );
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
    HistoSplit->GetXaxis()->SetRangeUser(-0.1,0.1);
  }
  
  double meanSplit = HistoSplit->GetMean();
  double meanErrorSplit = HistoSplit->GetMeanError();
  double rmsSplit = HistoSplit->GetRMS();
  double rmsErrorSplit = HistoSplit->GetRMSError();

  double meanNonSplit = HistoNonSplit->GetMean();
  double meanErrorNonSplit = HistoNonSplit->GetMeanError();
  double rmsNonSplit = HistoNonSplit->GetRMS();
  double rmsErrorNonSplit = HistoNonSplit->GetRMSError();

  HistoSplit->GetXaxis()->SetTitle(Form("Primary Vertex Resolution - %s coordinate",coordinate.c_str()));
  HistoSplit->GetYaxis()->SetTitle("A.U.");
  
  HistoSplit->Scale( 1./HistoSplit->Integral() ); 
  HistoSplit->SetLineColor(2);
  HistoSplit->SetMarkerColor(2);
  HistoSplit->SetMarkerStyle(20);
  HistoSplit->SetMarkerSize(0.7);
  HistoSplit->SetMaximum(5);
  HistoSplit->SetMinimum(5e-5);
    
  HistoNonSplit->Scale( 1./HistoNonSplit->Integral() );
  HistoNonSplit->SetMarkerStyle(20);
  HistoNonSplit->SetMarkerSize(0.7);

  TLegend *legenda = new TLegend( 0.603152,
				  0.804211,
				  0.835244,
				  0.884211 );
  legenda->SetFillColorAlpha(0,0);
  legenda->SetLineColorAlpha(0,0);

  legenda->AddEntry( HistoSplit,"Split","lp");
  legenda->AddEntry( HistoNonSplit,"Non-Split","lp");

  TCanvas *c0 = new TCanvas();
  gPad->SetLogy();

  HistoSplit->Draw("PE");
  HistoNonSplit->Draw("PESAME");
  legenda->Draw("SAME");

  ATLASLabel(   0.18, 0.85,"#scale[0.7]{Simulation Preliminary}");
  myText(       0.205, 0.81, 1, "#scale[0.7]{t#bar{t}, #sqrt{s} = 13 TeV}"); //#sqrt{s}= 13 TeV}" );
  myText(       0.205, 0.77, 1, "#scale[0.7]{Jet E_{T} > 55 GeV, |#eta| < 2.5}"); //MV2c10, Jet E_{T} > 55 GeV, | #eta_{Jet}| < 2.5}");

   double xPointCoordinate = 0.479943;
   if ( coordinate != "Z" ) xPointCoordinate = 0.0267106;
   
  TLatex *latexText = new TLatex();
  latexText->DrawLatex( xPointCoordinate,0.3175,"#scale[0.6]{Split}");
  latexText->DrawLatex( xPointCoordinate,0.181221,Form("    #scale[0.6]{#bf{Mean: %.4f +/- %.4f}}",meanSplit,meanErrorSplit ));
  latexText->DrawLatex( xPointCoordinate,0.10718743,Form("    #scale[0.6]{#bf{RMS: %.4f +/- %.4f}}",rmsSplit,rmsErrorSplit ));
  
  latexText->DrawLatex( xPointCoordinate,0.05471914,"#scale[0.6]{Non-Split}");
  latexText->DrawLatex( xPointCoordinate,0.03302276,Form("    #scale[0.6]{#bf{Mean: %.4f +/- %.4f}}",meanNonSplit,meanErrorNonSplit ));
  latexText->DrawLatex( xPointCoordinate,0.01998469,Form("    #scale[0.6]{#bf{RMS: %.4f +/- %.4f}}",rmsNonSplit,rmsErrorNonSplit ));
  
  
  c0->Draw();
  c0->Update();
  c0->SaveAs( Form("plotPrimaryVertexResolution_%s_%s.pdf",trigger.c_str(),coordinate.c_str()) );
}

std::vector< const std::string > getListOfFolders( TFile& file ) {
  std::vector< const std::string > *vec = new std::vector< const std::string >();

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
