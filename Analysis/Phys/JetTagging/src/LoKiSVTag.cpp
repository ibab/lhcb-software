// $Id: LokiSVTag.cpp,v 1.1 2011-06-24 10:10:10 aphan Exp $

// ============================================================================
// Includes

// Local
#include "LoKiSVTag.h"
// #include "LoKi/Kinematics.h"
// #include "LoKi/Geometry.h"
// LoKi
// #include "LoKi/ILoKiSvc.h"

// ============================================================================
// Declaration of the Tool Factory
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi,SVTag)


// ============================================================================
// Standard Constructor
LoKi::SVTag::SVTag
(const std::string& type, const std::string& name, const IInterface* parent)
  : GaudiTool(type, name, parent)
  , m_Jet(0)
  , m_JetVertex(0)
  , m_distCalc(0)
  , m_p2mcAssoc(0)
  , m_p2mcAssocType("DaVinciSmartAssociator")
  , m_seedFinder(0)
  , m_seedFinderName( "LoKi__SeedFinderNTrks"  ) // runs ntrack seeder by default
{
  // Interface
  declareInterface<IJetTagTool>(this);

  // Algorithm related properties
  declareProperty("AlgorithmMode",
                  m_AlgMode = "SigMax");

  declareProperty("SeedFinder",
                  m_seedFinderName);
  return;
}


// ============================================================================
// Standard Destructor
LoKi::SVTag::~SVTag()
{
  return;
}


// ============================================================================
// Standard Initialization
StatusCode LoKi::SVTag::initialize()
{
  if(msgLevel(MSG::VERBOSE))
    verbose() << "Initialising algorithm" << endmsg;

  StatusCode sc = GaudiTool::initialize();
  if(sc.isFailure())
    return sc;

  // svc<LoKi::ILoKiSvc>("LoKiSvc", true);

  m_distCalc = tool<IDistanceCalculator>("LoKi::DistanceCalculator",this);

  //create seeds with the input particles
  if(!m_seedFinder)
    {
      //select your prefered SeedFinder
      m_seedFinder = tool<IJetMaker>( m_seedFinderName,m_seedFinderName, this );
    }

  return sc;
}


// ============================================================================
// Standard Finalization
StatusCode LoKi::SVTag::finalize()
{
  if(msgLevel(MSG::VERBOSE))
    verbose() << "Finalising algorithm" << endmsg;

  return GaudiTool::finalize();
}


// ===========================================================================
// Set Jet Vertex
StatusCode LoKi::SVTag::setJetVertex()
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
bool LoKi::SVTag::calculateJetProperty
(const LHCb::Particle *jet,  std::map <std::string,double> &jetWeight)
{
  StatusCode sc = StatusCode::SUCCESS;

  // Set member variables
  m_Jet = jet;
  m_JetInputs = jet->daughtersVector();
  jetWeight.clear();

  // Set jet vertex
  sc = setJetVertex();
  if(sc.isFailure())
    {
      err() << "Unable to set jet vertex" << endmsg;
      return StatusCode::SUCCESS;
    }

  // Try to get seeds with respect to the jet vertex
  IJetMaker::Jets jetSeeds;
  m_seedFinder->makeJets(m_JetInputs, m_JetVertex, jetSeeds);
  
  double lengthMax = -999,
    lengthMaxN = -999,
    lengthMaxM = -999,
    lengthMaxChi2NDF = -999,
    lengthSum = 0,
    lengthMaxDirAngleJ2S = -999,
    lengthMaxDirAngleS2S = -999;

  
  double sigMax = -999,
    sigMaxN = -999,
    sigMaxM = -999,
    sigMaxChi2NDF  = -999,
    sigSum = 0,
    sigMaxDirAngleJ2S = -999,
    sigMaxDirAngleS2S = -999;


  if(jetSeeds.size() > 0)
    {
      for(unsigned iSeed = 0; iSeed < jetSeeds.size(); iSeed++)
	{
	  double ipDist, ipChi, ipSig;
	  m_distCalc->distance( (LHCb::VertexBase*)m_JetVertex.clone(),
				(LHCb::VertexBase*)jetSeeds.at(iSeed)->endVertex(),
				ipDist,
				ipChi );

	  ipSig = std::sqrt(ipChi);

	  Gaudi::LorentzVector jetMomentum = m_Jet->momentum();
	  Gaudi::LorentzVector seedMomentum = jetSeeds.at(iSeed)->momentum();

	  Gaudi::XYZVector jetSeg(jetMomentum.px(),jetMomentum.py(),jetMomentum.pz());
	  Gaudi::XYZVector seedSeg(seedMomentum.px(),seedMomentum.py(),seedMomentum.pz());

	  double product = seedSeg.Dot(jetSeg);
	  double product2 = seedSeg.Unit().Dot(jetSeg.Unit());
      
	  Gaudi::XYZVector jetRef =  Gaudi::XYZVector(m_Jet->referencePoint().X(),m_Jet->referencePoint().Y(), m_Jet->referencePoint().Z());
	  Gaudi::XYZVector seedDecay = Gaudi::XYZVector(jetSeeds.at(iSeed)->endVertex()->position().X(), jetSeeds.at(iSeed)->endVertex()->position().Y(),jetSeeds.at(iSeed)->endVertex()->position().Z());
      
      
	  Gaudi::XYZVector seedd = seedDecay -jetRef;
	  double cos1 = (seedSeg.Unit()).Dot(seedd.Unit());



	  int ipSign = product > 0 ? 1: -1;
	  double ipSignSig = ipSign*ipSig;

	  sigSum += ipSignSig;
	  lengthSum += ipDist;

	  if(ipSignSig > sigMax)
	    {
	      sigMax = ipSignSig;
	      sigMaxN = jetSeeds.at(iSeed)->daughtersVector().size();
	      sigMaxM = jetSeeds.at(iSeed)->measuredMass();
	      sigMaxChi2NDF = jetSeeds.at(iSeed)->endVertex()->chi2PerDoF();
	      sigMaxDirAngleJ2S = product2;
	      sigMaxDirAngleS2S = cos1;

	    }
	  if(ipDist > lengthMax)
	    {
	      lengthMax = ipDist;
	      lengthMaxN = jetSeeds.at(iSeed)->daughtersVector().size();
	      lengthMaxM = jetSeeds.at(iSeed)->measuredMass();
	      lengthMaxChi2NDF = jetSeeds.at(iSeed)->endVertex()->chi2PerDoF();
	      lengthMaxDirAngleJ2S = product2;
	      lengthMaxDirAngleS2S = cos1;
	    }
	}
    }

  double lengthAvg = jetSeeds.size() > 0 ? lengthSum / jetSeeds.size() : 0;
  double sigAvg = jetSeeds.size() > 0 ? sigSum / jetSeeds.size() : 0;

  std::pair<double,std::string> tmp;

  jetWeight["lengthAvg"]=lengthAvg;
  jetWeight["lengthMax"]=lengthMax;
  jetWeight["LengthMaxNtracks"]=lengthMaxN;
  jetWeight["LengthMaxMass"]=lengthMaxM;
  jetWeight["LengthMaxVtxChi2NDF"]=lengthMaxChi2NDF;
  jetWeight["LengthMaxDirAngleS2S"]=lengthMaxDirAngleS2S;
  jetWeight["LengthMaxDirAngleJ2S"]=lengthMaxDirAngleJ2S;
  jetWeight["SigAvg"]=sigAvg;
  jetWeight["SigMaxNtracks"]=sigMaxN;
  jetWeight["SigMaxMass"]=sigMaxM;
  jetWeight["SigMax"]=sigMax;
  jetWeight["SigMaxVtxChi2NDF"]=sigMaxChi2NDF;
  jetWeight["SigMaxDirAngleS2S"]=sigMaxDirAngleS2S;
  jetWeight["SigMaxDirAngleJ2S"]=sigMaxDirAngleJ2S;
  jetWeight["Nvertices"]=jetSeeds.size();

  
  double tag_global = 0;

  if (lengthMax > -999)
    tag_global =1;
  //IT HAS TO BEHTE LAST ONE: GLOBAL DECISTION
  jetWeight["Tag"] =tag_global ;

  jetWeight["extraInfo"] = 5200;
  if ( tag_global)
    return true;
  else
    return false;
}
