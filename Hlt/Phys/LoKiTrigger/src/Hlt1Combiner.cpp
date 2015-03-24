// $Id: Hlt1Combiner.cpp 180655 2014-11-25 10:38:48Z mkenzie $
// ============================================================================
// Include files
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
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/TrgToCpp.h"
#include "LoKi/Hlt1Combiner.h"
#include "LoKi/ToParticles.h"
#include "LoKi/Hlt1.h"
#include "LoKi/Constants.h"
#include "LoKi/ParticleProperties.h"
#include "LoKi/CompareParticles.h"
#include  "LTTools.h"
// ============================================================================
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
( const std::string                                              output  ,
  const LoKi::Hlt1::Hlt1CombinerConf&                            config  ,
  const std::string&                                             source1 ,
  const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&   cut1    )
  : LoKi::AuxFunBase  ( std::tie ( output , config , source1 , cut1 ) )
  , LoKi::BasicFunctors<const Hlt::Candidate*>::Source ()
  , LoKi::Hlt1::HelperTool( 1 )
  , m_sink { std::move(output) }
  , m_conf ( config )
  , m_source ( LoKi::Hlt1::Selection( source1 , cut1 ) )
{
  setup();
}
// ============================================================================
//   constructor from decay string, output location, sources and cuts 
// ============================================================================
LoKi::Hlt1::Hlt1Combiner::Hlt1Combiner
( const std::string                                              output  ,
  const LoKi::Hlt1::Hlt1CombinerConf&                            config  ,
  const std::string&                                             source1 ,
  const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&   cut1    ,
  const std::string&                                             source2 ,
  const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&   cut2    )
  : LoKi::AuxFunBase  ( std::tie ( output , config , source1 , cut1 , source2 , cut2 ) )
  , LoKi::BasicFunctors<const Hlt::Candidate*>::Source ()
  , LoKi::Hlt1::HelperTool( 1 )
  , m_sink { std::move(output) }
  , m_conf ( config )
  , m_source ( no_empty_union ( LoKi::Hlt1::Selection( source1 , cut1 ) ,
                                LoKi::Hlt1::Selection( source2 , cut2 ) ) )
{
  setup();
}
// ============================================================================
//   constructor from decay string, output location, sources and cuts 
// ============================================================================
LoKi::Hlt1::Hlt1Combiner::Hlt1Combiner
( const std::string                                              output  ,
  const LoKi::Hlt1::Hlt1CombinerConf&                            config  ,
  const std::string&                                             source1 ,
  const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&   cut1    ,
  const std::string&                                             source2 ,
  const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&   cut2    ,
  const std::string&                                             source3 ,
  const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&   cut3    ) 
  : LoKi::AuxFunBase  ( std::tie ( output , config , source1 , cut1 , source2 , cut2 , source3 , cut3 ) )
  , LoKi::BasicFunctors<const Hlt::Candidate*>::Source ()
  , LoKi::Hlt1::HelperTool( 1 )
  , m_sink { std::move(output) }
  , m_conf ( config )
  , m_source ( no_empty_union ( LoKi::Hlt1::Selection( source1 , cut1 ) ,
                                LoKi::Hlt1::Selection( source2 , cut2 ) ,
                                LoKi::Hlt1::Selection( source3 , cut3 ) ) )
{
  setup();
}
// ============================================================================
// setup function (common call from constructors)
// ============================================================================
StatusCode LoKi::Hlt1::Hlt1Combiner::setup()
{
  m_maxCand = 0 ;
  m_maxCandStop = false ;
  m_maxComb = 0 ;
  m_maxCombStop = false ;
  // get required tools
  LoKi::ILoKiSvc* svc = lokiSvc() ;
  SmartIF<IToolSvc> tsvc ( svc ) ;
  StatusCode sc = tsvc->retrieveTool(m_conf.combiner(), m_pc);
  return sc ;
}
// ============================================================================
// virtual destructor
// ============================================================================
LoKi::Hlt1::Hlt1Combiner::~Hlt1Combiner () {}
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
  ( LoKi::Hlt1::Hlt1Combiner::Selected& daughters ) const
{
  typedef result_type                CANDIDATES ;
  typedef CANDIDATES::const_iterator ITERATOR   ;
  const CANDIDATES a = source() () ;
  const CANDIDATES* arg1 = &a;

  //std::cout << "Hlt1Combiner::getDaughters()" << std::endl;
  // loop over the selections and add daughters
  // Source 1
  for ( ITERATOR icand1 = arg1->begin(); icand1 != arg1->end(); icand1++ )
  {
    const Hlt::Candidate* cand1 = *icand1 ;
    if ( !cand1 ) { continue ; }
    const LHCb::Particle* particle1 = cand1->get<LHCb::Particle> () ;
    if ( !particle1 ) { continue ; }
    daughters.add ( LoKi::Particles::nameFromPID(particle1->particleID()) , particle1 ) ;
  }
  return true ;
}
// ============================================================================
// combine particles 
// ============================================================================
void // should return a StatusCode instead?
LoKi::Hlt1::Hlt1Combiner::executeCombineParticles
  ( LoKi::Hlt1::Hlt1Combiner::result_type& output, const Decays::Decay& decay ) const
{
  // get daughters from the input sources 
  Selected daughters;
  getDaughters( daughters ) ;

  // the combination tools
  typedef LoKi::Combiner_<LHCb::Particle::ConstVector> Combiner ;
  const IParticleCombiner* combiner = pc() ;
  const LoKi::Particles::PidCompare compare = LoKi::Particles::PidCompare () ;
  
  // the combiner itself
  Combiner loop ;
  // loop children and add daughters of this type to the combiner
  const Decays::Decay::Items& dec_items = decay.children() ;
  for ( Decays::Decay::Items::const_iterator child = dec_items.begin() ;
        child != dec_items.end(); child++)
  {
    //std::cout << "Added daughters of type " << child->name() << " to loop" << std::endl;
    //child->fillStream(std::cout); std::cout << std::endl;
    loop.add( daughters ( child->name() ) ) ;
  }

  // now do actual loop of combinations
  //std::cout << "Will run " << loop.size() << " combinations " << std::endl;
  int i=-1;
  for ( ; loop.valid() ; loop.next() )
  {
      i++;
      if ( !loop.unique( compare ) ) {
        //std::cout << "  -- " << i << " fail PID compare" << std::endl;
        continue ;
      }

      // get current combination
      LHCb::Particle::ConstVector combination ( loop.dim() );
      loop.current ( combination.begin() ) ;

      // apply comb cuts
      if ( ! combcut ( combination ) ) {
        //std::cout << " -- " << i << " fail COMB cut" << std::endl;
        continue ;
      }

      LHCb::Vertex*     vertex = new LHCb::Vertex();
      LHCb::Particle*   mother = new LHCb::Particle();
      mother->setParticleID( decay.mother().pid() );

      // do combination
      combiner -> combine ( combination , *mother , *vertex ) ;

      //std::cout << i << ": Created mother particle: " << LoKi::Particles::nameFromPID(mother->particleID()) << " PID: " << mother->particleID().pid() << " M: " << mother->momentum().M() << " P: " << mother->p() << " PT: " << mother->pt() << std::endl;
      //std::cout << i << ": Used primary vertex: (" << vertex->position().X() << "," << vertex->position().Y() << "," << vertex->position().Z() << ")" << std::endl;

      // apply mother cuts
      if ( ! mothcut ( mother ) ) {
        delete vertex;
        delete mother;
        //std::cout << " -- " << i << " fail MOTH cut" << std::endl;
        continue ;
      }
      //std::cout << " -- PASS" << std::endl;

      // store in TES
      _storeParticle( mother ) ;

      // add new candidate
      Hlt::Candidate *candidate = newCandidate();
      candidate->addToWorkers ( alg() ) ;
      // add new stage
      Hlt::Stage* stage = newStage () ;
      candidate->addToStages( stage ) ;
      Hlt::Stage::Lock lock { stage, alg() } ;
      stage->set( mother ) ;
      output.push_back ( candidate ) ;
      // ====================================================================
  } //                                         end loop over combinations
}
// ============================================================================
// 3 body 
// ============================================================================
void
LoKi::Hlt1::Hlt1Combiner::execute3BodyCombination 
  ( LoKi::Hlt1::Hlt1Combiner::result_type& output, const Decays::Decay& decay ) const
{
  
  // get daughters from the input sources 
  Selected daughters;
  getDaughters( daughters ) ;

  // the combination tools
  const IParticleCombiner* combiner = pc() ;
  
  // the counter of recontructed/selected decays:
  size_t nTotal = 0 ;
  
  // Flag to indicate if processing is aborted
  bool problem = false ;
  
  LHCb::Particle::ConstVector saved ;
  saved.reserve ( 100 ) ; // CRJ : Was 1000. Seems a bit big ?
  
  // the counter of "good" selected decays
  size_t nGood = 0 ;

  // fill it with the input data
  const Decays::Decay::Items& items = decay.children() ;
  Selected::Range child1 = daughters ( items [0].name() ) ;
  if ( child1.empty() ) { return ; }                  
  Selected::Range child2 = daughters ( items [1].name() ) ;
  if ( child2.empty() ) { return ; }                 
  Selected::Range child3 = daughters ( items [2].name() ) ;
  if ( child3.empty() ) { return ; }                 

  /// check  combinatoric 
  double ncomb = 1.0 * child1.size() * child2.size() * child3.size() ;
  problem = problem || tooMuchCombinations ( ncomb , decay ) ;
  if ( problem ) { return  ; } 
  
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
      if ( ! combcut12 ( comb12 ) ) { continue ; } // CONTINUE, next c2 
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
        if ( ! combcut ( combination ) ) { 
          continue ; 
        }
        
        // here we can create the mother particle, the vertex, 
        // check them and save in TES 
        LHCb::Vertex*     vertex = new LHCb::Vertex();
        LHCb::Particle*   mother = new LHCb::Particle();
        mother->setParticleID( decay.mother().pid() );
        
        // do combination
        combiner -> combine ( combination , *mother , *vertex ) ;
        
        // apply mother cuts
        if ( ! mothcut ( mother ) ) {
          delete vertex;
          delete mother;
          //std::cout << " -- " << i << " fail MOTH cut" << std::endl;
          continue ;
        }
        
        // store in TES
        _storeParticle( mother ) ;

        // add new candidate
        Hlt::Candidate *candidate = newCandidate();
        candidate->addToWorkers ( alg() ) ;
        // add new stage
        Hlt::Stage* stage = newStage () ;
        candidate->addToStages( stage ) ;
        Hlt::Stage::Lock lock { stage, alg() } ;
        stage->set( mother ) ;
        output.push_back ( candidate ) ;
        
        // increment number of good decays
        ++nGood; 

        // ==================================================================
      }                                  // end of the loop over 3rd particle 
      // ====================================================================
    }                                    // end of the loop over 2nd particle 
    // ======================================================================
  }                                      // end of the loop over 1st particle
  // ========================================================================
}
// ============================================================================
// 4 body 
// ============================================================================
void
LoKi::Hlt1::Hlt1Combiner::execute4BodyCombination 
  ( LoKi::Hlt1::Hlt1Combiner::result_type& output, const Decays::Decay& decay ) const
{
  
  // get daughters from the input sources 
  Selected daughters;
  getDaughters( daughters ) ;

  const IParticleCombiner* combiner = pc() ; // get the particle combiner
  
  // the counter of recontructed/selected decays:
  size_t nTotal = 0 ;
  
  // Flag to indicate if processing is aborted
  bool problem = false ;
  
  LHCb::Particle::ConstVector saved ;
  saved.reserve ( 100 ) ; // CRJ : Was 1000. Seems a bit big ?
  
  // the counter of "good" selected decays
  size_t nGood = 0 ;
  
  // fill it with the input data
  const Decays::Decay::Items& items = decay.children() ;    
  Selected::Range child1 = daughters ( items [0].name() ) ;
  if ( child1.empty() ) { return ; }                  
  Selected::Range child2 = daughters ( items [1].name() ) ;
  if ( child2.empty() ) { return ; }                  
  Selected::Range child3 = daughters ( items [2].name() ) ;
  if ( child3.empty() ) { return ; }                  
  Selected::Range child4 = daughters ( items [3].name() ) ;
  if ( child4.empty() ) { return ; }                  
  
  /// check  combinatoric 
  double ncomb  = 1.0 * child1.size() * child2.size() ;
  ncomb        *=       child3.size() * child4.size() ;
  problem = problem || tooMuchCombinations ( ncomb , decay ) ;
  if ( problem ) { return  ; } 
  
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
      if ( ! combcut12 ( comb12 ) ) { continue ; } // CONTINUE, next c2 
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
        if ( ! combcut123 ( comb123 ) ) { continue ; } // CONTINUE, next c3
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
          if ( ! combcut ( combination ) ) { 
            continue ; 
          }
          
          // here we can create the mother particle, the vertex, 
          // check them and save in TES 
          LHCb::Vertex*     vertex = new LHCb::Vertex();
          LHCb::Particle*   mother = new LHCb::Particle();
          mother->setParticleID( decay.mother().pid() );
          
          // do combination
          combiner -> combine ( combination , *mother , *vertex ) ;
          
          // apply mother cuts
          if ( ! mothcut ( mother ) ) {
            delete vertex;
            delete mother;
            //std::cout << " -- " << i << " fail MOTH cut" << std::endl;
            continue ;
          }
          
          // store in TES
          _storeParticle( mother ) ;

          // add new candidate
          Hlt::Candidate *candidate = newCandidate();
          candidate->addToWorkers ( alg() ) ;
          // add new stage
          Hlt::Stage* stage = newStage () ;
          candidate->addToStages( stage ) ;
          Hlt::Stage::Lock lock { stage, alg() } ;
          stage->set( mother ) ;
          output.push_back ( candidate ) ;
          
          // increment number of good decays
          ++nGood; 

          // ================================================================
        }                                // end of the loop over 4th particle 
        // ==================================================================
      }                                  // end of the loop over 3rd particle 
      // ====================================================================
    }                                    // end of the loop over 2nd particle 
    // ======================================================================
  }                                      // end of the loop over 1st particle
  // ========================================================================
  return;
}
// ============================================================================
// the only one important method
// ============================================================================
LoKi::Hlt1::Hlt1Combiner::result_type
LoKi::Hlt1::Hlt1Combiner::operator()
  ( /*LoKi::Hlt1::Hlt1Combiner::argument a*/ ) const
{
  Assert ( alg() , "Invalid setup!" );

  typedef result_type                CANDIDATES ;
  const CANDIDATES a = source() () ;
  if ( a.empty() ) { return result_type () ; } // no action for EMPTY input

  // the output selection
  CANDIDATES output;

  const std::vector<Decays::Decay> decays = m_conf.decays();

  for ( std::vector<Decays::Decay>::const_iterator idecay = decays.begin() ; idecay != decays.end() ; idecay ++ ) {
    
    const unsigned short N = idecay->children().size();

    if ( N == 3 ) {
      execute3BodyCombination(output, *idecay);
    }
    else if ( N == 4 ) {
      execute4BodyCombination(output, *idecay);
    }
    else { // fine for 2 body (not at all optimal for N>4 body)
      executeCombineParticles(output, *idecay);
    }
  
  }
  /*
  //std::cout << "Daughters:" << std::endl;
  for ( Selected::map_iterator idaught = daughters.begin(); idaught != daughters.end(); idaught++){
    //std::cout << idaught->first << " : " << idaught->second.size() << std::endl;
    //for ( std::vector<LHCb::Particles>::const_iterator ipart = idaught->second.begin(); ipart != idaught->second.end(); ++ipart) {
    //  const LHCb::Particle* part = *ipart1;
    //  std::cout << "\t" << "PID: " << part->particleID().pid() << " P: " << part->p() << " PT: " << part->pt() << std::endl;
    //}
  }
  */
  
  /*
  // the output selection
  CANDIDATES output;

  // the combination tools
  typedef LoKi::Combiner_<LHCb::Particle::ConstVector> Combiner ;
  const IParticleCombiner* combiner = pc() ;
  const LoKi::Particles::PidCompare compare = LoKi::Particles::PidCompare () ;

  // loop over all decays
  //std::cout << "The decay string is: " << std::endl;
  //std::cout << m_conf.decay() << std::endl;
  const std::vector<Decays::Decay> decays = m_conf.decays();
  for ( std::vector<Decays::Decay>::const_iterator idecay = decays.begin();
        idecay != decays.end(); idecay++)
  {

    //std::cout << "In decay loop for decay: " << std::endl;
    //std::cout << idecay->toString() << std::endl;
    //idecay->fillStream(std::cout); std::cout << std::endl;

    Combiner loop ;
    const Decays::Decay::Items& dec_items = idecay->children() ;
    for ( Decays::Decay::Items::const_iterator child = dec_items.begin() ;
          child != dec_items.end(); child++)
    {
      //std::cout << "Added daughters of type " << child->name() << " to loop" << std::endl;
      //child->fillStream(std::cout); std::cout << std::endl;
      loop.add( daughters ( child->name() ) ) ;
    }

    // now do actual loop of combinations
    //std::cout << "Will run " << loop.size() << " combinations " << std::endl;
    int i=-1;
    for ( ; loop.valid() ; loop.next() )
    {
        i++;
        if ( !loop.unique( compare ) ) {
          //std::cout << "  -- " << i << " fail PID compare" << std::endl;
          continue ;
        }

        // get current combination
        LHCb::Particle::ConstVector combination ( loop.dim() );
        loop.current ( combination.begin() ) ;

        // apply comb cuts
        if ( ! combcut ( combination ) ) {
          //std::cout << " -- " << i << " fail COMB cut" << std::endl;
          continue ;
        }

        LHCb::Vertex*     vertex = new LHCb::Vertex();
        LHCb::Particle*   mother = new LHCb::Particle();
        mother->setParticleID( idecay->mother().pid() );

        // do combination
        combiner -> combine ( combination , *mother , *vertex ) ;

        //std::cout << i << ": Created mother particle: " << LoKi::Particles::nameFromPID(mother->particleID()) << " PID: " << mother->particleID().pid() << " M: " << mother->momentum().M() << " P: " << mother->p() << " PT: " << mother->pt() << std::endl;
        //std::cout << i << ": Used primary vertex: (" << vertex->position().X() << "," << vertex->position().Y() << "," << vertex->position().Z() << ")" << std::endl;

        // apply mother cuts
        if ( ! mothcut ( mother ) ) {
          delete vertex;
          delete mother;
          //std::cout << " -- " << i << " fail MOTH cut" << std::endl;
          continue ;
        }
        //std::cout << " -- PASS" << std::endl;

        // store in TES
        _storeParticle( mother ) ;

        // add new candidate
        Hlt::Candidate *candidate = newCandidate();
        candidate->addToWorkers ( alg() ) ;
        // add new stage
        Hlt::Stage* stage = newStage () ;
        candidate->addToStages( stage ) ;
        Hlt::Stage::Lock lock { stage, alg() } ;
        stage->set( mother ) ;
        output.push_back ( candidate ) ;
        // ====================================================================
      } //                                         end loop over combinations
    // ========================================================================
    } //                                           end of loop over decays
  // ==========================================================================
  */
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
      << "'" << location () << "')" ;
}
