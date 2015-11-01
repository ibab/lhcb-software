// $Id$
// ============================================================================
// Include files
// ============================================================================
#include <algorithm>
#include <functional>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IIncidentSvc.h"
// ============================================================================
// Boost
// ============================================================================
#include "boost/numeric/conversion/bounds.hpp"
#include "boost/limits.hpp"
// ============================================================================
// Functionality now in a tool
// ============================================================================
#include "Kernel/ISubstitutePID.h"
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/TreeCloners.h"
// ============================================================================
// local
// ============================================================================
#include "FitDecayTrees.h"
// ============================================================================
/** @class SubstitutePID
 *  Simple algorithm to substitute PID for certain decay components
 *
 *  @code
 *
 *  from Configurables import SubstitutePID
 *
 *  alg = SubstitutePID (
 *        'MakeUpsilon'  ,
 *        Code = " DECTREE('Jpsi(1S) -> mu+ mu-') & ( M > 9 * GeV ) " ,
 *        Substitute = {
 *            'J/psi(1S) -> mu+ mu-' : "Upsilon(1S)"
 *         }
 *        )
 *
 *  @endcode
 *
 *  - The algorithm does not use properly automatic CC!
 *  - Since the decay structure can change DURING the substitution
 *    proecdure, one needs to be rather careful
 *
 *  @code
 *
 *  alg = SubstitutePID (
 *        'MakeDsK'  ,
 *        Code = " DECTREE('[Beauty -> D+ X-]CC') & ( PT > 3 * GeV ) " ,
 *        Substitute = {
 *            ' Beauty -> ^( Charm & X+ ) X- '    : 'D_s+'  ,
 *            ' Beauty -> ^( Charm & X- ) X+ '    : 'D_s-'  ,
 *            ' Beauty ->  Charm ^pi-'            : 'K-'    ,
 *            ' Beauty ->  Charm ^pi+'            : 'K+'    ,
 *            ' B0     ->  Charm  X-'             : 'B_s0'  ,
 *            ' Beauty ->  Charm  X+'             : 'B_s~0'
 *         }
 *        )
 *
 *  @endcode
 *
 *
 *  @author Vanya BELYAEV   Ivan.Belyaev@cern.ch
 *  @date 2011-05-22
 *
 *                    $Revision$
 *  Last modification $Date$
 *                by  $Author$
 */
class SubstitutePID : public FitDecayTrees
{
  // ==========================================================================
  /// friend factory for instantiation
  friend class AlgFactory<SubstitutePID> ;
  // ==========================================================================
protected:
  // ==========================================================================
  /** standard constructor
   *  @see DVAlgorithm
   *  @see GaudiTupleAlg
   *  @see GaudiHistoAlg
   *  @see GaudiAlgorithm
   *  @see Algorithm
   *  @see AlgFactory
   *  @see IAlgFactory
   *  @param name the algorithm instance name
   *  @param pSvc pointer to Service Locator
   */
  SubstitutePID                                  //        standard constructor
  ( const std::string& name ,                    // the algorithm instance name
    ISvcLocator*       pSvc ) ;                  //  pointer to Service Locator
  // ==========================================================================
  /// virtual & protected destructor
  virtual ~SubstitutePID () ;                  // virtual & protected destructor
  // ==========================================================================
  IIncidentSvc* incSvc() const
  {
    if ( !m_incSvc ) { m_incSvc = svc<IIncidentSvc>("IncidentSvc",true); }
    return m_incSvc ;
  }
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
  /// decode the code
  virtual StatusCode decodeCode () ;
  // ==========================================================================
private:
  // ==========================================================================
  /// the default constructor is disabled
  SubstitutePID () ;                      // the default consructor is disabled
  /// copy constructor is disabled
  SubstitutePID ( const SubstitutePID& ) ;      // copy constructor is disabled
  /// assignement operator is disabled
  SubstitutePID& operator=( const SubstitutePID& ) ;          // no assignement
  // ==========================================================================
private:
  // ==========================================================================
  /// mapping : { 'decay-component' : "new-pid" } (property)
  ISubstitutePID::SubstitutionMap  m_map  ; // mapping : { 'decay-component' : "new-pid" }
  // ==========================================================================
  /// Substitute Tool
  ISubstitutePID* m_substitute; // tool
  unsigned int m_maxParticles;
  std::string m_stopIncidentType;
  mutable IIncidentSvc* m_incSvc; ///< the incident service
} ;
// ============================================================================
/* standard constructor
 *  @see DVAlgorithm
 *  @see GaudiTupleAlg
 *  @see GaudiHistoAlg
 *  @see GaudiAlgorithm
 *  @see Algorithm
 *  @see AlgFactory
 *  @see IAlgFactory
 *  @param name the algorithm instance name
 *  @param pSvc pointer to Service Locator
 */
// =============================================================================
SubstitutePID::SubstitutePID                   //        standard constructor
( const std::string& name ,                    // the algorithm instance name
  ISvcLocator*       pSvc )                   //  pointer to Service Locator
  : FitDecayTrees ( name , pSvc )
  , m_map()
  , m_substitute(NULL)
  , m_maxParticles(boost::numeric::bounds<unsigned int>::highest()) 
  , m_stopIncidentType() 
  , m_incSvc(NULL)
{
  FilterDesktop* _this = this ;
  declareProperty
    ( "Substitutions" ,
      m_map           ,
      "PID-substitutions :  { ' decay-component' : 'new-pid' }" )
    -> declareUpdateHandler ( &FilterDesktop::updateHandler1 , _this ) ;
  declareProperty("MaxParticles", m_maxParticles,
                  "max allowed particles to store");
  declareProperty("StopIncidentType", m_stopIncidentType, "incident type");
}
// ============================================================================
// virtual & protected destructor
// ============================================================================
SubstitutePID::~SubstitutePID () {}        // virtual & protected destructor
// ============================================================================
// decode the code --- overrides FilterDesktop method
// ============================================================================
StatusCode SubstitutePID::decodeCode ()
{
  //
  // 1. decode "Code"
  //
  StatusCode sc = FilterDesktop::decodeCode() ;
  if ( sc.isFailure() ) { return sc ; }
  //
  // 2. decode "substitutions"
  //
  if ( !m_substitute ) 
  {
    m_substitute = tool<ISubstitutePID>("SubstitutePIDTool",this);
  }
  if (msgLevel(MSG::DEBUG)) debug() << "Calling decodeCode with " << m_map << endmsg ;
  sc = m_substitute->decodeCode( m_map );
  if (!sc) return sc;
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
/*  the major method to filter input particles
 *  @param input    (INPUT) the input  container of particles
 *  @param filtered (OUPUT) the output container of particles
 *  @return Status code
 */
// ============================================================================
StatusCode SubstitutePID::filter
( const LHCb::Particle::ConstVector& input  ,
  LHCb::Particle::ConstVector&       output )
{
  //
  LHCb::Particle::ConstVector filtered ;
  filtered.reserve ( input.size() ) ;
  //
  // filter particles
  //
  std::copy_if ( input.begin () ,
                 input.end   () ,
                 std::back_inserter ( filtered ) , std::cref(predicate()) ) ;
  //
  if (filtered.empty()) return StatusCode::SUCCESS ;
  LHCb::Particle::ConstVector substituted ;
  substituted.reserve ( input.size() ) ;

  // ==========================================================================
  /**  @attention: "substituted" constains CLONED TREES
   *   @fixme:     one need to protect this usage and change
   *               the tool interface to avoid the misuse!!
   *  @see SubstitutePIDTool::substitute
   */
  m_substitute->substitute(filtered, substituted);
  //
  // refit if needed, store in TES
  for ( LHCb::Particle::ConstVector::const_iterator ip =
          substituted.begin() ; substituted.end() != ip ; ++ip )
  {
    //
    if ( i_markedParticles().size() > m_maxParticles )
    {
      Warning ( "Maximum number of allowed particles reached",
                StatusCode::SUCCESS);
      if(!m_stopIncidentType.empty())
      { incSvc()->fireIncident( Incident ( name(), m_stopIncidentType ) ); }
      break;
    }
    //
    const LHCb::Particle* tree = *ip ;
    if ( 0 == tree ) { continue ; }
    //
    // refit the tree ?
    if ( 0 < chi2cut()  )
    {
      // refit the original
      LHCb::DecayTree nTree = reFitted ( tree ) ;
      // delete the "original" at any circumstances
      DaVinci::deleteTree ( const_cast<LHCb::Particle*> ( tree ) ) ; // DELETE
      //
      if ( !nTree ) { continue ; }            // CONTINUE, fit failure..
      //
      tree = nTree.release() ;                // get the refitted tree
      //
    }
    //
    // mark & store new decay tree
    markNewTree       ( tree ) ; // mark & store new decay tree
    output.push_back  ( tree ) ;
    //
  }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
/// the factory
DECLARE_ALGORITHM_FACTORY(SubstitutePID)
// ============================================================================
// The END
// ============================================================================
