// $Id: PageDescriptionTextView.cpp,v 1.6 2010-05-18 17:13:30 robbep Exp $
// Include files

// local
#include "PageDescriptionTextView.h"

// boost
#include <boost/xpressive/xpressive.hpp>
#include <boost/algorithm/string.hpp>

// STL
#include <iostream>

// from Presenter
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
						  const std::string & address ) : 
  TGHtml( p , w , h ) ,
  m_problemDbServerAddress( address ) { }

//=============================================================================
// Destructor
//=============================================================================
PageDescriptionTextView::~PageDescriptionTextView() { }

//=============================================================================
// Retrieve current problems in problem DB
//=============================================================================
bool PageDescriptionTextView::retrieveListOfProblems( const std::string& pageName ) {
  // Get system name from page name
  // but first erase all spaces
  std::string pname( pageName ) ;
  boost::algorithm::erase_all(pname , " "); 
  boost::xpressive::mark_tag system( 1 ) ;
  boost::xpressive::sregex pb = "/Shift/" 
    >> (system=*~(boost::xpressive::set =':')) >> ":"  ;
  boost::xpressive::smatch what ;
  std::string systemName ;
  if ( boost::xpressive::regex_search( pname , what , pb ) ) {
    systemName = what[ system ] ;
    if ( systemName.find( "L0" ) != std::string::npos ) 
      systemName = "L0" ; 
  } else return false ;
  
  ProblemDB pbdb( m_problemDbServerAddress ) ;
  std::vector< std::vector< std::string > > problems ;
  pbdb.getListOfOpenedProblems( problems , systemName ) ;

  if ( problems.empty() ) return false ;

  int id ;
  Clear() ;

  std::ostringstream theStr ;

  theStr << "Problems reported in the problem database for the " << systemName
	 << " subsystem: <br />" << std::endl ;
  theStr << "<table border=0 cellspacing=0 width=100%>" << std::endl ;
  for ( std::vector< std::vector< std::string > >::iterator it = problems.begin() ;
	it != problems.end() ; ++it ) {
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
    
    theStr << "<tr><td bgcolor=\"" << cellColor << "\">" << ( ( vec[ 0 ] + ": " + vec[ 4 ] ).c_str() ) 
	   << "</td></tr>" << std::endl ;
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
  boost::algorithm::replace_all( theBuffer , "\n" , "<br />\n" ) ;
  theBuffer += "\n" ;

  // Replace html links with corresponding HTML code
  boost::xpressive::mark_tag link( 1 ) ;
  boost::xpressive::sregex html = "http://" >> ( link = *~boost::xpressive::_s ) ;
  boost::xpressive::smatch what ;
  std::string linkName ;
  if ( boost::xpressive::regex_search( theBuffer , what , html ) ) {
    linkName = "http://" + what[ link ] ;
  }

  std::string newLink = "<a href=\"" + linkName + "\">" + linkName + "</a>" ; 
  boost::algorithm::replace_all( theBuffer , linkName , newLink ) ;

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
    link = GetHref( event -> fX , event -> fY ) ;
    if ( 0 != link ) {
      //      if ( 0 == fork() ) {
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
