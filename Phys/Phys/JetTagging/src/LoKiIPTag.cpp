// $Id: LokiIPTag.cpp,v 1.1 2011-06-24 10:10:10 aphan Exp $

// ============================================================================
// Includes

// Local
#include "LoKiIPTag.h"

// #include "LoKi/Kinematics.h"
// #include "LoKi/Geometry.h"
// #include "LoKi/PhysTypes.h"
// #include "LoKi/ILoKiSvc.h" //  ??? why it is needed here???

// ============================================================================
// Declaration of the Tool Factory
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi,IPTag)


// ============================================================================
// Standard Constructor
LoKi::IPTag::IPTag
(const std::string& type, const std::string& name, const IInterface* parent)
  : GaudiTool(type, name, parent)
  , m_Jet(0)
  , m_JetVertex(0)
  , m_CleanedDaugthers(0)
  , m_distCalc(0)
{
  // Interface
  declareInterface<IJetTagTool>(this);

  // Algorithm related properties
  declareProperty("AlgorithmMode",
                  m_AlgMode = "SumTracks",
                  "Algorithm mode, either SumTracks (default) or FirstTrack or SecondTrack or ThirdTrack");

  declareProperty("JetDaugthersTrackCutChi",
                  m_DauTrackCutChi = 0.01,
                  "JetDaugthers Selection on Minimum Chi Squared Probability");

  declareProperty("JetDaugthersTrackCutGhost",
                  m_DauTrackCutGhost = 0.1,
                  "JetDaugthers Selection on Maximum Ghost Probability");

  declareProperty("V0maxDistTrack",m_DtrakMax = 0.2);
  declareProperty("V0KShortDeltaMass",m_DMK0 = 10);
  declareProperty("V0LambdaDeltaMass",m_DMLambda = 10);
  declareProperty("SecondTrkCutValue",m_secCut = 0.0);
  declareProperty("SumTrkCutValue",m_sumCut = 3.);

  return;
}


// ============================================================================
// Standard Destructor
LoKi::IPTag::~IPTag()
{
  return;
}


// ============================================================================
// Standard Initialization
StatusCode LoKi::IPTag::initialize()
{
  if(msgLevel(MSG::VERBOSE))
    verbose() << "Initialising algorithm" << endmsg;

  StatusCode sc = GaudiTool::initialize();
  if(sc.isFailure())
    return sc;

  // svc<LoKi::ILoKiSvc>("LoKiSvc", true);

  m_CleanedDaugthers = new LHCb::Particles();

  m_distCalc = tool<IDistanceCalculator>("LoKi::DistanceCalculator",this);

  return sc;
}


// ============================================================================
// Standard Finalization
StatusCode LoKi::IPTag::finalize()
{
  if(msgLevel(MSG::VERBOSE))
    verbose() << "Finalising algorithm" << endmsg;

  return GaudiTool::finalize();
}


// ============================================================================
// Get Muons
StatusCode LoKi::IPTag::removeV0()
{
  // Get daughters
  LHCb::Particle::ConstVector inputDaus;

  // Add jet daughters
  inputDaus.insert(inputDaus.begin(),m_JetInputs.begin(),m_JetInputs.end());

  if(msgLevel(MSG::DEBUG))
    debug() << inputDaus.size() << " daughters found in the jet" << endmsg;

  m_CleanedDaugthers->clear();

  std::map< double , double > VOTag;

  // Kshort Mass
  const double MK0 = 497.7;

  // Lambda Mass
  const double MLambda = 1115.;

  // Apply daughter selections
  for(LHCb::Particle::ConstVector::const_iterator iDau = inputDaus.begin();
      iDau != inputDaus.end();
      ++iDau)
    {
      // Identify Kshorts and Lambda
      const LHCb::Particle *particle = *iDau;
      const LHCb::ProtoParticle* protoparticle = particle->proto();
      if(protoparticle == NULL) continue;
      if(VOTag.count(protoparticle->key())>0.5) continue;

      for(LHCb::Particle::ConstVector::const_iterator iDau2 = iDau+1;
	  iDau2 != inputDaus.end();
	  ++iDau2)
	{
	  const LHCb::Particle *particle2 = *iDau2;
	  const LHCb::ProtoParticle* protoparticle2 = particle2->proto();

	  if(protoparticle2 == NULL) continue;
	  if(VOTag.count(protoparticle2->key())>0.5) continue;
	  if(particle->charge()*particle2->charge()!=-1) continue;

	  double dca12, dcae12;

	  if(!m_distCalc->distance(particle, particle2, dca12, dcae12))
	    warning()  << "can't mesure the dist "<< endmsg;

	  if(dca12 > m_DtrakMax) continue; // dca too large bwt the tracks

	  Gaudi::LorentzVector sum = (particle)->momentum() + (particle2)->momentum();

	  if( (std::fabs(sum.M()-MK0)<m_DMK0) ||
	      (std::fabs(sum.M()-MLambda)<m_DMLambda) )
	    {
	      // tag particles K0
	      if(VOTag.count(protoparticle->key())<0.5)
		VOTag[protoparticle->key()]  = 1.;
	      if(VOTag.count(protoparticle2->key())<0.5)
		VOTag[protoparticle2->key()] = 1.;
	    }
	}

      // Remove identified Kshorts and Lambda
      if(VOTag.count(protoparticle->key())>0.5)
	{
	  if(msgLevel(MSG::DEBUG))
	    debug() << "V0 daughter candidate" << endmsg;
	  continue;
	}

      // Check Track Properties
      const LHCb::Track* track = protoparticle->track();
      if(track == NULL) continue;

      // Chi Squared Probability
      double chiSquared =  track->probChi2();
      if(chiSquared < m_DauTrackCutChi)
	{
	  if(msgLevel(MSG::DEBUG))
	    debug() << "Daugther failed chi selection " << chiSquared << endmsg;

	  continue;
	}

      // Ghost Probability
      double ghost = track->ghostProbability();
      if(ghost > m_DauTrackCutGhost)
	{
	  if(msgLevel(MSG::DEBUG))
	    debug() << "Daugther failed ghost selection " << ghost << endmsg;

	  continue;
	}

      // If candidate passed all the criteria include it
      m_CleanedDaugthers->insert(new LHCb::Particle(*particle));
    }

  if(msgLevel(MSG::DEBUG))
    debug() << m_CleanedDaugthers->size() << " good daugthers found" << endmsg;

  return StatusCode::SUCCESS;
}


// ===========================================================================
// Set Jet Vertex
StatusCode LoKi::IPTag::setJetVertex()
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
bool LoKi::IPTag::calculateJetProperty
(const LHCb::Particle *jet, std::map <std::string,double > &jetWeight)
{
  StatusCode sc = StatusCode::SUCCESS;

  // Set member variables
  m_Jet = jet;
  m_JetInputs = jet->daughtersVector();
  jetWeight.clear();

  // Set jet vertex
  if(exist<LHCb::RecVertex::Container>(LHCb::RecVertexLocation::Primary)){
    LHCb::RecVertex::Container* vertices =
      get<LHCb::RecVertex::Container>(LHCb::RecVertexLocation::Primary);

    if (vertices->size() == 0) return sc;
  }

  sc = setJetVertex();
  if(sc.isFailure())
    {
      err() << "Unable to set jet vertex" << endmsg;
      return StatusCode::SUCCESS;
    }

  // Get good muons associated with the jet
  sc = removeV0();

  double SumSignValue = 0;
  double FirstSignValue = -999;
  double SecondSignValue = -999;
  double ThirdSignValue = -999;

  // Calculate jet weight
  for(LHCb::Particles::const_iterator iDau = m_CleanedDaugthers->begin();
      iDau != m_CleanedDaugthers->end();
      ++iDau)
    {
      const LHCb::Particle *jetdau = *iDau;

      double ipDist, ipSig;
      m_distCalc->distance(jetdau, m_Jet, ipDist, ipSig);
      ipSig = std::sqrt(ipSig);

      Gaudi::XYZVector ipSeg;
      LHCb::VertexBase jetVertex = (LHCb::VertexBase) m_JetVertex;
      m_distCalc->distance(jetdau, &jetVertex, ipSeg);

      Gaudi::LorentzVector jetMomentum = m_Jet->momentum();
      Gaudi::XYZVector jetSeg(jetMomentum.px(),jetMomentum.py(),jetMomentum.pz());
      double product = ipSeg.Dot(jetSeg);
      int ipSign = product > 0 ? 1: -1;

      double ipSignSig = ipSign*ipSig;

      // Calculate sum
      SumSignValue += ipSignSig;

      // Calculate first, second and third
      if(ipSignSig > FirstSignValue)
	{
	  ThirdSignValue = SecondSignValue;
	  SecondSignValue = FirstSignValue;
	  FirstSignValue = ipSignSig;
	}
      else if(ipSignSig > SecondSignValue)
	{
	  ThirdSignValue = SecondSignValue;
	  SecondSignValue = ipSignSig;
	}
      else if(ipSignSig > ThirdSignValue)
	{
	  ThirdSignValue = ipSignSig;
	}
    }



  jetWeight["ThirdTrack"]=ThirdSignValue;
  jetWeight["SecondTrack"]=SecondSignValue;
  jetWeight["FirstTrack"]=FirstSignValue;
  jetWeight["SumSignValue"] = SumSignValue;



  jetWeight["extraInfo"] = 5600;
  double tag_global = 0;

  if (SecondSignValue >  m_secCut && SumSignValue > m_sumCut)
    tag_global =1;
  //IT HAS TO BEHTE LAST ONE: GLOBAL DECISTION

  jetWeight["Tag"] = tag_global;



  if(msgLevel(MSG::DEBUG)) debug() << "Tag weight: " << jetWeight << endmsg;
  if(tag_global)
    return true;
  else
    return false;
}
