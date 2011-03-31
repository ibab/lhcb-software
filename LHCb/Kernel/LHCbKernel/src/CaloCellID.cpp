// ============================================================================
// Include files
// ============================================================================
// STD & STL
// ============================================================================
#include <iomanip>
#include <sstream>
// ============================================================================
// LHCbKernel
// ============================================================================
#include "Kernel/CaloCellCode.h"
#include "Kernel/CaloCellID.h"
// ============================================================================
// Boost
// ============================================================================
//#include "boost/assert.hpp"
// ============================================================================
/** @file
 *  Implementation file for non-inlined functions of class LHCb::CaloCellID
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-09-29
 */
// ============================================================================
// Set Calo form string
// ============================================================================
LHCb::CaloCellID& LHCb::CaloCellID::setCalo(const std::string& CaloName)
{
  const int calo = CaloCellCode::caloNum( CaloName ) ;
  // BOOST_ASSERT ( 0 <= calo && "CaloCellID::setCalo Invalid Calorimeter Index" ) ;
  return setCalo( calo ) ;
}
// ============================================================================
// Set Area from string
// ============================================================================
LHCb::CaloCellID& LHCb::CaloCellID::setArea
( unsigned int Calo , const std::string& Area)
{
  const int area = CaloCellCode::caloArea( Calo , Area ) ;
  // BOOST_ASSERT ( 0 <= area && "CaloCellID::setArea Invalid Calorimeter Area" ) ;
  return setArea ( area ) ;
}
// ============================================================================
// printout to output stream
// ============================================================================
std::ostream& LHCb::CaloCellID::fillStream ( std::ostream& os ) const
{
  os << "(" ;
  /// corrupted ?!
  if ( 0 != rest() ) { os << "?" << std::setw(4) << rest() << "?," ; }
  return os
    << "'" << CaloCellCode::caloName ( calo()           ) << "',"
    << "'" << CaloCellCode::caloArea ( calo() , area () ) << "',"
    << std::setw(2)  << row  () << ","
    << std::setw(2)  << col  () << ")" ;
}
// ============================================================================
// conversion to bit-string
// ============================================================================
std::string LHCb::CaloCellID::bits(char del) const
{
  std::string  str("[");
  const ContentType  one = 1;
  for( int n  = ( 0 == rest() )? CaloCellCode::BitsAll - 1 :
         CaloCellCode::BitsTotal - 1 ; n >= 0 ; --n  )
  {
    unsigned int pos = n;
    bool isNull =   ( 0 == ( ( one << pos ) & contents() ) );
    str += isNull ? '0' : '1';
    if      ( 0                       == pos )   break      ;
    if      ( 0                       == del )   continue   ;
    if      ( CaloCellCode::ShiftRest == pos ) { str += del ; }
    else if ( CaloCellCode::ShiftCalo == pos ) { str += del ; }
    else if ( CaloCellCode::ShiftArea == pos ) { str += del ; }
    else if ( CaloCellCode::ShiftRow  == pos ) { str += del ; }
    else if ( CaloCellCode::ShiftCol  == pos ) { str += del ; }
    else if ( CaloCellCode::ShiftAll  == pos ) { str += del ; }
  }
  return str += "]";
}
// ============================================================================
// conversion to string
// ============================================================================
std::string LHCb::CaloCellID::toString() const
{
  std::ostringstream s ;
  fillStream ( s ) ;
  return s.str() ;
}
// ============================================================================
// The END
// ============================================================================
