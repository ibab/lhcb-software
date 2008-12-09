// $Id: GetDecay.h,v 1.4 2008-12-09 07:09:52 pkoppenb Exp $
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
// PartProp
// ============================================================================
#include "Kernel/Decay.h"
// ============================================================================
class IDecodeSimpleDecayString ;
// ============================================================================
namespace Decays
{
  // ==========================================================================
  /** create decay object from the descriptor
   *  @attention only "main" decay is created!
   *  @param descriptor the decay descriptor 
   *  @param decoder the decoder tool
   *  @return the constructed decay
   */
  Decays::Decay decay 
  ( const std::string&        descriptor , 
    IDecodeSimpleDecayString* decoder    ) ;
  // ==========================================================================
  /** create all decay objects from the descriptor
   *  @param descriptor the decay descriptor 
   *  @param decoder the decoder tool
   *  @return the constructed decay
   */
  std::vector<Decays::Decay> decays
  ( const std::string&        descriptor , 
    IDecodeSimpleDecayString* decoder    ) ;
  /** create all decay objects from the descriptor
   *  @param descriptors the decay descriptors 
   *  @param decoder the decoder tool
   *  @return the constructed decay
   */
  std::vector<Decays::Decay> decays
  ( const std::vector<std::string>& descriptors , 
    IDecodeSimpleDecayString*       decoder     ) ;
  // ==========================================================================
} // end of namespace Decays
// ============================================================================
// The END
// ============================================================================
#endif // DAVINCIKERNEL_GETDECAY_H
// ============================================================================
