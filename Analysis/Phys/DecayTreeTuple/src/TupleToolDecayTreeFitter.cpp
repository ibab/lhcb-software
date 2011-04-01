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
#include "Kernel/ParticleProperty.h"
#include "Kernel/IParticleDescendants.h"
#include "Kernel/Escape.h"
#include "DecayTreeFitter/Fitter.h"
#include "Kernel/IParticlePropertySvc.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : TupleToolDecayTreeFitter
// Yasmine Amhis, Matt Needham, Patrick Koppenburg
// 30-10-2010, 01-04-2011 
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
  , m_dva()
  , m_ppSvc()
  , m_particleDescendants()
  
{
  declareProperty("daughtersToConstrain", m_massConstraints , "List of particles to contrain to mass");
  declareProperty("constrainToOriginVertex", m_constrainToOriginVertex = false, 
                  "Do a refit constraining to Origin Vertex (could be PV)");
  declareInterface<IParticleTupleTool>(this);
  
}
//=============================================================================
StatusCode TupleToolDecayTreeFitter::initialize()
{
  if( ! TupleToolBase::initialize() ) return StatusCode::FAILURE;
  
   
  // convert the list of names to a list of pids 
  const LHCb::ParticleProperty* prop = 0 ;
  m_ppSvc = svc<LHCb::IParticlePropertySvc>("LHCb::ParticlePropertySvc",true) ;
  for (std::vector<std::string>::const_iterator iterS = m_massConstraints.begin(); 
       iterS != m_massConstraints.end(); ++iterS ){
    prop = m_ppSvc->find( *iterS );
    if (!prop)  Exception("Unknown PID");
    m_massConstraintsPids.push_back(prop->pdgID());
  } // iterS

  m_dva = Gaudi::Utils::getDVAlgorithm ( contextSvc() ) ;
  if (0==m_dva) return Error("Couldn't get parent DVAlgorithm", StatusCode::FAILURE);

  m_particleDescendants = tool<IParticleDescendants> ( "ParticleDescendants");

  if ("" == m_extraName ){ 
    std::string en = name() ; // use tool name as prepended name
    unsigned int d = en.find_last_of(".");
    m_extraName = en.substr(d+1,en.size()-1); // from d to end
    if ( "TupleToolDecayTreeFitter" == m_extraName )  m_extraName = ""; // user has not chanegd instance name
    info() << "All fields will be prepended with ``" << m_extraName << "''" <<endmsg;
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Fill standard stuff
//=============================================================================
StatusCode TupleToolDecayTreeFitter::fill(const DecayTreeFitter::Fitter* fitter, 
                                          const Particle* P,
                                          const std::string& prefix, 
                                          const Tuples::Tuple& tuple) const{
   
  bool test = true; 

  test &= tuple->column( prefix+"_status", fitter->status() );
  test &= tuple->column( prefix+"_nDOF", fitter->nDof() );
  test &= tuple->column( prefix+"_chi2_B", fitter->chiSquare() );
  test &= tuple->column( prefix+"_nIter", fitter->nIter() );
     
  //Get the fit parameters
  const Gaudi::Math::ParticleParams* params = fitter->fitParams(P) ;
  Gaudi::Math::LorentzVectorWithError momentum = params->momentum() ; 
      
  test &= tuple->column( prefix+"_M",  momentum.m().value() );
  test &= tuple->column( prefix+"_MERR", momentum.m().error());
  test &= tuple->column( prefix+"_P", momentum.p().value());
  test &= tuple->column( prefix+"_PERR", momentum.p().error()) ;//MeV

  return StatusCode(test);
}
//=============================================================================
// Fill lifetime stuff
//=============================================================================
StatusCode TupleToolDecayTreeFitter::fillLT(const DecayTreeFitter::Fitter* fitter, 
                                          const Particle* P,
                                          const std::string& prefix, 
                                          const Tuples::Tuple& tuple) const{
   
  bool test = true; 

  const Gaudi::Math::ParticleParams* tParams = fitter->fitParams(P); 
  Gaudi::Math::ValueWithError decayLength = tParams->decayLength();
  Gaudi::Math::ValueWithError ctau = tParams->ctau();
  test &= tuple->column( prefix+"_ctau", ctau.value() );
  test &= tuple->column( prefix+"_ctauErr", ctau.error() );
  test &= tuple->column( prefix+"_decayLength", decayLength.value() );
  test &= tuple->column( prefix+"_decayLengthErr", decayLength.error() );
  
  return StatusCode(test);
}
//=============================================================================
// Fill lifetime information for non stable daughters
//=============================================================================
StatusCode TupleToolDecayTreeFitter::fillDaughters( const DecayTreeFitter::Fitter* fitter
                                                    ,const LHCb::Particle* P
                                                    ,const std::string& prefix
                                                    ,const Tuples::Tuple& tuple)const{
  bool test = true;
  
  LHCb::Particle::ConstVector daughters = m_particleDescendants->descendants(P);
  if (msgLevel(MSG::DEBUG)) debug() << "for id " << P->particleID().pid() << " daughter size is " << daughters.size() << endmsg;
  if ( daughters.size()==0 ) return test;
  for (LHCb::Particle::ConstVector::iterator it = daughters.begin();it<daughters.end(); it++) {
    const LHCb::Particle* particle = *it;
    if ( particle->isBasicParticle()) continue ;
    unsigned int pid = abs(particle->particleID().pid());
    std::string name = prefix+"_"+getName(pid) ;
    test &= fillLT(fitter,particle,name,tuple);
  }
  return StatusCode(test);
}

//=============================================================================
//  Fill fit inforation for top decay
//=============================================================================
StatusCode TupleToolDecayTreeFitter::fill( const LHCb::Particle* mother
                                           , const LHCb::Particle* P
                                           , const std::string& head
                                           , Tuples::Tuple& tuple ){

  const std::string prefix=fullName(head);
  bool test = true;
  if( !P ) return StatusCode::FAILURE;
  
  // Get the fitter
  DecayTreeFitter::Fitter* fitter = 0 ;

  if (m_constrainToOriginVertex){  
    if (msgLevel(MSG::DEBUG)) debug() << "Constrain the origin vertex" << endmsg; 
    // check for origin 
    const VertexBase* originVtx = NULL;
    if (mother != P){
      originVtx = originVertex( mother, P ); // the origin vertex is somewhere in the decay
    } else { // the origin vertex is the primary.
      originVtx = m_dva->bestPV( mother );
    }
    if( !originVtx ){
      Error("Can't get the origin vertex");
      return StatusCode::FAILURE;
    }
    fitter = new DecayTreeFitter::Fitter(*P, *originVtx);
  } else {
    if (msgLevel(MSG::DEBUG)) debug() << "Do not contrain the origin vertex" << endmsg; 
    fitter = new DecayTreeFitter::Fitter(*P); 
  }

  //add mass contraints
  if (!m_massConstraintsPids.empty()){
    for (std::vector<LHCb::ParticleID>::const_iterator iterC = m_massConstraintsPids.begin();  
         iterC != m_massConstraintsPids.end(); ++iterC) fitter->setMassConstraint(*iterC);
  }

  // fit
  fitter->fit();


  // fill the fit result
  fill(fitter,P,prefix,tuple );
  if (m_constrainToOriginVertex){
    fillLT(fitter,P,prefix,tuple );
    test &= fillDaughters(fitter,P,prefix,tuple );
  }

  delete fitter ;
  return StatusCode(test);
}

//=============================================================================
// get origin vertex
//=============================================================================
const VertexBase* TupleToolDecayTreeFitter::originVertex( const Particle* top, const Particle* P ) const {
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
      const VertexBase* vv = originVertex( *it, P );
      if( vv ) return vv;
    }
  }
  return NULL;
}


//=============================================================================
// Convert pid number in names
//=============================================================================
std::string TupleToolDecayTreeFitter::getName(int id) const {
  const LHCb::ParticleProperty* prop = m_ppSvc->find( LHCb::ParticleID(id) );
  if (!prop) Exception("Unknown PID");
  return Decays::escape(prop->name());
}
