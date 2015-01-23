// $Id: JetTagTruth.cpp,v 1.1 2011-07-27 12:12:12 aphan Exp $

// ============================================================================
// Includes

// Local
#include "LoKiJetTagTruthNew.h"

#include "Event/MCParticle.h"
#include "LoKi/GenParticleCuts.h"


#include "Kernel/IDVAlgorithm.h"
#include <Kernel/GetIDVAlgorithm.h>
// kernel
#include "Kernel/IParticle2MCAssociator.h"

// LoKi
#include "LoKi/ILoKiSvc.h"
#include "LoKi/GenExtract.h"
#include "LoKi/GenParticleCuts.h"


// ============================================================================
// Declaration of the Tool Factory
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi,JetTagTruthNew)


// ============================================================================
// Standard Constructor
LoKi::JetTagTruthNew::JetTagTruthNew
(const std::string& type, const std::string& name, const IInterface* parent)
  : GaudiTool(type, name, parent)
  , m_p2mcAssoc(0)
  , m_p2mcAssocType("DaVinciSmartAssociator")
  , m_dva(0)       
{
  // Interface
  declareInterface<IJetTagTool>(this);

  declareProperty( "IP2MCPAssociatorType", m_p2mcAssocType);
  
  return;
}


// ============================================================================
// Standard Destructor
LoKi::JetTagTruthNew::~JetTagTruthNew()
{
  return;
}


// ============================================================================
// Standard Initialization
StatusCode LoKi::JetTagTruthNew::initialize()
{
  if(msgLevel(MSG::VERBOSE))
    verbose() << "Initialising algorithm" << endmsg;

  StatusCode sc = GaudiTool::initialize();
  if(sc.isFailure())
    return sc;

  svc<LoKi::ILoKiSvc>("LoKiSvc", true);

  m_p2mcAssoc = tool<IParticle2MCAssociator>(m_p2mcAssocType, this);

  m_dva = Gaudi::Utils::getIDVAlgorithm ( contextSvc(), this ) ;
  if (0==m_dva) return Error("Couldn't get parent DVAlgorithm",
                             StatusCode::FAILURE);

  return sc;
}


// ============================================================================
// Standard Finalization
StatusCode LoKi::JetTagTruthNew::finalize()
{
  if(msgLevel(MSG::VERBOSE))
    verbose() << "Finalising algorithm" << endmsg;

  return GaudiTool::finalize();
}


// ============================================================================
// Calculate Tag Truth
bool LoKi::JetTagTruthNew::calculateJetProperty
(const LHCb::Particle *jet,  	std::map<std::string,double > &jetTruth)
{
  StatusCode sc = StatusCode::SUCCESS;


 
  jetTruth["extraInfo"] = 6000;


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




  // Get bquarks
  const LHCb::HepMCEvent::Container* hepmc = get<LHCb::HepMCEvent::Container>(LHCb::HepMCEventLocation::Default);
  
  std::vector<const HepMC::GenParticle*> bquarks;
  

  

  Extract::genParticles(hepmc,
			std::back_inserter(bquarks),
			Cuts::GABSID == 5 );
  // Get cquarks
  std::vector<const HepMC::GenParticle*> cquarks;
  
  Extract::genParticles(hepmc,
			std::back_inserter(cquarks),
			Cuts::GABSID == 4);
 
  //Anna's code Jet2HEPJet 
  
  double fromB04             = 0; 
  double fromBTrk04          = 0;
  double fromC04             = 0;
  double fromCTrk04          = 0;
  double fromB10             = 0; 
  double fromBTrk10          = 0;
  double fromC10             = 0;
  double fromCTrk10          = 0;


  LoKi::Cuts::GDR2 deltaRFunction = Cuts::GDR2(jet->momentum());

  std::vector<const HepMC::GenParticle*>::iterator bquarkIt;
  std::vector<const HepMC::GenParticle*>::iterator frombquarkIt;
  std::vector<const HepMC::GenParticle*>::iterator cquarkIt;
  std::vector<const HepMC::GenParticle*>::iterator fromcquarkIt;
  LHCb::Particle::ConstVector::const_iterator idau;  
  LHCb::Particle::ConstVector jetDau = jet->daughtersVector();
  const LHCb::VertexBase* JetOriVtx = m_dva->bestVertex ( jet );
  Gaudi::XYZVector A = Gaudi::XYZVector(JetOriVtx->position());


  for (bquarkIt = bquarks.begin();bquarkIt!=bquarks.end();bquarkIt++){
    double distance = std::sqrt(deltaRFunction((*bquarkIt)));

    
    const HepMC::GenVertex * GPV = 	(*bquarkIt)->production_vertex();

    Gaudi::XYZVector B = Gaudi::XYZVector(GPV->point3d());
    
    Gaudi::XYZVector C = A-B;
    
    if(std::sqrt(C.Mag2()) > 1.1)     continue;
    

    if (distance < 1.0){ 
      fromB10 = 1;
      if (distance < 0.4)  fromB04 = 1;
      std::vector<const HepMC::GenParticle*> frombquarks;
      frombquarks.clear();
      Extract::genParticles(hepmc,
			    std::back_inserter(frombquarks),
			    Cuts::GFROMTREE(*bquarkIt));
      
      for (idau = jetDau.begin(); idau!=jetDau.end(); idau++){
	LoKi::Cuts::GDR2 deltaRFunctionTrk = Cuts::GDR2((*idau)->momentum());
        for (frombquarkIt = frombquarks.begin();frombquarkIt!=frombquarks.end();frombquarkIt++){
	  if(((*frombquarkIt)->status() != 999) && ((*frombquarkIt)->status() != 1))  continue;
	  double distancetrk = std::sqrt(deltaRFunctionTrk((*frombquarkIt)));
	  if (distancetrk < 0.1){
	    fromBTrk10 = 1;
	    if (distance < 0.4){
	      fromBTrk04 = 1;
	      break;
	    }
	  }
	}
	if(fromBTrk04) break;
      }
    }
    if(fromBTrk04) break;
  }
  

  for (cquarkIt = cquarks.begin();cquarkIt!=cquarks.end();cquarkIt++){
    double distance = std::sqrt(deltaRFunction((*cquarkIt)));

 
    const HepMC::GenVertex * GPV = 	(*cquarkIt)->production_vertex();

    Gaudi::XYZVector B = Gaudi::XYZVector(GPV->point3d());
    
    Gaudi::XYZVector C = A-B;
    
    if(std::sqrt(C.Mag2()) > 1.1)     continue;
    



    if (distance < 1.0){ 
      fromC10 = 1;
      if (distance < 0.4) fromC04 = 1;
      std::vector<const HepMC::GenParticle*> fromcquarks;
      fromcquarks.clear();
      Extract::genParticles(hepmc,
			    std::back_inserter(fromcquarks),
			    Cuts::GFROMTREE(*cquarkIt));
      
      for (idau = jetDau.begin(); idau!=jetDau.end(); idau++){
	LoKi::Cuts::GDR2 deltaRFunctionTrk = Cuts::GDR2((*idau)->momentum());
        for (fromcquarkIt = fromcquarks.begin();fromcquarkIt!=fromcquarks.end();fromcquarkIt++){
	  if(((*fromcquarkIt)->status() != 999) && ((*fromcquarkIt)->status() != 1))  continue;
	  double distancetrk = std::sqrt(deltaRFunctionTrk((*fromcquarkIt)));
	  if (distancetrk < 0.1){
	    fromCTrk10 = 1;
	    if (distance < 0.4){
	      fromCTrk04 = 1;
	      break;
	    }
	  }
	}
	if(fromCTrk04) break;
      }
    }
    if(fromCTrk04) break;
  }
  
  


  


  jetTruth["B04"]=fromB04;
  jetTruth["BTrk04"]=fromBTrk04;
  jetTruth["C04"]=fromC04;
  jetTruth["CTrk04"]=fromCTrk04;


  jetTruth["B10"]=fromB10;
  jetTruth["BTrk10"]=fromBTrk10;
  jetTruth["C10"]=fromC10;
  jetTruth["CTrk10"]=fromCTrk10;




  
  double tag_global = fromBTrk04*10+fromCTrk04;
  //IT HAS TO BEHTE LAST ONE: GLOBAL DECISTION
  jetTruth["Tag"]= tag_global;

  debug() << "Tag truth: " << jetTruth << endmsg;

  if(tag_global>0)
    return true;
  else
    return false;


}





std::vector<double> LoKi::JetTagTruthNew::TruthBWeight(const LHCb::Particle* jet){
  
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


std::vector<double> LoKi::JetTagTruthNew::TruthCWeight(const LHCb::Particle* jet){
  
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



bool LoKi::JetTagTruthNew::MuonDecay(const LHCb::MCParticle* mcpart){
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



const LHCb::MCParticle* LoKi::JetTagTruthNew::Bancestor(const LHCb::MCParticle* mcpart){
  if (mcpart == NULL)
    return NULL;
  else if (mcpart->mother()==NULL)
    return NULL;
  else if (mcpart->mother()->particleID().hasBottom())
    return  mcpart->mother();
  else
    return Bancestor( mcpart->mother());	
}



const LHCb::MCParticle* LoKi::JetTagTruthNew::Cancestor(const LHCb::MCParticle* mcpart){
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

