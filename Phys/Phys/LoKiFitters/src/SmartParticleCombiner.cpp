// $Id$
// ============================================================================
// Include files 
// ============================================================================
// DaVinciInterfaces 
// ============================================================================
#include "Kernel/IParticleCombiner.h"
// ============================================================================
// local
// ============================================================================
#include "ParticleClassificator.h"
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  /** @class SmartParticleCombiner 
   *
   *  Implementation file for class
   *
   *  @see  IParticleCombiner
   *  @date 2010-11-13 
   *  @author Vanya Belyaev  Ivan.Belyaev@cern.ch
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
   *                    $Revision$
   *  Last modification $Date$
   *                 by $Author$
   */
  // ==========================================================================
  class SmartParticleCombiner 
    :  public extends1<ParticleClassificator,IParticleCombiner> 
  {
    // ========================================================================
    /// the friend factory for instantiation 
    friend class ToolFactory<LoKi::SmartParticleCombiner> ;
    // ========================================================================
  public:
    // ========================================================================
    /** combine set of particles into vertex 
     *  @see IParticleCombiner
     *  @see IParticleCombiner::combiner 
     *  @param daughters the vector of daughter particles    (input)
     *  @param mother    the "mother" particle               (output)
     *  @param vertex    the decay vertex of mother particle (output)
     *  @return status code 
     */
    virtual StatusCode combine
    ( const LHCb::Particle::ConstVector&  daughters ,  
      LHCb::Particle&                     mother    , 
      LHCb::Vertex&                       vertex    ) const ;
    // ========================================================================
  protected:
    // ========================================================================
    /// standard constructor 
    SmartParticleCombiner 
    ( const std::string& type   , 
      const std::string& name   , 
      const IInterface*  parent ) 
      : base_class ( type , name, parent ) 
    //
      , m_adderName  ( "MomentumCombiner/Adder"          ) 
      , m_fitterName ( "Loki::VertexFitter/VertexFitter" ) 
    //
      , m_adder  ( 0 ) 
      , m_fitter ( 0 )
    {
      // ======================================================================
      declareProperty 
        ( "Adder"     ,
          m_adderName , 
          "The type/name of MomentumCombiner/ParticleAdder" ) ;
      // ======================================================================
      declareProperty 
        ( "VertexFitter"     ,
          m_fitterName , 
          "The type/name of Vertex fitter " ) ;
      // ======================================================================
    }
    /// virtual & protected destrcutor
    virtual ~SmartParticleCombiner () {} ;  //   virtual & protected destructor
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled 
    SmartParticleCombiner () ;           // the default constructor is disabled 
    /// copy constructoir is disabled 
    SmartParticleCombiner ( const SmartParticleCombiner& ) ;  
    /// assignement operator is disabled 
    SmartParticleCombiner& operator=( const SmartParticleCombiner& ); 
    // ========================================================================
  private:
    // ========================================================================
    // get the adder 
    inline const IParticleCombiner* adder() const 
    {
      if ( 0 != m_adder ){ return m_adder ; }
      m_adder = tool<IParticleCombiner> ( m_adderName , this ) ;
      return m_adder ;
    }
    // get the fitter 
    inline const IParticleCombiner* fitter () const 
    {
      if ( 0 != m_fitter ){ return m_fitter ; }
      m_fitter = tool<IParticleCombiner> ( m_fitterName , this ) ;
      return m_fitter  ;
    }
    // ========================================================================
  private:
    // ========================================================================
    std::string                      m_adderName  ;  // the name of adder
    std::string                      m_fitterName ;  // the name of fitter 
    mutable const IParticleCombiner* m_adder      ;  // the adder 
    mutable const IParticleCombiner* m_fitter     ;  // the fitter    
    // ========================================================================
  } ;  
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
/*  combine set of particles into vertex 
 *  @see IParticleCombiner
 *  @see IParticleCombiner::combiner 
 *  @param daughters the vector of daughter particles    (input)
 *  @param mother    the "mother" particle               (output)
 *  @param vertex    the decay vertex of mother particle (output)
 *  @return status code 
 */
StatusCode LoKi::SmartParticleCombiner::combine
( const LHCb::Particle::ConstVector&  daughters ,  
  LHCb::Particle&                     mother    , 
  LHCb::Vertex&                       vertex    ) const 
{
  const IParticleCombiner* actor = 
    goodForVertex ( daughters ) ? fitter() : adder() ;
  //
  return actor -> combine ( daughters , mother , vertex ) ;
}
// ============================================================================
/// the factory needed for instantiation
DECLARE_NAMESPACE_TOOL_FACTORY ( LoKi , SmartParticleCombiner )
// ============================================================================

// ============================================================================
// The END 
// ============================================================================

