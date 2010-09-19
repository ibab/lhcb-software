// $Id: KnownProblemList.cpp,v 1.3 2010-09-19 18:49:53 robbep Exp $
// Include files

// local
#include "KnownProblemList.h"

// STL
#include <cstdlib>

// from Presenter
#include "ProblemDB.h"

//-----------------------------------------------------------------------------
// Implementation file for class : KnownProblemList
//
// 2010-04-19 : Patrick Robbe
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
KnownProblemList::KnownProblemList( const TGWindow * p ,
				    const std::string & address ) : 
  TGListBox( p ) ,
  m_problemDbServerAddress( address ) { }

//=============================================================================
// Destructor
//=============================================================================
KnownProblemList::~KnownProblemList() { }

//=============================================================================
// Retrieve current problems in problem DB
//=============================================================================
bool KnownProblemList::retrieveListOfProblems( const std::string& system ) {
  ProblemDB pbdb( m_problemDbServerAddress ) ;
  std::vector< std::vector< std::string > > problems ;
  pbdb.getListOfProblems( problems , system , 0 ) ;

  if ( problems.empty() ) return false ;

  RemoveAll() ;

  int id ;
  Pixel_t orange , green , red ;
  gClient -> GetColorByName( "orange" , orange ) ;
  gClient -> GetColorByName( "green"  , green  ) ;
  gClient -> GetColorByName( "red"    , red    ) ;
  for ( std::vector< std::vector< std::string > >::iterator it = problems.begin() ;
	it != problems.end() ; ++it ) {
    std::vector< std::string > vec = (*it) ;
    id = atoi( vec[ 3 ].c_str() ) ;
    TGTextLBEntry * ent = new TGTextLBEntry( this -> GetContainer() , 
					     new TGString( ( vec[ 0 ] + ": " + vec[ 4 ] ).c_str() ) ) ;
    AddEntry( ent , new TGLayoutHints( kLHintsExpandX | kLHintsTop ) ) ;
    if ( vec[ 1 ] == "Ok") 
      ent -> SetBackgroundColor( green  ) ;
    else if ( vec[ 1 ] == "Minor" ) 
      ent -> SetBackgroundColor( orange ) ;
    else if ( vec[ 1 ] == "Severe" ) 
      ent -> SetBackgroundColor( red ) ;
  }

  fLbc->ClearViewPort()  ;

  // Resize according to the number of entries
  int resizeMin = 1 ;
  int resizeMax = 4 ;
  if ( system == "" ) {
    resizeMin = 2 ;
    resizeMax = 15 ;
  }

  int nResize = 1 + problems.size() ;
  if ( nResize <= resizeMin ) nResize = resizeMin ;
  else if ( nResize >= resizeMax ) nResize = resizeMax ;
  Resize( GetDefaultWidth( ) , nResize * GetItemVsize() ) ;  

  return true ;
}
