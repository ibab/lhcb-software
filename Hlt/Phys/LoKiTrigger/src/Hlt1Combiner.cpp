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
//   constructor from decay string, output location, two Sources for daughters,
//   combination cut and mother cut
// ============================================================================
LoKi::Hlt1::Hlt1Combiner::Hlt1Combiner
( const std::string                        output      ,
  const LoKi::Hlt1::Hlt1CombinerConf&      config      )
  : LoKi::AuxFunBase  ( std::tie ( output     , config ) )
  , LoKi::BasicFunctors<const Hlt::Candidate*>::Pipe ()
  , LoKi::Hlt1::HelperTool( 1 )
  , m_sink { std::move(output) }
  , m_conf ( config )
{
  setup();
}
// ============================================================================
// setup function (common call from constructors)
// ============================================================================
StatusCode LoKi::Hlt1::Hlt1Combiner::setup()
{
  // get required tools
  LoKi::ILoKiSvc* svc = lokiSvc() ;
  SmartIF<IToolSvc> tsvc ( svc ) ;
  StatusCode sc = tsvc->retrieveTool("LoKi::FastVertexFitter:PUBLIC", m_pc);
  //sc = tsvc->retrieveTool("DaVinci::DefaultTools::ParticleCombiner", m_pc);
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
  ( LoKi::Hlt1::Hlt1Combiner::Selected& daughters, const LoKi::Hlt1::Hlt1Combiner::result_type* arg1 ) const
{
  typedef result_type                CANDIDATES ;
  typedef CANDIDATES::const_iterator ITERATOR   ;

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
// the only one important method
// ============================================================================
LoKi::Hlt1::Hlt1Combiner::result_type
LoKi::Hlt1::Hlt1Combiner::operator()
  ( LoKi::Hlt1::Hlt1Combiner::argument a ) const
{
  Assert ( alg() , "Invalid setup!" );

  typedef result_type                CANDIDATES ;
  const CANDIDATES* arg1 = &a ;
  if ( a.empty() ) { return result_type () ; } // no action for EMPTY input

  // add other sources with input and create daughters
  Selected daughters;
  getDaughters( daughters, arg1 ) ;

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
// ============================================================================
//   constructor from decay string, output location, two Sources for daughters,
//   combination cut and mother cut
// ============================================================================
LoKi::Hlt1::Hlt1Combiner2::Hlt1Combiner2
( const std::string                        output      ,
  const LoKi::Hlt1::Hlt1CombinerConf&      config      ,
  const Source&                            particles2  )
  : Hlt1Combiner ( std::move(output) , config )
  , m_source2 ( particles2 )
{
  setup();
}
// ============================================================================
//   constructor from decay string, output location, two Sources for daughters,
//   combination cut and mother cut
// ============================================================================
LoKi::Hlt1::Hlt1Combiner2::Hlt1Combiner2
( const std::string                        output      ,
  const LoKi::Hlt1::Hlt1CombinerConf&      config      ,
  const std::string                        particles2  )
  : Hlt1Combiner ( std::move(output) , config )
  , m_source2 { LoKi::Hlt1::Selection{ std::move(particles2) } }
{
  setup();
}
// ============================================================================
// virtual destructor
// ============================================================================
LoKi::Hlt1::Hlt1Combiner2::~Hlt1Combiner2 () {}
// ============================================================================
// clone method ("virtual constructor")
// ============================================================================
LoKi::Hlt1::Hlt1Combiner2* LoKi::Hlt1::Hlt1Combiner2::clone() const
{ return new LoKi::Hlt1::Hlt1Combiner2 { *this } ; }
// ============================================================================
// getDaughters helper function
// ============================================================================
bool
LoKi::Hlt1::Hlt1Combiner2::getDaughters
  ( LoKi::Hlt1::Hlt1Combiner::Selected& daughters, const LoKi::Hlt1::Hlt1Combiner::result_type* arg1 ) const
{
  typedef result_type                CANDIDATES ;
  typedef CANDIDATES::const_iterator ITERATOR   ;

  //std::cout << "Hlt1Combiner2::getDaughters()" << std::endl;

  const CANDIDATES a2 = source2() () ;
  if ( a2.empty() ) { return false; }

  //std::cout << "Hlt1Combiner2::getDaughters() source2 not empty" << std::endl;

  // addtional input selections:
  const CANDIDATES* arg2 = &a2;

  // loop over the selections and add daughters
  // Source 1
  for ( ITERATOR icand1 = arg1->begin(); icand1 != arg1->end(); icand1++ )
  {
    const Hlt::Candidate* cand1 = *icand1 ;
    if ( !cand1 ) { continue ; }
    const LHCb::Particle* particle1 = cand1->get<LHCb::Particle> () ;
    if ( !particle1 ) { continue ; }
    daughters.add ( LoKi::Particles::nameFromPID(particle1->particleID()) , particle1 ) ;
    //std::cout << "getDaughters() -- added " << LoKi::Particles::nameFromPID(particle1->particleID()) << std::endl;
  }
  // Source 2
  for ( ITERATOR icand2 = arg2->begin(); icand2 != arg2->end(); icand2++ )
  {
    const Hlt::Candidate* cand2 = *icand2 ;
    if ( !cand2 ) { continue ; }
    const LHCb::Particle* particle2 = cand2->get<LHCb::Particle> () ;
    if ( !particle2 ) { continue ; }
    daughters.add ( LoKi::Particles::nameFromPID(particle2->particleID()) , particle2 ) ;
    //std::cout << "getDaughters() -- added " << LoKi::Particles::nameFromPID(particle2->particleID()) << std::endl;
  }
  return true ;
}
// ============================================================================
// OPTIONAL: nice printout
// ============================================================================
std::ostream& LoKi::Hlt1::Hlt1Combiner2::fillStream ( std::ostream& s ) const
{
  return
    s << "TC_HLT1COMBINER2("
      << "'" << location () << "')" ;
}
// ============================================================================
//   constructor from decay string, output location, two Sources for daughters,
//   combination cut and mother cut
// ============================================================================
LoKi::Hlt1::Hlt1Combiner3::Hlt1Combiner3
( const std::string                        output      ,
  const LoKi::Hlt1::Hlt1CombinerConf&      config      ,
  const Source&                            particles2  ,
  const Source&                            particles3  )
  : Hlt1Combiner ( std::move(output) , config )
  , m_source2 ( particles2 )
  , m_source3 ( particles3 )
{
  setup();
}
// ============================================================================
//   constructor from decay string, output location, two Sources for daughters,
//   combination cut and mother cut
// ============================================================================
LoKi::Hlt1::Hlt1Combiner3::Hlt1Combiner3
( const std::string                        output      ,
  const LoKi::Hlt1::Hlt1CombinerConf&      config      ,
  const std::string                        particles2  ,
  const std::string                        particles3  )
  : Hlt1Combiner ( std::move(output) , config )
  , m_source2 { LoKi::Hlt1::Selection{ std::move(particles2) } }
  , m_source3 { LoKi::Hlt1::Selection{ std::move(particles3) } }
{
  setup();
}
// ============================================================================
// virtual destructor
// ============================================================================
LoKi::Hlt1::Hlt1Combiner3::~Hlt1Combiner3 () {}
// ============================================================================
// clone method ("virtual constructor")
// ============================================================================
LoKi::Hlt1::Hlt1Combiner3* LoKi::Hlt1::Hlt1Combiner3::clone() const
{ return new LoKi::Hlt1::Hlt1Combiner3 { *this } ; }
// ============================================================================
// getDaughters helper function
// ============================================================================
bool
LoKi::Hlt1::Hlt1Combiner3::getDaughters
  ( LoKi::Hlt1::Hlt1Combiner::Selected& daughters, const LoKi::Hlt1::Hlt1Combiner::result_type* arg1 ) const
{
  typedef result_type                CANDIDATES ;
  typedef CANDIDATES::const_iterator ITERATOR   ;

  const CANDIDATES a2 = source2() () ;
  const CANDIDATES a3 = source3() () ;
  if ( a2.empty() ) { return false; }
  if ( a3.empty() ) { return false; }

  //std::cout << "Hlt1Combiner3::getDaughters() source2 and source 3 not empty" << std::endl;

  // addtional input selections:
  const CANDIDATES* arg2 = &a2;
  const CANDIDATES* arg3 = &a3;

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
  // Source 2
  for ( ITERATOR icand2 = arg2->begin(); icand2 != arg2->end(); icand2++ )
  {
    const Hlt::Candidate* cand2 = *icand2 ;
    if ( !cand2 ) { continue ; }
    const LHCb::Particle* particle2 = cand2->get<LHCb::Particle> () ;
    if ( !particle2 ) { continue ; }
    daughters.add ( LoKi::Particles::nameFromPID(particle2->particleID()) , particle2 ) ;
  }
  // Source 3
  for ( ITERATOR icand3 = arg3->begin(); icand3 != arg3->end(); icand3++ )
  {
    const Hlt::Candidate* cand3 = *icand3 ;
    if ( !cand3 ) { continue ; }
    const LHCb::Particle* particle3 = cand3->get<LHCb::Particle> () ;
    if ( !particle3 ) { continue ; }
    daughters.add ( LoKi::Particles::nameFromPID(particle3->particleID()) , particle3 ) ;
  }
  return true ;
}
// ============================================================================
// OPTIONAL: nice printout
// ============================================================================
std::ostream& LoKi::Hlt1::Hlt1Combiner3::fillStream ( std::ostream& s ) const
{
  return
    s << "TC_HLT1COMBINER3("
      << "'" << location () << "')" ;
}
