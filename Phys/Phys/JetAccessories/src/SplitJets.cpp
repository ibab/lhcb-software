#ifndef SPLITJETS_H 
#define SPLITJETS_H 1

// Include files
// from DaVinci.
#include "Kernel/IParticleCombiner.h"
#include "Kernel/DaVinciAlgorithm.h"

#include "LoKi/Geometry.h"

/** @class SplitJets SplitJets.h
 *
 *
 *  @author Victor Coco
 *  @date   2013-05-22
 */
class SplitJets : public DaVinciAlgorithm {
public:
  /// Standard constructor
  SplitJets( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~SplitJets( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:
  // proposed jet ID
  int    m_jetID     ; ///< proposed jet ID
  // proposed pseudo jet ID
  int    m_pseudoJetID    ; ///< proposed pseudo jet ID
  // jet input location
  std::string    m_inputLocation   ; ///< jet input location
  // combiner
  std::string                m_combinerName ;
  const IParticleCombiner* m_combiner ; ///< combiner to be used
  //re set the extrainfo keys of the jet and its corrected momentum
  bool  m_reSetMomentumAndKeys;

};
#endif // SPLITJETS_H

//-----------------------------------------------------------------------------
// Implementation file for class : SplitJets
//
// 2013-05-22 : Victor Coco
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( SplitJets )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SplitJets::SplitJets( const std::string& name,
                      ISvcLocator* pSvcLocator)
: DaVinciAlgorithm ( name , pSvcLocator )
  , m_jetID        ( 24  )
  , m_pseudoJetID  ( 421  )
  , m_combiner     ( 0   )
  , m_reSetMomentumAndKeys  (true)
{
  declareProperty
    ( "JetID"          ,
      m_jetID          ,
      "Particle ID for the Jet") ;
  declareProperty
    ( "PseudoJetID"          ,
      m_pseudoJetID        ,
      "Particle ID for the PseudoJet") ;
  declareProperty
    ( "reSetMomentumAndKeys"          ,
      m_reSetMomentumAndKeys  ,
      "re set the extrainfo keys of the jet and its corrected momentum") ;
}
//=============================================================================
// Destructor
//=============================================================================
SplitJets::~SplitJets() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode SplitJets::initialize()
{
  const StatusCode sc = DaVinciAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  if ( 0 == m_combiner ){ m_combiner =  this->particleCombiner("MomentumCombiner") ; }

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode SplitJets::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  //

  LoKi::Point3D    point  = LoKi::Point3D( 0 , 0 , 0 ) ;
  const LHCb::Particle::Range jets = this->particles();

  for (LHCb::Particle::Range::iterator ip = jets.begin();ip != jets.end() ; ip++){
    const LHCb::Particle * p = (*ip);

    LHCb::Particle::ConstVector jetDaughters = p->daughtersVector ();
    LHCb::Particle::ConstVector newDaugs;
    const LHCb::Particle* theB;
    const LHCb::VertexBase * pv = bestVertex (p) ;


    for ( LHCb::Particle::ConstVector::const_iterator idaug = jetDaughters.begin() ; jetDaughters.end() != idaug ; ++idaug )
    {
      if ((*idaug)->particleID().hasBottom())theB = (*idaug);
      else newDaugs.push_back ( (*idaug) ) ;
    }



    if ( newDaugs.empty() )
    {
      Warning ("Empty list of of daughter particles, skip it") ;
      continue ;
    }
    // Create the pseudo jet
    LHCb::Particle              pPseudoJet ;
    LHCb::Vertex                vPseudoJet      ;
    // use the tool
    StatusCode sc = m_combiner->combine ( newDaugs ,  pPseudoJet, vPseudoJet ) ;
    if ( sc.isFailure() )
    {
      Warning ( "Error from momentum combiner, skip", sc, 0 ) ;
      continue ;
    }

    int tempID =  m_pseudoJetID;
    if(theB->particleID().pid() < 0)
      tempID = -m_pseudoJetID;

    pPseudoJet.setParticleID     (  LHCb::ParticleID( tempID )) ;
    pPseudoJet.setReferencePoint( Gaudi::XYZPoint(pv->position ()) );

    vPseudoJet.setPosition(pv->position());
    vPseudoJet.setCovMatrix(pv->covMatrix());
    vPseudoJet.setChi2(pv->chi2());
    vPseudoJet.setNDoF(pv->nDoF());
    vPseudoJet.setOutgoingParticles(pPseudoJet.daughters());
    pPseudoJet.setEndVertex(vPseudoJet.clone());

    LHCb::Particle* pseudoJet = pPseudoJet.clone();
    this->relate ( pseudoJet , pv );


    // Create the new jet
    LHCb::Particle::ConstVector finalJetDaughters;
    finalJetDaughters.push_back(theB);
    finalJetDaughters.push_back(pseudoJet);

    // Create the jet
    LHCb::Particle              pJet ;
    LHCb::Vertex                vJet      ;
    // use the tool
    sc = m_combiner->combine ( finalJetDaughters ,  pJet, vJet ) ;
    if ( sc.isFailure() )
    {
      Warning ( "Error from momentum combiner, skip", sc, 0 ) ;
      continue ;
    }


    int tempID2 =  m_jetID;
    if(theB->particleID().pid() < 0)
      tempID2 = -m_jetID;

    pJet.setParticleID     (  LHCb::ParticleID( tempID2 )) ;
    pJet.setReferencePoint ( point         ) ;
    pJet.setReferencePoint( Gaudi::XYZPoint(pv->position ()) );

    vJet.setPosition(pv->position());
    vJet.setCovMatrix(pv->covMatrix());
    vJet.setChi2(pv->chi2());
    vJet.setNDoF(pv->nDoF());
    vJet.setOutgoingParticles(pJet.daughters());
    pJet.setEndVertex(vJet.clone());


    if(m_reSetMomentumAndKeys){
      pJet.setMomentum(p->momentum());
      for(int i = 0; i < 10000; i++)
        if(p->hasInfo(i))
          pJet.addInfo(i,p->info(i,-999.0));
    }

    LHCb::Particle* ppJet = pJet.clone();
    this->relate ( ppJet , pv );

    this->markTree( ppJet );

  }

  setFilterPassed(true);  // Mandatory. Set to true if event is accepted.
  return StatusCode::SUCCESS;
}

//=============================================================================
