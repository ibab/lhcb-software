// $Id: PageDescriptionTextView.cpp,v 1.1 2010-04-25 18:10:27 robbep Exp $
// Include files

// local
#include "PageDescriptionTextView.h"

// boost
#include <boost/xpressive/xpressive.hpp>
#include <boost/algorithm/string.hpp>

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
  TGTextView( p , w , h ) ,
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
	 << " subsystem: " << std::endl ;
  for ( std::vector< std::vector< std::string > >::iterator it = problems.begin() ;
	it != problems.end() ; ++it ) {
    std::vector< std::string > vec = (*it) ;
    id = atoi( vec[ 3 ].c_str() ) ;
    theStr << ( ( vec[ 0 ] + ": " + vec[ 4 ] ).c_str() ) << std::endl ;
  }
  
  theStr << "------------------------------------------------" << std::endl ;

  fText -> LoadBuffer( theStr.str().c_str() ) ;
  Update() ;

  return true ;
}

//=============================================================================
// Load buffer in text display without clearing it first
//=============================================================================
Bool_t PageDescriptionTextView::AddToBuffer(const char *txtbuf) {
  // Load text from a text buffer. Return false in case of failure.
  if (!txtbuf || !strlen(txtbuf)) {
    return kFALSE;
  }

  TString currentBuffer = fText -> AsString() ;
  TString newBuffer( txtbuf ) ;

  currentBuffer += newBuffer ;

  Clear() ;
  fText -> LoadBuffer( currentBuffer.Data() );
  Update() ;
  return kTRUE ;
}
