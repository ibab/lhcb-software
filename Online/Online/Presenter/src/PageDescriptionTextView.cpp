// $Id: PageDescriptionTextView.cpp,v 1.13 2010-09-26 04:36:57 frankb Exp $
// Include files

// local
#include "PageDescriptionTextView.h"

// boost
#include <boost/xpressive/xpressive.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

// STL
#include <iostream>

// from Presenter
#include "presenter.h"
#include "ProblemDB.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PageDescriptionTextView
//
// 2010-04-22 : Patrick Robbe
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PageDescriptionTextView::PageDescriptionTextView( const TGWindow * p ,
                                                  UInt_t w ,
                                                  UInt_t h ,
                                                  const std::string & address,
                                                  const std::string& rundbAddress) :
  TGHtml( p , w , h ) ,
  m_problemDbServerAddress( address ),
  m_rundbServerAddress( rundbAddress )
{ }

//=============================================================================
// Destructor
//=============================================================================
PageDescriptionTextView::~PageDescriptionTextView() { }

//=============================================================================
// Retrieve current problems in problem DB
//=============================================================================
bool PageDescriptionTextView::retrieveListOfProblems( const std::string&
                                                      pageName ,
                                                      const std::string&
                                                      fileName ) {

  // Extract the run number from the file name, where the convention is
  // FILENAME = TaksName_RandomText_RunNumber_RandomText.root

  int runNumber = 0 ;

  if ( ! fileName.empty() ) {
    boost::filesystem::path fp( fileName ) ;
    boost::xpressive::mark_tag rn( 1 ) ;
    boost::xpressive::sregex rnbrx =
      ( boost::xpressive::as_xpr(std::string("Brunel"))|
        boost::xpressive::as_xpr(std::string("DaVinci"))|
        boost::xpressive::as_xpr(std::string("Boole"))|
        boost::xpressive::as_xpr(std::string("Gauss")) )
      >> +boost::xpressive::_w >> "_"
      >> (rn=+boost::xpressive::_d) >> "_"
      >> +boost::xpressive::_w >> ".root" ;
    boost::xpressive::smatch whrn ;
    if ( boost::xpressive::regex_search( fp.filename() , whrn , rnbrx ) ) {
      std::stringstream ss( whrn[ rn ] ) ;
      ss >> runNumber ;
    }
  }

  // Get system name from page name
  // but first erase all spaces
  std::string pname( pageName ) ;
  boost::algorithm::erase_all(pname , " ");
  boost::xpressive::mark_tag system( 1 ) ;

  boost::xpressive::sregex pb =
    (boost::xpressive::as_xpr(std::string("/OfflineDataQuality/"))|
     boost::xpressive::as_xpr(std::string("/Shift/")))
    >> (system=*~(boost::xpressive::set =':')) >> ":"  ;
  boost::xpressive::smatch what ;
  std::string systemName ;
  if ( boost::xpressive::regex_search( pname , what , pb ) ) {
    systemName = what[ system ] ;
    if ( systemName.find( "L0" ) != std::string::npos )
      systemName = "L0" ;
    else if ( systemName.find( "ALIGNMENT" ) != std::string::npos )
      systemName = "Alignment" ;
    else if ( systemName.find( "CALO" ) != std::string::npos )
      systemName = "ECAL HCAL PRS SPD" ;
    else if ( systemName.find( "RICH1" ) != std::string::npos ||
              systemName.find( "RICH2" ) != std::string::npos )
      systemName = systemName ;
    else if ( systemName.find( "RICH" ) != std::string::npos )
      systemName = "RICH1 RICH2" ;
  } else return false ;

  ProblemDB pbdb( m_problemDbServerAddress, m_rundbServerAddress ) ;
  std::vector< std::vector< std::string > > problems =  pbdb.listOfProblems( systemName , runNumber ) ;

  if ( problems.empty() ) return false ;

  Clear() ;

  std::ostringstream theStr ;

  if ( 0 == runNumber )
    theStr << "Open problems reported in the problem database for the "
           << systemName << " subsystem: <br />" << std::endl ;
  else
    theStr << "Problems reported in the problem database for the " << systemName
           << " subsystem and run " << runNumber << ": <br />" << std::endl ;

  theStr << "<table border=0 cellspacing=0 width=100%>" << std::endl ;
  for ( std::vector< std::vector< std::string > >::iterator
          it = problems.begin() ; it != problems.end() ; ++it ) {
    std::vector< std::string > vec = (*it) ;
    id = atoi( vec[ 3 ].c_str() ) ;

    std::string cellColor ;
    if ( vec[ 1 ] == "Ok")
      cellColor = "green" ;
    else if ( vec[ 1 ] == "Minor" )
      cellColor = "orange" ;
    else if ( vec[ 1 ] == "Severe" )
      cellColor = "red" ;
    else
      cellColor = "white" ;

    theStr << "<tr><td class=\"links\" bgcolor=\"" << cellColor << "\">"
           << "<a href=\"http://" << m_problemDbServerAddress
           << "/problems/" << vec[ 3 ] << "/\" STYLE=\"text-decoration: none\">"
           << "<font color=\"#000000\">"
           << ( ( vec[ 0 ] + ": " + vec[ 4 ] ).c_str() )
           << "</font>" << "</a>" << "</td></tr>" << std::endl ;
  }
  theStr << "</table>" << std::endl ;
  theStr << "<hr>" << std::endl ;

  ParseText( const_cast< char * >( theStr.str().c_str() ) ) ;

  return true ;
}

//=============================================================================
// Load buffer in text display without clearing it first
//=============================================================================
Bool_t PageDescriptionTextView::LoadBuffer(const char *txtbuf) {
  // Load text from a text buffer. Return false in case of failure.
  if (!txtbuf || !strlen(txtbuf)) {
    return kFALSE;
  }

  std::string theBuffer( txtbuf ) ;

  // Replace html links with corresponding HTML code
  boost::xpressive::mark_tag link( 1 ) ;
  boost::xpressive::sregex html = "http://" >>
    ( link = *~boost::xpressive::_s ) ;
  boost::xpressive::smatch what ;
  std::string linkName ;
  if ( boost::xpressive::regex_search( theBuffer , what , html ) ) {
    linkName = "http://" + what[ link ] ;
  }

  // Replace offline names by online names
  std::string linkNameOnline( linkName ) ;
  boost::algorithm::replace_all( linkNameOnline , "lbtwiki.cern.ch" ,
                                 "twiki.lbdaq.cern.ch" ) ;

  std::string
    newLink = "<a href=\"" + linkNameOnline + "\">" + linkName + "</a>" ;
  boost::algorithm::replace_all( theBuffer , linkName , newLink ) ;

  boost::algorithm::replace_all( theBuffer , "\n" , "<br />\n" ) ;
  theBuffer += "\n" ;

  ParseText( const_cast< char * >( theBuffer.c_str() ) ) ;

  return kTRUE ;
}

//=============================================================================
// Compute Virtual size
//=============================================================================
void TGHtml::ComputeVirtualSize() {
  fVirtualSize = TGDimension(0, 500);
}

//=============================================================================
// Click on text
//=============================================================================
Bool_t PageDescriptionTextView::HandleButton( Event_t * event ) {
  // Only on Linux for the moment
#ifndef WIN32
  if ( ( event -> fType == kButtonPress ) && ( event -> fCode == 1 ) ) {
    const char * link ;
    link = GetHref( event -> fX , event -> fY + fVisible.fY ) ;
    if ( 0 != link ) {
      // Check if firefox is already running:
      bool running = false ;
      FILE * fp ;
      fp = popen( "pgrep -u `whoami` firefox" , "r" ) ;
      if ( fp == NULL )
        std::cerr << "Could not launch firefox" << std::endl ;
      else {
        char line[256] ;
        while ( fgets( line , sizeof(line) , fp ) ) {
          running = true ;
        }
      }
      pclose( fp ) ;

      if ( ! running ) {
        if ( 0 == fork() ) {
          execlp( "firefox" , "firefox" , link , NULL ) ;
        }
      }
      else
        if ( 0 == fork() ) {
          std::string foxargs( link ) ;
          foxargs = "openURL(" + foxargs + ")" ;
          execlp( "firefox" , "firefox" , "-remote" , foxargs.c_str()
                  , NULL ) ;
        }
    }
  }
#endif
  return kTRUE ;
}

//=============================================================================
// Compute Virtual size
//=============================================================================
void TGHtml::FreeColor(ColorStruct_t *color) {
  //  gVirtualX->FreeColor(gClient->GetDefaultColormap(), color->fPixel);
  delete color;
}
