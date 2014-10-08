// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/SystemOfUnits.h"

// from FastJet
#include "fastjet/ClusterSequence.hh"
#include "fastjet/config.h"

// from LoKi
#include "LoKi/GenParticleCuts.h"
#include "LoKi/IGenHybridFactory.h"

// local
#include "InvariantMassQQMCJets.h"

// from LHCb 
#include "Kernel/ParticleID.h"

// from HepMC
#include "HepMC/GenParticle.h"

// from ROOT
#include "Math/VectorUtil.h"
#include "Math/Vector3D.h"

//-----------------------------------------------------------------------------
// Implementation file for class : InvariantMassQQMCJets
//
// 2013-10-29 : Xabier Cid Vidal
//-----------------------------------------------------------------------------


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================

LoKi::InvariantMassQQMCJets::InvariantMassQQMCJets( const std::string& type,
                                                    const std::string& name,
                                                    const IInterface* parent )
: LoKi::FilterTool ( type , name , parent )
  , m_cutval   ( _BOOLC ( false )  ) 

{
  //
  declareInterface<IFullGenEventCutTool> ( this ) ;
  //
  addToPreambulo ( "from LoKiGen.decorators     import *" ) ;
  addToPreambulo ( "from LoKiNumbers.decorators import *" ) ;
  addToPreambulo ( "from LoKiCore.functions     import *" ) ;
  addToPreambulo ( "from LoKiCore.math          import *" ) ;
  //
  StatusCode sc = setProperty ( "Factory" , "LoKi::Hybrid::GenTool/GenFactory") ;
  Assert ( sc.isSuccess() , "Unable to reset 'Factory' property" , sc ) ;

  declareProperty( "MCJetMinPT" , m_jetptmin = 5 * Gaudi::Units::GeV );
  declareProperty( "MinMassqq" , m_minmassqq = 30 * Gaudi::Units::GeV ) ;
  declareProperty( "MaxMassqq" , m_maxmassqq = 300 * Gaudi::Units::GeV ) ;
  declareProperty( "QuarkOneID" , m_quarkOnePDG = 5 );
  declareProperty( "QuarkTwoID" , m_quarkTwoPDG = -5 );
  declareProperty( "MCJetSize" , m_MCJetSize = 0.5 );
  declareProperty( "DeltaRHadMCJet" , m_DeltaRHadMCJet = 0.5 );
  declareProperty( "MCParticleTypes"  , m_particleTypes=true);
}

//=============================================================================
// Destructor
//=============================================================================
LoKi::InvariantMassQQMCJets::~InvariantMassQQMCJets() {} 


// ==============================================================================
// Finalization 
// ==============================================================================
StatusCode LoKi::InvariantMassQQMCJets::finalize () 
{
  // reset the functor 
  m_cutval = _BOOLC ( false )  ;
  // finalize the base class 
  return LoKi::FilterTool::finalize () ;
}

// ============================================================================
// perform the actual decoding of the functor 
// ============================================================================
StatusCode LoKi::InvariantMassQQMCJets::decode() 
{
  //
  typedef LoKi::IGenHybridFactory FACTORY ;
  typedef LoKi::Types::GCutVal    FUNCTOR ;
  //
  m_cutval = _BOOLC ( false )  ;
  return i_decode<FACTORY>( m_cutval ) ;
}


//=============================================================================  
// stupid function to determine the sign of an int
//=============================================================================
int LoKi::InvariantMassQQMCJets::sign(const int n) const
{
  if (n>0) return 1;
  else if (n<0) return -1;
  return 0;
}

//=============================================================================
// Acceptance function
//=============================================================================
bool LoKi::InvariantMassQQMCJets::studyFullEvent( LHCb::HepMCEvents * theEvents  ,
                                            LHCb::GenCollisions * /* theCollision */ ) const {
  
  debug()<<" LoKi::InvariantMassQQMCJets: applying LoKi cut"<< endmsg;
  counter( "#all_events" ) += 1 ;
  // obtain the Event from the list of events

  // apply the LoKi cut!
  // first copy all particles into one constainer                                                                   
  LoKi::GenTypes::GenContainer particles ;
  //                                                                                                          
  for ( LHCb::HepMCEvents::const_iterator ievent = theEvents->begin() ;
        theEvents -> end() != ievent ;  ++ievent )
  {
    //                                                                                                        
    const LHCb::HepMCEvent* event = *ievent ;
    if ( 0 == event )                   { continue ; }         // CONTINUE                                    
    //                                                                                                        
    const HepMC::GenEvent*  evt   = event -> pGenEvt () ;
    if ( 0 == evt   )                   { continue ; }         // CONTINUE                                    
    //                                                                                                        
    particles.insert ( particles.end ()          ,
                       evt -> particles_begin () ,
                       evt -> particles_end   () ) ;
  }

  // now evaluate the LoKi functor:                                                                                 
  if (! m_cutval (particles)  ) return false;
  counter( "#passes_LoKi" ) += 1 ;
  debug()<<" LoKi::InvariantMassQQMCJets: applying Inv Mass cut"<< endmsg;

  const HepMC::GenEvent * theEvent = (*(theEvents->begin()))->pGenEvt() ;
  // obtain list of MCJets from the event
  fastjet::ClusterSequence* clusters = 0;
  std::vector<fastjet::PseudoJet> listOfMCJets = buildMCJets( theEvent, clusters );
  debug()<<" Number of MCJets found is "<< listOfMCJets.size() << endmsg;


  std::vector<JetParticlesPair> MCJetPVector = addParticlesInfo(theEvent, listOfMCJets);
  
  
  // loop on them
  for ( std::vector< JetParticlesPair >::const_iterator
          it1 = MCJetPVector.begin() ;
        it1 !=  MCJetPVector.end() ; 
        ++it1 ){
    for ( std::vector< JetParticlesPair >::const_iterator
            it2 = MCJetPVector.begin() ;
          it2 !=  MCJetPVector.end() ; 
          ++it2 ){
      
      // Check combination (not same quark, has opposite sign)
      if ((it1)==(it2)) continue;
      
      // some debug information
      if (msgLevel(MSG::DEBUG) ){
        std::vector<HepMC::GenParticle*> particles1 = it1->second;
        std::vector<HepMC::GenParticle*> particles2 = it2->second;
        debug()<<"MCjet1 eta "<< (*it1).first.eta()
               <<", phi "<< (*it1).first.phi() 
               <<", pt " << (*it1).first.pt() <<endmsg;
        debug()<< "MCJet1 user_index "  << (*it1).first.user_index() << endmsg;
        debug()<< "MCJet1 associated particles " << endmsg;
        for ( std::vector< HepMC::GenParticle* >::const_iterator 
                it1 = particles1.begin();
              it1 != particles1.end();
              it1++ )
        {
          debug() << (*it1)->pdg_id()<<",";
        }
        debug()<<endmsg;
        debug()<<"MCjet2 eta "<< (*it2).first.eta()
               <<", phi "<< (*it2).first.phi() 
               <<", pt " << (*it2).first.perp() <<endmsg;
        debug()<< "MCJet2 user_index "  << (*it2).first.user_index() << endmsg;
        debug()<< "MCJet2 associated particles "  << endmsg;
        for ( std::vector< HepMC::GenParticle* >::const_iterator 
                it2 = particles2.begin();
              it2 != particles2.end();
              it2++ )
        {
          debug() << (*it2)->pdg_id()<<",";
        }  
      }
      debug() << " \n " << endmsg;
      
      // check combination
      if ( !isCombination( (*it1), (*it2) ) ) continue;
           
      fastjet::PseudoJet jet1 = (*it1).first;
      fastjet::PseudoJet jet2 = (*it2).first;
      
      debug()<<" Combination found, let's check the mass"<< endmsg;
      counter( "#good_combination" ) += 1 ;

      HepMC::FourVector sum;
      // Apply Mass Cut
      sum.setPx( jet1.px() + jet2.px());
      sum.setPy( jet1.py() + jet2.py());
      sum.setPz( jet1.pz() + jet2.pz());
      sum.setE ( jet1.e()  + jet2.e());
      double combMass = sum.m();
      debug()<<" Combination mass is "<< combMass << endmsg;

      if ( combMass >= m_minmassqq && combMass <= m_maxmassqq ){
        debug()<<" Combination passes mass cut "<< endmsg;
        delete clusters;
        counter( "#accept" ) += 1 ;
        return true;
      }
    }
  }
  // no combination found
  delete clusters;
  return false;
}


//associate each particle to the jet
std::vector<JetParticlesPair> LoKi::InvariantMassQQMCJets::addParticlesInfo(const HepMC::GenEvent *  theEvent,
                                                                  const  std::vector<fastjet::PseudoJet> listOfMCJets) const
{
  std::vector<JetParticlesPair> output;
  for ( std::vector< fastjet::PseudoJet >::const_iterator
          it1 = listOfMCJets.begin() ;
        it1 !=  listOfMCJets.end() ; 
        ++it1 ){
    fastjet::PseudoJet jet = (*it1);
    std::vector<HepMC::GenParticle*> mypdgl;    
    //loop in the event particles
    for ( HepMC::GenEvent::particle_const_iterator
            iParticle = theEvent -> particles_begin() ;
          iParticle !=  theEvent -> particles_end() ; 
          ++iParticle ){
      // get only particles with pT above minimum
      if ( (*iParticle)->momentum().perp() < m_jetptmin ) continue;
      ROOT::Math::XYZVector xyzv1(jet.px(),jet.py(),jet.pz());
      ROOT::Math::XYZVector xyzv2((*iParticle)->momentum().px(),(*iParticle)->momentum().py(),(*iParticle)->momentum().pz());
  
      //if ((*iParticle)->pdg_id()=
      double deltaR = ROOT::Math::VectorUtil::DeltaR(xyzv1,xyzv2);
      if (deltaR < m_DeltaRHadMCJet) mypdgl.push_back((*iParticle));
    }
    
    JetParticlesPair myjetparticlepair(jet,mypdgl);
    output.push_back(myjetparticlepair);
  }
  return output;
}


/// build a list of MC jets from the particles that come from strings
std::vector<fastjet::PseudoJet> LoKi::InvariantMassQQMCJets::buildMCJets(const HepMC::GenEvent *  theEvent, 
                                                                   fastjet::ClusterSequence* clusters ) const {


  debug()<<" Building MCJets"<< endmsg;
  // input container of "particles"
  std::vector<fastjet::PseudoJet> inputs ;

  // create cut (save only stable particles)
  LoKi::Types::GCut cut = LoKi::Cuts::GVALID ;
  // Give some default values to the types if not defined. 
  // This will be default false, since we are dealing with events before evtGen, 
  // so can not constraint ourselves to LHCb stable particles
  if (m_particleTypes){
  // ['photon', 'e-', 'mu-', 'pi+', 'K+', 'p', 'n', 'k0l']
  //create cut to store only the desired stable in the event
    cut = ( LoKi::Cuts::GABSID == 22   ||  
            LoKi::Cuts::GABSID == 11   ||  
            LoKi::Cuts::GABSID == 13   ||  
            LoKi::Cuts::GABSID == 211  ||   
            LoKi::Cuts::GABSID == 321  || 
            LoKi::Cuts::GABSID == 2212 || 
            LoKi::Cuts::GABSID == 2112 || 
            LoKi::Cuts::GABSID == 130);
    cut = (m_no_daughters && cut);
  }
  else cut = m_no_daughters;
  
  bool is_from_b = false;
  int my_check_counter1 = 0;
  int my_check_counter2 = 0;
  //fill the inputs list
  for ( HepMC::GenEvent::particle_const_iterator
          iParticle = theEvent -> particles_begin() ;
        iParticle !=  theEvent -> particles_end() ; 
        ++iParticle ){
    
    HepMC::GenParticle* particle = (*iParticle);
    if (msgLevel(MSG::DEBUG) ){
      LHCb::ParticleID myParticleID(particle->pdg_id());
      if (myParticleID.isHadron() && myParticleID.hasBottom() )
      {
        debug()<<"Bottom hadron found in this event"
               <<", pdg_id "<< particle->pdg_id()
               <<", eta "<< particle->momentum().eta()
               <<", phi "<< particle->momentum().phi()
               <<", pt "<< particle->momentum().perp() <<endmsg;
      }
      is_from_b = m_cut_from_b(particle);
      if ( is_from_b ) my_check_counter1 +=1;
    }

    // apply cut
    if ( !cut(particle) ) continue;
    
    verbose()<< "stable particle pdg_id "<< particle->pdg_id()<<endmsg;
    if ( msgLevel(MSG::DEBUG) && is_from_b ) my_check_counter2 +=1;
    
    fastjet::PseudoJet  jet ( particle-> momentum().px(),particle-> momentum().py(),
                              particle-> momentum().pz(),particle-> momentum().e());
    if (msgLevel(MSG::DEBUG) ) jet.set_user_index ( is_from_b ) ;
    inputs.push_back(jet);
    
  }

  if (msgLevel(MSG::DEBUG))
  {
    debug()<<" Total number of particles from b in the event is "<< my_check_counter1 << endmsg;
    debug()<<" Total number of input particles for fastjet is "<< inputs.size() << endmsg;
    debug()<<" Total number of input particles for fastjet from b is "<< my_check_counter2 << endmsg;
  }
  
  //configure jets (standard anti-KT)
  fastjet::JetFinder           finder   = (fastjet::JetFinder) 2  ;
  fastjet::RecombinationScheme scheme   = (fastjet::RecombinationScheme) 1 ;
  fastjet::Strategy            strategy = (fastjet::Strategy)  0;
  fastjet::JetDefinition       jet_def( finder   , m_MCJetSize      , scheme   , strategy ) ;

  //build jets
  clusters = new fastjet::ClusterSequence(inputs , jet_def ) ;

  // convert them in to the output format!
  std::vector<fastjet::PseudoJet> jets = sorted_by_pt (clusters->inclusive_jets ( m_jetptmin ) ) ;
  //delete clusters ;
  return jets;
}


/// Check if quark is in particle
bool LoKi::InvariantMassQQMCJets::hasParticleQuark( const HepMC::GenParticle* particle, 
                                              const int quark_id) const
{
  LHCb::ParticleID myParticleID(particle->pdg_id());
  // is the particle a hadron?
  // if we're looking for tops, we can not require a hadron...
  if ( (!myParticleID.isHadron()) && (std::abs(quark_id)!=6) )   return false;
  
  // does this hadron contain the desired quark?
  if ( std::abs(quark_id)==6 && (!myParticleID.hasTop()) ) return false;
  if ( std::abs(quark_id)==5 && (!myParticleID.hasBottom()) ) return false;
  if ( std::abs(quark_id)==4 && (!myParticleID.hasCharm()) ) return false;
  if ( std::abs(quark_id)==3 && (!myParticleID.hasStrange()) ) return false;
  if ( std::abs(quark_id)==2 && (!myParticleID.hasUp()) ) return false;
  if ( std::abs(quark_id)==1 && (!myParticleID.hasDown()) ) return false;
  
  // has the particle the right quark?
  // eg a meson, such as B+ (pid 521), should have a bbar (-5), so opposite sign
  if ( ( myParticleID.isMeson() ) && 
       ( sign(quark_id) == sign(particle->pdg_id()) ) ) return false;
  // but a baryon, such as Lambdab (pid 5122), should have a b (5), so same sign
  if ( ( myParticleID.isBaryon() ) && 
       ( sign(quark_id) != sign(particle->pdg_id()) ) ) return false;

  debug()<< "Particle with pdgID "<< particle->pdg_id() 
         << " has inside quark " << quark_id << endmsg;
  
  return true;  
}



// check if the combination of two MCjets is allowed (have b and antib related)
bool LoKi::InvariantMassQQMCJets::isCombination( const JetParticlesPair MCJetP1,
                                           const JetParticlesPair MCJetP2) const
{

  // cross check
  if ( &(MCJetP1) == &(MCJetP2)) return false ;
  
  std::vector<HepMC::GenParticle*> particles1 = MCJetP1.second;
  std::vector<HepMC::GenParticle*> particles2 = MCJetP2.second;

  // has jet1 q1 or q2
  bool has1q1 = false;
  bool has1q2 = false;
  for ( std::vector< HepMC::GenParticle* >::const_iterator
          it1 = particles1.begin() ;
        it1 !=  particles1.end() ; 
        ++it1 ){
    if (hasParticleQuark( (*it1),m_quarkOnePDG )) has1q1 = true;
    if (hasParticleQuark( (*it1),m_quarkTwoPDG )) has1q2 = true;
  }

  // has jet2 q1 or q2 (check if there is a hadron with the searched quark!)
  bool has2q1 = false;
  bool has2q2 = false;
  for ( std::vector< HepMC::GenParticle* >::const_iterator
          it2 = particles2.begin() ;
        it2 !=  particles2.end() ; 
        ++it2 ){
    if (hasParticleQuark( (*it2),m_quarkOnePDG )) has2q1 = true;
    if (hasParticleQuark( (*it2),m_quarkTwoPDG )) has2q2 = true;
  }
  
  // check if q1 and q2 or q2 and q1 (check if there is a hadron with the searched quark!)
  if (has1q1 && has2q2) return true;
  if (has1q2 && has2q1) return true;
  return false ;
}


// ============================================================================
// factory
// ============================================================================
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi,InvariantMassQQMCJets)
// ============================================================================
// The END 
// ============================================================================
