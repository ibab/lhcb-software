// $Id: CaloUtils.h,v 1.1 2009-11-28 19:12:40 ibelyaev Exp $
// ============================================================================
#ifndef CALOUTILS_CALOUTILS_H 
#define CALOUTILS_CALOUTILS_H 1
// ============================================================================
// Include files
// ============================================================================
// Event 
// ============================================================================
#include "Event/CaloDigit.h"
// ============================================================================
namespace LHCb
{
  // ==========================================================================
  /** @namespace LHCb::CaloHelpers
   *  Helper namespace for dcoration of Calo-objects 
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2009-11-28
   */  
  namespace CaloHelpers
  {
    // ========================================================================
    /// vector -> set converter 
    LHCb::CaloCellID::Set toSet ( const LHCb::CaloCellID::Vector&    inp ) ;            
    /// vector -> set converter 
    LHCb::CaloDigit::Set  toSet ( const LHCb::CaloDigit::Vector&     inp ) ;
    /// set -> vector converter 
    LHCb::CaloCellID::Vector toVector ( const LHCb::CaloCellID::Set& inp ) ;
    /// set -> vector converter 
    LHCb::CaloDigit::Vector  toVector ( const LHCb::CaloDigit::Set&  inp ) ;
    // ========================================================================
    /// vector -> set converter 
    void toSet    ( const LHCb::CaloCellID::Vector&       inp , 
                    LHCb::CaloCellID::Set&                out ) ;
    /// vector -> set converter 
    void toSet    ( const LHCb::CaloDigit::Vector&        inp , 
                    LHCb::CaloDigit::Set&                 out ) ;
    /// set -> vector converter 
    void toVector ( const LHCb::CaloCellID::Set&          inp , 
                    LHCb::CaloCellID::Vector&             out ) ;
    /// set -> vector converter 
    void toVector ( const LHCb::CaloDigit::Set&           inp , 
                    LHCb::CaloDigit::Vector&              out ) ;
    // ========================================================================    
    /** get the cell form the set 
     *  @param cells (INPUT) the input set
     *  @param index (INPUT) the index 
     *  @return the cell
     *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
     *  @date 2009-11-28
     */
    LHCb::CaloCellID       _get_at_ 
    ( const LHCb::CaloCellID::Set& cells ,
      const size_t                 index ) ;
    // ========================================================================
    /** get the digit from the set 
     *  @param cells (INPUT) the input set
     *  @param index (INPUT) the index 
     *  @return the digit
     *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
     *  @date 2009-11-28
     */
    const LHCb::CaloDigit* _get_at_ 
    ( const LHCb::CaloDigit::Set& cells ,
      const size_t                index ) ;
    // ========================================================================
    /// insert the object to set 
    size_t    _insert_ 
    ( LHCb::CaloCellID::Set&  cells ,
      const LHCb::CaloCellID& cell  ) ;
    // ========================================================================
    /// insert the object to set 
    size_t    _insert_ 
    ( LHCb::CaloDigit::Set&  cells ,
      const LHCb::CaloDigit* digit ) ;
    // ========================================================================
  } //                                         end of namespace LHCb::CaloUtils 
  // ==========================================================================
} //                                                      end of namespace LHCb
// ============================================================================
// The END 
// ============================================================================
#endif // CALOUTILS_CALOUTILS_H
// ============================================================================
