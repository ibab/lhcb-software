// $Id$ 
// =======================================================================================
#ifndef LOKI_LEGACY_H 
#define LOKI_LEGACY_H 1
// =======================================================================================
// Include files
// =======================================================================================
// Event
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
// DaVinciInterfaces
// ============================================================================
#include "Kernel/ILifetimeFitter.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/BasicFunctors.h"
#include "LoKi/Interface.h"
#include "LoKi/PhysTypes.h"
#include "LoKi/AuxDesktopBase.h"
#include "LoKi/Particles29.h"
#include "LoKi/Particles34.h"
// ============================================================================
/** @file LoKi/Legacy.h
 *  Collection of "Legacy" functors,  candidated for removal 
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design. 
 *   
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 *                    
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2009-04-30
 */
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Legacy
  {
    // ========================================================================
    /** @class DHCoplanarity
     *
     *  The functor for Jaap Panman:
     *   - """ I am looking (again) at B-> h D0 decays, where the D0-> pi pi Ks.  
     *         What we are trying to do is filter as much as possible the 
     *         trigger before we are going to invoke the seeding to find the Ks 
     *         in downstream tracks.
     *         The criterion which we cannot apply yet is the co-planarity 
     *         of the straight line connecting the primary vertex and 
     *         the D0 decay vertex, and the straight line defined by the "h" 
     *         (can be K, K*, mu, etc).  The co-planarity can be expressed 
     *         as a distance of closest approach of these two straight lines.
     *         As implementation I would suggest a "CombinationCut" with 
     *         the name ABPVDVDOCA (array-best primary vertex-decay vertex).  
     *         So if the decay descriptor would be [ "B+ -> D0 K+", 
     *         "B- -> D0 K-" ] this would make the straight line between BPV 
     *         and the D0, and the K+- vectors to make a DOCA.   
     *        (or if there is a Kstar, it would use the combined K+pi vector.) """
     *
     *  @attention the functor is sensitive to the decay structure 
     *             the first particle <b>MUST</b> have the valid end-vertex
     *
     *  @warning TO BE REMOVED RELATIVELY SOON 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-04-30
     */
    class GAUDI_API DHCoplanarity 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function 
      , public virtual LoKi::AuxDesktopBase 
    {
    public:
      // ======================================================================
      /// Default Constructor
      DHCoplanarity() ;
      /// MANDATORY : virtual destrcutor 
      virtual ~DHCoplanarity() ;
      /// MANDATORY: clone method ("virtual destructor") 
      virtual  DHCoplanarity* clone() const ;
      /** MANDATORY: the only one essential method 
       *
       *  """
       *  The criterion which we cannot apply yet is the co-planarity 
       *  of the straight line connecting the primary vertex and 
       *  the D0 decay vertex, and the straight line defined by the "h" 
       *  (can be K, K*, mu, etc). 
       *  The co-planarity can be expressed as a distance of closest 
       *  approach of these two straight lines.
       *  As implementation I would suggest a "CombinationCut" with 
       *  the name ABPVDVDOCA (array-best primary vertex-decay vertex).  
       *  So if the decay descriptor would be [ "B+ -> D0 K+", 
       *  "B- -> D0 K-" ] this would make the straight line between BPV 
       *  and the D0, and the K+- vectors to make a DOCA.   
       *  (or if there is a Kstar, it would use the combined K+pi vector.) 
       *  """
       *  @attention the functor is sensitive to the decay structure 
       *             the first particle <b>MUST</b> have the valid end-vertex
       */
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printot
      virtual std::ostream& fillStream  ( std::ostream& s ) const ;
      // ======================================================================
    };
    // ========================================================================
  } //                                         end of namespace LoKi::Particles 
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Legacy
  {
    // ========================================================================
    /** @class DaughterParticleCTau 
     *  Simple evaluator of c*tau for daughter particle 
     *  @see LoKi::Cuts::DPCTAU  
     *  @see ILifetimeFitter 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-05-15
     */
    class GAUDI_API DaughterParticleCTau
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      // ======================================================================
      // constructor from the index & tool  
      DaughterParticleCTau
      ( const unsigned short   index      , 
        const ILifetimeFitter* fitter = 0 ) ;
      // constructor from the index & tool  
      DaughterParticleCTau 
      ( const unsigned short                      index  , 
        const LoKi::Interface<ILifetimeFitter>&  fitter ) ;
      // constructor from the index & tool  
      DaughterParticleCTau
      ( const unsigned short index  , 
        const std::string&    fitter ) ;
      /// MANDATORY: virtual destructor 
      virtual ~DaughterParticleCTau () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  DaughterParticleCTau * clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// get c*tau 
      double ctau             ( const LHCb::Particle* p ) const ;
      /// get ctau error 
      double ctauErr          ( const LHCb::Particle* p ) const ;
      /// get ctau fit chi2 
      double ctauFitChi2      ( const LHCb::Particle* p ) const ;
      /// get ctau significance 
      double ctauSignificance ( const LHCb::Particle* p ) const ;
      /// get the index 
      unsigned short index () const { return m_index ; }
      // ======================================================================
    protected:
      // ======================================================================
      /// get results from the fit 
      StatusCode fit ( const LHCb::Particle* p           , 
                       double&               ctau        , 
                       double&               ctauerr     , 
                       double&               ctaufitchi2 ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is disabled 
      DaughterParticleCTau () ;
      // ======================================================================
    private:
      // ======================================================================
      /// the index for daughter particle 
      unsigned int m_index ;                 // the index for daughter particle 
      /// the actual evaluator of lifetime 
      mutable LoKi::Interface<ILifetimeFitter> m_fitter ;      // the evaluator 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class DaughterParticleCTauErr
     *  Simple evaluator of error in c*tau for daughter particle 
     *  @see LoKi::Cuts::DPCTAUERR  
     *  @see ILifetimeFitter 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-05-15
     */
    class GAUDI_API DaughterParticleCTauErr : public DaughterParticleCTau
    {
    public:
      // ======================================================================
      // constructor from the index & tool  
      DaughterParticleCTauErr
      ( const unsigned short   index      , 
        const ILifetimeFitter* fitter = 0 ) ;
      // constructor from the index & tool  
      DaughterParticleCTauErr 
      ( const unsigned short                      index  , 
        const LoKi::Interface<ILifetimeFitter>&  fitter ) ;
      // constructor from the index & tool  
      DaughterParticleCTauErr
      ( const unsigned short index  , 
        const std::string&    fitter ) ;
      /// MANDATORY: virtual destructor 
      virtual ~DaughterParticleCTauErr () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  DaughterParticleCTauErr* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is disabled 
      DaughterParticleCTauErr () ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class DaughterParticleCTauFitChi2
     *  Simple evaluator of error in c*tau for daughter particle 
     *  @see LoKi::Cuts::DPCTAUFITCHI2 
     *  @see ILifetimeFitter 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-05-15
     */
    class GAUDI_API DaughterParticleCTauFitChi2 : public DaughterParticleCTau
    {
    public:
      // ======================================================================
      // constructor from the index & tool  
      DaughterParticleCTauFitChi2
      ( const unsigned short   index      , 
        const ILifetimeFitter* fitter = 0 ) ;
      // constructor from the index & tool  
      DaughterParticleCTauFitChi2 
      ( const unsigned short                      index  , 
        const LoKi::Interface<ILifetimeFitter>&  fitter ) ;
      // constructor from the index & tool  
      DaughterParticleCTauFitChi2
      ( const unsigned short index  , 
        const std::string&    fitter ) ;
      /// MANDATORY: virtual destructor 
      virtual ~DaughterParticleCTauFitChi2 () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  DaughterParticleCTauFitChi2* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is disabled 
      DaughterParticleCTauFitChi2 () ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class DaughterParticleCTauSignificance
     *  Simple evaluator of c*tau-significance for daughter particle 
     *  @see LoKi::Cuts::DPCTAUSIGNIFICANCE
     *  @see ILifetimeFitter 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-05-15
     */
    class GAUDI_API DaughterParticleCTauSignificance : public DaughterParticleCTau
    {
    public:
      // ======================================================================
      // constructor from the index & tool  
      DaughterParticleCTauSignificance
      ( const unsigned short   index      , 
        const ILifetimeFitter* fitter = 0 ) ;
      // constructor from the index & tool  
      DaughterParticleCTauSignificance 
      ( const unsigned short                      index  , 
        const LoKi::Interface<ILifetimeFitter>&  fitter ) ;
      // constructor from the index & tool  
      DaughterParticleCTauSignificance
      ( const unsigned short index  , 
        const std::string&    fitter ) ;
      /// MANDATORY: virtual destructor 
      virtual ~DaughterParticleCTauSignificance () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  DaughterParticleCTauSignificance* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is disabled 
      DaughterParticleCTauSignificance () ;
      // ======================================================================
    } ;
    // ========================================================================
  } //                                            end of namespace LoKi::Legacy
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
// ============================================================================
//                                                                      The END 
// ============================================================================
//                                                                      The END  
// ============================================================================
#endif // LOKI_LEGACY_H
// ============================================================================
