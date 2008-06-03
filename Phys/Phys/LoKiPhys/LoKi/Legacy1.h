// $Id: Legacy1.h,v 1.1 2008-06-03 14:07:02 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_LEGACY1_H 
#define LOKI_LEGACY1_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/PhysTypes.h"
#include "LoKi/AuxDesktopBase.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** @namespace LoKi::Legacy
   *  the namespace which contains some "legacy" objects
   *  @todo the file LoKi/Legacy1.h and namespace LoKi::Legacy to be removed! 
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2008-06-03
   */
  namespace Legacy 
  {
    // ========================================================================
    /** @class DistanceSignificanceWithBestDV
     *  simple functor which makes the "incorrect" evaluation of the 
     *  significance distance between the particle's vertex anf the best 
     *  related primary vertex. The significance is evaluated (incorrectly) 
     *  as the ratio of distance over the uncertanty of the distance.  
     *  The related primary vertex is extarcted form the desktop. 
     *  @see LoKi::Cuts::BPVVDS_LEGACY
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-06-03
     */
    class DistanceSignificanceWithBestPV
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
      , public virtual LoKi::AuxDesktopBase 
    {
    public:
      /// MANDATORY: virtual destructor 
      virtual ~DistanceSignificanceWithBestPV () {} ;
      /// MANDATORY: the clone method ("virtual constructor")
      virtual  DistanceSignificanceWithBestPV* clone() const 
      { return new DistanceSignificanceWithBestPV ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout
      virtual  std::ostream& fillStream ( std::ostream& s ) const 
      { return s << "BPVVDS_LEGACY" ; }
    } ;
    // ========================================================================
  } // end of namespace LoKi::Legacy
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Cuts 
  {
    // ========================================================================
    /** @var BPVVDS_LEGACY 
     *  Sime functor to evaluate the incorrect distance significance
     *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
     *  @date 2008-06-03 
     */
    const LoKi::Legacy::DistanceSignificanceWithBestPV          BPVVDS_LEGACY ;
    // ========================================================================
  } // end of namespace LoKi::Cuts 
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
#endif // LOKI_LEGACY1_H
// ============================================================================
// The END 
// ============================================================================
