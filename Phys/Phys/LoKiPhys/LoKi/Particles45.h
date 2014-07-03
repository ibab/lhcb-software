#ifndef LOKI_PARTICLES45_H 
#define LOKI_PARTICLES45_H 1
// ============================================================================
// Include files
// ============================================================================
// Event
// ============================================================================
#include "Event/RelatedInfoMap.h"
#include "Relations/IRelation.h"
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
    class GAUDI_API RelatedInfo
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      // ======================================================================
      /// constructor from 
      RelatedInfo ( const std::string& location        , 
                    const short        index           , 
                    const double       bad     = -1000 ) ;

      // ======================================================================
      /// constructor from 
      RelatedInfo ( const std::string& location        , 
                    const std::string& variable        , 
                    const double       bad     = -1000 ) ;
      /// copy constructor  
      RelatedInfo ( const RelatedInfo& ) ;
      /// MANDATORY: virtual destructor
      virtual ~RelatedInfo () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  RelatedInfo* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      RelatedInfo () ;
      // ======================================================================
    private:
      // ======================================================================
      std::string m_location ;
      short       m_index    ;
      double      m_bad      ;
      // ======================================================================
      typedef IRelation<LHCb::Particle,LHCb::RelatedInfoMap>  IMAP ;
      mutable const IMAP*  m_table   ;
      // ======================================================================
    };
    // ========================================================================    
  } //                                         end of namespace LoKi::Particles
  // ==========================================================================
  namespace Cuts 
  {
    // ========================================================================
    typedef LoKi::Particles::RelatedInfo                              RELINFO ;
    // ========================================================================
  }
} //                                                      end of namespace LoKi
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_PARTICLES45_H
