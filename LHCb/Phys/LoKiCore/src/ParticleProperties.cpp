// $Id: ParticleProperties.cpp,v 1.5 2007-02-26 13:13:09 cattanem Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.5 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2006/05/08 09:04:27  cattanem
// Units now in Gaudi::Units namespace
//
// Revision 1.3  2006/05/02 14:29:11  ibelyaev
//  censored
//
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ParticleProperty.h"
// ============================================================================
// Gaudi Units (previously CLHEP)
// ============================================================================
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/SystemOfUnits.h"
// ============================================================================
// Kernel
// ============================================================================
#include "Kernel/ParticleID.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/ParticleProperties.h"
#include "LoKi/Report.h"
#include "LoKi/Services.h"
#include "LoKi/Constants.h"
// ============================================================================
// Boost 
// ============================================================================
#include "boost/lexical_cast.hpp"
// ============================================================================


// ============================================================================
/** @file
 *
 * Implementation file for functions form the file ParticleProperties.h
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23 
 */
// ============================================================================

// ============================================================================
/** retrieve particle ID from Particle name 
 *  @param name particle name 
 *  @return particle ID 
 */
// ============================================================================
LHCb::ParticleID LoKi::Particles::pidFromName( const std::string& name ) 
{
  const ParticleProperty* pp = ppFromName( name ) ;
  if( 0 == pp ) 
  {
    LoKi::Report::Error 
      ( std::string(" LoKi::Particles::pidFromName:") +
        "ParticleProperty* points to NULL for '"      + 
        name + "' return ParticleID() " ) ;
    return LHCb::ParticleID();
  }
  return LHCb::ParticleID( pp->jetsetID() );
};
// ============================================================================

// ============================================================================
/** retrieve particle ID from Particle name 
 *  @param name particle name 
 *  @return particle ID 
 */
// ============================================================================
const ParticleProperty* LoKi::Particles::_ppFromName
( const std::string& name ) 
{
  const LoKi::Services& services = LoKi::Services::instance () ;
  IParticlePropertySvc* ppSvc = services.ppSvc();
  if( 0 == ppSvc ) 
  {
    LoKi::Report::Error
      ( std::string(" LoKi::Particles::pidFromName:") + 
        "IParticlePropertySvc* points to NULL!"       + 
        " return NULL " ) ;
    return 0 ;
  }
  const ParticleProperty* pp = ppSvc -> find( name ) ;
  return pp ;
};
// ============================================================================


// ============================================================================
/** retrieve particle ID from Particle name 
 *  @param name particle name 
 *  @return particle ID 
 */
// ============================================================================
const ParticleProperty* LoKi::Particles::ppFromName
( const std::string& name ) 
{
  const ParticleProperty* pp = LoKi::Particles::_ppFromName ( name ) ;
  if ( 0 == pp ) 
  {
    LoKi::Report::Error
      ( std::string(" LoKi::Particles::ppFromName:")  + 
        "ParticleProperty* points to NULL for '"      + 
        name + "' return NULL " ) ;
    return 0 ;
  }
  return pp ;
};
// ============================================================================


// ============================================================================
/** retrieve ParticleProperty from ParticleID 
 *  @param pid particle ID 
 *  @param particle property 
 */
// ============================================================================
const ParticleProperty* LoKi::Particles::_ppFromPID
( const LHCb::ParticleID& pid ) 
{
  const LoKi::Services& services = LoKi::Services::instance () ;
  IParticlePropertySvc* ppSvc = services.ppSvc();
  if( 0 == ppSvc ) 
  {
    LoKi::Report::Error
      ( std::string(" LoKi::Particles::_ppFromPID(")   + 
        boost::lexical_cast<std::string>( pid.pid() )  + 
        "): IParticlePropertySvc* points to NULL!"     + 
        " return NULL " ) ;
    return 0 ;
  }
  const ParticleProperty* pp = ppSvc -> findByStdHepID( pid.pid() ) ;
  //
  return pp ;
};
// ============================================================================

// ============================================================================
/** retrieve ParticleProperty from ParticleID 
 *  @param pid particle ID 
 *  @param particle property 
 */
// ============================================================================
const ParticleProperty* LoKi::Particles::ppFromPID
( const LHCb::ParticleID& pid ) 
{
  const ParticleProperty* pp = LoKi::Particles::_ppFromPID( pid ) ;
  if( 0 == pp ) 
  {
    LoKi::Report::Error
      ( std::string(" LoKi::Particles::ppFromPID(")    +
        boost::lexical_cast<std::string>( pid.pid() )  + 
        "): ParticleProperty* points to NULL,"         + 
        " return NULL " ) ;
    return 0 ;
  }
  return pp ;
};
// ============================================================================

// ============================================================================
/** retrieve ParticleProperty from ParticleID 
 *  @param pid particle ID 
 *  @param particle mass 
 */
// ============================================================================
const double LoKi::Particles::massFromPID  ( const LHCb::ParticleID& pid   ) 
{
  const ParticleProperty* pp = LoKi::Particles::ppFromPID( pid ) ;
  if ( 0 == pp ) 
  {
    LoKi::Report::Error 
      ( " LoKi::Particles::massFromPID(" +
        boost::lexical_cast<std::string>( pid.pid() ) + 
        ") : ParticleProperty* points to NULL, return -1 * TeV " ) ;
    return -1.0 * Gaudi::Units::TeV  ;
  }
  return pp->mass() ;
};
// ============================================================================

// ============================================================================
/** retrieve ParticleProperty from ParticleID 
 *  @param pid particle ID 
 *  @param particle property 
 */
// ============================================================================
const double LoKi::Particles::massFromName ( const std::string&  name ) 
{
  const ParticleProperty* pp = LoKi::Particles::ppFromName( name ) ;
  if ( 0 == pp ) 
  {
    LoKi::Report::Error
      ( std::string(" LoKi::Particles::massFromName('") + 
        name + "')" + 
        "ParticleProperty* points to NULL, return -1 * TeV " ) ;
    return -1.0 * Gaudi::Units::TeV  ;
  }
  return pp->mass() ;
};
// ============================================================================

// ============================================================================
/** get name of 'antiparticle'
 *  @param name particlre name 
 *  @return anme of antiParticle 
 */
// ============================================================================
std::string LoKi::Particles::antiParticle( const std::string&      name ) 
{
  const ParticleProperty* pp = LoKi::Particles::ppFromName( name ) ;
  if ( 0 == pp ) 
  {
    LoKi::Report::Error
      ( std::string(" LoKi::Particles::antiParticle('") + 
        name + "')" + 
        "ParticleProperty* points to NULL, return '"     + 
        s_InvalidPIDName+"'" ) ;
    return s_InvalidPIDName ;
  }
  const ParticleProperty* antiPP = LoKi::Particles::antiParticle( pp ) ;
  if ( 0 == antiPP ) 
  {
    LoKi::Report::Error
      ( std::string(" LoKi::Particles::antiParticle:") + 
        "antiParticle('" + (pp->particle()) + 
        "')points to NULL" +" return '"     + 
        s_InvalidPIDName + "'" ) ;
    return s_InvalidPIDName ;
  }
  return pp->particle() ;
};
// ============================================================================

// ============================================================================
/** get name of 'antiparticle'
 *  @param name particlre name 
 *  @return anme of antiParticle 
 */
// ============================================================================
const ParticleProperty* 
LoKi::Particles::antiParticle ( const ParticleProperty* pp ) 
{
  if ( 0 == pp ) 
  {
    LoKi::Report::Error 
      ( std::string(" LoKi::Particles::antiParticle") +
        "ParticleProperty* points to NULL, return NULL" ) ;
    return 0 ;
  }
  const LHCb::ParticleID pid  = LHCb::ParticleID( pp->jetsetID() ) ;
  /// find ID for antiparticle 
  const LHCb::ParticleID anti = LoKi::Particles::antiParticle( pid ) ;
  /// the same particle ?
  if ( pid == anti ) { return pp ; }
  ///
  const ParticleProperty* antiPP = ppFromPID ( anti ) ;
  if ( 0 == antiPP ) 
  {
    LoKi::Report::Error
      ( std::string(" LoKi::Particles::antiParticle():") + 
        "antiParticle('" + (pp->particle())+"') points to NULL"+ 
        " return NULL" ) ;
    return  0 ;
  }
  return antiPP ;
};
// ============================================================================

// ============================================================================
/** get Particle ID for antiparticle 
 *  @param  pid particle 
 *  @return pid fro antiparticle 
 */
// ============================================================================
LHCb::ParticleID LoKi::Particles::antiParticle( const LHCb::ParticleID& pid  ) 
{
  // check the validity of own pid 
  const ParticleProperty* p1 = LoKi::Particles::_ppFromPID( pid ) ;
  //
  if ( 0 == p1 ) 
  {
    LoKi::Report::Error
      ( " LoKi::Particles::antiParticle("              + 
        boost::lexical_cast<std::string>( pid.pid() )  + 
        "): ParticleProperty* points to NULL "         + 
        " return LHCb::ParticleID()  "                 ) ;
    return LHCb::ParticleID()  ;
  }
  // check for other sign 
  const LHCb::ParticleID apid = -1 * pid.pid() ;
  const ParticleProperty* p2   = LoKi::Particles::_ppFromPID( apid ) ;
  // 
  return 0 == p2 ? pid : apid ;
};
// ============================================================================

// ============================================================================
/** retrieve particle name for given PID 
 *  @param pid particle PID 
 *  @return particle name 
 */
// ============================================================================
std::string  LoKi::Particles::nameFromPID ( const LHCb::ParticleID& pid ) 
{
  const ParticleProperty* pp = LoKi::Particles::_ppFromPID( pid ) ;
  if ( 0 == pp ) 
  {
    LoKi::Report::Error
      ( " LoKi::Particles::nameFromPID("               + 
        boost::lexical_cast<std::string>( pid.pid() )  + 
        "): ParticleProperty* points to NULL "         + 
        " return '" + s_InvalidPIDName + "'"           ) ;
    return s_InvalidPIDName ;
  }
  return pp->particle() ;
};
// ============================================================================

// ============================================================================
/** retrieve the lifetime for the particle form the name 
 *  @param name particle name 
 */
// ============================================================================
const double LoKi::Particles::lifeTime     
( const std::string&      name ) 
{
  const ParticleProperty* pp = LoKi::Particles::_ppFromName ( name ) ;
  if ( 0 == pp )
  {
    LoKi::Report::Error
      ( "LoKi::Particles::lifeTime('" + 
        name + 
        "'): ParticleProperty is NULL" );
    return LoKi::Constants::InvalidTime ;
  }
  //
  return pp -> lifetime() * Gaudi::Units::c_light ;
} ;
// ============================================================================

// ============================================================================
/** retrieve the lifetime for the particle form the name 
 *  @param name particle name 
 */
// ============================================================================
const double LoKi::Particles::lifeTime     
( const LHCb::ParticleID& pid ) 
{
  const ParticleProperty* pp = LoKi::Particles::_ppFromPID ( pid ) ;
  if ( 0 == pp )
  {
    LoKi::Report::Error
      ( "LoKi::Particles::lifeTime('" + 
        boost::lexical_cast<std::string>( pid.pid() )  + 
        "'): ParticleProperty* is NULL");
    return LoKi::Constants::InvalidTime ;
  }
  //
  return pp -> lifetime() * Gaudi::Units::c_light ;
} ;  
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
