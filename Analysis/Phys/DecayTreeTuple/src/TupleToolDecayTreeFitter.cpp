// $Id: $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "TupleToolDecayTreeFitter.h"

#include <Kernel/GetDVAlgorithm.h>
#include <Kernel/DVAlgorithm.h>
#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"
#include "Event/RecVertex.h"
#include "Event/VertexBase.h"
#include "Event/Particle.h"


#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"


// Boost
#include <boost/assign/std/vector.hpp>
#include <boost/assign/list_of.hpp>

using namespace LHCb;





//-----------------------------------------------------------------------------
// Implementation file for class : TupleToolDecayTreeFitter
// Yasmine Amhis and Matt Needham. 
// 30-10-10 
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TupleToolDecayTreeFitter );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolDecayTreeFitter::TupleToolDecayTreeFitter( const std::string& type,
                                                    const std::string& name,
                                                    const IInterface* parent )
  : TupleToolBase ( type, name , parent )
  , m_dva(0)
  ,m_pidJpsi(443)
  ,m_pidLambda(3122)
  ,m_pidKS0(310)
  ,m_pidPhi(333)
  ,m_pidKst0(313)
  
{
  declareProperty("particleToFit", m_mother = boost::assign::list_of("Lambda_b0"));
  declareProperty("daughterToConstrain", m_daughter = boost::assign::list_of("J/psi(1S)"));
  declareProperty("daughtersToConstrain", m_daughters = boost::assign::list_of("J/psi(1S)"));
  declareProperty("constrainToPV", m_constrainToPV = true);
  declareProperty("constrainToOneDaughter", m_constrainOneDaughter = true);
  declareProperty("constrainToAllDaughters", m_constrainAllDaughters = true);
  declareProperty("refit", m_refit = true);
  declareProperty("Verbose", m_verbose  = true);
  declareInterface<IParticleTupleTool>(this);
  
}
//=============================================================================
StatusCode TupleToolDecayTreeFitter::initialize()
{
  if( ! TupleToolBase::initialize() ) return StatusCode::FAILURE;
  
   
  // convert the list of names to a list of pids 
  LHCb::IParticlePropertySvc* propertysvc = 
      svc<LHCb::IParticlePropertySvc>("LHCb::ParticlePropertySvc",true) ;
  for (std::vector<std::string>::const_iterator iterS = m_daughters.begin(); iterS != m_daughters.end(); ++iterS ){
    const LHCb::ParticleProperty* prop = propertysvc->find( *iterS );
    m_daughtersPids.push_back(prop->pdgID());
  } // iterS

   for (std::vector<std::string>::const_iterator iterS = m_daughter.begin(); iterS != m_daughter.end(); ++iterS ){
    const LHCb::ParticleProperty* prop = propertysvc->find( *iterS );
    m_daughterPids.push_back(prop->pdgID());
   } // iterS
  
   
   for (std::vector<std::string>::const_iterator iterS = m_mother.begin(); iterS != m_mother.end(); ++iterS ){
     const LHCb::ParticleProperty* prop = propertysvc->find( *iterS );
     m_motherPid.push_back(prop->pdgID());
     
   } // iter the mothers
   
  propertysvc->release(); 
 
  m_dva = Gaudi::Utils::getDVAlgorithm ( contextSvc() ) ;
  if (0==m_dva) return Error("Couldn't get parent DVAlgorithm", 
                             StatusCode::FAILURE);
  
  return StatusCode::SUCCESS;
}

//=============================================================================

StatusCode TupleToolDecayTreeFitter::fill(const DecayTreeFitter::Fitter& fitter, 
                                      const Particle* P,
                                      const std::string& prefix, 
                                      const Tuples::Tuple& tuple) const{
   
   bool test = true; 

   test &= tuple->column( prefix+"_status", fitter.status() );
   test &= tuple->column( prefix+"_nDOF", fitter.nDof() );
   test &= tuple->column( prefix+"_chi2_B", fitter.chiSquare() );
   test &= tuple->column( prefix+"_nIter", fitter.nIter() );
     
   //Get the fit parameters
   const Gaudi::Math::ParticleParams* params = fitter.fitParams(P) ;
   Gaudi::Math::LorentzVectorWithError momentum = params->momentum() ; 
      
   test &= tuple->column( prefix+"_M",  momentum.m().value() );
   test &= tuple->column( prefix+"_MERR", momentum.m().error());
   test &= tuple->column( prefix+"_P", momentum.p().value());
   test &= tuple->column( prefix+"_PERR", momentum.p().error()) ;//MeV

   return StatusCode(test);
}
//=============================================================================
StatusCode TupleToolDecayTreeFitter::fill( const LHCb::Particle* mother
                                           , const LHCb::Particle* P
                                           , const std::string& head
                                           , Tuples::Tuple& tuple ){

  const std::string prefix=fullName(head);
  bool test = true;
  if( !P ) return StatusCode::FAILURE;
  
  for (std::vector<LHCb::ParticleID>::const_iterator iterC = m_motherPid.begin();  
       iterC != m_motherPid.end(); ++iterC){//loop over all the mothers
    unsigned motherPID =   abs(P->particleID().pid());
    debug() << "This is the particle we are looking at : "  << motherPID << endmsg; 
    if(iterC->abspid() == motherPID ){
      //-----------------
      // apply a refit
      //-----------------
      if (m_refit == true){
	debug() << "refit: Only the B " << endmsg; 
	DecayTreeFitter::Fitter refitterOnly(*P);
	refitterOnly.fit();
	fill(refitterOnly,P,prefix+"_refit",tuple );
      }
      //--------------------------
      // apply a PV constraint
      //-------------------------
      if (m_constrainToPV){
	debug() << "refit: Constrain the PV" << endmsg; 
	// check for origin 
	const VertexBase* originVtx = NULL;
	if (mother != P){
	  originVtx = originVertex( mother, P ); // the origin vertex is somewhere in the decay
	} else { // the origin vertex is the primary.
    originVtx = m_dva->bestPV( mother );
	}
	
	if( originVtx ){} // I'm happy
	else {
	  Error("Can't get the origin vertex");
	  return StatusCode::FAILURE;
	} 
	
	DecayTreeFitter::Fitter fitterWithPV( *P, *originVtx );
	fitterWithPV.fit();
	
	std::string taufit = "_taufit";
	if (m_verbose == true){filldaughters(fitterWithPV, P,taufit,tuple);}
	taufit = prefix+"_taufit";
	fill(fitterWithPV,P,taufit,tuple );
	const Gaudi::Math::ParticleParams* tParams = fitterWithPV.fitParams(P); 
	Gaudi::Math::ValueWithError decayLength = tParams->decayLength();
	Gaudi::Math::ValueWithError ctau = tParams->ctau();
	test &= tuple->column( taufit+"ctau", ctau.value() );
	test &= tuple->column( taufit+"ctauErr", ctau.error() );
	test &= tuple->column( taufit+"_decayLength", decayLength.value() );
	test &= tuple->column( taufit+"_decayLength", decayLength.error() );
	
	
      }//PV fit


      
      //----------------------------------------------
      // apply a constrained fit with the daughters 
      //----------------------------------------------
      if (m_constrainOneDaughter && m_daughterPids.size() != 0){
	debug() << "refit: Constrain a daughter" << endmsg; 
	// make the constraints you want 
	DecayTreeFitter::Fitter dfitter( *P );
	for (std::vector<LHCb::ParticleID>::const_iterator iterC = m_daughterPids.begin();  
	     iterC != m_daughterPids.end(); ++iterC) dfitter.setMassConstraint(*iterC);
	debug() <<"number of daughters to be constrained " << m_daughterPids.size() << endmsg;
	dfitter.fit();
	fill(dfitter,P, prefix+"_massConOneDaughter",tuple);
      }
      
      
      if (m_constrainAllDaughters && m_daughtersPids.size() != 0){
	debug() << "refit: Constrain all daughter" << endmsg; 
	// make the constraints you want 
	DecayTreeFitter::Fitter dfitter( *P );
	for (std::vector<LHCb::ParticleID>::const_iterator iterC = m_daughtersPids.begin();  
	     iterC != m_daughtersPids.end(); ++iterC) dfitter.setMassConstraint(*iterC);
	debug() <<"number of daughters to be constrained " << m_daughtersPids.size() << endmsg;
	dfitter.fit();
	fill(dfitter,P, prefix+"_massConAllDaughters",tuple);
      }
      //apply a constrained fit with 2 daughters
    }  //hard code the mothers} 
  }//loop over all the mothers
  return StatusCode(test);
}



//=============================================================================

StatusCode TupleToolDecayTreeFitter::filldaughters( const DecayTreeFitter::Fitter& fitter
						    ,const LHCb::Particle* P
						    ,const std::string& prefix
						    ,const Tuples::Tuple& tuple)const{
  bool test = true;
  
  // if (  P->isBasicParticle()) return StatusCode::SUCCESS;
  LHCb::Particle::ConstVector daughters = P->daughtersVector();
  debug() << "for id " << P->particleID().pid() << " daugh size is " << daughters.size() << endmsg;
  if ( daughters.size()==0 ) return test;
  for (LHCb::Particle::ConstVector::iterator it = daughters.begin();it<daughters.end(); it++) {
    const LHCb::Particle* particle = *it;
    unsigned int pid = abs(particle->particleID().pid());
    std::string name =   getName(pid)+ prefix; ;
    const Gaudi::Math::ParticleParams* daughtersparameters = fitter.fitParams(particle);
    Gaudi::Math::ValueWithError decayLength = daughtersparameters->decayLength();
    Gaudi::Math::ValueWithError ctau = daughtersparameters->ctau();
    test &= tuple->column( name+"ctau", ctau.value() );
    test &= tuple->column( name+"ctauErr", ctau.error() );
    test &= tuple->column( name+"_decayLength", decayLength.value() );
    test &= tuple->column( name+"_decayLength", decayLength.error() );
    
    //loop over the daughters
    
  }
  return StatusCode(test);
  
}

//=============================================================================

//=============================================================================
// Destructor
//=============================================================================
TupleToolDecayTreeFitter::~TupleToolDecayTreeFitter() {} 

//=============================================================================
const Vertex* TupleToolDecayTreeFitter::originVertex( const Particle* top
						 , const Particle* P ) const {
  if( top == P || P->isBasicParticle() ) return NULL;

  const SmartRefVector< LHCb::Particle >& dau = top->daughters ();
  if( dau.empty() ) return NULL;

  SmartRefVector< LHCb::Particle >::const_iterator it;
  for( it = dau.begin(); dau.end()!=it; ++it ){
    if( P == *it ){
      return top->endVertex();
    }
  }
  
  // vertex not yet found, get deeper in the decay:
  for( it = dau.begin(); dau.end()!=it; ++it ){
    if( P != *it && !(*it)->isBasicParticle() ){
      const Vertex* vv = originVertex( *it, P );
      if( vv ) return vv;
    }
  }
  return NULL;
}


//=============================================================================
// Convert pid number in names
//=============================================================================
std::string TupleToolDecayTreeFitter::getName(int id) const {
  std::string name = "";
  unsigned int pid = abs(id);
  if (pid == m_pidKS0)
    name = "KS0";
  else if (pid == m_pidJpsi )
    name  = "J_psi_1S"; 
  else if (pid == m_pidLambda)
    name = "Lambda0"; 
  else if (pid == m_pidPhi)
    name = "Phi"; 
  else if (pid == m_pidKst0)
    name = "K_star0";
 
  return name;
}
