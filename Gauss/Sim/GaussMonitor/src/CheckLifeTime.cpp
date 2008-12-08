// $Id: CheckLifeTime.cpp,v 1.2 2008-12-08 10:11:22 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/PhysicalConstants.h"
// ============================================================================
// Kernel/PartProp
// ============================================================================
#include "Kernel/ParticleProperty.h"
#include "Kernel/IParticlePropertySvc.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/ParticleProperties.h"
// ============================================================================
// local
// ============================================================================
#include "CheckLifeTime.h"
// ============================================================================
/** @file 
 *  Implementation file for class GaussMonitor::CheckLifeTime
 *  @date 2008-06-23 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 */
// ============================================================================
/*  The standard constructor 
 *  @param name the algorithm instance name 
 *  @param pSvc service locator
 */
// ============================================================================
GaussMonitor::CheckLifeTime::CheckLifeTime 
( const std::string& name ,  // algorithm instance name 
  ISvcLocator*       pSvc )  // service locator 
  : GaudiHistoAlg ( name , pSvc ) 
  , m_particles (     )
  , m_frequency ( 100 )
  , m_event     ( 0   )
{
  //
  m_particles.push_back ( "B0"        ) ;
  m_particles.push_back ( "B+"        ) ;
  m_particles.push_back ( "B_s0"      ) ;
  m_particles.push_back ( "B_c+"      ) ;
  m_particles.push_back ( "Lambda_b0" ) ;
  //
  m_particles.push_back ( "D0"        ) ;
  m_particles.push_back ( "D+"        ) ;
  m_particles.push_back ( "D_s+"      ) ;
  m_particles.push_back ( "Lambda_c+" ) ;
  //
  declareProperty
    ( "Particles" , m_particles , 
      "The list of particles to be checked" ) ;
  declareProperty 
    ( "Frequency" , m_frequency , 
      "The frequence to check (#of events)" ) ;
  //
  setProperty ( "HistoPrint"  , true   ) ;
}
// ============================================================================
// standard initialization of the algorithm
// ============================================================================
StatusCode GaussMonitor::CheckLifeTime::initialize () 
{
  // initialize the base class 
  StatusCode sc = GaudiHistoAlg::initialize () ;
  if ( sc.isFailure() ) { return sc ; }
  // load LoKi service 
  svc<IService>( "LoKiSvc" , true ) ;
  // check particle lists
  if ( m_particles.empty() ) 
  { Warning ( "No particles are specified" ) ; }
  // create the slots 
  for ( PIDs::const_iterator ipid = m_particles.begin() ; 
        m_particles.end() != ipid ; ++ipid ) 
  { 
    const std::string title = cntName ( *ipid ) ;
    counter ( title ) ; 
    //
    // get the particle property 
    const LHCb::ParticleProperty* pp = LoKi::Particles::ppFromName ( *ipid ) ;
    if ( 0 == pp ) { return Error ( "Invalid particle: " + (*ipid) ) ;}
    book ( title , 0 , highEdge ( *pp )  , 100 ) ;
  }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// standard finalization   of  the algorithm 
// ============================================================================
StatusCode GaussMonitor::CheckLifeTime::finalize   ()  
{
  check ( true )  ;
  // finalize the base 
  return GaudiHistoAlg::finalize () ;
}
// ============================================================================
// check the lifetimes 
// ============================================================================
void GaussMonitor::CheckLifeTime::check ( const bool iErr ) const 
{
  // 
  for ( PIDs::const_iterator ip = particles().begin()  ; 
        particles().end() != ip ; ++ip ) 
  {
    // get the cunter
    const StatEntity& cnt = counter ( cntName ( *ip ) ) ;
    // get the particle property 
    const LHCb::ParticleProperty* pp = LoKi::Particles::ppFromName ( *ip ) ;
    if ( 0 == pp ) { Error ( "Invalid particle: " + (*ip)) ; continue ; }
    if ( 0 == cnt.nEntries() ) 
    { Warning ( "No '" + (*ip) + "' Particles are found!") ; continue ; }
    // get the mean lifetime (in mm) 
    const double lTime = cnt.flagMean    () ;
    // get the error in ctau  (in mm) 
    const double ltErr = cnt.flagMeanErr () ;
    // get the nominal  ctau  (in mm) 
    double nominal = pp->ctau () / Gaudi::Units::mm ;
    if ( nominal < 1.e-5 * Gaudi::Units::micrometer ) { nominal = 0 ; }
    //
    const double diff =  ( lTime - nominal ) ;
    if ( std::fabs ( diff ) > 3 * ltErr ) 
    {
      fatal ()   << " >3   sigma deviation in lifetime for '" << (*ip) 
                 << "' \tActual is "  << lTime         << "+-" 
                 <<  ltErr  << "("    << cnt.flagRMS() << ")"
                 << "\tNominal is "   << nominal       << " [mm] "
                 << endreq ;
      if ( iErr ) { Error   ( ">3 sigma deviation for '" +(*ip)+ "'") ; }
    }
    else if ( std::fabs ( diff ) > 2 * ltErr ) 
    {
      error ()   << " >2   sigma deviation in lifetime for '" << (*ip) 
                 << "' \tActual is "  << lTime         << "+-"           
                 << ltErr << "("      << cnt.flagRMS() << ")"
                 << "\tNominal is "   << nominal       << " [mm] "
                 << endreq ;
      if ( iErr ) { Error   ( ">2 sigma deviation for '" +(*ip)+ "'") ; }
    }
    else if ( std::fabs ( diff ) > 1 * ltErr ) 
    {
      warning () << " >1   sigma deviation in lifetime for '" << (*ip) 
                 << "' \tActual is "  << lTime         << "+-"    
                 << ltErr << "("      << cnt.flagRMS() << ")"
                 << "\tNominal is "   << nominal       << " [mm] "
                 << endreq ;
      if ( iErr ) { Warning ( ">1 sigma deviation for '" +(*ip)+ "'") ; }
    }
    else if ( std::fabs ( diff ) > 0.5 * ltErr ) 
    { 
      info ()    << " >0.5 sigma deviation in lifetime for '" << (*ip) 
                 << "' \tActual is "    << lTime  << "+-" 
                 << ltErr << "(" << cnt.flagRMS() << ")"
                 << "\tNominal is "   << nominal  << " [mm] "
                 << endreq ;
    }
    else 
    { 
      info ()    << " <0.5 sigma deviation in lifetime for '" << (*ip) 
                 << "' \tActual is "    << lTime  << "+-" 
                 << ltErr << "(" << cnt.flagRMS() << ")"
                 << "\tNominal is "   << nominal  << " [mm] "
                 << endreq ;
    }
  }
}
// ============================================================================
// The END 
// ============================================================================
