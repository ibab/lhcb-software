// $Id: LokiMuonTag.cpp,v 1.1 2011-06-24 10:10:10 aphan Exp $

// ============================================================================
// Includes

// Local
#include "LoKiMuonTag.h"


#include "LoKi/PhysTypes.h"
#include "LoKi/ParticleCuts.h"


// ============================================================================
// Declaration of the Tool Factory
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi,MuonTag)


// ============================================================================
// Standard Constructor
LoKi::MuonTag::MuonTag
(const std::string& type, const std::string& name, const IInterface* parent)
  : GaudiTool(type, name, parent)
  , m_Jet(0)
  , m_JetVertex(0)
  , m_Muons(0)
  , m_distCalc(0)
{
  // Interface
  declareInterface<IJetTagTool>(this);

  // Algorithm related properties
  declareProperty("AlgorithmMode",
                  m_AlgMode = "PtRel",
                  "Algorithm mode, either PtRel (default) or IPSig");

  // Muon selections
  declareProperty("MuonCutDR",
                  m_MuonCutDR = 1.5,
                  "Muon Selection on Maximum Delta R between Jet and Muon");

  declareProperty("MuonCutPT",
                  m_MuonCutPT = 400,
                  "Muon Selection on Minimum PT");

  declareProperty("MuonCutPTRel",
                  m_MuonCutPTRel = 0.0,
                  "Muon Selection on Minimum PtRel");

  declareProperty("MuonTrackCutChi",
                  m_MuonTrackCutChi = 0.01,
                  "Muon Selection on Minimum Chi Squared Probability");

  declareProperty("MuonTrackCutIPSig",
                  m_MuonTrackCutIPSig = 9.,
                  "Muon Selection on Minimum Signed IP Significance");

  declareProperty("MuonTrackCutGhost",
                  m_MuonTrackCutGhost = 0.1,
                  "Muon Selection on Maximum Ghost Probability");

  return;
}


// ============================================================================
// Standard Destructor
LoKi::MuonTag::~MuonTag()
{
  return;
}


// ============================================================================
// Standard Initialization
StatusCode LoKi::MuonTag::initialize()
{
  if(msgLevel(MSG::VERBOSE))
    verbose() << "Initialising algorithm" << endmsg;

  StatusCode sc = GaudiTool::initialize();
  if(sc.isFailure())
    return sc;

  svc<LoKi::ILoKiSvc>("LoKiSvc", true);

  m_Muons = new LHCb::Particles();

  m_distCalc = tool<IDistanceCalculator>("LoKi::DistanceCalculator",this);

  return sc;
}


// ============================================================================
// Standard Finalization
StatusCode LoKi::MuonTag::finalize()
{
  if(msgLevel(MSG::VERBOSE))
    verbose() << "Finalising algorithm" << endmsg;

  return GaudiTool::finalize();
}


// ============================================================================
// Get Muons
StatusCode LoKi::MuonTag::getMuons()
{
  // Get muon candidates
  LHCb::Particle::ConstVector inputMuons;

  // Add jet daughters
  inputMuons.insert(inputMuons.begin(),m_JetInputs.begin(),m_JetInputs.end());

  if(msgLevel(MSG::DEBUG))
    debug() << inputMuons.size() << " muon candidates found" << endmsg;

  m_Muons->clear();

  // Apply muon selections (other than pt and ipsig)
  for(LHCb::Particle::ConstVector::const_iterator iMuon = inputMuons.begin();
      iMuon != inputMuons.end();
      ++iMuon)
  {
    const LHCb::Particle *particle = *iMuon;

    if (particle == NULL )      continue;

    const LHCb::ProtoParticle* protoparticle = particle->proto();


    if (protoparticle == NULL )      continue;

    // Check PID
    const LHCb::MuonPID *muonPID = protoparticle->muonPID();
    bool isMuon = false;
    if(muonPID)
    {
      isMuon = muonPID->IsMuon();
    }

    if(!isMuon)
    {
      if(msgLevel(MSG::DEBUG))
        debug() << "Muon candidate failed isMuon selection" << endmsg;

      continue;
    }

    // Check DeltaR
    LoKi::Types::Fun fdeltaR = LoKi::Cuts::DR2(m_Jet->momentum());
    double deltaR = fdeltaR(particle);
    if(deltaR > m_MuonCutDR)
    {
      if(msgLevel(MSG::DEBUG))
        debug() << "Muon candidate failed dR selection " << deltaR << endmsg;

      continue;
    }

    // Check Track Properties
    const LHCb::Track* track = protoparticle->track();

    // Chi Squared Probability
    double chiSquared =  track->probChi2();
    if(chiSquared < m_MuonTrackCutChi)
    {
      if(msgLevel(MSG::DEBUG))
        debug() << "Muon candidate failed chi selection " << chiSquared << endmsg;

      continue;
    }

    // Ghost Probability
    double ghost = track->ghostProbability();
    if(ghost > m_MuonTrackCutGhost)
    {
      if(msgLevel(MSG::DEBUG))
        debug() << "Muon candidate failed ghost selection " << ghost << endmsg;

      continue;
    }

    // If candidate passed all the criteria, assume it is a muon
    m_Muons->insert(new LHCb::Particle(*particle));
  }

  if(msgLevel(MSG::DEBUG))
    debug() << m_Muons->size() << " good muons found" << endmsg;

  return StatusCode::SUCCESS;
}


// ===========================================================================
// Set Jet Vertex
StatusCode LoKi::MuonTag::setJetVertex()
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
bool LoKi::MuonTag::calculateJetProperty
(const LHCb::Particle *jet,  std::map <std::string,double > &jetWeight)
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

  // Get good muons associated with the jet
  sc = getMuons();

  // Return the highest jet tag weight
  double bestIPSig      = -1;
  double bestIPSigPtRel = -1;
  double bestPtRel      = -1;
  double bestPtRelIPSig = -1;
  double bestPtRelforIPSig16      = -1;

  for(LHCb::Particles::const_iterator iMuon = m_Muons->begin();
      iMuon != m_Muons->end();
      ++iMuon)
  {
    const LHCb::Particle *muon = *iMuon;

    // Calculate muon related quantities
    double pt = muon->pt();

    double ipDist, ipSig;
    m_distCalc->distance(muon, m_Jet, ipDist, ipSig);
    ipSig = std::sqrt(ipSig);

    Gaudi::XYZVector ipSeg;
    LHCb::VertexBase jetVertex = (LHCb::VertexBase) m_JetVertex;
    m_distCalc->distance(muon, &jetVertex, ipSeg);

    Gaudi::LorentzVector jetMomentum = m_Jet->momentum();
    Gaudi::XYZVector jetSeg(jetMomentum.px(),jetMomentum.py(),jetMomentum.pz());
    double product = ipSeg.Dot(jetSeg);
    int ipSign = product > 0 ? 1: -1;

    double ipSignSig = ipSign*ipSig;

    // double ptrel = muon->hlv().perp((jet.hlv()+muon->hlv()).vect());
    // double ptRel = m_Muon->momentum().Perp2( (m_Jet->momentum() + m_Muon->momentum()) )
    //   atlas calculation can't be performed in version of ROOT vectors in DaVinci

    Gaudi::XYZVector muonVector = muon->momentum().Vect();
    Gaudi::XYZVector jetVector = m_Jet->momentum().Vect();

    //   copying code from Perp2 from ROOT
    double tot = jetVector.Mag2();
    double ss = muonVector.Dot(jetVector);
    double per = muonVector.Mag2();
    if(tot > 0.0) per -= ss*ss/tot;
    if(per < 0)   per = 0;

    double ptRel = std::sqrt(per);
    if(msgLevel(MSG::DEBUG))
      debug() << "Pt: " << pt
              << "PtRel: " << ptRel
              << "IPSig: " << ipSignSig
              << endmsg;

    if(ptRel > bestPtRel)
    {
      bestPtRel = ptRel;
      bestPtRelIPSig = ipSignSig;
    }

    if(ipSignSig > bestIPSig)
    {
      bestIPSig = ipSignSig;
      bestIPSigPtRel = ptRel;
    }
    if(ptRel > bestPtRelforIPSig16 && ipSignSig > m_MuonTrackCutIPSig)
      bestPtRelforIPSig16 = ptRel;


  }
  jetWeight["bestIPSig"]      = bestIPSig;
  jetWeight["bestIPSigPtRel"] = bestIPSigPtRel;
  jetWeight["bestPtRel"]      = bestPtRel;
  jetWeight["bestPtRelIPSig"] = bestPtRelIPSig;
  jetWeight["bestPtRelforIPSigCut"] = bestPtRelforIPSig16;


  double tag_global = 0;

  if (bestPtRel >  m_MuonCutPTRel)
    tag_global =1;
  //IT HAS TO BEHTE LAST ONE: GLOBAL DECISTION

  jetWeight["Tag"] = tag_global;


  jetWeight["extraInfo"] = 5400;

  if(msgLevel(MSG::DEBUG)) debug() << "Tag weight: " << jetWeight << endmsg;
  if(tag_global)
    return true;
  else
    return false;
}
