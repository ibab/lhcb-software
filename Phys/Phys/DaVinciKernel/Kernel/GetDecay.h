// $Id: GetDecay.h,v 1.2 2008-05-29 14:55:49 cattanem Exp $
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
   *  @param decoder the decoder tool
   *  @return the constructed decay
   */
  LHCb::Decay decay 
  ( const std::string&        descriptor , 
    IDecodeSimpleDecayString* decoder    ) ;
  // ==========================================================================
  /** create all decay objects from the descriptor
   *  @param descriptor the decay descriptor 
   *  @param decoder the decoder tool
   *  @return the constructed decay
   */
  LHCb::Decays decays
  ( const std::string&        descriptor , 
    IDecodeSimpleDecayString* decoder    ) ;
  /** create all decay objects from the descriptor
   *  @param descriptors the decay descriptors 
   *  @param decoder the decoder tool
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
