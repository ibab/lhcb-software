// $Id: KnownProblemList.cpp,v 1.1 2010-04-22 10:18:00 robbep Exp $
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
  m_problemDbServerAddress( address ) {
  retrieveListOfProblems() ;
}

//=============================================================================
// Destructor
//=============================================================================
KnownProblemList::~KnownProblemList() { }

//=============================================================================
// Retrieve current problems in problem DB
//=============================================================================
void KnownProblemList::retrieveListOfProblems() {
  ProblemDB pbdb( m_problemDbServerAddress ) ;
  std::vector< std::vector< std::string > > problems ;
  pbdb.getListOfOpenedProblems( problems ) ;
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
}
