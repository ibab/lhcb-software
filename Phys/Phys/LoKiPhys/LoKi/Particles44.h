// $Id$
// ============================================================================
#ifndef LOKI_PARTICLES44_H 
#define LOKI_PARTICLES44_H 1
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
// DaVinciInterfaces 
// ============================================================================
#include "Kernel/IParticleValue.h"
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
 *  @author Sebastian Neubert
 *  @date 2013-08-05 
 */
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Particles 
  {
    // ========================================================================
    /** @class Filter
     *  Simple adapter which allos to use the tool IParticleValue
     *  as LoKi functor
     *  @see IParticleValue
     *  @see LoKi::Cuts::VALUE
     *  @author Sebastian Neubert
     *  @date 2013-08-05
     */
    class GAUDI_API Value
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      // ======================================================================
      /// constructor from the tool name 
      Value ( const std::string&      tool ) ;
      /// constructor from the tool 
      Value ( const IParticleValue*  tool ) ;
      /// copy constructor
      Value ( const Value& right ) ;
      /// MANDATORY: virtual destructor
      virtual ~Value() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual Value* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is disabled
      Value () ;                            // default constructor is disabled
      // ======================================================================
    private:
      // ======================================================================
      /// the tool itself 
      LoKi::Interface<IParticleValue> m_function ;            // the tool itself 
      // ======================================================================
    } ;
    // ========================================================================    
  } //                                         end of namespace LoKi::Particles
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_PARTICLES44_H
// ============================================================================
