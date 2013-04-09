// local
#include "HijingProduction.h"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "Kernel/ParticleProperty.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/PhysicalConstants.h"

// from Event
#include "Event/GenCollision.h"

// HepMC
#include "HepMC/IO_HEPEVT.h"
#include "HepMC/HEPEVT_Wrapper.h"


// local
#include "Generators/StringParse.h"
#include "LbHijing/Hijing.h"
#include "LbHijing/HijingCB.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HijingProduction
//
// 2006-02-25 : Daniele Gregori
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( HijingProduction );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HijingProduction::HijingProduction(const std::string& type,
                                   const std::string& name,
                                   const IInterface* parent)
  : GaudiTool ( type, name , parent ) ,
    m_beam_to_use( 1 ) , 
    m_efrm( 0. ) ,
    m_iap( 0 ) ,
    m_izp( 0 ) ,
    m_iat( 0 ) ,
    m_izt( 0 ) ,
    m_bmin( 0. ) ,
    m_bmax( 0. ) , 
    m_beta( 0. ) , 
    m_gamma( 1. ) 
{
  declareInterface< IProductionTool >( this ) ;
  declareProperty( "Commands" , m_commandVector ) ;
  m_defaultSettings.clear() ;
  m_defaultSettings.push_back( "hijinginit efrm 7000." );
  m_defaultSettings.push_back( "hijinginit frame LAB" );
  m_defaultSettings.push_back( "hijinginit proj P" );
  m_defaultSettings.push_back( "hijinginit targ A" );
  m_defaultSettings.push_back( "hijinginit beam1" );
  m_defaultSettings.push_back( "hijinginit iap 1" );
  m_defaultSettings.push_back( "hijinginit izp 1" );
  m_defaultSettings.push_back( "hijinginit iat 131" );
  m_defaultSettings.push_back( "hijinginit izt 54" );
  m_defaultSettings.push_back( "hijinginit bmin 0." );
  m_defaultSettings.push_back( "hijinginit bmax 0." );
  m_defaultSettings.push_back( "hiparnt ihpr2 12 1" );
  m_defaultSettings.push_back( "hiparnt ihpr2 21 0" );
  
}

//=============================================================================
// Destructor 
//=============================================================================
HijingProduction::~HijingProduction(){ ; }

//=============================================================================
// Initialize method
//=============================================================================
StatusCode HijingProduction::initialize() {
  debug() << "HijingProduction::initialize called" << endreq;
  
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

  // Set default Hijing settings
  sc = parseHijingCommands( m_defaultSettings ) ;
  // read Hijing command vector from job options
  sc = parseHijingCommands( m_commandVector ) ;
  if ( ! sc.isSuccess( ) )
    return Error( "Unable to read Hijing commands" , sc ) ;

  Hijing::HijingInit(m_efrm, m_frame, m_proj, m_targ, m_iap, m_izp, m_iat, m_izt);
  
  // Set size of common blocks in HEPEVT: note these correspond to stdhep
  HepMC::HEPEVT_Wrapper::set_sizeof_int( 4 ) ;
  HepMC::HEPEVT_Wrapper::set_sizeof_real( 8 ) ;
  HepMC::HEPEVT_Wrapper::set_max_number_entries( 10000 ) ;

  //To boost to Lab frame from CMS frame, by zhwyang
  m_beta  = 0. ;
  m_gamma = 1. ;
  if ( m_frame=="CMS" ) {
    double rp = m_izp/(double)m_iap ; //ratio of Z/A for projectile
    double rt = m_izt/(double)m_iat ; //ratio of Z/A for target
    double ep = m_efrm/2.*sqrt(rp/rt) ; //energy or momentum for projectile
    double et = m_efrm/2.*sqrt(rt/rp) ; //energy or momentum for target
    m_beta = (ep-et)/sqrt( m_efrm*m_efrm + (ep-et)*(ep-et) ) ; //velocity
    m_gamma=1./sqrt(1.-m_beta*m_beta); //Lorentz boost factor
  } 
  always() << "Lorentz Boost: beta = " << m_beta << "\tgamma = " << m_gamma << endmsg ;

  return StatusCode::SUCCESS ;
}

//=============================================================================
//   Function called to generate one event with Hijing
//=============================================================================
StatusCode HijingProduction::generateEvent( HepMC::GenEvent * theEvent , 
                                            LHCb::GenCollision* /*theCollision*/ ) {
  debug() << "HijingProduction::generateEvent called" << endreq;
  Hijing::HijingEvnt(m_frame, m_bmin, m_bmax);
  
  // Convert to HepEvt format
  Hijing::LunHep( m_beam_to_use ) ;
  
  // Convert event in HepMC Format
  HepMC::IO_HEPEVT theHepIO ;
  if ( ! theHepIO.fill_next_event( theEvent ) ) 
    return Error( "Could not fill HepMC event" ) ;

  // Now convert to LHCb units:
  for ( HepMC::GenEvent::particle_iterator p = theEvent -> particles_begin() ;
        p != theEvent -> particles_end() ; ++p ) {
    //For Lorentz boost in pA and Ap run
    //pz and energy of particle in GeV in CMS (before Lorentz boost)
    double pz1 = (*p) -> momentum() . pz() * Gaudi::Units::GeV ;
    double pe1 = (*p) -> momentum() . e() * Gaudi::Units::GeV ;
    //pz and energy of particle in GeV in LAB (after Lorentz boost)
    double pz2 = m_gamma*(pz1 + m_beta*pe1) ;
    double pe2 = m_gamma*(pe1 + m_beta*pz1) ;
   
    HepMC::FourVector newMom ;
    newMom.setX( (*p) -> momentum() . px() * Gaudi::Units::GeV ) ;
    newMom.setY( (*p) -> momentum() . py() * Gaudi::Units::GeV ) ;
    newMom.setZ( pz2 ) ;
    newMom.setT( pe2) ;
    (*p) -> set_momentum( newMom );
    // Change generated mass 
    (*p) -> 
      set_generated_mass( (*p) -> generated_mass() * Gaudi::Units::GeV ) ;
    
  }
  
  // Not really needed since all particles come from (0,0,0,0)
  // but done anyway to have a consistent implementation
  // with the other generators
  for ( HepMC::GenEvent::vertex_iterator v = theEvent -> vertices_begin() ;
        v != theEvent -> vertices_end() ; ++v ) {
    HepMC::FourVector newPos ;
    newPos.setX( (*v) -> position() . x() ) ;
    newPos.setY( (*v) -> position() . y() ) ;
    newPos.setZ( (*v) -> position() . z() ) ;
    newPos.setT( ( (*v) -> position() . t() * Gaudi::Units::mm ) / 
                 Gaudi::Units::c_light ) ;

    (*v) -> set_position( newPos ) ;
  }
  

  // beam particles in HepMC do not make sense, erase them:
  theEvent -> set_beam_particles( 0 , 0 ) ;
  
  return StatusCode::SUCCESS ;
}


//=============================================================================
// Set stable the given particle in Hijing
//=============================================================================
void HijingProduction::setStable( const LHCb::ParticleProperty* /*thePP*/ ) {
  debug() << "HijingProduction::setStable called" << endreq;
}

//=============================================================================
// Update particle properties
//=============================================================================
void HijingProduction::updateParticleProperties( const LHCb::ParticleProperty* 
                                                 /*thePP*/ ) {
  debug() << "HijingProduction::updateParticleProperties called" << endreq;
}

//=============================================================================
// Turn on fragmentation in Hijing
//=============================================================================
void HijingProduction::turnOnFragmentation() {
  debug() << "HijingProduction::turnOnFragmentation called" << endreq;
}

//=============================================================================
// Turn off fragmentation in Hijing
//=============================================================================
void HijingProduction::turnOffFragmentation() {
  debug() << "HijingProduction::turnOffFragmentation called" << endreq;
}

//=============================================================================
// Save parton event
//=============================================================================
void HijingProduction::savePartonEvent( HepMC::GenEvent * /* theEvent */ ) {
  debug() << "HijingProduction::savePartonEvent called" << endreq;
}

//=============================================================================
// Load parton event
//=============================================================================
void HijingProduction::retrievePartonEvent( HepMC::GenEvent * /* theEvent */ ) {
  debug() << "HijingProduction::retrievePartonEvent called" << endreq;
}

//=============================================================================
// Hadronize Hijing event
//=============================================================================
StatusCode HijingProduction::hadronize( HepMC::GenEvent* /*theEvent*/ , 
                                        LHCb::GenCollision* /*theCollision*/) {
  debug() << "HijingProduction::hadronize called" << endreq;
  return StatusCode::SUCCESS;
}

//=============================================================================
// Debug print out to be printed after all initializations
//=============================================================================
void HijingProduction::printRunningConditions() {
  debug() << "HijingProduction::printRunningConditions called" << endreq;
}

//=============================================================================
// TRUE if the particle is a special particle and must not be modified
//=============================================================================
bool HijingProduction::isSpecialParticle( const LHCb::ParticleProperty * thePP ) const
{
  debug() << "HijingProduction::isSpecialParticle called" << endreq;
  switch ( thePP -> pid().abspid() )
  {
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
  case 32:
  case 33:
  case 34:
  case 35:
  case 36:
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
    return false ;  
  }
}

//=============================================================================
// Setup for forced fragmentation 
//=============================================================================
StatusCode HijingProduction::setupForcedFragmentation( const int /*thePdgId*/ ) {
  debug() << "HijingProduction::setupForcedFragmentation called" << endreq;
  return StatusCode::SUCCESS;
}

//=============================================================================
// Parse Hijing commands stored in a vector
//=============================================================================
StatusCode HijingProduction::parseHijingCommands( const CommandVector &
                                                  theCommandVector ) {
  
  debug() << "HijingProduction::parseHijingCommands called" << endreq;
  
  for ( CommandVector::const_iterator iter = theCommandVector.begin();
        theCommandVector.end() != iter; ++iter ) {
    
    debug() << " Command is: " << (*iter) << endmsg ;
    
    StringParse mystring( *iter ) ;
    std::string block = mystring.piece(1);
    std::string entry = mystring.piece(2);
    std::string setting = mystring.piece(3);
    int int1 = mystring.intpiece(3);
    int int2 = mystring.intpiece(4);
    double fl1 = mystring.numpiece(3);
    double fl2 = mystring.numpiece(4);
    
    if ( "hijinginit" == block )
      if( "efrm" == entry ) m_efrm = fl1;
      else if( "frame" == entry ) m_frame = setting;
      else if( "proj" == entry ) m_proj = setting;
      else if( "targ" == entry ) m_targ = setting;
      else if( "beam1" == entry ) m_beam_to_use = 1;
      else if( "beam2" == entry ) m_beam_to_use = 2;
      else if( "beam12" == entry ) m_beam_to_use = 3;
      else if( "iap" == entry ) m_iap = int1;
      else if( "izp" == entry ) m_izp = int1;
      else if( "iat" == entry ) m_iat = int1;
      else if( "izt" == entry ) m_izt = int1;
      else if( "bmin" == entry ) m_bmin = fl1;
      else if( "bmax" == entry ) m_bmax = fl1;
      else return Error(std::string("HIJING ERROR, entry HIJINGINIT has BEAM1, BEAM2, IAP ")+
                        std::string("FRAME, PROJ, TARG, EFRM, IZP, IAT, IZT, BMIN and BMAX: ")+
                        std::string("YOU HAVE SPECIFIED ") +
                        std::string( entry ) ) ;
    else if ( "hiparnt" == block )
      if( "hipr1" == entry ) hiparnt.hipr1[int1-1] = fl2;
      else if( "ihpr2" == entry ) hiparnt.ihpr2[int1-1] = int2;
      else if( "hint1" == entry ) hiparnt.hint1[int1-1] = fl2;
      else if( "ihnt2" == entry ) hiparnt.ihnt2[int1-1] = int2;
      else return Error( std::string( "HIJING ERROR, block HIPARNT has HIPR1 " )+
                         std::string( "IHPR2, HINT1 and IHNT2: YOU HAVE SPECIFIED " ) +
                         std::string( entry ) ) ;
    else return Error( std::string( " ERROR in HIJING PARAMETERS   " ) +
                       std::string( block ) +
                       std::string( " is and invalid common block name !" ) ) ;
    
    
  }
  
  return StatusCode::SUCCESS ;
}


//=============================================================================
// Finalize method
//=============================================================================
StatusCode HijingProduction::finalize( ) {
  return GaudiTool::finalize( ) ;
}  

//=============================================================================
// dummy method
//=============================================================================
StatusCode HijingProduction::initializeGenerator() {
  return StatusCode::SUCCESS;
}

//=============================================================================
