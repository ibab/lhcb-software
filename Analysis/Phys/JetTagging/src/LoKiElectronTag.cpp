// $Id: LokiMuonmTag.cpp,v 1.1 2011-06-24 10:10:10 aphan Exp $

// ============================================================================
// Includes

// Local
#include "LoKiElectronTag.h"


#include "LoKi/PhysTypes.h"
#include "LoKi/ParticleCuts.h"


// ============================================================================
// Declaration of the Tool Factory
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi,ElectronTag)


// ============================================================================
// Standard Constructor
LoKi::ElectronTag::ElectronTag
(const std::string& type, const std::string& name, const IInterface* parent)
  : GaudiTool(type, name, parent)
  , m_Jet(0)
  , m_JetVertex(0)
  , m_Electrons(0)
  , m_distCalc(0)
  , m_p2mcAssoc(0)
  , m_p2mcAssocType("DaVinciSmartAssociator")
{
  // Interface
  declareInterface<IJetTagTool>(this);

  // Algorithm related properties
  declareProperty("AlgorithmMode",
                  m_AlgMode = "PtRel",
                  "Algorithm mode, either PtRel (default) or IPSig");

  // Electron selections
  declareProperty("ElectronCutDR",
                  m_ElectronCutDR = 0.3,
                  "Electron Selection on Maximum Delta R between Jet and Electron");

  declareProperty("ElectronCutPT",
                  m_ElectronCutPT = 400,
                  "Electron Selection on Minimum PT");

  declareProperty("ElectronCutPTRel",
                  m_ElectronCutPTRel = 400.,
                  "Electron Selection on Minimum PtRel");

  declareProperty("ElectronTrackCutChi",
                  m_ElectronTrackCutChi = 0.01,
                  "Electron Selection on Minimum Chi Squared Probability");

  declareProperty("ElectronTrackCutIP",
                  m_ElectronTrackCutIP = 9.,
                  "Electron Selection on Minimum Signed IP Significance");

  declareProperty("ElectronTrackCutGhost",
                  m_ElectronTrackCutGhost = 0.1,
                  "Electron Selection on Maximum Ghost Probability");

  return;
}


// ============================================================================
// Standard Destructor
LoKi::ElectronTag::~ElectronTag()
{
  return;
}


// ============================================================================
// Standard Initialization
StatusCode LoKi::ElectronTag::initialize()
{
  if(msgLevel(MSG::VERBOSE))
    verbose() << "Initialising algorithm" << endmsg;

  StatusCode sc = GaudiTool::initialize();
  if(sc.isFailure())
    return sc;

  svc<LoKi::ILoKiSvc>("LoKiSvc", true);

  m_Electrons = new LHCb::Particles();

  m_distCalc = tool<IDistanceCalculator>("LoKi::DistanceCalculator",this);

  return sc;
}


// ============================================================================
// Standard Finalization
StatusCode LoKi::ElectronTag::finalize()
{
  if(msgLevel(MSG::VERBOSE))
    verbose() << "Finalising algorithm" << endmsg;

  return GaudiTool::finalize();
}


// ============================================================================
// Get Electrons
StatusCode LoKi::ElectronTag::getElectrons()
{
  // Get electron candidates
  LHCb::Particle::ConstVector inputElectrons;

  // Add jet daughters
  inputElectrons.insert(inputElectrons.begin(),m_JetInputs.begin(),m_JetInputs.end());

  if(msgLevel(MSG::DEBUG))
    debug() << inputElectrons.size() << " electron candidates found" << endmsg;

  m_Electrons->clear();

  // Apply electron selections (other than pt and ipsig)
  for(LHCb::Particle::ConstVector::const_iterator iElectron = inputElectrons.begin();
      iElectron != inputElectrons.end();
      ++iElectron)
  {
    const LHCb::Particle *particle = *iElectron;

    if (particle == NULL )      continue;

    const LHCb::ProtoParticle* protoparticle = particle->proto();


    if (protoparticle == NULL )      continue;

    // Check PID
    int electronPID = particle->particleID().abspid();
    bool isElectron = false;
    if(electronPID == 11)
    {
      isElectron = true;
    }

    if(!isElectron)
    {
      if(msgLevel(MSG::DEBUG))
        debug() << "Electron candidate failed isElectron selection" << endmsg;

      continue;
    }

    // Check DeltaR
    LoKi::Types::Fun fdeltaR = LoKi::Cuts::DR2(m_Jet->momentum());
    double deltaR = fdeltaR(particle);
    if(deltaR > m_ElectronCutDR)
    {
      if(msgLevel(MSG::DEBUG))
        debug() << "Electron candidate failed dR selection " << deltaR << endmsg;

      continue;
    }

    // Check Track Properties
    const LHCb::Track* track = protoparticle->track();

    // Chi Squared Probability
    double chiSquared =  track->probChi2();
    if(chiSquared < m_ElectronTrackCutChi)
    {
      if(msgLevel(MSG::DEBUG))
        debug() << "Electron candidate failed chi selection " << chiSquared << endmsg;

      continue;
    }

    // Ghost Probability
    double ghost = track->ghostProbability();
    if(ghost > m_ElectronTrackCutGhost)
    {
      if(msgLevel(MSG::DEBUG))
        debug() << "Electron candidate failed ghost selection " << ghost << endmsg;

      continue;
    }

    // If candidate passed all the criteria, assume it is a electron
    m_Electrons->insert(new LHCb::Particle(*particle));
  }

  if(msgLevel(MSG::DEBUG))
    debug() << m_Electrons->size() << " good electrons found" << endmsg;

  return StatusCode::SUCCESS;
}


// ===========================================================================
// Set Jet Vertex
StatusCode LoKi::ElectronTag::setJetVertex()
{
  // If jet has a vertex
  //  use that
  if(m_Jet->referencePoint() != Gaudi::XYZPoint(0,0,0))
  {
    m_JetVertex.setPosition(m_Jet->referencePoint());
    return StatusCode::SUCCESS;
  }

  // Otherwise return false
  return StatusCode::FAILURE;
}


// ===========================================================================
// Calculate Tag Weight
bool LoKi::ElectronTag::calculateJetProperty
(const LHCb::Particle *jet,    std::map < std::string,double > &jetWeight)
{
  StatusCode sc = StatusCode::SUCCESS;
  jetWeight.clear();

  // Set member variables
  m_Jet = jet;
  m_JetInputs = jet->daughtersVector();

  if(exist<LHCb::RecVertex::Container>(LHCb::RecVertexLocation::Primary)){
    LHCb::RecVertex::Container* vertices =
      get<LHCb::RecVertex::Container>(LHCb::RecVertexLocation::Primary);

    if (vertices->size() == 0) return sc;
  }

  // Set jet vertex
  sc = setJetVertex();
  if(sc.isFailure())
  {
    err() << "Unable to set jet vertex" << endmsg;
    return StatusCode::SUCCESS;
  }

  // Get good electrons associated with the jet
  sc = getElectrons();

  // Return the highest jet tag weight
  double bestIPSig = -1;
  double bestPtRel = -1;
  double bestPt = -1;

  for(LHCb::Particles::const_iterator iElectron = m_Electrons->begin();
      iElectron != m_Electrons->end();
      ++iElectron)
  {
    const LHCb::Particle *electron = *iElectron;

    // Calculate electron related quantities
    double pt = electron->pt();

    double ipDist, ipSig;
    m_distCalc->distance(electron, m_Jet, ipDist, ipSig);
    ipSig = std::sqrt(ipSig);

    Gaudi::XYZVector ipSeg;
    LHCb::VertexBase jetVertex = (LHCb::VertexBase) m_JetVertex;
    m_distCalc->distance(electron, &jetVertex, ipSeg);

    Gaudi::LorentzVector jetMomentum = m_Jet->momentum();
    Gaudi::XYZVector jetSeg(jetMomentum.px(),jetMomentum.py(),jetMomentum.pz());
    double product = ipSeg.Dot(jetSeg);
    int ipSign = product > 0 ? 1: -1;

    double ipSignSig = ipSign*ipSig;

    // double ptrel = electron->hlv().perp((jet.hlv()+electron->hlv()).vect());
    // double ptRel = m_Electron->momentum().Perp2( (m_Jet->momentum() + m_Electron->momentum()) )
    //   atlas calculation can't be performed in version of ROOT vectors in DaVinci

    Gaudi::XYZVector electronVector = electron->momentum().Vect();
    Gaudi::XYZVector jetVector = m_Jet->momentum().Vect();

    //   copying code from Perp2 from ROOT
    double tot = jetVector.Mag2();
    double ss = electronVector.Dot(jetVector);
    double per = electronVector.Mag2();
    if(tot > 0.0) per -= ss*ss/tot;
    if(per < 0)   per = 0;

    double ptRel = std::sqrt(per);
    if(msgLevel(MSG::DEBUG))
      debug() << "Pt: " << pt
              << " PtRel: " << ptRel
              << " IPSig: " << ipSignSig
              << endmsg;

    if(ptRel > bestPtRel)
    {
      bestPtRel = ptRel;
      bestPt =  electron->momentum().Pt();
    }

    if(ipSignSig > bestIPSig)
    {
      bestIPSig = ipSignSig;
    }
  }

  jetWeight["IPSig"] = bestIPSig;
  jetWeight["bestPtRel"] = bestPtRel;
  jetWeight["bestPt"] = bestPt;


  double tag_global = 0;

  if (bestPtRel >  m_ElectronCutPTRel)
    tag_global =1;
  //IT HAS TO BEHTE LAST ONE: GLOBAL DECISTION

  jetWeight["Tag"] = tag_global;

  jetWeight["extraInfo"] = 5500;

  if(msgLevel(MSG::DEBUG)) debug() << "Tag weight: " << jetWeight << endmsg;


  if( tag_global )
    return true;
  else
    return false;
}
