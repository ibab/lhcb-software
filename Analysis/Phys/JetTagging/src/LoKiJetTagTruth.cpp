// $Id: JetTagTruth.cpp,v 1.1 2011-07-27 12:12:12 aphan Exp $

// ============================================================================
// Includes

// Local
#include "LoKiJetTagTruth.h"

#include "Event/MCParticle.h"

// kernel
#include "Kernel/IParticle2MCAssociator.h"

// LoKi
#include "LoKi/ILoKiSvc.h"
#include "LoKi/GenExtract.h"
#include "LoKi/GenParticleCuts.h"

#include "LoKi/ParticleCuts.h"
#include "LoKi/VertexCuts.h"
#include "LoKi/MCVertexCuts.h"
#include "LoKi/MCParticleCuts.h"
#include "LoKi/GenParticleCuts.h"
#include "LoKi/PhysMCParticleCuts.h"

// ============================================================================
// Declaration of the Tool Factory
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi,JetTagTruth)


// ============================================================================
// Standard Constructor
LoKi::JetTagTruth::JetTagTruth
(const std::string& type, const std::string& name, const IInterface* parent)
  : GaudiTool(type, name, parent)
  , m_p2mcAssoc(0)
  , m_p2mcAssocType("DaVinciSmartAssociator")
{
  // Interface
  declareInterface<IJetTagTool>(this);

  declareProperty( "IP2MCPAssociatorType", m_p2mcAssocType);
  
  
  declareProperty("matchMinWeight",
                  m_matchMinWeight = 0.2,
                  "minimum weight for matching");
  
  declareProperty("matchMaxDeltaR",
                  m_matchMaxDeltaR = 0.4,
                  "maximum delta r for matching");
  
  declareProperty("jetMatchTableHepMC2Jets",
                  m_jetMatchTableHepMC2Jets = "Relations/Phys/StdHepMCJets2StdJets",
                  "visible particle-reconstructed jet relation table");

  declareProperty("jetMatchTablePart2GenPart",
                  m_jetMatchTablePart2GenPart = "Relations/Phys/StdHepMC2HepMC",
                  "particles-genparticles relation table");



  return;
}


// ============================================================================
// Standard Destructor
LoKi::JetTagTruth::~JetTagTruth()
{
  return;
}


// ============================================================================
// Standard Initialization
StatusCode LoKi::JetTagTruth::initialize()
{
  if(msgLevel(MSG::VERBOSE))
    verbose() << "Initialising algorithm" << endmsg;

  StatusCode sc = GaudiTool::initialize();
  if(sc.isFailure())
    return sc;

  svc<LoKi::ILoKiSvc>("LoKiSvc", true);

  m_p2mcAssoc = tool<IParticle2MCAssociator>(m_p2mcAssocType, this);

  return sc;
}


// ============================================================================
// Standard Finalization
StatusCode LoKi::JetTagTruth::finalize()
{
  if(msgLevel(MSG::VERBOSE))
    verbose() << "Finalising algorithm" << endmsg;

  return GaudiTool::finalize();
}


// ============================================================================
// Calculate Tag Truth
bool LoKi::JetTagTruth::calculateJetProperty
(const LHCb::Particle *jet,  	std::map<std::string,double > &jetTruth)
{
  StatusCode sc = StatusCode::SUCCESS;
  
  //Cedric's code JetP2MCP 
  jetTruth.clear();

  Assert( m_p2mcAssoc 
          , "The DaVinci smart associator hasn't been initialized!");

  std::vector<double> weightB = TruthBWeight(jet);
  std::vector<double> weightC = TruthCWeight(jet);
  
  double btf =  weightB.at(0);
  double bti =  weightB.at(1);
  double btm =  weightB.at(2);
  jetTruth["BTag_JP2MCP"]=btf;
  jetTruth["BTagN_JP2MCP"]= bti;
  jetTruth["B2MuXTag_JP2MCP"] = btm;
  double ctf =  weightC.at(0);
  double cti =  weightC.at(1);
  double ctm =  weightC.at(2);
  jetTruth["CTag_JP2MCP"] = ctf;
  jetTruth["CTagN_JP2MCP"] = cti;
  jetTruth["C2MuXTag_JP2MCP"]= ctm;



  //Anna's code Jet2HEPJet 
  
  bool fromB      = false;
  bool fromC      = false;
  bool fromOther  = false;

  // Check relation tables exist
  typedef IJets2Jets::Table TableJets2Jets;
  if(exist<TableJets2Jets>(m_jetMatchTableHepMC2Jets)){
    typedef LHCb::Relation2D<LHCb::Particle,HepMC::GenParticle*>  TablePart2GenPart;
    if(exist<TablePart2GenPart>(m_jetMatchTablePart2GenPart)){
      // Get relation tables
      TableJets2Jets *tableHepMC2Jets =
	get<TableJets2Jets>(m_jetMatchTableHepMC2Jets);
      
      TablePart2GenPart *tablePart2GenPart =
	get<TablePart2GenPart>(m_jetMatchTablePart2GenPart);
      
      // Invert relation tables
      TableJets2Jets *tableJets2HepMC =
	new TableJets2Jets(*tableHepMC2Jets,1);

      // Get bquarks
      const LHCb::HepMCEvent::Container* hepmc =
	get<LHCb::HepMCEvent::Container>(LHCb::HepMCEventLocation::Default);
      
      std::vector<const HepMC::GenParticle*> bquarks;
      
      Extract::genParticles(hepmc,
			    std::back_inserter(bquarks),
			    Cuts::GBEAUTY);
      
      std::vector<const HepMC::GenParticle*> frombquarks;
      
      Extract::genParticles(hepmc,
			    std::back_inserter(frombquarks),
			    Cuts::GFROMTREE(bquarks));
      
      // Get cquarks
      std::vector<const HepMC::GenParticle*> cquarks;
      
      Extract::genParticles(hepmc,
			    std::back_inserter(cquarks),
			    Cuts::GCHARM);
      
      std::vector<const HepMC::GenParticle*> fromcquarks;
      
      Extract::genParticles(hepmc,
			    std::back_inserter(fromcquarks),
			    Cuts::GFROMTREE(cquarks));
      
      
      // Get truth particle jet
      TableJets2Jets::Range recoRelations = tableJets2HepMC->i_relations(jet);
      if(recoRelations.size() > 0)
	{
	  double maxWeight = m_matchMinWeight;
	  LHCb::Particle *truthJet = NULL;
	  
	  Types::Fun deltaRFunction = Cuts::DR2(jet->momentum());
	  
	  IJets2Jets::Table::Range::iterator link;
	  IJets2Jets::Table::Range::iterator linkBegin = recoRelations.begin();
	  IJets2Jets::Table::Range::iterator linkEnd = recoRelations.end();
	  
	  for(link = linkBegin; link != linkEnd; link++)
	    {
	      LHCb::Particle *jetTmp = link->to();
	      double weight = link->weight();
	      double distance = std::sqrt(deltaRFunction(jetTmp));
	      
	      if(msgLevel(MSG::DEBUG))
		debug() << "Jet truth match weight: " << weight
			<< " distance " << distance
			<< endmsg;
	      
	      if(weight > maxWeight && distance < m_matchMaxDeltaR)
		{
		  truthJet = jetTmp;
		  maxWeight = weight;
		}
	    }
	  
	  if(truthJet)
	    {
	      IJetMaker::Input truthJetInputs = truthJet->daughtersVector();
	      
	      // Make a vector of all the HepMC particles of the truth particle jet
	      std::vector<const HepMC::GenParticle*> jetParticles;
	      for(LHCb::Particle::ConstVector::const_iterator iInput = truthJetInputs.begin();
		  iInput != truthJetInputs.end();
		  ++iInput)
		{
		  TablePart2GenPart::Range links = tablePart2GenPart->i_relations(*iInput);
		  for(TablePart2GenPart::iterator ilink = links.begin();
		      ilink != links.end();
		      ++ilink)
		    {
		      const HepMC::GenParticle *genParticle = ilink->to();
		      if(genParticle == 0) continue;
		      else jetParticles.push_back(genParticle);
		    }
		}
	      
	      // See if any of the jet particles originate from a bquark or a cquark
	      std::vector<const HepMC::GenParticle*>::iterator jetIt;
	      std::vector<const HepMC::GenParticle*>::iterator bquarkIt;
	      std::vector<const HepMC::GenParticle*>::iterator cquarkIt;
	      
	      for(jetIt = jetParticles.begin(); jetIt != jetParticles.end(); jetIt++)
		{
		  bquarkIt = find(frombquarks.begin(),frombquarks.end(),*jetIt);
		  if(bquarkIt != frombquarks.end()) fromB = true;
		  
		  cquarkIt = find(fromcquarks.begin(),fromcquarks.end(),*jetIt);
		  if(cquarkIt != fromcquarks.end()) fromC = true;
		}
	      
	      if(!fromB && !fromC) fromOther = true;
	    }
	  else
	    {
	      if(msgLevel(MSG::DEBUG)) debug() << "No good truth matches found" << endmsg;
	    }
	}
      else
	{
	  if(msgLevel(MSG::DEBUG)) debug() << "No truth matches found" << endmsg;
	}
      
      delete tableJets2HepMC;
      
      jetTruth["BTag_J2HJ"]=fromB;
      jetTruth["CTag_J2HJ"]=fromC;
      jetTruth["LTag_J2HJ"]=fromOther;

      int NT = 0;
      if(fromOther || fromC || fromB)
	NT =1;
      
      jetTruth["JTag_J2HJ"] = NT;
      
  jetTruth["extraInfo"] = 6100;
      
    }
  }
  

  debug() << "Tag truth: " << jetTruth << endmsg;

  return true;
}




std::vector<double> LoKi::JetTagTruth::TruthBWeight(const LHCb::Particle* jet){
  
  double weight = 0;
  std::vector<double> we;
  std::map<double,const LHCb::MCParticle*>BMD;
  std::map<double,const LHCb::MCParticle*>BM;
  std::map<double,const LHCb::MCParticle*>::iterator iBMD;
  std::map<double,const LHCb::MCParticle*>::iterator imc;
  
  LHCb::Particle::ConstVector Daus = jet->daughtersVector();

  int k = 0;
  for(LHCb::Particle::ConstVector::const_iterator iInput = Daus.begin();
      iInput != Daus.end();
      ++iInput){
    
    const LHCb::MCParticle* mcp(0);
    mcp = m_p2mcAssoc->relatedMCP((*iInput));
    if(mcp == NULL) continue;
    const LHCb::MCParticle* bancestor = Bancestor(mcp);
    if (bancestor == NULL)
      continue;
    else{
      BM[bancestor->pt()]=bancestor;
      BMD[bancestor->pt()+k] = mcp;
      k++;
    }
  }
  
  if(BM.empty()){
    we.push_back(0);
    we.push_back(0);
    we.push_back(0);
    return we;
  }
  bool MDecay = false;
  for(imc = BM.begin(); imc != BM.end(); ++imc){
    k=0;
    if(!MDecay)
      MDecay = MuonDecay( ((*imc).second));
    
    for(iBMD = BMD.begin(); iBMD != BMD.end(); ++iBMD){
      if((((*iBMD).first)-k) == ((*imc).first))
	weight += (((*iBMD).second)->momentum().E())/ ((*imc).second)->momentum().E();
      k++;
    }
  }
  
  
  we.push_back(weight);
  we.push_back(BM.size());
  we.push_back(MDecay);
 
  return we;  
}


std::vector<double> LoKi::JetTagTruth::TruthCWeight(const LHCb::Particle* jet){
  
  double weight = 0;
  std::vector<double> we;
  std::map<double,const LHCb::MCParticle*>CMD;
  std::map<double,const LHCb::MCParticle*>::iterator iCMD;
  std::map<double,const LHCb::MCParticle*>CM;
  std::map<double,const LHCb::MCParticle*>::iterator imc;

  LHCb::Particle::ConstVector Daus = jet->daughtersVector();
  int k = 0;
  for(LHCb::Particle::ConstVector::const_iterator iInput = Daus.begin();
      iInput != Daus.end();
      ++iInput){
    
    const LHCb::MCParticle* mcp(0);
    mcp = m_p2mcAssoc->relatedMCP((*iInput));
    if(mcp == NULL) continue;
    const LHCb::MCParticle* cancestor = Cancestor(mcp);
    if (cancestor == NULL)
      continue;
    else{
      CM[cancestor->pt()] = cancestor;
      CMD[cancestor->pt()+k] = mcp;
      k++;
    }
  }
  
  if(CM.size() == 0) {
    we.push_back(0);
    we.push_back(0);
    we.push_back(0);
    return we;
  }
  
  bool MDecay = false;
  for(imc = CM.begin(); imc != CM.end(); ++imc){
    k=0;
    if(!MDecay)
      MDecay = MuonDecay( ((*imc).second));

    for(iCMD = CMD.begin(); iCMD != CMD.end(); ++iCMD){
      if((((*iCMD).first)-k) == ((*imc).first))
	weight += (((*iCMD).second)->momentum().E())/ ((*imc).second)->momentum().E();
      k++;
    }
  }
  
  
  we.push_back(weight);
  we.push_back(CM.size());
  we.push_back(MDecay);
  return we;
}



bool LoKi::JetTagTruth::MuonDecay(const LHCb::MCParticle* mcpart){
  if (mcpart == NULL)
    return false;
  
  const SmartRefVector< LHCb::MCVertex > endV = mcpart->endVertices();
  
  SmartRefVector< LHCb::MCVertex >::const_iterator itv;
  
  for(itv = endV.begin(); itv!= endV.end(); itv++){
    const SmartRefVector < LHCb::MCParticle > endP = (*itv).target()->products();
    SmartRefVector< LHCb::MCParticle >::const_iterator itp;
    for(itp = endP.begin(); itp!= endP.end(); itp++)
      if((*itp).target()->particleID().abspid() == 13) return true;
  }
 
  return false;	
}



const LHCb::MCParticle* LoKi::JetTagTruth::Bancestor(const LHCb::MCParticle* mcpart){
  if (mcpart == NULL)
    return NULL;
  else if (mcpart->mother()==NULL)
    return NULL;
  else if (mcpart->mother()->particleID().hasBottom())
    return  mcpart->mother();
  else
    return Bancestor( mcpart->mother());	
}



const LHCb::MCParticle* LoKi::JetTagTruth::Cancestor(const LHCb::MCParticle* mcpart){
  if (mcpart == NULL)
    return NULL;
  else if (mcpart->mother()==NULL)
    return NULL;
  else if (mcpart->mother()->particleID().hasCharm()){
    //  if (Bancestor(mcpart->mother()) == NULL)
    return  mcpart->mother();
  }//else
  //return NULL;
  else
    return Cancestor( mcpart->mother());	
}

