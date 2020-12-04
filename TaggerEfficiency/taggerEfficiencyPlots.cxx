 
#include "iostream"
#include "memory"
#include "string"
#include "vector"
#include "map"
#include "tuple"

#include "TApplication.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TH1.h"
#include "TDirectory.h"
#include "TEfficiency.h"
#include "TGraphAsymmErrors.h"
#include "TLegend.h"

#include "../OfficialMacros/AtlasLabels.C"
#include "../OfficialMacros/AtlasStyle.C"
#include "../OfficialMacros/AtlasUtils.C"

TApplication Runner("gui",0,NULL);

// ====================================================================================

enum TaggerElement { OFFLINE_TAGGER=0, ONLINE_TAGGER=1, NUM=2, DEN=3 };

std::vector< 
  std::tuple< const std::string,
	      std::vector< const std::string >,
	      std::vector< TH1D* >,
	      std::vector< TH1D*> >
  > taggerCombinations;

// ====================================================================================

void setTaggers();
const std::string getFolderName( const std::string&,const std::string& );
std::pair< const std::string,const std::string > separateTaggerAndWp( const std::string& );
void retrieveHistograms( TFile*,
			 std::tuple< const std::string,
                         std::vector< const std::string >,
                         std::vector< TH1D* >,
                         std::vector< TH1D*> >& );
void retrieveHistogramsFromFile( TFile*,const std::string&,const std::string&,std::vector< TH1D* >& );
void producePlot( const std::string&,
		  std::tuple< const std::string,
		  std::vector< const std::string >,
		  std::vector< TH1D* >,
		  std::vector< TH1D*> >& );

// ====================================================================================

int main() {
  SetAtlasStyle();
  setTaggers();
  
  std::shared_ptr< TFile > inFile( new TFile( "./data/Outcome_TaggerEfficiencyCurve.root","READ" ) );

  for ( unsigned int i(0); i<taggerCombinations.size(); i++ ) {
    retrieveHistograms( inFile.get(),taggerCombinations.at(i) );    

    // Checking ...
    std::cout << "Found num histograms: " << std::get< TaggerElement::NUM >( taggerCombinations.at(i) ).size() << std::endl;
    for ( unsigned int j(0); j<std::get< TaggerElement::NUM >( taggerCombinations.at(i) ).size(); j++ )
      if ( std::get< TaggerElement::NUM >( taggerCombinations.at(i) ).at(j) == nullptr )
	std::cout<<"  **** Found a nullptr! "<< std::endl;

    std::cout << "Found den histograms: " << std::get< TaggerElement::DEN >( taggerCombinations.at(i) ).size() << std::endl;
    for ( unsigned int j(0); j<std::get< TaggerElement::DEN >( taggerCombinations.at(i) ).size(); j++ )
      if ( std::get< TaggerElement::DEN >( taggerCombinations.at(i) ).at(j) == nullptr )
	std::cout<<"  **** Found a nullptr! "<< std::endl;



    // Need to create two different sets
    const std::string offlineCombo = std::get< TaggerElement::OFFLINE_TAGGER >( taggerCombinations.at(i) );
    std::vector< const std::string > onlineCombo_1( std::get< TaggerElement::ONLINE_TAGGER >( taggerCombinations.at(i) ).begin(),
						    std::get< TaggerElement::ONLINE_TAGGER >( taggerCombinations.at(i) ).begin()+3 );
    std::vector< const std::string > onlineCombo_2( std::get< TaggerElement::ONLINE_TAGGER >( taggerCombinations.at(i) ).begin()+3,
						    std::get< TaggerElement::ONLINE_TAGGER >( taggerCombinations.at(i) ).end() );
    std::vector< TH1D* > num_1( std::get< TaggerElement::NUM >( taggerCombinations.at(i) ).begin(),
				      std::get< TaggerElement::NUM >( taggerCombinations.at(i) ).begin()+3 );
    std::vector< TH1D* > num_2( std::get< TaggerElement::NUM >( taggerCombinations.at(i) ).begin()+3,
				      std::get< TaggerElement::NUM >( taggerCombinations.at(i) ).end() );
    std::vector< TH1D* > den_1( std::get< TaggerElement::DEN >( taggerCombinations.at(i) ).begin(),
                                      std::get< TaggerElement::DEN >( taggerCombinations.at(i) ).begin()+3 );
    std::vector< TH1D* > den_2( std::get< TaggerElement::DEN >( taggerCombinations.at(i) ).begin()+3,
                                      std::get< TaggerElement::DEN >( taggerCombinations.at(i) ).end() );

    std::tuple< const std::string,
                std::vector< const std::string >,
                std::vector< TH1D* >,
                std::vector< TH1D* > > firstHalf( offlineCombo,onlineCombo_1,num_1,den_1 );

    std::tuple< const std::string,
                std::vector< const std::string >,
                std::vector< TH1D* >,
                std::vector< TH1D* > > secondHalf( offlineCombo,onlineCombo_2,num_2,den_2 );
    
    producePlot( "EfficiencyPlot_70.pdf",firstHalf );
    producePlot( "EfficiencyPlot_40.pdf",secondHalf );
  }

  Runner.Run( true );
  inFile->Close();
}

// ====================================================================================

void setTaggers() {
  // MV2C1070 offline
  {
    std::vector< const std::string > toAddVector;
    toAddVector.push_back( "COMBmedium" );
    toAddVector.push_back( "MV2C2070" );
    toAddVector.push_back( "MV2C1070" );
    toAddVector.push_back( "COMBloose" );
    toAddVector.push_back( "MV2C2040" );
    toAddVector.push_back( "MV2C1040" );

    std::tuple< const std::string,
		std::vector< const std::string >,
		std::vector< TH1D* >,
		std::vector< TH1D*> > toAddTuple( "MV2C1070",toAddVector,std::vector< TH1D* >(),std::vector< TH1D*>() );
    
    taggerCombinations.push_back( toAddTuple );
  }

}

std::pair< const std::string,const std::string > separateTaggerAndWp( const std::string& combo ) {

  std::string tagger = "";
  std::string wp = "";

  if ( combo.find("MV") != std::string::npos ) {
    tagger = combo.substr( 0,combo.length() - 2 );
    wp = combo.substr( combo.length() - 2,2 );
  } else if ( combo.find("COMB") != std::string::npos ) {
    tagger = combo.substr( 0,4 );
    wp = combo.substr( 4,combo.length() - 4 );
  }

  if ( tagger.find("MV") != std::string::npos )
    std::replace( tagger.begin(), tagger.end(), 'C', 'c');
  
  return std::pair< const std::string,const std::string >( tagger,wp );
}

const std::string getFolderName( const std::string& offline,const std::string& online ) {

  std::pair< const std::string,const std::string > offlineTagger = separateTaggerAndWp( offline );
  std::pair< const std::string,const std::string > onlineTagger = separateTaggerAndWp( online );

  std::string chainConfiguration = "HLT_j35_boffperf";
  if ( online.find( "COMB" ) == std::string::npos ) chainConfiguration += "_split";
  
  std::string folderName = Form( "%s/ONLINE_%s_OFFLINE_%s",
				 chainConfiguration.c_str(),
				 onlineTagger.first.c_str(),
				 offlineTagger.first.c_str() );
  std::cout<<"Folder Name is " << folderName << std::endl;
  
  return folderName;
}

void retrieveHistograms( TFile* inFile,
			 std::tuple< const std::string,
			 std::vector< const std::string >,
			 std::vector< TH1D* >,
			 std::vector< TH1D*> >& myCombination ) {

  std::cout << "Getting Histograms for offline tagger: " <<
    std::get< TaggerElement::OFFLINE_TAGGER >( myCombination ) << std::endl;

  std::vector< const std::string > onlineTaggers = std::get< TaggerElement::ONLINE_TAGGER >( myCombination );
  std::cout<<"  -- Running on " << onlineTaggers.size() <<" online taggers..." << std::endl;

  for ( unsigned int i(0); i<onlineTaggers.size(); i++ ) {
    std::string offlineCombo = std::get< TaggerElement::OFFLINE_TAGGER >( myCombination );
    std::string onlineCombo = std::get< TaggerElement::ONLINE_TAGGER >( myCombination ).at(i);

    const std::string folderName = getFolderName( offlineCombo,onlineCombo );

    if ( offlineCombo.find( "COMB" ) == std::string::npos )
      std::replace( offlineCombo.begin(),offlineCombo.end(),'C','c' );
    if ( onlineCombo.find( "COMB" ) == std::string::npos )
      std::replace( onlineCombo.begin(),offlineCombo.end(),'C','c' );

    const std::string histoName_num = Form( "%s_%s",
					    onlineCombo.c_str(),
					    offlineCombo.c_str() );

    const std::string histoName_den = Form( "%sPERF_%s",
					    separateTaggerAndWp( onlineCombo ).first.c_str(),
					    offlineCombo.c_str() );

    std::cout<<"   ** NUM: "<< histoName_num << std::endl;
    std::cout<<"   ** DEN: "<< histoName_den << std::endl;

    retrieveHistogramsFromFile( inFile,
				folderName,
				histoName_num,
				std::get< TaggerElement::NUM >( myCombination ) );
    retrieveHistogramsFromFile( inFile,
				folderName,
				histoName_den,
				std::get< TaggerElement::DEN >( myCombination ) );
  }

}


void retrieveHistogramsFromFile( TFile* inFile,const std::string& path,const std::string& histoName,std::vector< TH1D* >& storage ) {

  std::string fullName = Form( "%s/%s",path.c_str(),histoName.c_str() );
  
  TH1D* toAdd = ( TH1D* ) inFile->Get( fullName.c_str() );
  storage.push_back( toAdd );

  if ( toAdd == nullptr ) 
    std::cout << "ERROR:: NULLPTR for " << path << "/" << histoName << "!!!" << std::endl;
}

void producePlot( const std::string& plotName,
		  std::tuple< const std::string,
                  std::vector< const std::string >,
                  std::vector< TH1D* >,
                  std::vector< TH1D*> >& myCombination ) {

  
  TLegend *legenda = new TLegend( 0.217765,
				  0.667368,
				  0.518625,
				  0.808421 );
  legenda->SetFillColor(0);
  legenda->SetLineColor(0);

    
    std::vector< TEfficiency* > efficiencies;
  for ( unsigned int i(0); i<std::get< TaggerElement::NUM >( myCombination ).size(); i++ ) {
    std::get< TaggerElement::NUM >( myCombination ).at(i)->GetXaxis()->SetTitle( Form( "Offline %s",std::get< TaggerElement::OFFLINE_TAGGER >( myCombination ).c_str() ) );
    std::get< TaggerElement::NUM >( myCombination ).at(i)->GetYaxis()->SetTitle( "Efficiency" );

    std::get< TaggerElement::DEN >( myCombination ).at(i)->GetXaxis()->SetTitle( Form( "Offline %s",std::get< TaggerElement::OFFLINE_TAGGER >( myCombination ).c_str() ) );
    std::get< TaggerElement::DEN >( myCombination ).at(i)->GetYaxis()->SetTitle( "Efficiency" );

    TEfficiency *toAdd = new TEfficiency( *std::get< TaggerElement::NUM >( myCombination ).at(i),
                                          *std::get< TaggerElement::DEN >( myCombination ).at(i) );
    efficiencies.push_back( toAdd );

    std::string legendLabel = std::get< TaggerElement::ONLINE_TAGGER >( myCombination ).at(i);
    // Fixing bug with COMB apparently loose <--> tight
    if ( legendLabel.find( "loose" ) != std::string::npos )
      legendLabel.replace( legendLabel.find( "loose" ),5,"tight" );
    else if ( legendLabel.find( "tight" ) != std::string::npos )
      legendLabel.replace( legendLabel.find( "tight" ),5,"loose" );
      
    legenda->AddEntry( efficiencies.back(),legendLabel.c_str(),"lp");
  }

  TCanvas *c0 = new TCanvas();
  //  gPad->SetLogy();
  
  for ( unsigned int i(0); i<efficiencies.size(); i++ ) {
    std::string option = "SAME";
    if ( i == 0 ) {
      option = "AP";
    }

    efficiencies.at(i)->Draw( option.c_str() );

    efficiencies.at(i)->SetMarkerColor( i+1 );
    efficiencies.at(i)->SetLineColor( i +1 );
    efficiencies.at(i)->SetMarkerStyle( 20 );
    efficiencies.at(i)->SetMarkerSize( 0.7 );

    c0->Draw();
    c0->Update();

    efficiencies.at(i)->GetPaintedGraph()->GetYaxis()->SetRangeUser(0,1.3);

    c0->Draw();
    c0->Update();    
  }

  legenda->Draw( "SAME" );

  ATLASLabel(   0.20, 0.87,"#scale[0.7]{Simulation Preliminary}");
  myText(       0.225, 0.83, 1, "#scale[0.7]{#sqrt{s}= 13 TeV}" );
  //  myText(       0.205, 0.195, 1, "#scale[0.7]{MV2c10, Jet E_{T} > 55 GeV, | #eta_{Jet}| < 2.5}");

  c0->Draw();
  c0->Update();    
  c0->SaveAs( plotName.c_str() );
}
