// $Id: GetDecay.h,v 1.1 2008-05-05 11:46:04 ibelyaev Exp $
// ============================================================================
#ifndef DAVINCIKERNEL_GETDECAY_H 
#define DAVINCIKERNEL_GETDECAY_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <vector>
#include <string>
// ============================================================================
// LHCbKernel
// ============================================================================
#include "Kernel/Decay.h"
// ============================================================================
class IDecodeSimpleDecayString ;
// ============================================================================
namespace DaVinci  
{
  // ==========================================================================
  /** create decay object from the descriptor
   *  @attention only "main" decay is created!
   *  @param descriptor the decay descriptor 
   *  @param decode the decoder tool
   *  @return the constructed decay
   */
  LHCb::Decay decay 
  ( const std::string&        descriptor , 
    IDecodeSimpleDecayString* decoder    ) ;
  // ==========================================================================
  /** create all decay objects from the descriptor
   *  @param descriptor the decay descriptor 
   *  @param decode the decoder tool
   *  @return the constructed decay
   */
  LHCb::Decays decays
  ( const std::string&        descriptor , 
    IDecodeSimpleDecayString* decoder    ) ;
  /** create all decay objects from the descriptor
   *  @param descriptors the decay descriptors 
   *  @param decode the decoder tool
   *  @return the constructed decay
   */
  LHCb::Decays decays
  ( const std::vector<std::string>& descriptors , 
    IDecodeSimpleDecayString*       decoder     ) ;
  // ==========================================================================
} // end of namespace DaVinci
// ============================================================================
// The END
// ============================================================================
#endif // DAVINCIKERNEL_GETDECAY_H
// ============================================================================
