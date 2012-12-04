// Include files

// Event
#include "Event/GenCollision.h"

// Generators
#include "Generators/IProductionTool.h"

// local
#include "LbPythia8/LbLHAupFortran.h"

//-----------------------------------------------------------------------------
// Implementation file for class : LbLHAupFortran
//
// 2012-11-30 : Patrick Robbe
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
LbLHAupFortran::LbLHAupFortran(  ) : m_productionTool( 0 ) { }

//=============================================================================
// Destructor
//=============================================================================
LbLHAupFortran::~LbLHAupFortran() {} 

//=============================================================================
// Initialization routine
//=============================================================================
bool LbLHAupFortran::setInit() {
  if ( ! fillHepRup() ) return false ;
  
  setBeamA( Pythia8::heprup_.idbmup[0] , 
            Pythia8::heprup_.ebmup[0] ,
            Pythia8::heprup_.pdfgup[0] ,
            Pythia8::heprup_.pdfsup[0] ) ;
  setBeamB( Pythia8::heprup_.idbmup[1] , 
            Pythia8::heprup_.ebmup[1] , 
            Pythia8::heprup_.pdfgup[1] ,
            Pythia8::heprup_.pdfsup[1] ) ;
  setStrategy( Pythia8::heprup_.idwtup ) ;

  for ( int ip = 0 ; ip < Pythia8::heprup_.nprup ; ++ip ) {
    double xsec = max( 1e-10 , Pythia8::heprup_.xsecup[ ip ] ) ;
    addProcess( Pythia8::heprup_.lprup[ ip ] , xsec , 
                Pythia8::heprup_.xerrup[ ip ] , 
                Pythia8::heprup_.xmaxup [ ip ] ) ;
  }
  
  eBeamA = Pythia8::heprup_.ebmup[ 0 ] ;
  eBeamB = Pythia8::heprup_.ebmup[ 1 ] ;
  return true ;
}

//=============================================================================
// Event routine
//=============================================================================
bool LbLHAupFortran::setEvent( int idProcIn ) {  
  Pythia8::hepeup_.idprup = idProcIn ;

  if ( ! fillHepEup() ) return false ;

  setProcess( Pythia8::hepeup_.idprup , 
              Pythia8::hepeup_.xwgtup , 
              Pythia8::hepeup_.scalup , 
              Pythia8::hepeup_.aqedup , 
              Pythia8::hepeup_.aqcdup ) ;
  
  for ( int ip = 0 ; ip < Pythia8::hepeup_.nup ; ++ip ) 
    addParticle( Pythia8::hepeup_.idup[ ip ] , 
                 Pythia8::hepeup_.istup[ ip ] ,
                 Pythia8::hepeup_.mothup[ ip ][ 0 ] ,
                 Pythia8::hepeup_.mothup[ ip ][ 1 ] ,
                 Pythia8::hepeup_.icolup[ ip ][ 0 ] , 
                 Pythia8::hepeup_.icolup[ ip ][ 1 ] ,
                 Pythia8::hepeup_.pup[ ip ][ 0 ] ,
                 Pythia8::hepeup_.pup[ ip ][ 1 ] ,
                 Pythia8::hepeup_.pup[ ip ][ 2 ] ,
                 Pythia8::hepeup_.pup[ ip ][ 3 ] ,
                 Pythia8::hepeup_.pup[ ip ][ 4 ] ,
                 Pythia8::hepeup_.vtimup[ ip ] , 
                 Pythia8::hepeup_.spinup[ ip ] ) ;
  
  setPdf( Pythia8::hepeup_.idup[ 0 ] , 
          Pythia8::hepeup_.idup[ 1 ] , 
          Pythia8::hepeup_.pup[ 0 ][ 3 ] / eBeamA ,
          Pythia8::hepeup_.pup[ 1 ][ 3 ] / eBeamB , 
          0. , 0. , 0. , false ) ;

  setIdX( Pythia8::hepeup_.idup[ 0 ] , 
          Pythia8::hepeup_.idup[ 1 ] , 
          Pythia8::hepeup_.pup[ 0 ][ 3 ] / eBeamA ,
          Pythia8::hepeup_.pup[ 1 ][ 3 ] / eBeamB ) ;
  
  return true ;
}

//===============================================================
// initialization
//===============================================================
bool LbLHAupFortran::fillHepRup( ) {
  return true ;
}

//===============================================================
// event generation
//===============================================================
bool LbLHAupFortran::fillHepEup( ) {
  HepMC::GenEvent ev ;
  LHCb::GenCollision col ;
  m_productionTool -> generateEvent( &ev , &col ) ;
  return true ;
}



