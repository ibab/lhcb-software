// $Id$
// ============================================================================
#ifndef PARTICLECLASSIFICATOR_H 
#define PARTICLECLASSIFICATOR_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <algorithm>
#include <set>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/AlgTool.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
#include "Event/Vertex.h"
// ============================================================================
// PartProp
// ============================================================================
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/NodesPIDs.h"
// ============================================================================
// KalmanFilter 
// ============================================================================
#include "Kernel/IParticleClassifier.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/IDecay.h"
#include "LoKi/Trees.h"
// ============================================================================
// KalmanFilter 
// ============================================================================
#include "KalmanFilter/ParticleTypes.h"
// ============================================================================
// Local
// ============================================================================
#include "MessagingBase.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** @class ParticleClassificator ParticleClassificator.h
   *
   *  The useful base class for classification of particles 
   *  @see LoKi::KalmanFilter::ParticleType
   *
   *  This file is a part of 
   *  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
   *  ``C++ ToolKit for Smart and Friendly Physics Analysis''
   *
   *  The package has been designed with the kind help from
   *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
   *  contributions and advices from G.Raven, J.van Tilburg, 
   *  A.Golutvin, P.Koppenburg have been used in the design.
   *
   *  By usage of this code one clearly states the disagreement 
   *    with the campain of Dr.O.Callot et al.: 
   *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
   *
   *  @author Vanya Belyaev Ivan.Belyaev@cern.ch
   *  @date   2010-11-11
   * 
   *                    $Revision$
   *  Last modification $Date$
   *                 by $Author$
   */
  class ParticleClassificator :public extends1<MessagingBase,IParticleClassifier> 
  {
    // ========================================================================
    /// friend factory for instantiation
    friend class ToolFactory<LoKi::ParticleClassificator> ;
    // ========================================================================
  public:
    // ========================================================================
    /// initialize the tool 
    virtual StatusCode initialize () ;                   // initialize the tool 
    /// finalize   the tool 
    virtual StatusCode finalize   () ;                   // finalize   the tool 
    // ========================================================================
  public: // see IParticleClassifier 
    // ========================================================================
    /** get the particle type 
     *  @see LoKi::KalmanFilter::ParticleType
     */
    LoKi::KalmanFilter::ParticleType 
    particleType ( const LHCb::Particle* p ) const 
    { return particleType_ ( p ) ; }
    // ========================================================================
    /** good for vertex ? 
     *  @attention This definiton is <b>different</b> from the 
     *  definition by LoKi::KalmanFilter::okForVertex
     *  @see LoKi::KalmanFilter::okForVertex
     */
    bool goodForVertex ( const LHCb::Particle::Range&    parts ) const 
    { return goodForVertex_ ( parts ) ; }
    // ========================================================================
  protected:
    // ========================================================================
    /** get the particle type 
     *  @see LoKi::KalmanFilter::ParticleType
     */
    LoKi::KalmanFilter::ParticleType 
    particleType_ ( const LHCb::Particle& p ) const ;
    // ========================================================================
    /** get the particle type 
     *  @see LoKi::KalmanFilter::ParticleType
     */
    LoKi::KalmanFilter::ParticleType 
    particleType_ ( const LHCb::Particle* p ) const 
    {
      if ( 0 == p ) { return LoKi::KalmanFilter::UnspecifiedParticle ; }  // RETURN
      return particleType_ ( *p ) ;
    }    
    // ========================================================================
    /** good for vertex ? 
     *  @attention This definiton is <b>different</b> from the 
     *  definition by LoKi::KalmanFilter::okForVertex
     *  @see LoKi::KalmanFilter::okForVertex
     */
    bool goodForVertex_ ( const LHCb::Particle::Range&    parts ) const 
    {
      /// two or more long-lived particles are required for vertex
      return 2 <= nForVertex ( parts.begin() , parts.end() ) ;
    }
    // ========================================================================
  protected:
    // ========================================================================
    /// get the correct algorithm context 
    bool getMyAlg () const ;
    // ========================================================================
  protected:
    // ========================================================================
    /** standard constructor 
     *  @param type   the actual type of the tool 
     *  @param name   the instance name 
     *  @param parent the parent 
     */
    ParticleClassificator 
    ( const std::string& type   ,           //         the actual tool type (?)
      const std::string& name   ,           //            the instance name 
      const IInterface*  parent ) ;         //                   the parent 
    /// virtual & protected destructor
    virtual ~ParticleClassificator () ;     //   virtual & protected destructor
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled 
    ParticleClassificator () ;           // the default constructor is disabled 
    /// copiy constructoir is disabled 
    ParticleClassificator ( const ParticleClassificator& ) ; // copy is disabled 
    /// assignement operator is disabled 
    ParticleClassificator& operator=( const ParticleClassificator& ); // disabled 
    // ========================================================================
  private:
    // ========================================================================
    /// good gor vertex ? 
    template <class PARTICLE>
    std::size_t nForVertex ( PARTICLE first , PARTICLE last  ) const ;
    // ========================================================================
  private:
    // ========================================================================
    /// particle property service 
    mutable const LHCb::IParticlePropertySvc* m_ppSvc        ;
    /// Long-lived particles 
    mutable Decays::Nodes::LongLived_         m_longLived    ;
    /// Short-lived particles 
    mutable Decays::Nodes::ShortLived_        m_shortLived   ;
    /// Gamma-like particles 
    mutable Decays::Trees::Stable_<const LHCb::Particle*> m_gammaLike     ;
    /// GammaC-like particles (gamma-> e+ e-)
    mutable Decays::IDecay::Tree              m_gammaCLike   ;
    /// Di-Gamma-like particles ( pi0 -> gamma gamma , eta -> gamma gamma ) 
    mutable Decays::IDecay::Tree              m_digammaLike  ;
    /// Merged-pi0-like particles 
    mutable Decays::Trees::Stable_<const LHCb::Particle*> m_mergedPi0Like ;
    /// get like particle 
    Decays::Node                              m_jetsLike      ;
    // ========================================================================
    /// decay descriptor for gammaC-like particles:
    std::string m_dd_gammaC  ;  //   decay descriptor for gammaC-like particles
    /// decay descriptor for di-gamma-like particles:
    std::string m_dd_digamma ;  // decay descriptor for di-gamma-like particles
    /// descriptor for jets-like particle
    std::string m_dd_jets    ;  // descriptor for jets-like particle
    // ========================================================================
  private:
    // ========================================================================
    /// Unclassified particles 
    mutable std::set<LHCb::ParticleID>        m_unclassified   ;
    mutable std::set<LHCb::ParticleID>        m_gamma_like     ;
    mutable std::set<LHCb::ParticleID>        m_gammaC_like    ;
    mutable std::set<LHCb::ParticleID>        m_digamma_like   ;
    mutable std::set<LHCb::ParticleID>        m_mergedPi0_like ;
    mutable std::set<LHCb::ParticleID>        m_jets_like      ;
    // ========================================================================
  };
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
// good for vertex ?
// ============================================================================
template <class PARTICLE>
inline std::size_t 
LoKi::ParticleClassificator::nForVertex
( PARTICLE first ,
  PARTICLE last  ) const
{
  //
  std::size_t nTr = 0 ;
  //
  for ( ; first != last ; ++first )
  {
    const LHCb::Particle* p = *first ;
    if ( NULL == p ) { continue ; }
    //
    LoKi::KalmanFilter::ParticleType pType = particleType_ ( p ) ;
    //
    // 1 for long-lived particles
    if      ( LoKi::KalmanFilter::LongLivedParticle  == pType )
    { ++nTr ; }
    // number for long-lived particles for short lived resonance
    else if ( LoKi::KalmanFilter::ShortLivedParticle == pType )
    {
      const SmartRefVector<LHCb::Particle>& daughters = p->daughters() ;
      nTr += nForVertex ( daughters.begin() , daughters.end() ) ;
    }
    //
  }
  return nTr ;
}
// ============================================================================
// The END 
// ============================================================================
#endif // PARTICLECLASSIFICATOR_H
// ============================================================================
