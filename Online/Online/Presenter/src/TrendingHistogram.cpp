// $Id: TrendingHistogram.cpp,v 1.1 2010-08-08 15:12:33 robbep Exp $

// STL
#include <iostream>

// ROOT
#ifdef WIN32
#  pragma warning( push )
#  pragma warning( disable : 4800 )
#endif
#include <TGraph.h>
#include <TCanvas.h>
#ifdef WIN32
#  pragma warning( pop )
#endif

// Trending
#include "Trending/ITrendingTool.h" 

// Online
#include "OnlineHistDB/OnlineHistogram.h"

// Boost
#include <boost/filesystem.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/date_time/posix_time/posix_time_types.hpp"

// This class
#include "TrendingHistogram.h" 

// global variable
namespace PresenterGaudi { 
  extern ITrendingTool * trendingTool ;
};

// static variable
const int TrendingHistogram::s_timeOffset = 3600 ;

//====================================================================
// Constructor
//====================================================================
TrendingHistogram::TrendingHistogram( const std::string & identifier,
				      const std::string & dimServiceName,
				      int refreshTime,
				      int instance,
				      OnlineHistDB* histogramDB,
				      OMAlib* analysisLib,
				      OnlineHistogram* onlineHist,
				      pres::MsgLevel verbosity,
				      DimBrowser* DimBr,
				      boost::recursive_mutex * oraMutex,
				      boost::recursive_mutex * dimMutex,
				      std::vector<std::string*> & tasksNotRunning,
				      boost::recursive_mutex * rootMutex ,
				      int startTime , 
				      int endTime , 
				      TrendingMode mode ) : 
  DbRootHist( identifier , dimServiceName, refreshTime ,
	      instance , histogramDB , analysisLib ,
	      onlineHist , verbosity, DimBr, oraMutex ,
	      dimMutex , tasksNotRunning , rootMutex ) ,
  m_timeGraph( 0 ) , 
  m_timeArray( 0 ) ,
  m_valueArray( 0 ) , 
  m_size( 0 ) , 
  m_startTime( startTime ) , 
  m_endTime( endTime ) ,
  m_trendingMode( mode ) {
  boost::filesystem::path filePath( onlineHist -> hname() ) ;
  m_trendFileName = filePath.replace_extension().string() ;
  m_tagName = onlineHist -> algorithm() ;
}

//====================================================================
// Destructor
//====================================================================
TrendingHistogram::~TrendingHistogram( ) { 
  if ( 0 != m_timeArray ) delete [] m_timeArray ;
  if ( 0 != m_valueArray ) delete [] m_valueArray ;
}

//====================================================================
// Draw 
//====================================================================
void TrendingHistogram::fillHistogram( ) {

  gPad -> SetEditable( true ) ;
  if ( 0 != PresenterGaudi::trendingTool ) { 
    bool status = PresenterGaudi::trendingTool -> openRead( m_trendFileName ) ;

    if ( ! status ) {
      std::cerr << "Trend file does not exist : " << m_trendFileName 
		<< std::endl ;
      return ;
    }
    
    boost::posix_time::ptime now ;
    std::tm temp ;
    switch ( m_trendingMode ) {
    case Last5Minutes:
      now = boost::posix_time::second_clock::local_time() ;
      temp = 
	boost::posix_time::to_tm( now - 
				  boost::posix_time::time_duration( boost::posix_time::minutes( 5 ) ) ) ;
      m_startTime = 
	std::mktime( &temp ) ;
      m_endTime = std::time( 0 ) ;
      break ;
    case Last30Minutes:
      now = boost::posix_time::second_clock::local_time() ;
      temp = 
	boost::posix_time::to_tm( now - 
				  boost::posix_time::time_duration( boost::posix_time::minutes( 30 ) ) ) ;
      m_startTime = 
	std::mktime( &temp ) ;
      m_endTime = std::time( 0 ) ;
      break ;
    case Last2Hours:
      now = boost::posix_time::second_clock::local_time() ;
      temp = 
	boost::posix_time::to_tm( now - 
				  boost::posix_time::time_duration( boost::posix_time::hours( 2 ) ) ) ;
      m_startTime = 
	std::mktime( &temp ) ;
      m_endTime = std::time( 0 ) ;
      break ;
    case LastDay:
      now = boost::posix_time::second_clock::local_time() ;
      temp = 
	boost::posix_time::to_tm( now - 
				  boost::posix_time::time_duration( boost::posix_time::hours( 24 ) ) ) ;
      m_startTime = 
	std::mktime( &temp ) ;
      m_endTime = std::time( 0 ) ;
      break ;
    case LastWeek:
      now = boost::posix_time::second_clock::local_time() ;
      temp = 
	boost::posix_time::to_tm( now - 
				  boost::posix_time::time_duration( boost::posix_time::hours( 7*24 ) ) ) ;
      m_startTime = 
	std::mktime( &temp ) ;
      m_endTime = std::time( 0 ) ;
      break ;
    case LastYear:
      now = boost::posix_time::second_clock::local_time() ;
      temp = 
	boost::posix_time::to_tm( now - 
				  boost::posix_time::time_duration( boost::posix_time::hours( 24*365 ) ) ) ;
      m_startTime = 
	std::mktime( &temp ) ;
      m_endTime = std::time( 0 ) ;
      break ;
    case Last10Years:
      now = boost::posix_time::second_clock::local_time() ;
      temp = 
	boost::posix_time::to_tm( now - 
				  boost::posix_time::time_duration( boost::posix_time::hours( 24*365*10 ) ) ) ;
      m_startTime = 
	std::mktime( &temp ) ;
      m_endTime = std::time( 0 ) ;
      break ;
    default:
      break ;
    }
    
    status = PresenterGaudi::trendingTool->select( m_startTime , m_endTime , 
						   m_tagName );

    if ( ! status ) {
      std::cerr << "Bad tag name " << m_tagName << std::endl ;
      return ;
    }
    
    int theTime ;
    float theValue ;
   
    std::vector< double_t > vtime , vvalue ;

    if ( 0 != m_timeArray ) delete [] m_timeArray ;
    if ( 0 != m_valueArray ) delete [] m_valueArray ;
    
    // Read the values in the selected time interval and fill a vector    
    while ( PresenterGaudi::trendingTool -> nextValue( theTime , theValue ) ) {
      vtime.push_back( theTime - s_timeOffset ) ;
      vvalue.push_back( theValue ) ;
    }
    
    if ( vtime.empty() ) {
      vtime.push_back( m_startTime - s_timeOffset ) ;
      vtime.push_back( m_endTime - s_timeOffset ) ;
      vvalue.push_back( 0. ) ;
      vvalue.push_back( 0. ) ;
    } else {
      if ( vtime.front() > m_startTime ) {
	vtime.insert( vtime.begin() , m_startTime - s_timeOffset ) ;
	vvalue.insert( vvalue.begin() , vvalue.front() ) ;
      }
    }

    if ( vtime.back() < m_endTime ) {
      vtime.push_back( m_endTime - s_timeOffset ) ;
      vvalue.push_back( vvalue.back() ) ;
    }

    m_size = vtime.size() ;
    m_timeArray = new double_t[ m_size ] ;
    std::copy( vtime.begin() , vtime.end() , m_timeArray ) ;
    
    m_valueArray = new double_t[ m_size ] ;
    std::copy( vvalue.begin() , vvalue.end() , m_valueArray ) ;

    if ( 0 != m_timeGraph ) delete m_timeGraph ;
    m_timeGraph = new TGraph( m_size , m_timeArray , m_valueArray ) ; 

    m_timeGraph -> GetXaxis() -> SetTimeDisplay( 1 ) ;
    m_timeGraph -> GetXaxis() -> SetTimeFormat( "#splitline{%d/%m/%y}{%H:%M:%S}" ) ;
    m_timeGraph -> GetXaxis() -> SetTimeOffset( 0 ) ;
    
    copyAttributes() ;

    if ( 0 != m_hostingPad ) { 
      m_hostingPad -> Modified( ) ;
      m_timeGraph -> Draw( "ALP" ) ; 
    }
    
    PresenterGaudi::trendingTool -> closeFile( ) ;
  }
}

//================================================================================
// Initialize histograms
//================================================================================
void TrendingHistogram::initHistogram( ) {
  beEmptyHisto() ;
  fillHistogram() ;
}

//======================================================================================
// Draw the histogram on the page
//======================================================================================
void TrendingHistogram::draw(TCanvas* editorCanvas, double xlow, double ylow, double xup, 
			     double yup, bool /*fastHitMapDraw*/, TPad* /*overlayOnPad*/) {
  TPad * pad = new TPad( m_identifier.c_str(),
			 TString(""),
			 TMath::Abs(xlow), TMath::Abs(ylow) ,
			 TMath::Abs(xup), TMath::Abs(yup) ) ;
  setPadMarginsFromDB(pad);

  editorCanvas->cd();

  pad -> Draw() ;
  pad -> cd();

  pad->SetBit( kNoContextMenu ) ;
  pad->SetFillColor( 10 ) ;
  
  setDrawOptionsFromDB( pad ) ;

  pad -> SetName( m_histoRootName ) ;

  m_hostingPad = pad;

  if ( 0 == m_timeGraph ) return ;

  m_timeGraph -> Draw( "ALP" ) ;
}

//==================================================================================
// Copy attributes of the histogram into the graph
//==================================================================================
void TrendingHistogram::copyAttributes( ) {
  TAxis * grax = m_timeGraph -> GetYaxis() ;
  TAxis * hax  = m_rootHistogram -> GetYaxis() ;

  (( TNamed ) (*hax)).Copy( *grax ) ; 
  (( TAttAxis ) (*hax)).Copy( *grax ) ;

  grax = m_timeGraph -> GetXaxis() ;
  hax  = m_rootHistogram -> GetXaxis() ;
  (( TAttAxis ) (*hax)).Copy( *grax ) ;
  grax -> SetRangeUser( (Double_t) m_startTime , (Double_t) m_endTime ) ;

  (( TAttMarker ) (*m_rootHistogram)).Copy( *m_timeGraph ) ;
  (( TAttLine )   (*m_rootHistogram)).Copy( *m_timeGraph ) ;

  m_timeGraph -> SetTitle( m_rootHistogram -> GetTitle() ) ;
  m_timeGraph -> SetName( m_rootHistogram -> GetName() ) ;
}

//===================================================================================
// Refresh histo
//===================================================================================
void TrendingHistogram::refresh( ) {
  fillHistogram( ) ;
}
