// $Id: Particles15.h,v 1.5 2007-07-23 17:35:45 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_PARTICLES15_H 
#define LOKI_PARTICLES15_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKiCore
// ============================================================================
#include "LoKi/Interface.h"
// ============================================================================
// LoKiPhys 
// ============================================================================
#include "LoKi/PhysTypes.h"
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/IFilterCriterion.h"
// ============================================================================
/** @file
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-03-20 
 */
// ============================================================================
namespace LoKi
{
  namespace Particles 
  {
    // ========================================================================
    /** @class Filter
     *  Simple adapter which allos to use the tool IFilterCriterion
     *  as LoKi functor
     *  @see IFilterCriterion
     *  @see LoKi::Cuts::FILTER
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-03-20
     */
    class Filter : public LoKi::Predicate<const LHCb::Particle*>
    {
    public:
      /// constructor from the filter 
      Filter ( const IFilterCriterion* filter );
      /// copy constructor 
      Filter ( const Filter& right ) ;
      /// MANDATORY: virtual destructor
      virtual ~Filter() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual Filter* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    private:
      // default constructor is disabled
      Filter () ;
    private:
      // the tool itself 
      LoKi::Interface<IFilterCriterion> m_filter ; ///< the tool itself 
    } ;
    // ========================================================================    
  } // end of namespace LoKi::Particles
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PARTICLES15_H
// ============================================================================
