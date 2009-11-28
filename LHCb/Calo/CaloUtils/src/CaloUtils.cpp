// $Id: CaloUtils.cpp,v 1.1 2009-11-28 19:12:40 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// local
// ============================================================================
#include "CaloUtils/CaloUtils.h"
// ============================================================================
/** @file 
 *  The Implementation file for namespace LHCb::CaloHelpers
 *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
 *  @date 2009-11-28
 */
// ============================================================================
// vector -> set converter 
// ============================================================================
LHCb::CaloCellID::Set 
LHCb::CaloHelpers::toSet 
( const LHCb::CaloCellID::Vector& inp ) 
{
  LHCb::CaloCellID::Set out ;
  out.insert ( inp.begin() , inp.end() ) ;
  return out ;
}  
// ============================================================================
// vector -> set converter 
// ============================================================================
LHCb::CaloDigit::Set
LHCb::CaloHelpers::toSet 
( const LHCb::CaloDigit::Vector&  inp )
{
  LHCb::CaloDigit::Set out ;
  out.insert ( inp.begin() , inp.end() ) ;
  return out ;
} 
// ============================================================================
// set -> vector converter 
// ============================================================================
LHCb::CaloCellID::Vector
LHCb::CaloHelpers::toVector 
( const LHCb::CaloCellID::Set& inp ) 
{
  LHCb::CaloCellID::Vector out ;
  out.insert ( out.end() , inp.begin() , inp.end() ) ;
  return out ;
}  
// ============================================================================
// set -> vetcor converter 
// ============================================================================
LHCb::CaloDigit::Vector
LHCb::CaloHelpers::toVector 
( const LHCb::CaloDigit::Set&  inp )
{
  LHCb::CaloDigit::Vector out ;
  out.insert ( out.end() , inp.begin() , inp.end() ) ;
  return out ;
}
// ============================================================================
// vector -> set converter 
// ============================================================================
void LHCb::CaloHelpers::toSet
( const LHCb::CaloCellID::Vector& inp , 
  LHCb::CaloCellID::Set&          out ) 
{
  out.clear() ;
  out.insert ( inp.begin() , inp.end() ) ;
}  
// ============================================================================
// vector -> set converter v
// ============================================================================
void LHCb::CaloHelpers::toSet
( const LHCb::CaloDigit::Vector& inp , 
  LHCb::CaloDigit::Set&          out ) 
{
  out.clear() ;
  out.insert ( inp.begin() , inp.end() ) ;
}
// ============================================================================
// set -> vector converter 
// ============================================================================
void LHCb::CaloHelpers::toVector
( const LHCb::CaloCellID::Set&    inp , 
  LHCb::CaloCellID::Vector&       out ) 
{
  out.clear() ;
  out.insert ( out.end() , inp.begin() , inp.end() ) ;
}  
// ============================================================================
// set -> vector converter v
// ============================================================================
void LHCb::CaloHelpers::toVector
( const LHCb::CaloDigit::Set&     inp , 
  LHCb::CaloDigit::Vector&        out ) 
{
  out.clear() ;
  out.insert ( out.end() , inp.begin() , inp.end() ) ;
}
  
// ============================================================================
/*  get the cell form the set 
 *  @param cells (INPUT) the input set
 *  @param index (INPUT) the index 
 *  @return the cell
 *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
 *  @date 2009-11-28
 */
// ============================================================================
LHCb::CaloCellID LHCb::CaloHelpers::_get_at_ 
( const LHCb::CaloCellID::Set& cells ,
  const size_t                 index ) 
{
  if ( cells.size() <= index ) { return LHCb::CaloCellID() ; }
  LHCb::CaloCellID::Set::const_iterator it = cells.begin() ;
  std::advance ( it , index ) ;
  return *it ;
}
// ========================================================================
/*  get the digit from the set 
 *  @param cells (INPUT) the input set
 *  @param index (INPUT) the index 
 *  @return the digit
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-11-28
 */
// ========================================================================
const LHCb::CaloDigit* LHCb::CaloHelpers::_get_at_ 
( const LHCb::CaloDigit::Set&  cells ,
  const size_t                 index ) 
{
  if ( cells.size() <= index ) { return 0 ; }
  LHCb::CaloDigit::Set::const_iterator it = cells.begin() ;
  std::advance ( it , index ) ;
  return *it ;
}
// ========================================================================
// insert the object in set 
// ========================================================================
size_t LHCb::CaloHelpers::_insert_ 
( LHCb::CaloCellID::Set&  cells ,
  const LHCb::CaloCellID& cell  ) 
{
  cells.insert ( cell ) ;
  return cells.size() ;
}
// ========================================================================
// insert the object in set 
// ========================================================================
size_t LHCb::CaloHelpers::_insert_ 
( LHCb::CaloDigit::Set&  cells ,
  const LHCb::CaloDigit* digit ) 
{
  cells.insert ( digit ) ;
  return cells.size() ;
}
// ============================================================================
// The END 
// ============================================================================
