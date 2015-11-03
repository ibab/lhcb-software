// $Id: JetTagTruth.cpp,v 1.1 2011-07-27 12:12:12 aphan Exp $

// ============================================================================
// Includes

// Local
#include "LoKiJetTagTruthMCPart.h"

#include "Event/MCParticle.h"
#include "LoKi/GenParticleCuts.h"
#include "LoKi/MCExtract.h"


#include "Kernel/IDVAlgorithm.h"
#include <Kernel/GetIDVAlgorithm.h>
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
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi,JetTagTruthMCPart)


// ============================================================================
// Standard Constructor
LoKi::JetTagTruthMCPart::JetTagTruthMCPart
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
LoKi::JetTagTruthMCPart::~JetTagTruthMCPart()
{
  return;
}


// ============================================================================
// Standard Initialization
StatusCode LoKi::JetTagTruthMCPart::initialize()
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
StatusCode LoKi::JetTagTruthMCPart::finalize()
{
  if(msgLevel(MSG::VERBOSE))
    verbose() << "Finalising algorithm" << endmsg;

  return GaudiTool::finalize();
}


// ============================================================================
// Calculate Tag Truth
bool LoKi::JetTagTruthMCPart::calculateJetProperty
(const LHCb::Particle *jet,  	std::map<std::string,double > &jetTruth)
{


  using namespace LoKi::Types ;
  using namespace LoKi::Cuts  ;
  







  StatusCode sc = StatusCode::SUCCESS;


 
  jetTruth["extraInfo"] = 7000;


  //Cedric's code JetP2MCP 
  jetTruth.clear();

  Assert( m_p2mcAssoc 
          , "The DaVinci smart associator hasn't been initialized!");
  /*
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
  
  */
  LHCb::MCParticles* mcparts = get< LHCb::MCParticles>( LHCb::MCParticleLocation::Default );

  // std::vector<const LHCb::MCParticle*> bquarks;
  
  //  Extract::mcParticles(hepmc,
  //		       bquarks,
  //		       true);//Cuts::BEAUTY);
  
  // Get bquarks
  
  //std::vector<const LHCb::MCParticle*> mcps =     get<LHCb::MCParticle::Container> (  LHCb::MCParticleLocation::Default );
  
  
  // MCRange b = mcselect ( "beauty" , BEAUTY ) ;
  //  MCRange c = mcselect ( "charm"  , CHARM  ) ;
  //  MCRange bquarks;
  //  const LHCb::MCParticle::Container* bquarks ;
  //  std::vector<const LHCb::MCParticle*> bquarks;

  //  Extract::mcParticles(mcps,std::back_inserter(bquarks),	Cuts::CHARM );
  // Get cquarks
  
  //  MCRange  cquarks;
  //  Extract::mcParticles(mcpart,
  //  //			std::back_inserter(cquarks),
  //			Cuts::CHARM);
  
  //Anna's code Jet2HEPJet 
  
  double fromBTrk04          = 0;
  double fromCTrk04          = 0;
  double fromBTrk          = 0;
  double fromCTrk          = 0;


  LoKi::Cuts::MCDR2 deltaRFunction = Cuts::MCDR2(jet->momentum());

  LHCb::MCParticle::Vector::iterator  mcIt;





  std::map<int,double> BEfrac;
  std::map<int,double> BR;
  std::map<int,double> CEfrac;
  std::map<int,double> CR;
  std::map<int,double> CFromB;
  std::map<int,double> BR04;
  std::map<int,double> CR04;
  BEfrac.clear();
  BR.clear();
  BR04.clear();
  CEfrac.clear();
  CR.clear();
  CR04.clear();
  CFromB.clear();



  LHCb::Particle::ConstVector::const_iterator idau;  
  LHCb::Particle::ConstVector jetDau = jet->daughtersVector();
  //  const LHCb::VertexBase* JetOriVtx = m_dva->bestVertex ( jet );
  Gaudi::XYZVector A = Gaudi::XYZVector(jet->endVertex()->position());


  for (mcIt = mcparts->begin();mcIt!=mcparts->end();mcIt++){



    const LHCb::MCVertex * MCPV = 	(*mcIt)->primaryVertex();

    Gaudi::XYZVector B = Gaudi::XYZVector(MCPV->position());
    
    Gaudi::XYZVector C = A-B;
    
    if(std::sqrt(C.Mag2()) > 0.2)     continue;

    LHCb::MCParticle* myMCPart = *mcIt;
    

    if(((*mcIt)->particleID().hasBottom())) {

    if(myMCPart != BGrandMother(myMCPart)) continue;
    //    std::cout<< myMCPart << std::endl;

    double distance = std::sqrt(deltaRFunction((*mcIt)));   
    
    if (distance < 1.0){ 
      fromBTrk+=1;
      BR[myMCPart->key()] = distance;
      BEfrac[myMCPart->key()] = 0;
      
      if (distance < 0.4){ 
	fromBTrk04+=1;
	BR04[myMCPart->key()] = distance;
      }
      
      for(LHCb::Particle::ConstVector::const_iterator iInput = jetDau.begin();
	  iInput != jetDau.end();
	  ++iInput){
	
	const LHCb::MCParticle* mcp(0);
	mcp = m_p2mcAssoc->relatedMCP((*iInput));
	if(mcp == NULL) continue;
	const LHCb::MCParticle* bancestor = BGrandMother(Bancestor(mcp));
	if (bancestor == NULL)
	  continue;
	else if(bancestor!=myMCPart)
	  continue;
	else{
	  BEfrac[bancestor->key()] += (mcp->momentum().E())/(myMCPart->momentum().E());
	}
      }
    }
    }


    if(((*mcIt)->particleID().hasCharm())) {

    if(myMCPart != CGrandMother(myMCPart)) continue;


    double distance = std::sqrt(deltaRFunction((*mcIt)));   
    

    if (distance < 1.0){ 

      fromCTrk+=1;
      CR[myMCPart->key()] = distance;
      CEfrac[myMCPart->key()] = 0;
      CFromB[myMCPart->key()] = 0;
      if(Bancestor(myMCPart)!=NULL)
	CFromB[myMCPart->key()] = 1;


      if (distance < 0.4){ 
	fromCTrk04+=1;
	CR04[myMCPart->key()] = distance;
      }
      
      for(LHCb::Particle::ConstVector::const_iterator iInput = jetDau.begin();
	  iInput != jetDau.end();
	  ++iInput){
	
	const LHCb::MCParticle* mcp(0);
	mcp = m_p2mcAssoc->relatedMCP((*iInput));
	if(mcp == NULL) continue;
	const LHCb::MCParticle* cancestor = CGrandMother(Cancestor(mcp));
	if (cancestor == NULL)
	  continue;
	else if(cancestor!=myMCPart)
	  continue;
	else{
	  CEfrac[myMCPart->key()] += (mcp->momentum().E())/(myMCPart->momentum().E());
	}
      }
    }
  }
  }

  

  std::map<int,double>::iterator imb;
  std::map<int,double>::iterator imb2;

  double  minR = 10;
  int  minRKey = -1;
  int  minRKey04 = -1;
  for(imb = BR.begin(); imb != BR.end(); ++imb){
    if(((*imb).second) < minR) {
      minR = ((*imb).second) ;
      minRKey = ((*imb).first);
      if( BR04.find((*imb).first) != BR04.end())
	minRKey04 = minRKey;
    }
  }
  double  maxEf = -1;
  double  maxEfKey = -1;
  int  maxEf04 = -1;
  int  maxEfKey04 = -1;
  for(imb = BEfrac.begin(); imb != BEfrac.end(); ++imb){
    if((*imb).second > maxEf) {
      maxEf = (*imb).second ;
      maxEfKey = (*imb).first;
    }  
    for(imb2 =BR04.begin(); imb2 != BR04.end(); ++imb2){
      if( (*imb).first  !=  (*imb2).first )
	continue;
      maxEf04 = (*imb).second ;
      maxEfKey04 = (*imb).first;
      }
    }

  

 
  minR = 10;
  int  minRKeyC = -1;
  int  minRKeyC04 = -1;
  for(imb = CR.begin(); imb != CR.end(); ++imb){
    if((*imb).second < minR) {
      minR = (*imb).second ;
      minRKeyC = (*imb).first;
      if( CR04.find((*imb).first) != CR04.end())
	minRKeyC04 = minRKey;
    }
  }
  maxEf = 0;
  maxEf04 = 0;
  int maxEfKeyC = -1;
  int  maxEfKeyC04 = -1;
  for(imb = CEfrac.begin(); imb != CEfrac.end(); ++imb){
    if((*imb).second > maxEf) {
      maxEf = (*imb).second ;
      maxEfKeyC = (*imb).first;
    }
    if( CR04.find((*imb).first) != CR04.end()){
      if((*imb).second > maxEf04) {
	maxEf04 = (*imb).second ;
	maxEfKeyC04 = (*imb).first;
      }
    }
  }
  




  if(fromBTrk > 0){
    jetTruth["BminR"]  = BR[minRKey];
    jetTruth["BminREf"]= BEfrac[minRKey];
    jetTruth["BmaxEfR"] = BR[maxEfKey];
    jetTruth["BmaxEf"]= BEfrac[maxEfKey];
    jetTruth["BinDist"] = fromBTrk;
  }else{
    jetTruth["BminR"]  = -1;
    jetTruth["BminREf"]= -1;
    jetTruth["BmaxEf"] = -1;
    jetTruth["BmaxEfR"]= -1;
    jetTruth["BinDist"] = 0;
  }
  if(fromCTrk > 0){
    jetTruth["CminR"]  = CR[minRKeyC];
    jetTruth["CminREf"]= CEfrac[minRKeyC];
    jetTruth["CminRFromB"]= CFromB[minRKeyC];
    jetTruth["CmaxEfR"] = CR[maxEfKeyC];
    jetTruth["CmaxEf"]= CEfrac[maxEfKeyC];
    jetTruth["CmaxEfFromB"]= CFromB[maxEfKeyC];
    jetTruth["CinDist"] = fromCTrk;
  }else{
    jetTruth["CminR"]  = -1;
    jetTruth["CminREf"]= -1;
    jetTruth["CminRFromB"]= -1;
    jetTruth["CmaxEf"] = -1;
    jetTruth["CmaxEfR"]= -1;
    jetTruth["CmaxEfFromB"]= -1;
    jetTruth["CinDist"] = 0;
  }



  if(fromBTrk04 > 0){
    jetTruth["BminR04"]  = BR04[minRKey04];
    jetTruth["BminREf04"]= BEfrac[minRKey04];
    jetTruth["BmaxEfR04"] = BR04[maxEfKey04];
    jetTruth["BmaxEf04"]= BEfrac[maxEfKey04];
    jetTruth["BinDist04"] = fromBTrk04;
  }else{
    jetTruth["BminR04"]  = -1;
    jetTruth["BminREf04"]= -1;
    jetTruth["BmaxEfR04"] = -1;
    jetTruth["BmaxEf04"]= -1;
    jetTruth["BinDist04"] = 0;
  }
  if(fromCTrk04 > 0){
    jetTruth["CminR04"]  = CR04[minRKeyC04];
    jetTruth["CminREf04"]= CEfrac[minRKeyC04];
    jetTruth["CminRFromB04"]= CFromB[minRKeyC04];
    jetTruth["CmaxEf04"]= CEfrac[maxEfKeyC04];
    jetTruth["CmaxEfR04"] = CR04[maxEfKeyC04];
    jetTruth["CmaxEfFromB04"]= CFromB[maxEfKeyC04];
    jetTruth["CinDist04"] = fromCTrk04;
  }else{
    jetTruth["CminR04"]  = -1;
    jetTruth["CminREf04"]= -1;
    jetTruth["CminRFromB04"]= -1;
    jetTruth["CmaxEf04"] = -1;
    jetTruth["CmaxEfR04"]= -1;
    jetTruth["CmaxEfFromB04"]= -1;
    jetTruth["CinDist04"] = 0;
  }



  
  double tag_global = fromBTrk04*10+fromCTrk04;
  //IT HAS TO BEHTE LAST ONE: GLOBAL DECISTION
  jetTruth["Tag"]= tag_global;

  debug() << "Tag truth: " << jetTruth << endmsg;

  if(tag_global>0)
    return true;
  else
    return false;

}





std::vector<double> LoKi::JetTagTruthMCPart::TruthBWeight(const LHCb::Particle* jet){
  
  double weight = 0;
  std::vector<double> we;
  std::map<double,const LHCb::MCParticle*>BMD;
  std::map<double,const LHCb::MCParticle*>BM;
  std::map<double,const LHCb::MCParticle*>::iterator iBMD;
  std::map<double,const LHCb::MCParticle*>::iterator imc;
  
  LHCb::Particle::ConstVector jetDau = jet->daughtersVector();

  int k = 0;
  for(LHCb::Particle::ConstVector::const_iterator iInput = jetDau.begin();
      iInput != jetDau.end();
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


std::vector<double> LoKi::JetTagTruthMCPart::TruthCWeight(const LHCb::Particle* jet){
  
  double weight = 0;
  std::vector<double> we;
  std::map<double,const LHCb::MCParticle*>CMD;
  std::map<double,const LHCb::MCParticle*>::iterator iCMD;
  std::map<double,const LHCb::MCParticle*>CM;
  std::map<double,const LHCb::MCParticle*>::iterator imc;

  LHCb::Particle::ConstVector jetDau = jet->daughtersVector();
  int k = 0;
  for(LHCb::Particle::ConstVector::const_iterator iInput = jetDau.begin();
      iInput != jetDau.end();
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



bool LoKi::JetTagTruthMCPart::MuonDecay(const LHCb::MCParticle* mcpart){
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



const LHCb::MCParticle* LoKi::JetTagTruthMCPart::BGrandMother(const LHCb::MCParticle* mcpart){
  if (mcpart == NULL)
    return NULL;
  else if (mcpart->mother()==NULL)
    return mcpart;
  else if (mcpart->mother()->particleID().hasBottom())
    return  BGrandMother(mcpart->mother());
  else
    return mcpart;	
}



const LHCb::MCParticle* LoKi::JetTagTruthMCPart::CGrandMother(const LHCb::MCParticle* mcpart){
  if (mcpart == NULL)
    return NULL;
  else if (mcpart->mother()==NULL)
    return mcpart;
  else if (mcpart->mother()->particleID().hasCharm())
    return  CGrandMother(mcpart->mother());
  else
    return mcpart;	
}


const LHCb::MCParticle* LoKi::JetTagTruthMCPart::Bancestor(const LHCb::MCParticle* mcpart){
  if (mcpart == NULL)
    return NULL;
  else if (mcpart->mother()==NULL)
    return NULL;
  else if (mcpart->mother()->particleID().hasBottom())
    return  mcpart->mother();
  else
    return Bancestor( mcpart->mother());	
}



const LHCb::MCParticle* LoKi::JetTagTruthMCPart::Cancestor(const LHCb::MCParticle* mcpart){
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

