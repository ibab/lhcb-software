// $Id: PythiaProduction.cpp,v 1.5 2008-01-27 15:03:08 robbep Exp $

// Include files

// local
#include "LbPythia/Pythia.h"
#include "LbPythia/PythiaProduction.h"

// from SEAL
#include "SealBase/ShellEnvironment.h"
#include "SealBase/Filename.h"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/ParticleProperty.h"

// from Event
#include "Event/GenCollision.h"

// Generators 
#include "Generators/StringParse.h"
#include "Generators/IBeamTool.h"
#include "Generators/F77Utils.h"

// HepMC
#include "HepMC/IO_HEPEVT.h"
#include "HepMC/IO_Ascii.h"
#include "HepMC/HEPEVT_Wrapper.h"


// ============================================================================
/** @file 
 *  Implementation file for class PythiaProduction
 *
 *  @date 2005-08-16 
 *  @author Patrick Robbe
 */
//-----------------------------------------------------------------------------


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PythiaProduction::PythiaProduction( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : GaudiTool ( type, name , parent ) ,
    m_userProcess( 0 ) ,
    m_frame( "3MOM" )  ,
    m_beam( "p+" )     ,
    m_target( "p+" )   ,
    m_win( 0. )        ,
    //
    m_defaultSettings () ,
    m_commandVector   () ,
    m_pygive          () ,
    //
    m_variableEnergy( false ) ,
    //
    m_eventListingLevel          ( -1 ) ,
    m_eventListingLevel2         ( -1 ) ,
    m_initializationListingLevel ( 1  ) ,
    m_finalizationListingLevel   ( 1  ) ,
    m_pythiaListingFileName      ( "" ) ,
    m_pythiaListingUnit          ( 0  ) ,
    //
    m_particleDataUnit   ( 59 ) ,
    // no default value!
    m_particleDataOutput () , 
    // no default value!
    m_particleDataInput  () , 
    // no default value! 
    m_particleDataLevel  ( 0 ) ,
    // MSTU(1)/MSTU(2) for initialization PYLIST
    m_ini_mstu_1 ( 0 ) , 
    m_ini_mstu_2 ( 0 ) , 
    // MSTU(1)/MSTU(2) for "generateEvent" PYLIST
    m_eve_mstu_1 ( 0 ) , 
    m_eve_mstu_2 ( 0 ) , 
    // MSTU(1)/MSTU(2) for "hadronize" PYLIST
    m_had_mstu_1 ( 0 ) , 
    m_had_mstu_2 ( 0 ) , 
    // list of particles to be printed using PyList(12) 
    m_pdtlist    (   ) , 
    m_widthLimit ( 1.5e-6 * GeV ) 
{
  declareInterface< IProductionTool >( this ) ;
  declareProperty( "Commands" , m_commandVector ) ;
  declareProperty( "PygiveCommands" , m_pygive  ) ;
  declareProperty( "PDTList"        , m_pdtlist ) ;
  declareProperty( "BeamToolName" , m_beamToolName = "CollidingBeams" ) ;
  declareProperty( "WidthLimit" , m_widthLimit = 1.5e-6 * GeV ) ;
  declareProperty( "SLHADecayFile" , m_slhaDecayFile = "empty" ) ;

  // Set the default settings for Pythia here:
  m_defaultSettings.clear() ;
  m_defaultSettings.push_back( "pysubs msel 0" ) ;
  m_defaultSettings.push_back( "pysubs msub 11 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 12 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 13 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 28 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 53 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 68 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 91 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 92 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 93 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 94 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 95 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 86 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 87 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 88 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 89 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 106 1" ) ;
  m_defaultSettings.push_back( "pypars mstp 2 2" ) ;
  m_defaultSettings.push_back( "pypars mstp 33 3" ) ;
  m_defaultSettings.push_back( "pypars mstp 128 2" ) ;
  m_defaultSettings.push_back( "pypars mstp 82 3" ) ;
  m_defaultSettings.push_back( "pypars mstp 52 2" ) ;
  m_defaultSettings.push_back( "pypars mstp 51 10042" ) ;
  m_defaultSettings.push_back( "pypars parp 67 1.0" ) ;
  m_defaultSettings.push_back( "pypars parp 82 3.41" ) ;
  m_defaultSettings.push_back( "pypars parp 89 14000" ) ;
  m_defaultSettings.push_back( "pypars parp 90 0.162" ) ;
  m_defaultSettings.push_back( "pypars parp 85 0.33" ) ;
  m_defaultSettings.push_back( "pypars parp 86 0.66" ) ;
  m_defaultSettings.push_back( "pypars parp 91 1.0" ) ;
  m_defaultSettings.push_back( "pydat1 parj 13 0.750" ) ;
  m_defaultSettings.push_back( "pydat1 parj 14 0.162" ) ;
  m_defaultSettings.push_back( "pydat1 parj 15 0.018" ) ;
  m_defaultSettings.push_back( "pydat1 parj 16 0.054" ) ;
  m_defaultSettings.push_back( "pydat1 parj 17 0.090" ) ;
  m_defaultSettings.push_back( "pydat1 mstj 26 0" ) ;
  m_defaultSettings.push_back( "pydat1 parj 33 0.4" ) ;
} ;


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

  always()
    << "=================================================================="
    << endmsg;
  always()
    << "Using as production engine  " << this->type()
    << endmsg;
  always()
    << "=================================================================="
    << endmsg;

  // Obtain beam tool
  m_beamTool = tool< IBeamTool >( m_beamToolName , this ) ;

  // Name of PYSLHA file to read
  if ( "empty" != m_slhaDecayFile ) {
    if ( seal::ShellEnvironment().has( "DECFILESROOT" ) ) {
      std::string temp = m_slhaDecayFile ;
      m_slhaDecayFile = seal::ShellEnvironment().get( "DECFILESROOT" ) +
        "/dkfiles/" + temp ;
    }
    // Check if file exists
    seal::Filename shlaDecayFile( m_slhaDecayFile ) ;
    if ( ! shlaDecayFile.exists() ) 
      return Error( "The SLHA decay file does not exist" ) ;
    if ( ! shlaDecayFile.isReadable() ) 
      return Error( "The SLHA decay file cannot be read" ) ;
  }

  // Set size of common blocks in HEPEVT: note these correspond to stdhep
  HepMC::HEPEVT_Wrapper::set_sizeof_int( 4 ) ;
  HepMC::HEPEVT_Wrapper::set_sizeof_real( 8 ) ;
  HepMC::HEPEVT_Wrapper::set_max_number_entries( 10000 ) ;  

  return initializeGenerator() ;
}

//=============================================================================
// Part specific to generator initialization
//=============================================================================
StatusCode PythiaProduction::initializeGenerator( ) {
  StatusCode sc = StatusCode::SUCCESS ;
  
    // Initialize output
  if ( msgLevel( MSG::DEBUG ) ) {
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

  // Set User process to 0 for normal Pythia to be overriden for
  // specific generation
  Pythia::SetUserProcess( m_userProcess ) ;

  // Prepare the PYINIT call by setting the beam parameters if necessary
  std::transform( m_frame.begin() , m_frame.end() , m_frame.begin() , 
                  tolower ) ;
  if ( ( "4mom" == m_frame ) || ( "5mom" == m_frame ) ) 
    return Error( "This FRAME is not yet implemented" ) ;
  
  if ( "3mom" == m_frame ) {
    Gaudi::XYZVector pBeam1 , pBeam2 ;
    m_beamTool -> getMeanBeams( pBeam1 , pBeam2 ) ;
    // Pythia Units are GeV
    Pythia::SetBeam( pBeam1.X() / GeV , pBeam1.Y() / GeV , pBeam1.Z() /GeV ,
                     pBeam2.X() / GeV , pBeam2.Y() / GeV , pBeam2.Z() /GeV ) ;

    Pythia::pypars().mstp( 171 ) = 1 ;  // new energy given for each event 
    Pythia::pypars().mstp( 172 ) = 1 ;  // event generated at requested energy
    m_variableEnergy = true ;
  }
  
  Pythia::pydat3().mdme( 4178 , 1 ) = -1 ; // like in egpyinit  

  // Set default Pythia settings
  sc = parsePythiaCommands( m_defaultSettings ) ;
  // read Pythia command vector from job options
  sc = parsePythiaCommands( m_commandVector ) ;
  if ( ! sc.isSuccess( ) ) 
    return Error( "Unable to read Pythia commands" , sc ) ;
  
  // read input decay table (if needed)
  if ( 0 != m_particleDataUnit && !m_particleDataInput.empty() ) {

    if ( 2 != m_particleDataLevel && 3 != m_particleDataLevel ) { 
      return Error ( "Illegal value for pdtlevel ( should be [2-3])" ) ; 
    }

    //
    StatusCode sc = F77Utils::openOld( m_particleDataUnit , 
                                       m_particleDataInput ) ;

    if ( sc.isFailure() ) { 
      return Error ( "Could not open input PDT file '" + 
                      m_particleDataInput+"'" , sc ) ; 
    }

    // update the table
    info() << " CALL PYUPDA(" << m_particleDataLevel 
           << ","  << m_particleDataUnit
           << "/'" << m_particleDataInput <<"') " << endreq ;
    Pythia::PyUpda( m_particleDataLevel  , m_particleDataUnit ) ;

    // close the file 
    F77Utils::close ( m_particleDataUnit ) ;
    always() <<" Particle Data Table  has been read from the file '" 
             << m_particleDataInput << "'" << endreq ;  
  }
  
  // use PYGIVE commands (if any) (as THE LAST action)
  for ( CommandVector::const_iterator item = m_pygive.begin() ; 
        m_pygive.end() != item ; ++item ) {
    // use FORTRAN PYGIVE routine
    debug() << " CALL PYGIVE(' " << (*item) << "')" << endreq ;
    const int mstu_13 = Pythia::pydat1().mstu(13) ;
    Pythia::pydat1().mstu(13) =1   ;
    Pythia::PyGive( *item ) ;
    Pythia::pydat1().mstu(13) = mstu_13 ;
  }

  // Now call pyinit and set listing
  // if file already exist, delete it
  std::remove( m_pythiaListingFileName.c_str() ) ;
  Pythia::InitPyBlock( m_pythiaListingUnit , m_pythiaListingFileName ) ;

  // if specified, read SHLA decay file
  if ( "empty" != m_slhaDecayFile ) {
    int lunUnit = F77Utils::getUnit( msgLevel( MSG::DEBUG ) ) ;
    if ( 0 >= lunUnit ) 
      return Error( "No Fortran Unit available" ) ;
    sc = F77Utils::openOld( lunUnit , m_slhaDecayFile , 
                                       msgLevel( MSG::INFO ) ) ;
    if ( sc.isFailure() ) 
      return Error( "Cannot open SLHA decay file" ) ;

    Pythia::pymssm().imss(22) = lunUnit ;
    // KSUSY1 = 1000000
    int KSUSY1 = 1000000 ;
    int status = 0 ;
    Pythia::PySlha( 2 , KSUSY1+22 , status ) ;

    sc = F77Utils::close( lunUnit , msgLevel( MSG::INFO ) ) ;
    if ( sc.isFailure() ) 
      return Error( "Cannot close SLHA decay file" ) ;
  }
  
  Pythia::PyInit( m_frame, m_beam, m_target, m_win ) ;  

  // Reset forced fragmentation flag
  Pythia::pydat1().mstu( 150 ) = 0 ;

  return sc ;
}

//=============================================================================
//   Function called to generate one event with Pythia
//=============================================================================
StatusCode PythiaProduction::generateEvent( HepMC::GenEvent * theEvent , 
                                            LHCb::GenCollision * theCollision )
{
  // Set beam parameters if variable energy
  if ( m_variableEnergy ) {
    Gaudi::XYZVector pBeam1 , pBeam2 ;
    m_beamTool -> getBeams( pBeam1 , pBeam2 ) ;
    // PYTHIA Units are GeV
    Pythia::SetBeam( pBeam1.X() / GeV , pBeam1.Y() / GeV , pBeam1.Z() / GeV ,
                     pBeam2.X() / GeV , pBeam2.Y() / GeV , pBeam2.Z() / GeV ) ;
  }
  
  // Generate Event
  Pythia::PyEvnt( ) ;
  
  // Update event counter
  ++m_nEvents ;
  
  // Debugging output: print each event if required
  if ( m_eventListingLevel >= 0 ) 
  { 
    const int mstu_1 =  Pythia::pydat1().mstu(1) ;
    const int mstu_2 =  Pythia::pydat1().mstu(2) ;    
    Pythia::pydat1().mstu(1) = m_eve_mstu_1 ;
    Pythia::pydat1().mstu(2) = m_eve_mstu_2 ;
    Pythia::PyList( m_eventListingLevel ) ;
    Pythia::pydat1().mstu(1) = mstu_1 ;
    Pythia::pydat1().mstu(2) = mstu_2 ;
  }
  
  return toHepMC( theEvent , theCollision ) ;
}

//=============================================================================
// Set stable the given particle in Pythia
//=============================================================================
void PythiaProduction::setStable( const ParticleProperty * thePP ) {
  int pythiaId = thePP -> pythiaID() ;
  if ( 0 != pythiaId ) {
    int kc = Pythia::PyComp( pythiaId ) ;
    if ( kc > 0 ) Pythia::pydat3().mdcy( kc , 1 ) = 0 ;
  }
}

//=============================================================================
// Update particle properties
//=============================================================================
void PythiaProduction::updateParticleProperties( const ParticleProperty * 
                                                 thePP ) {
  int pythiaId = thePP -> pythiaID() ;
  double pwidth , lifetime ;
  if ( 0 != pythiaId ) {
    int kc = Pythia::PyComp( pythiaId ) ;
    if ( kc > 0 ) {
      if ( 0 == thePP -> lifetime() ) pwidth = 0. ;
      else pwidth = ( hbarc / ( thePP -> lifetime() * c_light ) ) ;
      //      if ( pwidth < ( 1.5e-6 * GeV ) ) pwidth = 0. ;
      if ( pwidth < m_widthLimit ) pwidth = 0. ;

      lifetime =  thePP -> lifetime() * c_light ;
      if ( ( lifetime <= 1.e-4 * mm ) || ( lifetime >= 1.e16 * mm ) ) 
        lifetime = 0. ;
      
      verbose() << "Change particle property of KC = " << kc 
                << " (" << pythiaId << ")" << endmsg ;
      verbose() << "Mass (GeV) from " << Pythia::pydat2().pmas( kc , 1 ) 
                << " to " << thePP -> mass() / GeV << endmsg ;
      verbose() << "Width (GeV) from " << Pythia::pydat2().pmas( kc , 2 ) 
                << " to " << pwidth / GeV << endmsg ;
      verbose() << "MaxWidth (GeV) from " << Pythia::pydat2().pmas( kc , 3 ) 
                << " to " << thePP -> maxWidth() / GeV << endmsg ;
      verbose() << "Lifetime from " << Pythia::pydat2().pmas( kc , 4 ) 
                << " to " << lifetime / mm << endmsg ;
      
      Pythia::pydat2().pmas( kc , 1 ) = thePP -> mass() / GeV ;
      // For Higgs, top, Z and W: update only masses
      if ( ( 6 != pythiaId ) && ( 23 != pythiaId ) && ( 24 != pythiaId ) 
           && ( 25 != pythiaId ) ) {
        Pythia::pydat2().pmas( kc , 2 ) = pwidth / GeV ;
        Pythia::pydat2().pmas( kc , 3 ) = thePP -> maxWidth() / GeV ;
        Pythia::pydat2().pmas( kc , 4 ) = lifetime / mm ;
      }
    }
  }
}


//=============================================================================
// Retrieve the Hard scatter information
//=============================================================================
void PythiaProduction::hardProcessInfo( LHCb::GenCollision * theCollision ) {
  theCollision -> setProcessType( Pythia::pypars().msti(1) ) ;
  theCollision -> setSHat( Pythia::pypars().pari(14) );
  theCollision -> setTHat( Pythia::pypars().pari(15) );
  theCollision -> setUHat( Pythia::pypars().pari(16) );
  theCollision -> setPtHat( Pythia::pypars().pari(17) );
  theCollision -> setX1Bjorken( Pythia::pypars().pari(33) );
  theCollision -> setX2Bjorken( Pythia::pypars().pari(34) );

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
      else if ( "pylisti" == entry ) 
      { 
        m_initializationListingLevel   = int1 ;
        if ( 0 < int2 ) { m_ini_mstu_1 = int2 ; }
        if ( 0 < int3 ) { m_ini_mstu_2 = int3 ; }
      }
      else if ( "pyliste" == entry ) 
      {
        m_eventListingLevel          = int1  ; 
        if ( 0 < int2 ) { m_eve_mstu_1 = int2 ; }
        if ( 0 < int3 ) { m_eve_mstu_2 = int3 ; }
      }
      else if ( "pylisth" == entry ) 
      {
        m_eventListingLevel2         = int1  ; 
        if ( 0 < int2 ) { m_had_mstu_1 = int2 ; }
        if ( 0 < int3 ) { m_had_mstu_2 = int3 ; }
      }
      else if ( "pystatf" == entry ) m_finalizationListingLevel   = int1  ; 
      else if ( "output"  == entry ) {
        m_pythiaListingFileName         = str                 ;
        m_pythiaListingUnit             = 49                  ;
        Pythia::pydat1().mstu(11)       = m_pythiaListingUnit ; 
        Pythia::pydat1().mstu( 12 )     = 1                   ;
        Pythia::pydat1().mstu( 13 )     = 1                   ;
        Pythia::pydat1().mstu( 25 )     = 1                   ;
        Pythia::pypars().mstp( 122 )    = 1                   ;
      }
    //
      else if (  "pdtunit"   == entry ) { m_particleDataUnit   = int1 ; }
      else if (  "pdtoutput" == entry ) { m_particleDataOutput = str  ; }
      else if (  "pdtlevel"  == entry ) { m_particleDataLevel  = int1 ; }
      else if (  "pdtinput"  == entry ) { m_particleDataInput  = str  ; }
    //
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
void PythiaProduction::retrievePartonEvent( HepMC::GenEvent * /* theEvent */ )
{
  Pythia::PyEdit( 22 ) ;
}

//=============================================================================
// Hadronize Pythia event
//=============================================================================
StatusCode PythiaProduction::hadronize( HepMC::GenEvent * theEvent , 
                                        LHCb::GenCollision * theCollision ) {
  Pythia::PyExec( ) ;
  
  // Debugging output: print each event if required
  if ( m_eventListingLevel2 >= 0 ) {
    const int mstu_1 = Pythia::pydat1().mstu(1) ;
    const int mstu_2 = Pythia::pydat1().mstu(2) ;
    Pythia::pydat1().mstu(1) = m_had_mstu_1 ;
    Pythia::pydat1().mstu(2) = m_had_mstu_2 ;
    Pythia::PyList ( m_eventListingLevel2 ) ;
    Pythia::pydat1().mstu(1) = mstu_1 ;
    Pythia::pydat1().mstu(2) = mstu_2  ;
  }

  return toHepMC ( theEvent , theCollision ) ;  
} ;


//=============================================================================
// Debug print out to be printed after all initializations
//=============================================================================
void PythiaProduction::printRunningConditions( ) 
{
  // PYLIST call is managed by Pythia job options
  if ( m_initializationListingLevel >= 0 ) {
    info() << " CALL PYLIST(" <<  m_initializationListingLevel << ") " 
           << " using MSTU(1/2)=" << m_ini_mstu_1 << "/" << m_ini_mstu_2 
           << endreq ;
    //
    const int mstu_1 = Pythia::pydat1().mstu(1) ;
    const int mstu_2 = Pythia::pydat1().mstu(2) ;
    //
    Pythia::pydat1().mstu(1) = m_ini_mstu_1 ;
    Pythia::pydat1().mstu(2) = m_ini_mstu_2 ; 
    Pythia::PyList( m_initializationListingLevel ) ;
    Pythia::pydat1().mstu(1) = mstu_1 ;
    Pythia::pydat1().mstu(2) = mstu_2 ; 
  }

  // print out Pythia settings
  printPythiaParameter() ;

  // print particle properties (if needed)
  for ( std::vector<int>::const_iterator ip = m_pdtlist.begin() ; 
        m_pdtlist.end() != ip ; ++ip ) {
    const int i = *ip ;
    if ( 0 > i ) { continue ; } 
    const int mstu_1 = Pythia::pydat1().mstu(1) ;
    const int mstu_2 = Pythia::pydat1().mstu(2) ;      
    Pythia::pydat1().mstu(1) = i ;
    Pythia::pydat1().mstu(2) = i ;
    Pythia::PyList( 12 ) ;      
    Pythia::pydat1().mstu(1) = mstu_1 ;
    Pythia::pydat1().mstu(2) = mstu_2 ; 
  }
  
  // write output decay table (if needed)
  if ( 0 != m_particleDataUnit && !m_particleDataOutput.empty() ) {
    StatusCode sc = F77Utils::open ( m_particleDataUnit , 
                                     m_particleDataOutput ) ;
    if ( sc.isFailure() ) { 
      Error ( "Could not open output PDS file '" + 
              m_particleDataOutput + "'" , sc ) ; 
      return ;
    }
    
    // update the table 
    info() << " CALL PYUPDA(1," 
           << m_particleDataUnit<<"/'" << m_particleDataOutput << "') " 
           << endreq ;
    Pythia::PyUpda( 1 , m_particleDataUnit ) ;
    // close the file 
    F77Utils::close ( m_particleDataUnit ) ;
    always() <<" Particle Data Table  has been dump to  the file '" 
             << m_particleDataOutput << "'" << endreq;
  }
}

//=============================================================================
// TRUE if the particle is a special particle which must not be modify
//=============================================================================
bool PythiaProduction::isSpecialParticle( const ParticleProperty * thePP ) 
  const {
  switch ( abs( thePP -> pdgID() ) ) {
  case 1:
  case 2:
  case 3:
  case 4:
  case 5:
  case 6:
  case 7:
  case 8:
  case 17:
  case 18:
  case 21:
  case 110:
  case 990:
    //case 32:
  case 33:
  case 34:
    //case 35:
    //case 36:
  case 37:
  case 39:
  case 41:
  case 42:
  case 43:
  case 44:
  case 81:
  case 82:
  case 83:
  case 84:
  case 85:
  case 88:
  case 89:
  case 90:
  case 91:
  case 92:
  case 93:
  case 94:
  case 95:
  case 96:
  case 97:
  case 98:
  case 99:
  case 9900110:
  case 9900210:
  case 9900220:
  case 9900330:
  case 9900440:
  case 9902110:
  case 9902210:
  case 1101:
  case 1103:
  case 2101:
  case 2103:
  case 2201:
  case 2203:
  case 3101:
  case 3103:
  case 3201:
  case 3203:
  case 3301:
  case 3303:
  case 4101:
  case 4103:
  case 4201:
  case 4203:
  case 4301:
  case 4303:
  case 4401:
  case 4403:
  case 5101:
  case 5103:
  case 5201:
  case 5203:
  case 5301:
  case 5303:
  case 5401:
  case 5403:
  case 5501:
  case 5503:
    return true ;
    break ;
  default:
    return false ;
    break ;
  }
  return false ;    
}
//=============================================================================
// Setup for forced fragmentation 
//=============================================================================
StatusCode PythiaProduction::setupForcedFragmentation( const int thePdgId ) {
  Pythia::pydat1().mstj( 30 ) = 1 ;
  Pythia::pydat1().mstj( 31 ) = 100 ;
  Pythia::pydat1().mstj( 33 ) = thePdgId ;
  Pythia::pydat1().mstu( 150 ) = 1 ;
  Pythia::pydat1().mstu( 151 ) = 0 ;
  Pythia::pydat1().mstu( 152 ) = 0 ;

  switch( thePdgId ) {
  case 511:
  case -511:
  case 521:
  case -521:
  case 531:
  case -531:
  case 541:
  case -541:
  case 551:
    Pythia::pydat1().mstj( 32 ) = - thePdgId / 100 ;
    Pythia::pydat1().mstj( 34 ) = ( thePdgId / 10 ) % 10 ;
    break ;
  case 5122:
  case -5122:
    Pythia::pydat1().mstj( 32 ) = thePdgId / 1000 ;
    Pythia::pydat1().mstj( 34 ) = thePdgId / abs( thePdgId ) * 2101 ;
    break ;
  default:
    error() << 
      format( "It is not possible to force fragmentation for particleID %d" ,
              thePdgId ) << endmsg ;
    return StatusCode::FAILURE ;
    break ;
  }

  return StatusCode::SUCCESS ;  
}
// ============================================================================
/// PYTHIA -> HEPEVT -> HEPMC 
// ============================================================================
StatusCode PythiaProduction::toHepMC
( HepMC::GenEvent*     theEvent    , 
  LHCb::GenCollision * theCollision )
{
  
  // Convert to HepEvt format
  Pythia::LunHep( 1 ) ;
  
  // Convert event in HepMC Format
  HepMC::IO_HEPEVT theHepIO ;
  if ( ! theHepIO.fill_next_event( theEvent ) )
    return Error( "Could not fill HepMC event" ) ;
  
  // Now convert to LHCb units:
  for ( HepMC::GenEvent::particle_iterator p = theEvent -> particles_begin() ;
        p != theEvent -> particles_end() ; ++p ) 
    (*p) -> set_momentum( (*p) -> momentum() * GeV ) ;
  
  for ( HepMC::GenEvent::vertex_iterator v = theEvent -> vertices_begin() ;
        v != theEvent -> vertices_end() ; ++v ) {
    CLHEP::HepLorentzVector newPos ;
    newPos.setX( (*v) -> position() . x() ) ;
    newPos.setY( (*v) -> position() . y() ) ;
    newPos.setZ( (*v) -> position() . z() ) ;
    newPos.setT( ( (*v) -> position() . t() * mm ) / CLHEP::c_light ) ;
    
    (*v) -> set_position( newPos ) ;
  }
  
  theEvent -> set_signal_process_id( Pythia::pypars().msti( 1 ) ) ;
  
  // Retrieve hard process information
  hardProcessInfo( theCollision ) ;
  
  return StatusCode::SUCCESS ;
} ;
// ============================================================================


// ============================================================================
/// The END 
// ============================================================================

