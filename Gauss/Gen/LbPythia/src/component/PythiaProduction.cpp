// $Id: PythiaProduction.cpp,v 1.2 2005-11-04 11:02:50 robbep Exp $
// Include files 

// local
#include "PythiaProduction.h"

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/ParticleProperty.h"

// from Event
#include "Event/HardInfo.h"

// HepMC
#include "HepMC/IO_HEPEVT.h"
#include "HepMC/IO_Ascii.h"
#include "HepMC/HEPEVT_Wrapper.h"

// local
#include "Generators/StringParse.h"
#include "Generators/IBeamTool.h"
#include "LbPythia/Pythia.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PythiaProduction
//
// 2005-08-16 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<PythiaProduction>          s_factory ;
const        IToolFactory& PythiaProductionFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PythiaProduction::PythiaProduction( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : GaudiTool ( type, name , parent ) ,
    m_frame( "3MOM" )  ,
    m_beam( "p+" )     ,
    m_target( "p+" )   ,
    m_win( 0. )        ,
    m_eventListingLevel( -1 ) ,
    m_initializationListingLevel( 1 ) ,
    m_finalizationListingLevel( 1 ) ,
    m_pythiaListingFileName( "" ) ,
    m_pythiaListingUnit( 0 ) ,
    m_variableEnergy( false ) {
    declareInterface< IProductionTool >( this ) ;
    declareProperty( "BasePythiaCommand" , m_basePythiaCommandVector ) ;
    declareProperty( "UserPythiaCommand" , m_userPythiaCommandVector ) ;
    declareProperty( "BeamToolName" , m_beamToolName = "CollidingBeams" ) ;
}

//=============================================================================
// Destructor 
//=============================================================================
PythiaProduction::~PythiaProduction( ) { ; }

//=============================================================================
// Initialize method
//=============================================================================
StatusCode PythiaProduction::initialize( ) {
  StatusCode sc = GaudiTool::initialize( ) ;
  if ( sc.isFailure() ) return sc ;

  // Obtain beam tool
  m_beamTool = tool< IBeamTool >( m_beamToolName , this ) ;
  
  // Initialize output
  if ( msgLevel( MSG::DEBUG ) ) {
    if ( 12345 != Pythia::pydat1().mstu( 12 ) ) 
      Pythia::pydat1().mstu( 12 ) = 1 ;
    Pythia::pydat1().mstu( 13 ) = 1 ;
    Pythia::pydat1().mstu( 25 ) = 1 ;
    Pythia::pypars().mstp( 122 ) = 1 ;
  } else {
    Pythia::pydat1().mstu( 12 ) = 12345 ;
    Pythia::pydat1().mstu( 13 ) = 0 ;
    Pythia::pydat1().mstu( 25 ) = 0 ;
    Pythia::pypars().mstp( 122 ) = 0 ;
    m_initializationListingLevel = -1 ;
  }

  // Suppress pdflib output
  if ( ! msgLevel( MSG::DEBUG ) ) Pythia::PdfLib_Init() ;

  // Set User process to 0 for normal Pythia to be overriden for
  // specific generation
  Pythia::SetUserProcess( 0 ) ;

  // Prepare the PYINIT call by setting the beam parameters if necessary
  std::transform( m_frame.begin() , m_frame.end() , m_frame.begin() , 
                  tolower ) ;
  if ( ( "4mom" == m_frame ) || ( "5mom" == m_frame ) ) 
    return Error( "This FRAME is not yet implemented" ) ;
  
  if ( "3mom" == m_frame ) {
    Hep3Vector pBeam1 , pBeam2 ;
    m_beamTool -> getMeanBeams( pBeam1 , pBeam2 ) ;
    // Pythia Units are GeV
    Pythia::SetBeam( pBeam1.x() / GeV , pBeam1.y() / GeV , pBeam1.z() /GeV ,
                     pBeam2.x() / GeV , pBeam2.y() / GeV , pBeam2.z() /GeV ) ;

    Pythia::pypars().mstp( 171 ) = 1 ;  // new energy given for each event 
    Pythia::pypars().mstp( 172 ) = 1 ;  // event generated at requested energy
    m_variableEnergy = true ;
  }
  
  Pythia::pydat3().mdme( 4178 , 1 ) = -1 ; // like in egpyinit  

  // read base Pythia command vector from job options
  sc = parsePythiaCommands( m_basePythiaCommandVector ) ;
  if ( ! sc.isSuccess( ) ) 
    return Error( "Unable to read base Pythia commands" , sc ) ;

  // Read user pythia command vector from option file
  sc = parsePythiaCommands( m_userPythiaCommandVector ) ;
  if ( ! sc.isSuccess() ) 
    return Error( "Unable to read User Pythia Commands" , sc ) ;

  // Now call pyinit and set listing
  // if file already exist, delete it
  std::remove( m_pythiaListingFileName.c_str() ) ;
  Pythia::InitPyBlock( m_pythiaListingUnit , m_pythiaListingFileName ) ;

  Pythia::PyInit( m_frame, m_beam, m_target, m_win ) ;

  if ( m_initializationListingLevel != -1 ) 
    Pythia::PyList( m_initializationListingLevel ) ;

  // Set size of common blocks in HEPEVT: note these correspond to stdhep
  HepMC::HEPEVT_Wrapper::set_sizeof_int( 4 ) ;
  HepMC::HEPEVT_Wrapper::set_sizeof_real( 8 ) ;
  HepMC::HEPEVT_Wrapper::set_max_number_entries( 10000 ) ; 

  // Reset forced fragmentation flag
  Pythia::pydat1().mstu( 150 ) = 0 ;

  // print out Pythia settings
  printPythiaParameter() ;

  return sc ;
}

//=============================================================================
//   Function called to generate one event with Pythia
//=============================================================================
StatusCode PythiaProduction::generateEvent( HepMC::GenEvent * theEvent , 
                                            HardInfo * theInfo ) {
  // Set beam parameters if variable energy
  if ( m_variableEnergy ) {
    Hep3Vector pBeam1 , pBeam2 ;
    m_beamTool -> getBeams( pBeam1 , pBeam2 ) ;
    // PYTHIA Units are GeV
    Pythia::SetBeam( pBeam1.x() / GeV , pBeam1.y() / GeV , pBeam1.z() / GeV ,
                     pBeam2.x() / GeV , pBeam2.y() / GeV , pBeam2.z() / GeV ) ;
  }
  
  // Generate Event
  Pythia::PyEvnt( ) ;

  // Update event counter
  ++m_nEvents ;
  
  // Debugging output: print each event if required
  if ( m_eventListingLevel >= 0 ) Pythia::PyList( m_eventListingLevel ) ;

  // Convert to HepEvt format
  Pythia::LunHep( 1 ) ;
  
  // Convert event in HepMC Format
  HepMC::IO_HEPEVT theHepIO ;
  if ( ! theHepIO.fill_next_event( theEvent ) ) 
    return Error( "Could not fill HepMC event" ) ;

  theEvent -> set_signal_process_id( Pythia::pypars().msti( 1 ) ) ;
  
  // Retrieve hard process information
  hardProcessInfo( theInfo ) ;

  return StatusCode::SUCCESS ;
}

//=============================================================================
// Set stable the given particle in Pythia
//=============================================================================
void PythiaProduction::setStable( const ParticleProperty * thePP ) {
  int pythiaId = thePP -> pythiaID() ;
  if ( 0 != pythiaId ) {
    int kc = Pythia::PyComp( pythiaId ) ;
    Pythia::pydat3().mdcy( kc , 1 ) = 0 ;
  }
}

//=============================================================================
// Retrieve the Hard scatter information
//=============================================================================
void PythiaProduction::hardProcessInfo(HardInfo * hardInfo) {
  hardInfo->setSHat( Pythia::pypars().pari(14) );
  hardInfo->setTHat( Pythia::pypars().pari(15) );
  hardInfo->setUHat( Pythia::pypars().pari(16) );
  hardInfo->setPtHat( Pythia::pypars().pari(17) );
  hardInfo->setX1Bjorken( Pythia::pypars().pari(33) );
  hardInfo->setX2Bjorken( Pythia::pypars().pari(34) );

  debug() << "Hard process = " 
          << Pythia::pypars().msti(1)  << " "
          << Pythia::pypars().pari(14) << " " 
          << Pythia::pypars().pari(15) << " " 
          << Pythia::pypars().pari(16) << " " 
          << Pythia::pypars().pari(17) << " " 
          << Pythia::pypars().pari(33) << " " 
          << Pythia::pypars().pari(34)
          << endreq ;
} 

//=============================================================================
// Finalize method
//=============================================================================
StatusCode PythiaProduction::finalize( ) {
  Pythia::PyStat( m_finalizationListingLevel ) ;
  
  if ( ! msgLevel( MSG::DEBUG ) ) Pythia::PdfLib_End( ) ;
  
  std::string delcmd( "rm -f " ) ;
  delcmd += "pdflib_init.tmp" ;
#ifndef WIN32
  system( delcmd.c_str( ) ) ;
#endif
  
  return GaudiTool::finalize( ) ;
}  

//=============================================================================
// Parse Pythia commands stored in a vector
//=============================================================================
StatusCode PythiaProduction::parsePythiaCommands( const CommandVector & 
                                                  theCommandVector ) {
  //
  // Parse Commands and Set Values from Properties Service...
  //
  CommandVector::const_iterator iter ;
  for ( iter = theCommandVector.begin() ; theCommandVector.end() != iter ; 
        ++iter ) {
    debug() << " Command is: " << (*iter) << endmsg ;
    StringParse mystring( *iter ) ;
    std::string block = mystring.piece(1);
    std::string entry = mystring.piece(2);
    std::string str   = mystring.piece(3);
    int    int1  = mystring.intpiece(3);
    int    int2  = mystring.intpiece(4);
    int    int3  = mystring.intpiece(5);
    int    int4  = mystring.intpiece(6);
    double fl1   = mystring.numpiece(4);
    double fl2   = mystring.numpiece(5);
    double fl0   = mystring.numpiece(3);
    
    // Note that Pythia needs doubles hence the convert here
    debug() << block << " block  " << entry << " item  " << int1 
            << "  value " << fl1 << endmsg ;
    if ( "pyinit" == block ) 
      if      ( "pbar"    == entry ) m_beam                       = "pbar-" ; 
      else if ( "win"     == entry ) m_win                        = fl0   ; 
      else if ( "pylisti" == entry ) m_initializationListingLevel = int1  ; 
      else if ( "pyliste" == entry ) m_eventListingLevel          = int1  ; 
      else if ( "pystatf" == entry ) m_finalizationListingLevel   = int1  ; 
      else if ( "output"  == entry ) {
        m_pythiaListingFileName = str                 ;
        m_pythiaListingUnit     = 49                  ;
        Pythia::pydat1().mstu(11)       = m_pythiaListingUnit ; 
      }
      else return Error(std::string("PYTHIA ERROR, entry PYINIT has PBAR ")+
                        std::string("PYLISTI PYLISTE OUTPUT DUMPR AND WIN: ")+ 
                        std::string("YOU HAVE SPECIFIED ") + 
                        std::string( entry ) ) ;
    else if ( "pysubs" == block )  
      if      ( "msel" == entry ) Pythia::pysubs().msel()              = int1 ;
      else if ( "msub" == entry ) Pythia::pysubs().msub( int1 )        = int2 ;
      else if ( "ckin" == entry ) Pythia::pysubs().ckin( int1 )        = fl1  ;
      else if ( "kfin" == entry ) Pythia::pysubs().kfin( int1 , int2 ) = int3 ;
      else return Error( std::string( "PYTHIA ERROR, block PYSUBS has MSEL" ) +
                         std::string( ", MSUB, KFIN AND CKIN: YOU HAVE " ) + 
                         std::string( "SPECIFIED " ) + std::string( entry ) ) ;
    else if ( "pypars" == block ) 
      if      ( "mstp" == entry ) Pythia::pypars().mstp( int1 ) = int2 ;
      else if ( "msti" == entry ) Pythia::pypars().msti( int1 ) = int2 ;
      else if ( "parp" == entry ) Pythia::pypars().parp( int1 ) = fl1  ;
      else if ( "pari" == entry ) Pythia::pypars().pari( int1 ) = fl1  ; 
      else return Error( std::string( "PYTHIA ERROR, block PYPARS has MSTP," )+
                         std::string( "PARP, MSTI AND PARI: YOU HAVE " ) +
                         std::string( "SPECIFIED " ) + std::string( entry ) );
    else if ( "pydat1" == block ) 
      if      ( "mstu" == entry ) Pythia::pydat1().mstu( int1 ) = int2 ;
      else if ( "mstj" == entry ) Pythia::pydat1().mstj( int1 ) = int2 ;
      else if ( "paru" == entry ) Pythia::pydat1().paru( int1 ) = fl1  ; 
      else if ( "parj" == entry ) Pythia::pydat1().parj( int1 ) = fl1  ;
      else return Error( std::string( "PYTHIA ERROR, block PYDAT1  HAS ")+
                         std::string( "MSTU, MSTJ, PARU AND PARJ: YOU HAVE ") +
                         std::string( "SPECIFIED " ) + std::string( entry ) );
    else if ( "pydat2" == block ) 
      if      ( "kchg" == entry ) Pythia::pydat2().kchg( int1 , int2 ) = int3 ;
      else if ( "pmas" == entry ) Pythia::pydat2().pmas( int1 , int2 ) = fl2  ;
      else if ( "parf" == entry ) Pythia::pydat2().parf( int1 )        = fl1  ;
      else if ( "vckm" == entry ) Pythia::pydat2().vckm( int1 , int2 ) = fl2  ;
      else return Error( std::string("PYTHIA ERROR, block PYDAT2  HAS KCHG,")+
                         std::string("PMAS, PARF AND VCKM: YOU HAVE ")+
                         std::string("SPECIFIED " ) + std::string(entry )) ;
    else if ( "pydat3" == block ) 
      if      ( "mdcy" == entry ) 
        Pythia::pydat3().mdcy(Pythia::PyComp( int1 ) , int2 ) = int3 ; 
      else if ( "mdme" == entry ) Pythia::pydat3().mdme( int1 , int2 ) = int3 ;
      else if ( "brat" == entry ) Pythia::pydat3().brat( int1 )        = fl1  ;
      else if ( "kfdp" == entry ) Pythia::pydat3().kfdp( int1 , int2 ) = int3 ;
      else return Error( std::string("PYTHIA ERROR, block PYDAT3  HAS KFDP, ")+
                         std::string("MDCY, BRAT AND MDME : YOU HAVE " ) + 
                         std::string("SPECIFIED ") + std::string( entry ) ) ;
    else if ( "pydatr" == block ) 
      if      ( "mrpy" == entry ) Pythia::pydatr().mrpy( int1 ) = int2 ;
      else if ( "rrpy" == entry ) Pythia::pydatr().rrpy( int1 ) = fl1  ;
      else return Error( std::string( "PYTHIA ERROR, block PYDATR HAS MRPY " )+
                         std::string( "AND RRPY : YOU HAVE SPECIFIED " ) +
                         std::string( entry ) ) ;
    else if ( "pymssm" == block )
      if      ( "imss" == entry ) Pythia::pymssm().imss( int1 ) = int2 ;
      else if ( "rmss" == entry ) Pythia::pymssm().rmss( int1 ) = fl1  ;
      else return Error( std::string( "PYTHIA ERROR, block PYMSSM has IMSS " )+
                         std::string( "AND RMSS: YOU HAVE SPECIFIED " ) +
                         std::string( entry ) ) ;
    else if ( "pyint2" == block ) 
      if      ( "iset" == entry ) Pythia::pyint2().iset( int1 )        = int2 ;
      else if ( "kfpr" == entry ) Pythia::pyint2().kfpr( int1 , int2 ) = int3 ;
      else if ( "coef" == entry ) Pythia::pyint2().coef( int1 , int2 ) = fl2  ;
      else if ( "icol" == entry ) 
        Pythia::pyint2().icol( int1 , int2 , int3 ) = int4 ;
      else return Error( std::string( "PYTHIA ERROR, block PYINT2 has " ) +
                         std::string( "ISET KFPR COEF AND ICOL: YOU HAVE " ) +
                         std::string( "SPECIFIED ") + std::string( entry ) ) ;
    else return Error( std::string( " ERROR in PYTHIA PARAMETERS   " ) +
                       std::string( block ) +
                       std::string( " is and invalid common block name !" ) ) ;
  }
  
  return StatusCode::SUCCESS ;
}

//=============================================================================
// Print Pythia parameters
//=============================================================================
void PythiaProduction::printPythiaParameter( ) {  
  debug() << "****************************************************" << endmsg ;
  debug() << "**        Pythia parameters for this job            " << endmsg ;
  debug() << "**                                                  " << endmsg ;
  debug() << "** MSTP(2) ( alpha_s order )                      : " 
          << Pythia::pypars().mstp( 2 )   << endmsg ;
  debug() << "** MSTP(33) ( k factors )                         : "  
          << Pythia::pypars().mstp( 33 )  << endmsg ;
  debug() << "** MSTP(128) ( doc. lines )                       : " 
          << Pythia::pypars().mstp( 128 ) << endmsg ;
  debug() << "** MSTP(82) ( multiple interactions )             : "
          << Pythia::pypars().mstp( 82 )  << endmsg ;
  debug() << "** MSTP(52) ( PDF 1 = Pythia, 2 = PDFLIB )        : "  
          << Pythia::pypars().mstp( 52 )  << endmsg ;
  debug() << "** MSTP(51) ( PDF Function Code )                 : "
          << Pythia::pypars().mstp( 51 )  << endmsg ;
  debug() << "** PARP(82) ( scale for mult inter )              : "
          << Pythia::pypars().parp( 82 )  << endmsg ;
  debug() << "** PARP(89) ( reference energy scale )            : "
          << Pythia::pypars().parp( 89 )  << endmsg ;
  debug() << "** PARP(90) ( power of energy resc. term )        : " 
          << Pythia::pypars().parp( 90 )  << endmsg ;
  debug() << "** PARJ(13) ( prob. that heavy meson has spin 1 ) : "
          << Pythia::pydat1().parj( 13 )  << endmsg ;
  debug() << "** PARJ(14) ( prob. for spin=0 meson to be produced" << endmsg ; 
  debug() << "**            with ang. mom= 1 if total spin=1 )  : "
          << Pythia::pydat1().parj( 14 )  << endmsg ;
  debug() << "** PARJ(15) ( prob. for spin=1 meson to be produced " << endmsg ;
  debug() << "**            with ang. mom= 1 if total spin=0 )  : "
          << Pythia::pydat1().parj( 15 )  << endmsg ;
  debug() << "** PARJ(16) ( prob. for spin=1 meson to be produced " << endmsg ;
  debug() << "**            with ang. mom= 1 if total spin=1 )  : "
          << Pythia::pydat1().parj( 16 )  << endmsg ;
  debug() << "** PARJ(17) ( prob. for spin=1 meson to be produced " << endmsg ;
  debug() << "**            with ang. mom= 1 if total spin=2 )  : "
          << Pythia::pydat1().parj( 17 )  << endmsg ;
  debug() << "** MSTU(150) ( Forced fragmentation flag )        : "
          << Pythia::pydat1().mstu( 150 ) << endmsg ;
  debug() << "** MSTJ(30) ( Forced fragmentation flag )         : "
          << Pythia::pydat1().mstj( 30 )  << endmsg ;
  debug() << "** MSTJ(31) ( Forced fragmentation flag )         : "
          << Pythia::pydat1().mstj( 31 )  << endmsg ;
  debug() << "** MSTJ(32) ( Forced fragmentation flag )         : "
          << Pythia::pydat1().mstj( 32 )  << endmsg ;
  debug() << "** MSTJ(33) ( Forced fragmentation flag )         : "
          << Pythia::pydat1().mstj( 33 )  << endmsg ;
  debug() << "** MSTJ(34) ( Forced fragmentation flag )         : "
          << Pythia::pydat1().mstj( 34 )  << endmsg ;
  debug() << "** MSTJ(26) ( B0 mixing in Pythia, must be 0 )    : "
          << Pythia::pydat1().mstj( 26 )  << endmsg ;
  debug() << "** PARJ(33) ( cut off for fragmentation )         : "
          << Pythia::pydat1().parj( 33 )  << endmsg;
  debug() << "** MSEL ( processes )                             : "
          << Pythia::pysubs().msel()      << endmsg ;
  debug() << "** Activated processes                            : " << endmsg ;
  for ( int i = 1 ; i <= 500 ; ++i ) if ( 1 == Pythia::pysubs() . msub( i ) ) 
    debug() << "** --- Process " << i << endmsg ;
  debug() << "**                                                  " << endmsg ;
  debug() << "****************************************************" << endmsg ;
}

//=============================================================================
// Turn on fragmentation in Pythia
//=============================================================================
void PythiaProduction::turnOnFragmentation( ){
  Pythia::pydat1().mstj( 1 ) = 1 ;
}

//=============================================================================
// Turn off fragmentation in Pythia
//=============================================================================
void PythiaProduction::turnOffFragmentation( ){
  Pythia::pydat1().mstj( 1 ) = 0 ;
}

//=============================================================================
// Save parton event
//=============================================================================
void PythiaProduction::savePartonEvent( HepMC::GenEvent * /* theEvent */ ) {
  Pythia::PyEdit( 21 ) ;
}

//=============================================================================
// Load parton event
//=============================================================================
void PythiaProduction::retrievePartonEvent( HepMC::GenEvent * /* theEvent */ ) {
  Pythia::PyEdit( 22 ) ;
}

//=============================================================================
// Hadronize Pythia event
//=============================================================================
StatusCode PythiaProduction::hadronize( HepMC::GenEvent * theEvent , 
                                  HardInfo * theInfo) {
  Pythia::PyExec( ) ;

  // Debugging output: print each event if required
  if ( m_eventListingLevel >= 0 ) Pythia::PyList( m_eventListingLevel ) ;
  
  // Convert to HepEvt format
  Pythia::LunHep( 1 ) ;
  
  // Convert to HepMC format
  HepMC::IO_HEPEVT theHepIO ;
  if ( ! theHepIO.fill_next_event( theEvent ) ) 
    return Error( "Could not fill HepMC event" ) ;
  
  theEvent -> set_signal_process_id( Pythia::pypars().msti( 1 ) ) ;
  
  // Retrieve hard process information
  hardProcessInfo( theInfo ) ;
  
  return StatusCode::SUCCESS ;
}

