// $Id$
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/IDecay.h"
#include "LoKi/Decays.h"
#include "LoKi/select.h"
#include "LoKi/ParticleProperties.h"
// ============================================================================
// Boost
// ============================================================================
#include <boost/foreach.hpp>
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
 *        Code = " DECTREE('[Beauty -> D+ X-]CC') " ,
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
public:
  // ==========================================================================
  /// finalize  the algorithm 
  virtual StatusCode finalize   () ; 
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
  // perform the actual sustitution 
  unsigned int substitute ( LHCb::Particle* p0 ) ;
  // ==========================================================================
  // perform correction of 4-momentum of the particle
  unsigned int correctP4 ( LHCb::Particle* p0 ) ; 
  // ==========================================================================
private:
  // ==========================================================================
  typedef std::map<std::string,std::string>                   SubstitutionMap ;
  // ==========================================================================
  struct  Substitution
  {
    // ========================================================================
    Substitution () 
      : m_finder ( Decays::Trees::Types_<const LHCb::Particle*>::Invalid() ) 
      , m_pid    ( 0 ) 
      , m_used   ( 0 ) 
    {}  
    Substitution 
    ( Decays::IDecay::iTree&  tree , 
      const LHCb::ParticleID& pid  ) 
      : m_finder ( tree ) 
      , m_pid    ( pid  ) 
      , m_used   ( 0    ) 
    {}
    // ========================================================================
    Decays::IDecay::Finder m_finder  ;     //                 the decay finder 
    LHCb::ParticleID       m_pid     ;     //                              PID 
    unsigned long long     m_used    ;     //                            #used 
    // ========================================================================
  } ;
  // ==========================================================================
  typedef std::vector<Substitution> Substitutions ;
  // ========================================================================== 
  /// mapping : { 'decay-component' : "new-pid" } (property)
  SubstitutionMap  m_map  ; // mapping : { 'decay-component' : "new-pid" }
  /// the actual substitution engine 
  Substitutions    m_subs ; // the actual substitution engine 
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
// mapping : { 'decay-component' : "new-pid" } (property)
  , m_map  ()
// the actual substitution engine 
  , m_subs () 
{
  FilterDesktop* _this = this ;
  declareProperty 
    ( "Substitutions" , 
      m_map           , 
      "PID-substitutions :  { ' decay-component' : 'new-pid' }" )
    -> declareUpdateHandler ( &FilterDesktop::updateHandler1 , _this ) ;
  //
}
// ============================================================================
// virtual & protected destructor 
// ============================================================================
SubstitutePID::~SubstitutePID () {}        // virtual & protected destructor  
// ============================================================================
// finalize  the algorithm 
// ============================================================================
StatusCode SubstitutePID::finalize   () 
{
  m_subs.clear() ;
  return FitDecayTrees::finalize () ;
}
// ============================================================================
// decode the code 
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
  if ( m_map.empty() ) { return Error ( "Empty 'Substitute' map" ) ; }
  //
  /// get the factory
  Decays::IDecay* factory = tool<Decays::IDecay>( "LoKi::Decay" , this ) ;
  ///
  m_subs.clear() ;
  for ( SubstitutionMap::const_iterator item = m_map.begin() ; 
        m_map.end() != item ; ++item ) 
  {
    /// construct the tree 
    Decays::IDecay::Tree tree = factory->tree ( item->first ) ;
    if ( !tree  )
    {
      sc = tree.validate ( ppSvc () ) ;
      if ( sc.isFailure() ) 
      {
        return Error ( "Unable to validate the tree '" + 
                       tree.toString() + "' built from the descriptor '"
                       + item->first   + "'" , sc ) ;
      }
    }
    // get ParticleID 
    const LHCb::ParticleProperty* pp = ppSvc()->find ( item->second ) ;
    if ( 0 == pp ) 
    { return Error ( "Unable to find ParticleID for '" + item->second + "'" ) ; }
    //
    Substitution sub ( tree , pp->particleID() ) ;
    //
    m_subs.push_back ( sub ) ; 
  }
  //
  if ( m_subs.size() != m_map.size() ) 
  { return Error("Mismatch in decoded substitution container") ; }
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
  LoKi::select ( input.begin () , 
                 input.end   () , 
                 std::back_inserter ( filtered ) , predicate() ) ;
  // 
  // substitute 
  //
  StatEntity& cnt = counter("#substituted") ;
  for ( LHCb::Particle::ConstVector::const_iterator ip = 
          filtered.begin() ; filtered.end() != ip ; ++ip ) 
  {
    const LHCb::Particle* p = *ip ;
    if ( 0 == p ) { continue ; }
    //
    // clone the whole decay tree 
    LHCb::DecayTree tree ( *p ) ;
    //
    cnt += substitute ( tree.head()     ) ;
    //
    // the final tree 
    //
    // refit the tree ?
    if ( 0 < chi2cut()  ) 
    {
      LHCb::DecayTree nTree = reFitted ( tree.head() ) ;
      if ( !nTree ) { continue ; }
      //
      // mark & store new decay tree 
      markNewTree       ( nTree.head()     ) ; // mark & store new decay tree 
      //
      output.push_back  ( nTree.release () ) ;
    }
    else
    {
      // mark & store new decay tree 
      markNewTree       ( tree.head()     ) ; // mark & store new decay tree 
      //
      output.push_back  ( tree.release () ) ;
    }
    //
  }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// perform the actual substitution 
// ============================================================================
unsigned int SubstitutePID::substitute ( LHCb::Particle* p )
{
  if ( 0 == p ) { return 0 ; }
  //
  unsigned int substituted = 0 ;

  for ( Substitutions::iterator isub = m_subs.begin() ; 
        m_subs.end() != isub ; ++isub ) 
  {
    LHCb::Particle** _p = &p ;
    //
    LHCb::Particle::ConstVector found ;
    if ( 0 == isub->m_finder.findDecay ( _p , _p + 1 , found ) ) { continue ; }
    //
    for ( LHCb::Particle::ConstVector::const_iterator ip = found.begin() ; 
          found.end() != ip ; ++ip ) 
    {
      const LHCb::Particle* pf = *ip ;
      if ( 0 == pf  ) { continue ; }
      LHCb::Particle* pf_ = const_cast<LHCb::Particle*>( pf ) ;
      if ( 0 == pf_ ) { continue ; }
      pf_ ->setParticleID ( isub -> m_pid ) ;
      //
      ++substituted    ;   
      ++(isub->m_used) ;
    }
  }
  if ( 0 < substituted ) {
    correctP4( p ) ;
  }
  //
  return substituted ;
  //
}
// ============================================================================
// perform recursive 4-momentum correction
// ============================================================================
unsigned int SubstitutePID::correctP4 ( LHCb::Particle* p )
{
  if ( 0 == p ) { return 0 ; }
  //
  const SmartRefVector<LHCb::Particle> daughters = p->daughters() ;
  if ( !daughters.empty() ) {
    double energySum = 0.0, pxSum = 0.0, pySum = 0.0, pzSum = 0.0;
    BOOST_FOREACH( const LHCb::Particle* daughter, daughters ) {
      correctP4( const_cast<LHCb::Particle*>(daughter) ) ;
      energySum += daughter->momentum().E() ;
      pxSum += daughter->momentum().Px();
      pySum += daughter->momentum().Py();
      pzSum += daughter->momentum().Pz();
    }    
    // Correct momentum
    Gaudi::LorentzVector oldMom = p->momentum () ;
    Gaudi::LorentzVector newMom = Gaudi::LorentzVector () ;
    newMom.SetXYZT ( pxSum , pySum , pzSum , energySum ) ;
    p->setMomentum(newMom) ;
    p->setMeasuredMass(newMom.M());
  } else {
    double newMass = LoKi::Particles::massFromPID ( p->particleID() ) ;
    Gaudi::LorentzVector oldMom = p->momentum () ;
    Gaudi::LorentzVector newMom = Gaudi::LorentzVector () ;
    newMom.SetXYZT ( oldMom.Px() , oldMom.Py() , oldMom.Pz() , ::sqrt( oldMom.P2() + newMass*newMass ) ) ;
    p->setMomentum(newMom) ;
    p->setMeasuredMass(newMass);
  }
  return 1 ;
}
// ============================================================================
/// the factory 
DECLARE_ALGORITHM_FACTORY(SubstitutePID) 
// ============================================================================
// The END 
// ============================================================================
