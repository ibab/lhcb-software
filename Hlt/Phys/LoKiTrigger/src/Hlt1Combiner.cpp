// $Id: Hlt1Combiner.cpp 180655 2014-11-25 10:38:48Z mkenzie $
// ============================================================================
// Include files
// ============================================================================
// STD&STL
// ============================================================================
#include <memory>
// ============================================================================
#ifdef __INTEL_COMPILER
#pragma warning(disable:1572) // non-pointer conversion ... may lose significant bits
#pragma warning(push)
#endif
// ============================================================================
// HltBase
// ============================================================================
#include "HltBase/HltUtils.h"
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
// ============================================================================
// Kernel
// ============================================================================
#include "Kernel/IParticleCombiner.h"
#include "Kernel/DaVinciAlgorithm.h"
#include "Kernel/IDecodeSimpleDecayString.h"
#include "Kernel/PVSentry.h"
#include "Kernel/GetDecay.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Constants.h"
#include "LoKi/TrgToCpp.h"
// #include "LoKi/Hlt1.h"
#include "LoKi/Hlt1Combiner.h"
#include "LoKi/NBodyCompare.h"
#include "LoKi/CompareParticles.h"
#include "LoKi/GetPhysDesktop.h"
// ============================================================================
#include  "LTTools.h"
#ifdef __INTEL_COMPILER
#pragma warning(pop)
#endif
// ============================================================================
/** @file
 *
 *  Implementation file for class LoKi::Hlt1::Hlt1Combiner
 *  This file is part of LoKi project:
 *   ``C++ ToolKit for Smart and Friendly Physics Analysis''
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas,
 *  contributions and advices from G.Raven, J.van Tilburg,
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Matthew KENZIE matthew.kenzie@cern.ch
 *  @date   2014-11-27
 *
 *                    $Revision: 180655 $
 *  Last Modification $Date: 2014-11-25 11:38:48 +0100 (Tue, 25 Nov 2014) $
 *                 by $Author: mkenzie $
 */
// ============================================================================
//   constructor from decay string, output location, sources and cuts
// ============================================================================
LoKi::Hlt1::Hlt1Combiner::Hlt1Combiner
( const std::string&                                             output  ,
  const LoKi::Hlt1::Hlt1CombinerConf&                            config  ,
  const std::string&                                             source1 ,
  const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&   cut1    )
  : LoKi::AuxFunBase  ( std::tie ( output , config , source1 , cut1 ) )
  , LoKi::BasicFunctors<const Hlt::Candidate*>::Source ()
  , LoKi::Hlt1::HelperTool( 1 )
  , m_sink   ( output )
  , m_conf   ( config )
  , m_source ( LoKi::Hlt1::Selection ( source1 , cut1 ) )
  , m_pc     ()
  , m_decays ()
  , m_dvAlg  ()
{
  setup();
}
// ============================================================================
//   constructor from decay string, output location, sources and cuts
// ============================================================================
LoKi::Hlt1::Hlt1Combiner::Hlt1Combiner
( const std::string&                                             output  ,
  const LoKi::Hlt1::Hlt1CombinerConf&                            config  ,
  const std::string&                                             source1 ,
  const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&   cut1    ,
  const std::string&                                             source2 ,
  const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&   cut2    )
  : LoKi::AuxFunBase  ( std::tie ( output  , config ,
                                   source1 , cut1   ,
                                   source2 , cut2   ) )
  , LoKi::BasicFunctors<const Hlt::Candidate*>::Source ()
  , LoKi::Hlt1::HelperTool( 1 )
  , m_sink ( output )
  , m_conf ( config )
  , m_source ( no_empty_union ( LoKi::Hlt1::Selection( source1 , cut1 ) ,
                                LoKi::Hlt1::Selection( source2 , cut2 ) ) )
  , m_pc     ()
  , m_decays ()
  , m_dvAlg  ()
{
  setup();
}
// ============================================================================
//   constructor from decay string, output location, sources and cuts
// ============================================================================
LoKi::Hlt1::Hlt1Combiner::Hlt1Combiner
( const std::string&                                             output  ,
  const LoKi::Hlt1::Hlt1CombinerConf&                            config  ,
  const std::string&                                             source1 ,
  const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&   cut1    ,
  const std::string&                                             source2 ,
  const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&   cut2    ,
  const std::string&                                             source3 ,
  const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&   cut3    )
  : LoKi::AuxFunBase  ( std::tie ( output  , config ,
                                   source1 , cut1   ,
                                   source2 , cut2   ,
                                   source3 , cut3   ) )
  , LoKi::BasicFunctors<const Hlt::Candidate*>::Source ()
  , LoKi::Hlt1::HelperTool( 1 )
  , m_sink ( output )
  , m_conf ( config )
  , m_source ( no_empty_union ( LoKi::Hlt1::Selection( source1 , cut1 ) ,
                                LoKi::Hlt1::Selection( source2 , cut2 ) ,
                                LoKi::Hlt1::Selection( source3 , cut3 ) ) )
  , m_pc     ()
  , m_decays ()
  , m_dvAlg  ()
{
  setup();
}
// ============================================================================
// setup function (common call from constructors)
// ============================================================================
StatusCode LoKi::Hlt1::Hlt1Combiner::setup()
{
  m_maxCand     = 0     ;
  m_maxCandStop = false ;
  m_maxComb     = 0     ;
  m_maxCombStop = false ;
  //
  // get required tools
  //
  LoKi::ILoKiSvc* svc = lokiSvc() ;
  SmartIF<IToolSvc> tsvc ( svc ) ;
  IParticleCombiner*  _pc = 0 ;
  StatusCode sc = tsvc->retrieveTool ( m_conf.combiner() , _pc ) ;
  m_pc = _pc ;
  Assert ( sc.isSuccess() && m_pc ,
           "Unable to get Particle Combiner'" + m_conf.combiner() + "'" , sc ) ;
  //
  IDecodeSimpleDecayString *dsds ;
  sc = tsvc->retrieveTool("DecodeSimpleDecayString:PUBLIC", dsds ) ;
  Assert ( sc.isSuccess() && 0 != dsds  ,
           "Unable to get DecodeSimpleDecayString:PUBLIC " , sc ) ;
  //
  m_decays = Decays::decays ( m_conf.decaystrs () , dsds ) ;
  LoKi::release ( dsds ) ;
  Assert ( !m_decays.empty(), "Unable to decode DecayDescriptor" );
  //
  for ( std::vector<Decays::Decay>::const_iterator idecay = m_decays.begin() ;
        m_decays.end() != idecay ; ++idecay )
  {
    const Decays::Decay::Items& children  = idecay->children () ;
    for ( const Decays::Decay::Item& item : children )
    { m_items  [ item.pid() ] = item.name() ; }
  }
  //
  Assert ( !m_items.empty(), "Unable to collect all daughters!" );
  //
  // get DValgorithm
  m_dvAlg = LoKi::Hlt1::Utils::getPhysDesktop ( *this ) ;
  Assert ( m_dvAlg , "Unable to find IDVAlgorithm"  );
  //
  // ===========================================================
  return sc ;
}
// ============================================================================
// virtual destructor
// ============================================================================
LoKi::Hlt1::Hlt1Combiner::~Hlt1Combiner ()
{
  if ( m_pc    && !gaudi() ) { m_pc   .reset() ; }
  if ( m_dvAlg && !gaudi() ) { m_dvAlg.reset() ; }
}
// ============================================================================
// clone method ("virtual constructor")
// ============================================================================
LoKi::Hlt1::Hlt1Combiner* LoKi::Hlt1::Hlt1Combiner::clone() const
{ return new LoKi::Hlt1::Hlt1Combiner { *this } ; }
// ============================================================================
// getDaughters helper function
// ============================================================================
bool
LoKi::Hlt1::Hlt1Combiner::getDaughters
( const  LoKi::Hlt1::Hlt1Combiner::CANDIDATES& input     ,
  LoKi::Hlt1::Hlt1Combiner::Selected&          daughters ) const
{
  //
  typedef CANDIDATES::const_iterator ITERATOR   ;
  //
  // loop input candidates and add to daughters
  //
  for ( ITERATOR icand = input.begin(); icand != input.end(); ++icand )
  {
    const Hlt::Candidate* cand     = *icand ;
    if ( !cand      ) { continue ; }                                  // SKIP
    const LHCb::Particle* particle = cand->get<LHCb::Particle> () ;
    if ( !particle  ) { continue ; }                                  // SKIP
    //
    MAP::const_iterator ifind = m_items.find ( particle->particleID() ) ;
    if ( m_items.end() == ifind ) { continue ; }                         // SKIP
    //
    daughters.add ( ifind->second , particle ) ;
  }
  //
  return true ;
}
// ============================================================================
// combine particles
// ============================================================================
unsigned long
LoKi::Hlt1::Hlt1Combiner::executeCombineParticles
( LoKi::Hlt1::Hlt1Combiner::CANDIDATES&       output    ,
  const LoKi::Hlt1::Hlt1Combiner::Selected&   daughters ,
  const Decays::Decay&                        decay     ) const
{
  //
  // the combination tools
  typedef LoKi::Combiner_<LHCb::Particle::ConstVector> Combiner ;
  const IParticleCombiner* combiner = pc() ;
  const LoKi::Particles::PidCompare compare{} ; //  = LoKi::Particles::PidCompare () ;
  //

  // the combiner itself
  Combiner loop ;
  // loop children and add daughters of this type to the combiner
  const Decays::Decay::Items& dec_items = decay.children() ;
  for ( Decays::Decay::Items::const_iterator child = dec_items.begin() ;
        child != dec_items.end(); child++)
  {
    loop.add( daughters ( child->name() ) ) ;
  }
  //
  StatEntity& cnt_c  =  alg()->counter ( "#pass combcut"    ) ;
  StatEntity& cnt_m  =  alg()->counter ( "#pass mother cut" ) ;
  //
  unsigned long nGood = 0 ;

  LHCb::Vertex   vertex  ;
  LHCb::Particle mother ( decay.mother().pid() ) ;

  // now do actual loop of combinations
  for ( ; loop.valid() ; loop.next() )
  {
    if ( !loop.unique( compare ) ) { continue ; }

    // get current combination
    LHCb::Particle::ConstVector combination ( loop.dim() );
    loop.current ( combination.begin() ) ;

    // apply comb cuts
    const bool pass_combination = combcut ( combination ) ;
    cnt_c += pass_combination ;
    if ( !pass_combination ) { continue ; }


    // do combination
    StatusCode sc = combiner -> combine ( combination , mother , vertex ) ;
    if ( sc.isFailure() ) { continue ; }

    //
    /// setup sentry before mother cuts
    //
    DaVinci::PVSentry sentry ( m_dvAlg , &mother ) ;

    //
    // apply mother cuts
    //
    const bool pass_mother = mothcut ( &mother ) ;
    cnt_m += pass_mother ;
    if ( !pass_mother ) { continue ; }

    //
    // store in TES
    //
    LHCb::Particle* pnew = mother.clone() ;
    LHCb::Vertex*   vnew = vertex.clone() ;
    pnew->setEndVertex ( vnew ) ;
    //
    _storeParticle ( pnew ) ;
    _storeVertex   ( vnew ) ;

    //
    // add new candidate
    //
    Hlt::Candidate *candidate = newCandidate();
    output.push_back ( candidate ) ;
    candidate->addToWorkers ( alg() ) ;

    //
    // add new stage
    //
    Hlt::Stage* stage = newStage () ;
    candidate->addToStages( stage ) ;
    Hlt::Stage::Lock lock { stage, alg() } ;
    stage->set            ( pnew  ) ;          // ATTENTION!!

    ++nGood ;
    // =======================================================================
  } // end loop over combinations
  // =========================================================================
  //
  return nGood ;
}
// ============================================================================
// 3 body
// ============================================================================
unsigned long
LoKi::Hlt1::Hlt1Combiner::execute3BodyCombination
( LoKi::Hlt1::Hlt1Combiner::CANDIDATES&     output    ,
  const LoKi::Hlt1::Hlt1Combiner::Selected& daughters ,
  const Decays::Decay&                      decay     ) const
{
  //
  const LoKi::Particles::NBodyCompare compare{} ; // = LoKi::Particles::NBodyCompare () ;
  //
  // the combination tools
  const IParticleCombiner* combiner = pc() ;

  // Flag to indicate if processing is aborted
  bool problem = false ;

  // the counter of "good" selected decays
  unsigned long nGood = 0 ;

  // fill it with the input data
  const Decays::Decay::Items& items = decay.children() ;
  Selected::Range child1 = daughters ( items [0].name() ) ;
  if ( child1.empty() ) { return nGood ; }
  Selected::Range child2 = daughters ( items [1].name() ) ;
  if ( child2.empty() ) { return nGood ; }
  Selected::Range child3 = daughters ( items [2].name() ) ;
  if ( child3.empty() ) { return nGood ; }

  /// check  combinatoric
  double ncomb = 1.0 * child1.size() * child2.size() * child3.size() ;
  problem = problem || tooMuchCombinations ( ncomb , decay ) ;
  if ( problem ) { return  nGood ; }

  StatEntity& cnt_c12 = alg()->counter ( "#pass comb12 cut"  ) ;
  StatEntity& cnt_c   = alg()->counter ( "#pass combcut"     ) ;
  StatEntity& cnt_m   = alg()->counter ( "#pass mother cut"  ) ;
  //

  LHCb::Vertex   vertex  ;
  LHCb::Particle mother ( decay.mother().pid() ) ;
  //
  // make explicit 3-body loop
  //
  for ( Selected::Range::const_iterator it1 = child1.begin() ;
        child1.end() != it1 && !problem ; ++it1 )
  {
    //
    problem = problem || tooMuchCandidates ( nGood , decay ) ;
    if ( problem ) { break ; }  // BREAK the loop
    //
    const LHCb::Particle* c1 = *it1 ;
    if ( 0  == c1 ) { continue ; }                   // CONTINUE, next c1
    //
    // get the actual('current') combination:
    LHCb::Particle::ConstVector comb12      ( 2 ) ;
    // get the actual('current') combination:
    LHCb::Particle::ConstVector combination ( 3 ) ;
    comb12      [0] = c1 ;
    combination [0] = c1 ;
    //
    // loop over 2nd particle
    for ( Selected::Range::const_iterator it2 = child2.begin() ;
          child2.end() != it2 && !problem ; ++it2 )
    {
      //
      problem = problem || tooMuchCandidates ( nGood , decay ) ;
      if ( problem ) { break ; }  // BREAK the loop
      //
      const LHCb::Particle* c2 = *it2 ;
      //
      // good and unique ?
      if ( !compare ( c1 , c2 ) ) { continue ; }     // CONTINUE, next c2
      //
      comb12      [1] = c2 ;
      combination [1] = c2 ;
      //
      // Use combination cuts!
      // it is a heart of all game:
      // here we have the combination and can apply the cut:
      const bool pass_combination12 = combcut12 ( comb12 ) ;
      cnt_c12 += pass_combination12 ;
      if ( !pass_combination12 ) { continue ; }
      //
      //
      // loop over 3rd particle
      for ( Selected::Range::const_iterator it3 = child3.begin() ;
            child3.end() != it3 && !problem ; ++it3 )
      {
        //
        problem = problem || tooMuchCandidates ( nGood , decay ) ;
        if ( problem ) { break ; }  // BREAK the loop
        //
        const LHCb::Particle* c3 = *it3 ;
        //
        // good and unique ?
        if ( !compare ( c1 , c2 , c3 ) ) { continue ; }   // CONTINUE, next c3
        //
        combination [2] = c3 ;

        //
        // check the combinations for overlaps and cuts
        //
        const bool pass_combination = combcut ( combination ) ;
        cnt_c += pass_combination ;
        if ( !pass_combination ) { continue ; }

        // here we can create the mother particle, the vertex,
        // check them and save in TES

        // do combination
        StatusCode sc = combiner -> combine ( combination , mother , vertex ) ;
        if ( sc.isFailure() ) { continue ; }

        //
        /// setup sentry before mother cuts
        //
        DaVinci::PVSentry sentry ( m_dvAlg , &mother ) ;

        // apply mother cuts
        const bool pass_mother = mothcut ( &mother ) ;
        cnt_m += pass_mother ;
        if ( !pass_mother ) { continue ; }

        //
        // store in TES
        //
        LHCb::Particle* pnew = mother.clone() ;
        LHCb::Vertex*   vnew = vertex.clone() ;
        pnew->setEndVertex ( vnew ) ;
        //
        _storeParticle ( pnew ) ;
        _storeVertex   ( vnew ) ;

        //
        // add new candidate
        //
        Hlt::Candidate *candidate = newCandidate();
        output.push_back ( candidate ) ;
        candidate->addToWorkers ( alg() ) ;

        //
        // add new stage
        //
        Hlt::Stage* stage = newStage () ;
        candidate->addToStages( stage ) ;
        Hlt::Stage::Lock lock { stage, alg() } ;
        stage->set            ( pnew  ) ;  // ATTENTION!

        // increment number of good decays
        ++nGood;
        // ====================================================================
      }                                    // end of the loop over 3rd particle
      // ======================================================================
    }                                      // end of the loop over 2nd particle
    // ========================================================================
  }                                        // end of the loop over 1st particle
  // ==========================================================================
  //
  return nGood ;
  // ==========================================================================
}
// ============================================================================
// 4 body
// ============================================================================
unsigned long
LoKi::Hlt1::Hlt1Combiner::execute4BodyCombination
( LoKi::Hlt1::Hlt1Combiner::CANDIDATES&     output    ,
  const LoKi::Hlt1::Hlt1Combiner::Selected& daughters ,
  const Decays::Decay&                      decay     ) const
{
  const LoKi::Particles::NBodyCompare compare{} ; // = LoKi::Particles::NBodyCompare () ;

  const IParticleCombiner* combiner = pc() ; // get the particle combiner

  // Flag to indicate if processing is aborted
  bool problem = false ;

  // the counter of "good" selected decays
  size_t nGood = 0 ;

  // fill it with the input data
  const Decays::Decay::Items& items = decay.children() ;
  Selected::Range child1 = daughters ( items [0].name() ) ;
  if ( child1.empty() ) { return nGood ; }
  Selected::Range child2 = daughters ( items [1].name() ) ;
  if ( child2.empty() ) { return nGood ; }
  Selected::Range child3 = daughters ( items [2].name() ) ;
  if ( child3.empty() ) { return nGood ; }
  Selected::Range child4 = daughters ( items [3].name() ) ;
  if ( child4.empty() ) { return nGood ; }

  /// check  combinatoric
  double ncomb  = 1.0 * child1.size() * child2.size() ;
  ncomb        *=       child3.size() * child4.size() ;
  problem = problem || tooMuchCombinations ( ncomb , decay ) ;
  if ( problem ) { return nGood ; }

  StatEntity& cnt_c12  = alg()->counter ( "#pass comb12 cut"  ) ;
  StatEntity& cnt_c123 = alg()->counter ( "#pass comb123 cut" ) ;
  StatEntity& cnt_c    = alg()->counter ( "#pass combcut"     ) ;
  StatEntity& cnt_m    = alg()->counter ( "#pass mother cut"  ) ;

  //
  LHCb::Vertex   vertex  ;
  LHCb::Particle mother ( decay.mother().pid() ) ;

  //
  // make explicit 4-body loop
  //
  for ( Selected::Range::const_iterator it1 = child1.begin() ;
        child1.end() != it1 && !problem ; ++it1 )
  {
    //
    problem = problem || tooMuchCandidates ( nGood , decay ) ;
    if ( problem ) { break ; }  // BREAK the loop
    //
    const LHCb::Particle* c1 = *it1 ;
    if ( 0  == c1 ) { continue ; }                   // CONTINUE, next c1
    //
    // get the actual('current') combinations
    LHCb::Particle::ConstVector comb12      ( 2 ) ;
    LHCb::Particle::ConstVector comb123     ( 3 ) ;
    LHCb::Particle::ConstVector combination ( 4 ) ;
    comb12      [0] = c1 ;
    comb123     [0] = c1 ;
    combination [0] = c1 ;
    //
    // loop over 2nd particle
    for ( Selected::Range::const_iterator it2 = child2.begin() ;
          child2.end() != it2 && !problem ; ++it2 )
    {
      //
      problem = problem || tooMuchCandidates ( nGood , decay ) ;
      if ( problem ) { break ; }  // BREAK the loop
      //
      const LHCb::Particle* c2 = *it2 ;
      //
      // good & unique ?
      if ( !compare ( c1 , c2 ) ) { continue ; } // CONTINUE, next c2
      //
      comb12      [1] = c2 ;
      comb123     [1] = c2 ;
      combination [1] = c2 ;
      //
      // Use combination cuts! it is a heart of all game:
      // here we have the combination and can apply the cut:
      const bool pass_combination12 = combcut12 ( comb12 ) ;
      cnt_c12 += pass_combination12 ;
      if ( !pass_combination12 ) { continue ; }
      //
      //
      // loop over 3rd particle
      for ( Selected::Range::const_iterator it3 = child3.begin() ;
            child3.end() != it3 && !problem ; ++it3 )
      {
        //
        problem = problem || tooMuchCandidates ( nGood , decay ) ;
        if ( problem ) { break ; }  // BREAK the loop
        //
        const LHCb::Particle* c3 = *it3 ;
        //
        // good & unique ?
        if ( ! compare ( c1 ,c2 , c3 ) ) { continue ; } // CONTINUE, next c3
        //
        comb123     [2] = c3 ;
        combination [2] = c3 ;

        // Use combination cuts! it is a heart of all game:
        // here we have the combination and can apply the cut:
        const bool pass_combination123 = combcut123 ( comb123 ) ;
        cnt_c123 += pass_combination123 ;
        if ( !pass_combination123 ) { continue ; }
        //

        // loop over 4th particle
        for ( Selected::Range::const_iterator it4 = child4.begin() ;
              child4.end() != it4 && !problem ; ++it4 )
        {
          //
          problem = problem || tooMuchCandidates ( nGood , decay ) ;
          if ( problem ) { break ; }  // BREAK the loop
          //
          const LHCb::Particle* c4 = *it4 ;
          //
          // good & unique ?
          if ( !compare ( c1 ,c2 , c3 , c4 ) ) { continue ; } // CONTINUE, next c4
          //
          combination [3] = c4 ;

          //
          // check the combinations for overlaps and cuts
          //
          const bool pass_combination = combcut ( combination ) ;
          cnt_c += pass_combination ;
          if ( !pass_combination ) { continue ; }

          // here we can create the mother particle, the vertex,
          // check them and save in TES

          // do combination
          StatusCode sc = combiner -> combine ( combination , mother , vertex ) ;
          if ( sc.isFailure() ) { continue ; }

          DaVinci::PVSentry sentry ( m_dvAlg , &mother ) ;

          // apply mother cuts
          const bool pass_mother = mothcut ( &mother ) ;
          cnt_m += pass_mother ;
          if ( !pass_mother ) { continue ; }

          //
          // store in TES
          //
          LHCb::Particle* pnew = mother.clone() ;
          LHCb::Vertex*   vnew = vertex.clone() ;
          pnew->setEndVertex ( vnew ) ;
          //
          _storeParticle ( pnew ) ;
          _storeVertex   ( vnew ) ;

          //
          // add new candidate
          //
          Hlt::Candidate *candidate = newCandidate();
          output.push_back ( candidate ) ;
          candidate->addToWorkers ( alg() ) ;

          //
          // add new stage
          //
          Hlt::Stage* stage = newStage () ;
          candidate->addToStages( stage ) ;
          Hlt::Stage::Lock lock { stage, alg() } ;
          stage->set            ( pnew  ) ;  // ATTENTION!

          // increment number of good decays
          ++nGood;

          // ==================================================================
        }                                  // end of the loop over 4th particle
        // ====================================================================
      }                                    // end of the loop over 3rd particle
      // ======================================================================
    }                                      // end of the loop over 2nd particle
    // ========================================================================
  }                                        // end of the loop over 1st particle
  // ==========================================================================
  //
  return nGood ;
  // ==========================================================================
}
// ============================================================================
// the only one important method
// ============================================================================
LoKi::Hlt1::Hlt1Combiner::result_type
LoKi::Hlt1::Hlt1Combiner::operator()
  ( /*LoKi::Hlt1::Hlt1Combiner::argument a*/ ) const
{
  Assert ( m_dvAlg , "Invalid setup!" );

  typedef result_type                CANDIDATES ;
  const CANDIDATES input = source() () ;

  // get daughters from the input sources
  Selected daughters ;
  getDaughters ( input , daughters ) ;
  //
  // a little bit of monitoring of input particles
  //
  for ( Selected::map_iterator id = daughters.begin() ; daughters.end() != id ; ++id )
  { alg()->counter("#input " + id->first ) += id->second.size() ; }
  //
  if ( input.empty() ) { return result_type () ; } // no action for EMPTY input
  //
  // the output selection
  CANDIDATES output;
  output.reserve ( 100 ) ;
  //
  for ( std::vector<Decays::Decay>::const_iterator idecay = m_decays.begin() ;
        idecay != m_decays.end() ; idecay ++ )
  {
    //
    const unsigned short N = idecay->children().size();
    //
    if      ( 3 == N ) { execute3BodyCombination(output, daughters, *idecay) ; }
    else if ( 4 == N ) { execute4BodyCombination(output, daughters, *idecay) ; }
    // fine for 2 body (not at all optimal for N>4 body)
    else               { executeCombineParticles(output, daughters, *idecay) ; }
  }
  //
  // register the selection in Hlt Data Service
  return !m_sink ? output : m_sink ( output ) ;
  // ==========================================================================
}
// ============================================================================
// OPTIONAL: nice printout
// ============================================================================
std::ostream& LoKi::Hlt1::Hlt1Combiner::fillStream ( std::ostream& s ) const
{
  return
    s << "TC_HLT1COMBINER("
      << "'" << location () << "', "
      << m_conf             << ")" ;
}
// ============================================================================
// The END
// ============================================================================
