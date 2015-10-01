// $ID:$ 
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <cmath>
#include <sstream>
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/LHCbMath.h"
#include "LHCbMath/Power.h"
// ============================================================================
// Local
// ============================================================================
#include "LHCbMath/WStatEntity.h"
// ============================================================================
// Boost
// ============================================================================
#include "boost/format.hpp"
// ============================================================================
/** @file 
 *  Implementation file for class Gaudi::Math::WStatEntity
 *  @see Gaudi::Math::WStatEntity
 *  @see StatEntity
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 2014-04-07 
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ============================================================================
namespace 
{
  // ==========================================================================
  const LHCb::Math::Equal_To<double> s_equal ;
  // ==========================================================================
  inline bool isnull ( const double value ) 
  { return 0 == value || s_equal ( 0 , value ) ; }
  // ==========================================================================
}
// ============================================================================
// empty constructor 
// ============================================================================
Gaudi::Math::WStatEntity::WStatEntity()
  : m_sum     ( 0 ) 
  , m_sum2    ( 0 )
  , m_values  (   )
  , m_weights (   )
{}
// ============================================================================
// constructor from StatEntity of values 
// ============================================================================
Gaudi::Math::WStatEntity::WStatEntity ( const StatEntity& values ) 
  : m_sum     ( values.sum      () ) 
  , m_sum2    ( values.sum2     () ) 
  , m_values  ( values             ) 
  , m_weights ( values.nEntries () , 
                values.nEntries () , 
                values.nEntries () , 1 , 1 ) 
{}
// ============================================================================
// update statistics 
// ============================================================================
Gaudi::Math::WStatEntity&
Gaudi::Math::WStatEntity::add   
( const double value  ,  
  const double weight )
{
  m_sum     += weight * value         ;
  m_sum2    += weight * value * value ;  
  //
  if ( !isnull ( weight ) ) { m_values += value ; }
  //
  m_weights += weight ;
  //
  return *this ;
}
// ============================================================================
// get the sample mean
// ============================================================================
double Gaudi::Math::WStatEntity::mean () const
{
  return ( 0 == nEntries () 
           || isnull ( m_sum            ) 
           || isnull ( m_weights.sum () ) ) ? 0.0 
    : m_sum / m_weights.sum () ;
}
// ============================================================================
// get the sample mean
// ============================================================================
double Gaudi::Math::WStatEntity::meanErr () const
{
  const double neff = nEff() ;
  if ( isnull ( neff ) ) { return 0 ; }
  //
  const double v = dispersion() / neff ;
  //
  return v <= 0 ? 0.0 : std::sqrt ( v ) ;
}
// ============================================================================
// calculate dispersion 
// ============================================================================
double Gaudi::Math::WStatEntity::dispersion () const
{ 
  //
  if ( 1 >= nEntries() || isnull ( m_weights.sum() ) ) { return 0 ; }
  //
  return m_sum2 / m_weights.sum () - Gaudi::Math::pow ( mean() , 2 ) ;
}
// ============================================================================
// calculate rms 
// ============================================================================
double Gaudi::Math::WStatEntity::rms () const
{
  const double d = dispersion () ;
  //
  if ( 0 >= d || isnull ( d ) ) { return 0 ; }
  return std::sqrt ( d ) ;
}
// ============================================================================
// calculate effectiev number of entries 
// ============================================================================
double Gaudi::Math::WStatEntity::nEff () const
{
  //
  if ( 0 == nEntries() || isnull ( m_weights.sum2 () ) ) { return 0 ; }
  //
  return Gaudi::Math::pow ( m_weights.sum() , 2 ) /  m_weights.sum2 () ;
}
// ============================================================================
// reset statistic 
// ============================================================================
void Gaudi::Math::WStatEntity::reset () 
{
  m_sum  = 0 ;
  m_sum2 = 0 ;
  m_weights.reset() ;
}
// ============================================================================
// printout 
// ============================================================================
std::ostream& Gaudi::Math::WStatEntity::fillStream ( std::ostream& o ) const 
{
  boost::format fmt1 ("#=%|-11.5g| Sum=%|-11.5g|" ) ;
  o << fmt1 % nEff() % m_sum ;
  boost::format fmt2 ( " Mean=%|#10.4g| +- %|-#10.5g| Min/Max=%|#10.4g|/%|-#10.4g|" ) ;
  o << fmt2 % mean() % rms() % m_values.min() % m_values.max() ;
  return o ;
}
// ============================================================================
// convert to tring 
// ============================================================================
std::string Gaudi::Math::WStatEntity::toString () const
{
  std::ostringstream ost ;
  fillStream ( ost )  ;
  return ost.str () ;
}
// ============================================================================
// The END 
// ============================================================================
