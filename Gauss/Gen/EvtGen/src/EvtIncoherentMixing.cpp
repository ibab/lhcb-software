// $Id: EvtIncoherentMixing.cpp,v 1.4 2003-10-29 09:25:33 robbep Exp $
// Include files 



// local
#include "EvtGenBase/EvtIncoherentMixing.hh"

#include "EvtGenBase/EvtPDL.hh"
#include "EvtGenBase/EvtId.hh"
#include "EvtGenBase/EvtRandom.hh"

//-----------------------------------------------------------------------------
// Implementation file for class : EvtIncoherentMixing
//
// 2003-10-09 : Patrick Robbe
//-----------------------------------------------------------------------------


bool EvtIncoherentMixing::_doB0Mixing = false ;
bool EvtIncoherentMixing::_doBsMixing = false ;
bool EvtIncoherentMixing::_enableFlip = false ;
double EvtIncoherentMixing::_dGammad = 0. ;
double EvtIncoherentMixing::_deltamd = 0.489e12 ;
double EvtIncoherentMixing::_dGammas = 6.517e10 ;
double EvtIncoherentMixing::_deltams = 15.e12 ;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
EvtIncoherentMixing::EvtIncoherentMixing(  ) {
  _doB0Mixing = false ;
  _doBsMixing = false ;
  _dGammad = 0. ;
  _dGammas = 0. ;
  _deltamd = 0.489e12 ;
  _deltams = 0. ;
  _enableFlip = false ;
}
//=============================================================================
EvtIncoherentMixing::~EvtIncoherentMixing( ) 
{
}
// ============================================================================
void EvtIncoherentMixing::incoherentB0Mix( const EvtId id, double &t , 
                                           int &mix )
{
  static EvtId B0  = EvtPDL::getId( "B0" ) ;
  static EvtId B0B = EvtPDL::getId( "anti-B0" ) ;
 
  if ( ( B0 != id ) && ( B0B != id ) ) {
    report(ERROR,"EvtGen") << "Bad configuration in incoherentB0Mix" 
                           << std::endl ;
    ::abort() ;
  }
  
  double x = getdeltamd() * EvtPDL::getctau( B0 ) / EvtConst::c ;

  double y = getdGammad() / ( 2. * EvtPDL::getctau( B0 ) / EvtConst::c ) ;

  double fac = 1. ; // No CP violation

  double mixprob = ( x*x + y*y ) / ( x*x + y*y + ( 1./fac ) * 
                                     ( 2. + x*x - y*y ) ) ;

  int mixsign ;
  
  // decide if state is mixed
  mixsign = ( mixprob > EvtRandom::Flat( 0. , 1. ) ) ? -1 : 1 ;

  double prob ;
  
  do {
    t = -log( EvtRandom::Flat() ) * EvtPDL::getctau( B0 ) ;
    prob = 1. + exp( y * t / EvtPDL::getctau( B0 ) ) +
      mixsign * 2. * exp( 0.5 * y * t / EvtPDL::getctau( B0 ) ) * 
      cos( getdeltamd() * t / EvtConst::c ) ;
  } while ( prob < 4. * EvtRandom::Flat() ) ;
 
  mix = 0 ;
  if ( mixsign == -1 ) mix = 1 ;
  
  return ;  
}
// ============================================================================
void EvtIncoherentMixing::incoherentBsMix( const EvtId id, double &t , 
                                           int &mix )
{
  static EvtId BS  = EvtPDL::getId( "B_s0" ) ;
  static EvtId BSB = EvtPDL::getId( "anti-B_s0" ) ;
 
  if ( ( BS != id ) && ( BSB != id ) ) {
    report(ERROR,"EvtGen") << "Bad configuration in incoherentBsMix" 
                           << std::endl ;
    ::abort() ;
  }
  
  double x = getdeltams() * EvtPDL::getctau( BS ) / EvtConst::c ;

  double y = getdGammas() * ( EvtPDL::getctau( BS ) / EvtConst::c ) / 2. ;

  double fac = 1. ; // No CP violation

  double mixprob = ( x*x + y*y ) / ( x*x + y*y + ( 1./fac ) * 
                                     ( 2. + x*x - y*y ) ) ;

  int mixsign ;
  
  // decide if state is mixed
  mixsign = ( mixprob > EvtRandom::Flat( 0. , 1. ) ) ? -1 : 1 ;

  double prob ;
  
  do {
    t = -log( EvtRandom::Flat() ) * EvtPDL::getctau( BS ) ;
    prob = 1. + exp( y * t / EvtPDL::getctau( BS ) ) +
      mixsign * 2. * exp( 0.5 * y * t / EvtPDL::getctau( BS ) ) * 
      cos( getdeltams() * t / EvtConst::c ) ;
  } while ( prob < 4. * EvtRandom::Flat() ) ;
 
  mix = 0 ;
  if ( mixsign == -1 ) mix = 1 ;
  
  return ;  
}

// ========================================================================
bool EvtIncoherentMixing::isBsMixed ( EvtParticle * p ) 
{ 
  if ( ! ( p->getParent() ) ) return false ;
  
  static EvtId BS0=EvtPDL::getId("B_s0");
  static EvtId BSB=EvtPDL::getId("anti-B_s0");
  
  if ( ( p->getId() != BS0 ) && ( p->getId() != BSB ) ) return false ;
  
  if ( ( p->getParent()->getId() == BS0 ) ||
       ( p->getParent()->getId() == BSB ) ) return true ;
  
  return false ;
}

// ========================================================================
bool EvtIncoherentMixing::isB0Mixed ( EvtParticle * p ) 
{ 
  if ( ! ( p->getParent() ) ) return false ;
  
  static EvtId B0 =EvtPDL::getId("B0");
  static EvtId B0B=EvtPDL::getId("anti-B0");
  
  if ( ( p->getId() != B0 ) && ( p->getId() != B0B ) ) return false ;
  
  if ( ( p->getParent()->getId() == B0 ) ||
       ( p->getParent()->getId() == B0B ) ) return true ;
  
  return false ;
}
//============================================================================
// Return the tag of the event (ie the anti-flavour of the produced 
// B meson). Flip the flavour of the event with probB probability
//============================================================================
void EvtIncoherentMixing::OtherB( EvtParticle * p ,
                                  double & t ,
                                  EvtId & otherb ,
                                  double probB ) 
{
  if ( ( isB0Mixed( p ) ) || ( isBsMixed( p ) ) ) {
    p->getParent()->setLifetime() ;
    t = p->getParent()->getLifetime() ;
  }
  else {
    p->setLifetime() ;
    t = p->getLifetime() ;
  }

  if ( flipIsEnabled() ) {
  
    // Flip the flavour of the particle with probability probB
    bool isFlipped = ( EvtRandom::Flat( 0. , 1. ) < probB ) ;
    
    if ( isFlipped ) {
      if ( ( isB0Mixed( p ) ) || ( isBsMixed( p ) ) ) {
        p->getParent()
          ->setId( EvtPDL::chargeConj( p->getParent()->getId() ) ) ;
        p->setId( EvtPDL::chargeConj( p->getId() ) ) ;
      }
      else {
        p->setId( EvtPDL::chargeConj( p->getId() ) ) ;
      }
    }
  }
  
  if ( ( isB0Mixed( p ) ) || ( isBsMixed( p ) ) ) {
    // if B has mixed, tag flavour is charge conjugate of parent of B-meson
    otherb = EvtPDL::chargeConj( p->getParent()->getId() ) ;
  }
  else {
    // else it is opposite flavour than this B hadron
    otherb = EvtPDL::chargeConj( p->getId() ) ;
  }

  return ;
}
//============================================================================
// Return the tag of the event (ie the anti-flavour of the produced 
// B meson). No flip
//============================================================================
void EvtIncoherentMixing::OtherB( EvtParticle * p ,
                                  double & t ,
                                  EvtId & otherb ) 
{
  if ( ( isB0Mixed( p ) ) || ( isBsMixed( p ) ) ) {
    p->getParent()->setLifetime() ;
    t = p->getParent()->getLifetime() ;
  }
  else {
    p->setLifetime() ;
    t = p->getLifetime() ;
  }
  
  if ( ( isB0Mixed( p ) ) || ( isBsMixed( p ) ) ) {
    // if B has mixed, tag flavour is charge conjugate of parent of B-meson
    otherb = EvtPDL::chargeConj( p->getParent()->getId() ) ;
  }
  else {
    // else it is opposite flavour than this B hadron
    otherb = EvtPDL::chargeConj( p->getId() ) ;
  }

  return ;
}

