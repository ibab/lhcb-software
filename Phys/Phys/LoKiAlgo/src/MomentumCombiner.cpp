// $Id: MomentumCombiner.cpp,v 1.4 2007-04-16 16:16:40 pkoppenb Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2007/03/05 11:09:59  pkoppenb
// Get vectors from gaudiKernel
//
// Revision 1.2  2006/04/09 16:39:54  ibelyaev
//  v1r0
//
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/IAlgTool.h" 
#include "GaudiKernel/ToolFactory.h" 
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiTool.h"
// ============================================================================
// LHCbKernel
// ============================================================================
#include "GaudiKernel/GenericVectorTypes.h"
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/IParticleCombiner.h"
#include "Kernel/IParticleTransporter.h"
// ============================================================================


// ============================================================================
/** @file
 *
 *  The most trivial implementation of IParticleCombiner interface 
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
 *  @date 2006-03-26 
 */
// ============================================================================

// ============================================================================
/** @class MomentumCombiner
 *  The most trivial implementation of interface IParticleCombiner
 *  The tool just "combines" the 4-momenta of daughter particles
 *  (with their covaraince matrices) to 
 *  get a 4-momentum of mother particle and its covariance matrix
 *  
 *  Only "daughters/momentum/momcovMatrix/measuredMass/MeasuredMassErr" fields 
 *  are set for mother particle.
 *  If ParticleTranporter is specified, the momenta of daughter particles 
 *  are transported to mother.referencePoint().Z() *BEFORE* summing
 *
 *  The tool has only 1 specific property:
 *    - "Transporter" : the type/name of ParticleTransporter toolto be used 
 *                      to transport daughter particles to new Z-position 
 *  
 *  @see IParticleCombiner
 *  @see LHCb::Particle
 *  @see IParticleTransporter
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-03-26 
 */
// ============================================================================
class MomentumCombiner 
  : public virtual IParticleCombiner 
  , public                 GaudiTool
{
  /// friend factory for instantiation 
  friend class ToolFactory<MomentumCombiner> ;
public:  
  /// standard initialization of the tool
  virtual StatusCode initialize ()
  {
    StatusCode sc = GaudiTool::initialize() ;
    if ( sc.isFailure() ) { return sc ; }
    if ( !m_transporterName.empty() )
    { m_transporter = tool<IParticleTransporter> ( m_transporterName , this ) ; }
    if ( 0 == m_transporter ) 
    { Warning(" Only 4-momenta sum will be used (no ParticleTransporter specified)") ; }
    //
    return StatusCode::SUCCESS ;
  };
  /** the only one seesntial method for combining a particles.
   *
   *  Only "daughters/momentum/momcovMatrix/measuredMass/MeasuredMassErr" fields 
   *  are set for mother particle.
   *
   *  If ParticleTranporter is specified, the momenta of daughter particles 
   *  are transported to mother.referencePoint().Z() *BEFORE* summing
   *  @see IParticleCombiner 
   *  @see IParticleTransporer
   *  @see LHCb::Particle 
   *  @see LHCb::Particle::ConstVector 
   *  @see LHCb::Vertex 
   *  @param daughters list of daughter particles 
   *  @param mother    mother particle to be modifier 
   *  @param vertex    - not used - 
   *  @return status code 
   */
  virtual StatusCode combine
  ( const LHCb::Particle::ConstVector&  daughters ,  
    LHCb::Particle&                     mother    , 
    LHCb::Vertex&                       vertex    ) const ;
protected:
  /// standard constructor from type, name and the parent 
  MomentumCombiner
  ( const std::string& type   , 
    const std::string& name   , 
    const IInterface*  parent ) 
    : GaudiTool ( type , name , parent ) 
    , m_transporterName ()
    , m_transporter     ( 0 )    
  {
    declareInterface<IParticleCombiner> ( this ) ;
    declareProperty ( "Transporter" , m_transporterName ) ;
  };
  /// viertuakl and protected destructor
  virtual ~MomentumCombiner() {}
private:
  // default constructor is disabled 
  MomentumCombiner() ;
  // copy constructor is disabled 
  MomentumCombiner ( const MomentumCombiner& ) ;
  // assignement is disabled 
  MomentumCombiner& operator=( const MomentumCombiner& ) ;
private:
  // transporter to be used before summing all momenta
  std::string           m_transporterName ;
  // the transporter itself 
  IParticleTransporter* m_transporter     ; ///< the transporter itself 
} ;
// ============================================================================
// Declaration of the Tool Factory
// ============================================================================
DECLARE_TOOL_FACTORY( MomentumCombiner );
// ============================================================================
/** the only one seesntial method for combining a particles.
 *
 *  Only "daughters/momentum/momcovMatrix/measuredMass/MeasuredMassErr" fields 
 *  are set for mother particle.
 *
 *  If ParticleTranporter is specified, the momenta of daughter particles 
 *  are transported to mother.referencePoint().Z() *BEFORE* summing
 *  @see IParticleCombiner 
 *  @see IParticleTransporer
 *  @see LHCb::Particle 
 *  @see LHCb::Particle::ConstVector 
 *  @see LHCb::Vertex 
 *  @param daughters list of daughter particles 
 *  @param mother    mother particle to be modifier 
 *  @param vertex    - not used - 
 *  @return status code 
 */
// ============================================================================
StatusCode MomentumCombiner::combine
( const LHCb::Particle::ConstVector&     daughters ,  
  LHCb::Particle&                        mother    , 
  LHCb::Vertex&                       /* vertex */ ) const 
{
  typedef LHCb::Particle::ConstVector PARTICLES ;
  LHCb::Particle tmp ;
  // summed momentum and momentum covariance matrix:
  Gaudi::LorentzVector momentum   ;
  Gaudi::SymMatrix4x4  covariance ;
  mother.clearDaughters() ;
  for ( PARTICLES::const_iterator ip = daughters.begin() ; 
        daughters.end() != ip ; ++ip ) 
  {
    const LHCb::Particle* p = *ip ;
    if ( 0 == p ) { continue ; }
    //
    const LHCb::Particle* aux = p ;
    if ( 0 != m_transporter ) 
    {
      // assign the temporary particle 
      tmp = *p ;
      // extrapolate it to new position
      const double zNew = mother.referencePoint().z() ;
      StatusCode sc = m_transporter->transport ( p , zNew , tmp ) ;
      if ( sc.isFailure() ) { Warning ("unable to transport the particle!", sc ) ;  }
      else { aux = &tmp ; } // use the properly transported particle
    }
    //
    momentum   += aux->momentum     () ;
    covariance += aux->momCovMatrix () ;
    //
    mother.addToDaughters ( p ) ;
  }  
  // fill the particle 1) 4-momentum 
  mother.setMomentum       ( momentum     ) ;
  mother.setMomCovMatrix   ( covariance   ) ;
  // fill the particle 1) measured mass 
  const double mass = momentum.M() ;
  mother.setMeasuredMass   ( mass ) ;
  // transsform vector 
  Gaudi::Vector4 vct 
    ( - momentum.Px () , - momentum.Py () , - momentum.Pz () , momentum.E  () ) ;                     
  vct /= mass ;
  //
  const double massErr2 = ROOT::Math::Similarity ( covariance , vct ) ;
  //const double massErr2 = ROOT::Math::Product( covariance , vct ) ;
  mother.setMeasuredMassErr ( sqrt( massErr2 ) ) ;
  //
  return StatusCode::SUCCESS ;
} ;
// ============================================================================
// The END 
// ============================================================================


