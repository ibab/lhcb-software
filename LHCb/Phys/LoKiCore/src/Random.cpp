// $Id: Random.cpp,v 1.2 2009-12-06 18:20:57 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <stdlib.h>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Services.h"
#include "LoKi/Random.h"
// ============================================================================
/** @file
 *  the implementation file for functors from the namespace LoKi::Random
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2008-03-19
 */
// ============================================================================
// constructor from the numbers 
// ============================================================================
LoKi::Random::Numbers::Numbers 
( const  Rndm::Numbers&  numbers ) 
  : LoKi::BasicFunctors<double>::Function () 
  , m_service ( (IRndmGenSvc*) 0 ) 
  , m_param   ( 0       ) 
  , m_numbers ( numbers ) 
{}
// ============================================================================
// constructor from the generator and the service 
// ============================================================================
LoKi::Random::Numbers::Numbers 
( const IRndmGen::Param& param   , 
  IRndmGenSvc*           service ) 
  : LoKi::BasicFunctors<double>::Function () 
  , m_service ( service        ) 
  , m_param   ( param.clone()  ) 
  , m_numbers () 
{}
// ============================================================================
// constructor from the generator and the service 
// ============================================================================
LoKi::Random::Numbers::Numbers 
( IRndmGenSvc*           service     ,
  const IRndmGen::Param& param       )
  : LoKi::BasicFunctors<double>::Function () 
  , m_service ( service        ) 
  , m_param   ( param.clone()  ) 
  , m_numbers () 
{}
// ============================================================================
// copy constructor 
// ============================================================================
LoKi::Random::Numbers::Numbers 
( const LoKi::Random::Numbers& right ) 
  : LoKi::AuxFunBase                      ( right )  
  , LoKi::BasicFunctors<double>::Function ( right ) 
  , m_service ( right.m_service ) 
  , m_param   ( 0 )
  , m_numbers ( right.m_numbers ) 
{ if ( 0 != right.m_param ) { m_param = right.m_param->clone() ; } }
// ============================================================================
// destructor 
// ============================================================================
LoKi::Random::Numbers::~Numbers() 
{ if ( 0 != m_param ) { delete m_param ; m_param = 0 ; } }
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Random::Numbers*
LoKi::Random::Numbers::clone() const 
{ return new LoKi::Random::Numbers(*this) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Random::Numbers::result_type 
LoKi::Random::Numbers::operator() 
  ( LoKi::Random::Numbers::argument /* a */ ) const { return shoot() ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Random::Numbers::result_type 
LoKi::Random::Numbers::shoot() const 
{
  // everything is ok ? SHOOT! 
  if ( m_numbers ) { return m_numbers.shoot() ; }                // RETURN
  // 
  Assert ( 0 != m_param , "Invalid parameters for Random Number Generator" ) ;
  // locate the service if needed 
  if ( !m_service )
  {
    LoKi::Services& svcs = LoKi::Services::instance() ;
    //  try to catch the random number service from LoKi Service 
    m_service  = svcs.randSvc() ;
    Assert ( !!m_service , 
             "Unable to locate the Random Numbers Service " ) ;
  }
  // try to initialize it:
  StatusCode sc = m_numbers.initialize( m_service , *m_param ) ;
  Assert ( sc.isSuccess() , 
           "Unable to initialize Random Number Generator", sc ) ;
  //
  return m_numbers.shoot() ;                                  // RETURN 
}
// ============================================================================
// constructor  from the descriptor and the service 
// ============================================================================
LoKi::Random::Uniform::Uniform 
( const Rndm::Flat& flat    , 
  IRndmGenSvc*      service ) 
  : LoKi::Random::Numbers ( flat , service )
  , m_flat ( flat ) 
{}
// ============================================================================
// constructor  from the descriptor and the service 
// ============================================================================
LoKi::Random::Uniform::Uniform 
( IRndmGenSvc*      service ,
  const Rndm::Flat& flat    )  
  : LoKi::Random::Numbers ( flat , service )
  , m_flat ( flat ) 
{}
// ============================================================================
// constructor from low, high and service 
// ============================================================================
LoKi::Random::Uniform::Uniform 
( const double      low     , 
  const double      high    , 
  IRndmGenSvc*      service ) 
  : LoKi::Random::Numbers ( Rndm::Flat ( low , high ) , service )
  , m_flat ( low , high ) 
{}
// ============================================================================
// constructor from service , low and high 
// ============================================================================
LoKi::Random::Uniform::Uniform 
( IRndmGenSvc*      service , 
  const double      low     , 
  const double      high    ) 
  : LoKi::Random::Numbers ( Rndm::Flat ( low , high ) , service )
  , m_flat ( low , high ) 
{}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Random::Uniform*
LoKi::Random::Uniform::clone() const 
{ return new LoKi::Random::Uniform(*this) ; }
// ============================================================================
// OPTIONAL: nice a nice printout 
// ============================================================================
std::ostream& 
LoKi::Random::Uniform::fillStream ( std::ostream& s  ) const 
{
  if ( 0 == m_flat.minimum() && 1 == m_flat.maximum() ) 
  { return s << "XRNDM" ; }
  return s << "XRND(" << m_flat.minimum() << "," << m_flat.maximum() << ")" ;  
}
// ============================================================================
// constructor  from the descriptor and the service 
// ============================================================================
LoKi::Random::Gaussian::Gaussian
( const Rndm::Gauss& gauss   , 
  IRndmGenSvc*       service ) 
  : LoKi::Random::Numbers ( gauss , service )
  , m_gauss ( gauss ) 
{}
// ============================================================================
// constructor  from the descriptor and the service 
// ============================================================================
LoKi::Random::Gaussian::Gaussian
( IRndmGenSvc*       service ,
  const Rndm::Gauss& gauss   )
  : LoKi::Random::Numbers ( gauss , service )
  , m_gauss ( gauss ) 
{}
// ============================================================================
// constructor from mean, sigma and service 
// ============================================================================
LoKi::Random::Gaussian::Gaussian
( const double      mean    , 
  const double      sigma   , 
  IRndmGenSvc*      service ) 
  : LoKi::Random::Numbers ( Rndm::Gauss ( mean , sigma ) , service )
  , m_gauss ( mean , sigma  ) 
{}
// ============================================================================
// constructor from the service, mean and sigma 
// ============================================================================
LoKi::Random::Gaussian::Gaussian
( IRndmGenSvc*      service ,
  const double      mean    , 
  const double      sigma   )
  : LoKi::Random::Numbers ( Rndm::Gauss ( mean , sigma ) , service )
  , m_gauss ( mean , sigma  ) 
{}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Random::Gaussian* 
LoKi::Random::Gaussian::clone() const 
{ return new LoKi::Random::Gaussian(*this) ; }
// ============================================================================
// OPTIONAL: nice a nice printout 
// ============================================================================
std::ostream& 
LoKi::Random::Gaussian::fillStream ( std::ostream& s  ) const 
{
  if ( 0 == m_gauss.mean() && 1 == m_gauss.sigma() )
  { return s << "XGAUSS" ; }
  return s << "XGAUS(" << m_gauss.mean() << "," << m_gauss.sigma() << ")" ;
}
// ============================================================================
/*  Constructor 
 *  if seed is not equal to 0, the call for <c>srand</c> will be performed
 *  @param seed the seed (if not zero) 
 */
// ============================================================================
LoKi::Random::Rand::Rand ( const unsigned int seed ) 
  : LoKi::BasicFunctors<double>::Function () 
{
  if ( 0 != seed ) { srand ( seed ) ; } // reset the seed 
}
// ============================================================================
// virtual and protected desctrustor 
// ============================================================================
LoKi::Random::Rand::~Rand () { }
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Random::Rand* 
LoKi::Random::Rand::clone() const 
{ return new LoKi::Random::Rand(*this) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Random::Rand::result_type 
LoKi::Random::Rand::operator() 
  ( LoKi::Random::Rand::argument /* a */ ) const 
{
  double res = rand()  ;
  res /= RAND_MAX ;
  return res ;
}
// ============================================================================
// OPTIONAL: just a nice printout 
// ============================================================================
std::ostream& LoKi::Random::Rand::fillStream ( std::ostream& s  ) const 
{ return s << "XRAND" ; }
// ============================================================================



// ============================================================================
// The END 
// ============================================================================
