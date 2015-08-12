// $Id$
// ============================================================================
#ifndef FITDECAYTREES_H 
#define FITDECAYTREES_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToStream.h"
// ============================================================================
// TrackInterfaces 
// ============================================================================
#include "TrackInterfaces/ITrackStateProvider.h"
// ============================================================================
// local
// ============================================================================
#include "FilterDesktop.h"
// ============================================================================
/** @class FitDecayTrees 
 *  Simple algorithm to perform the decay tree fit for 'good' particles
 *
 *  The algorothm selectd certains decay trees form the input and 
 *  perform the refit of <b>cloned</b> decay trees.
 * 
 *  @code 
 * 
 *  from Configurables import FitDecayTrees 
 *
 *  myAlg = FitDecayTrees ( 
 *     ...                                                , 
 *     Code          = " DECTREE('[B_s0 -> J/psi(1S) phi(1020)]CC') " , 
 *     MaxChi2PerDoF = 10                                 ,
 *     ...                                                ,
 *     ) 
 *    
 *  @endcode  
 *
 *  Optionally one can apply the Primary Vertex pointing constraint:
 * 
 *  @code 
 * 
 *  myAlg = FitDecayTrees ( 
 *     ...                                                  , 
 *     Code            = " 'B_s0' == ABSID " , 
 *     MaxChi2PerDoF   = 10                                 ,
 *     UsePVConstraint = True                               , 
 *     ...                                                  ,
 *     ) 
 *    
 *  @endcode  
 *
 *  Also optionally one can apply certain mass-constraints:
 * 
 *  @code 
 * 
 *  myAlg = FitDecayTrees ( 
 *     ...                                                  , 
 *     Code            = " ( 'B_s0' == ABSID ) & INTTREE ( 'J/psi(1S)' == ID ) " , 
 *     MaxChi2PerDoF   = 10                                 ,
 *     MassConstraints = [ 'J/psi(1S)' ]                    , 
 *     ...                                                  ,
 *     ) 
 *    
 *  @endcode  
 *
 *  Clearly all constraints can be combined together:
 * 
 *  @code 
 * 
 *  myAlg = FitDecayTrees ( 
 *     ...                                                  , 
 *     Code            = " DECTREE('[B_s0 -> J/psi(1S) phi(1020)]CC') " , 
 *     MaxChi2PerDoF   = 10                                 ,
 *     UsePVConstraint = True                               , 
 *     MassConstraints = [ 'J/psi(1S)' ]                    , 
 *     ...                                                  ,
 *     ) 
 *    
 *  @endcode  
 *
 *  See also 
 *  <a href="https://twiki.cern.ch/twiki/bin/view/LHCb/FAQ/DaVinciFAQ#How_to_perform_the_refit_of_the"/>DaVinci TWiki FAQ </a>
 * 
 *  @author Vanya BELYAEV   Ivan.Belyaev@nikhef.nl
 *  @date 2010-07-09
 *
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
class FitDecayTrees : public FilterDesktop
{
  // ==========================================================================
  /// friend factory for instantiation 
  friend class AlgFactory<FitDecayTrees> ;
  // ==========================================================================
public:
  // ==========================================================================
  /// intialize the algorithm 
  virtual StatusCode initialize () ;
  /// finalize the algorithm 
  virtual StatusCode finalize   () ;
  // ==========================================================================
protected:
  // ==========================================================================
  /** standard constructor 
   *  @see DaVinciAlgorithm
   *  @see GaudiTupleAlg
   *  @see GaudiHistoAlg
   *  @see GaudiAlgorithm
   *  @see Algorithm
   *  @see AlgFactory
   *  @see IAlgFactory
   *  @param name the algorithm instance name 
   *  @param pSvc pointer to Service Locator 
   */
  FitDecayTrees                                // standard contructor 
  ( const std::string& name ,                  // the algorithm instance name 
    ISvcLocator*       pSvc ) ;                // pointer to Service Locator
  /// virtual & protected destructor 
  virtual ~FitDecayTrees() ;                  // virtual & protected destructor  
  // ==========================================================================
public:
  // ==========================================================================
  /** the major method for filter input particles 
   *  @param input    (INPUT) the input  container of particles
   *  @param filtered (OUPUT) the output container of particles
   *  @return Status code 
   */
  virtual StatusCode filter 
  ( const LHCb::Particle::ConstVector& input    , 
    LHCb::Particle::ConstVector&       filtered ) ;
  // ==========================================================================
public:
  // ==========================================================================
  /// attention:restore the original action by DaVinciAlgorithm
  virtual StatusCode _saveInTES () ; 
  /// attention:restore the original action by DaVinciAlgorithm
  virtual void       writeEmptyTESContainers() ;
  // ==========================================================================
  void updateExtrapolator ( Property& /* p */ );      // update the extrapolator
  // ==========================================================================
  void updateConstraints ( Property& /* p */ ) ;      // update the constraints 
  // ==========================================================================
  /// update handler for 'CloneFilteredParticles' property 
  void updateCloneProp ( Property& /* p */ ) ;
  /// decode mass-consraints 
  StatusCode decodeConstraints () ;
  // ==========================================================================
private:
  // ==========================================================================
  /// the default constructor is disabled 
  FitDecayTrees () ;                      // the default consructor is disabled
  /// copy constructor is disabled 
  FitDecayTrees ( const FitDecayTrees& ) ;      // copy constructor is disabled 
  /// assignement operator is disabled 
  FitDecayTrees& operator=( const FitDecayTrees& ) ;          // no assignement
  // ==========================================================================

protected:
  // ========================================================================
  /// get track extrapolator
  inline const ITrackStateProvider* extrapolator() const
  {
    if ( 0 != m_extrapolator        ) { return m_extrapolator ; } // RETURN
    if ( m_extrapolatorName.empty() ) { return              0 ; } // RETURN
    m_extrapolator = tool<ITrackStateProvider> ( m_extrapolatorName ) ;
      return m_extrapolator ;
  }
  // ========================================================================   
protected:
  // ==========================================================================
  /// the chi2-cut for the decay tree fit 
  double chi2cut           () const { return m_chi2cut           ; }
  /// use PV-constraint for re-Fit ? 
  bool   use_PV_Constraint () const { return m_use_PV_Constraint ; }
  // get the refitted tree 
  LHCb::DecayTree reFitted ( const LHCb::Particle* p ) const ;
  // ==========================================================================
private:
  /// track extrapolator
  mutable const ITrackStateProvider* m_extrapolator ;   // the track extrapolator
  std::string               m_extrapolatorName ; //  the name of extrapolator
    // ========================================================================     
  // ==========================================================================
  /// the chi2-cut for the decay tree fit 
  double         m_chi2cut ;             // the chi2-cut for the decay tree fit 
  /// the list of mass-constraints to be applied 
  std::vector<std::string>      m_mc_1 ; //        the list of mass-constraints
  /// the list of mass-constraints to be applied 
  std::vector<LHCb::ParticleID> m_mc_2 ; //        the list of mass-constraints
  /// use PV-constraint for re-Fit ? 
  bool   m_use_PV_Constraint ;           //      use PV-constraint for re-Fit ? 
  // ==========================================================================
};
// ============================================================================
// The END 
// ============================================================================
#endif // FITDECAYTREES_H
