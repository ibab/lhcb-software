// $Id: HltSelChecker.cpp,v 1.1 2007-07-09 18:45:08 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// from DaVinci
#include "Kernel/IBackgroundCategory.h" 

// local
#include "HltSelChecker.h"

using namespace LHCb;
using namespace Gaudi;
//-----------------------------------------------------------------------------
// Implementation file for class : HltSelChecker
//
// 2007-07-06 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltSelChecker );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltSelChecker::HltSelChecker( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator )
  , m_background() 
  , m_pLinker()
{
  declareProperty("DoMC",m_doMC = true );
}
//=============================================================================
// Destructor
//=============================================================================
HltSelChecker::~HltSelChecker() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltSelChecker::initialize() {
  //=== The following two lines should be commented for DC04 algorithms ! ===
  StatusCode sc = DVAlgorithm::initialize(); 
  if ( sc.isFailure() ) return sc;

  debug() << "==> Initialize" << endmsg;

  m_background = tool<IBackgroundCategory>("BackgroundCategory",this); 
  m_pLinker = new Particle2MCLinker(this,Particle2MCMethod::Chi2,
                                    //   m_particlePath);
                                    ""); /// @todo from desktop?

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltSelChecker::execute() {

  if (msgLevel(MSG::DEBUG)) debug() << "==> Execute" << endmsg;

  // code goes here
  PartVec BCands = desktop()->particles();
  for ( P_iter b = BCands.begin(); b!=BCands.end(); ++b){
    Tuple tuple = nTuple("HltSelChecker");
    StatusCode sc = saveP("Mo",*b,tuple);
    if (!sc) return sc;
    if (msgLevel(MSG::DEBUG)) debug() << "writing Tuple " << (*b)->key()<< endmsg;
    sc = tuple->write();
    if (!sc) return sc;
  }
  if (msgLevel(MSG::DEBUG)) debug() << "Done loop" << endmsg;

  // Mandatory. Set to true if event is accepted. 
  setFilterPassed(true) ;   // Ignore returned status
  return StatusCode::SUCCESS;
}
//=========================================================================
// Save B  
//=========================================================================
StatusCode HltSelChecker::saveP(std::string part , const LHCb::Particle* P, 
                                Tuple& tuple){
  if (msgLevel(MSG::DEBUG)) debug() << "==> saveP " << P->key()
                                    << " " << P->particleID().pid() << endmsg;
  // momentum
  tuple->column( part+"Pid",  P->particleID().pid());
  tuple->column( part+"P",    P->momentum());    
  tuple->column( part+"PXe2", P->momCovMatrix()(0,0));
  tuple->column( part+"PYe2", P->momCovMatrix()(1,1));
  tuple->column( part+"PZe2", P->momCovMatrix()(2,2));
  tuple->column( part+"PEe2", P->momCovMatrix()(3,3));
  tuple->column( part+"Pt",   P->pt());
  tuple->column( part+"M",    P->measuredMass());    
  // position
  tuple->column( part+"V",    P->referencePoint());    
  tuple->column( part+"VXe2", P->posCovMatrix()(0,0));    
  tuple->column( part+"VYe2", P->posCovMatrix()(1,1));    
  tuple->column( part+"VZe2", P->posCovMatrix()(2,2));
  // vertex
  if ( NULL!=P->endVertex()){
    const Vertex* V = P->endVertex() ;
    tuple->column( part+"Chi2", V->chi2());
  }  

  // best PV
  const LHCb::VertexBase* rPV = desktop()->relatedVertex(P);

  if ( NULL==rPV ) {
    warning() << "No related PV for " << P->key() << " " 
              << P->particleID().pid() << endmsg ; 
    tuple->column( "PV", Gaudi::XYZVector(-1.,-1.,-1.)  );    
    tuple->column( part+"IP",  -1.);
    tuple->column( part+"IPe", -1.);
  } else {
    tuple->column( "PV",   rPV->position());    
    // ip
    double ip, ipe;
    geomDispCalculator()->calcImpactPar(*P, *rPV, ip, ipe);
    tuple->column( part+"IP",  ip);
    tuple->column( part+"IPe", ipe);
  }
  
  if (m_doMC) {
    StatusCode sc = saveMC(part,P,tuple);
    if (!sc) return sc;
  }
  if ( P->daughters().size()==2 ){
    StatusCode sc = twoBody(part,P,tuple);
    if (!sc) return sc;
  }

  if (msgLevel(MSG::DEBUG)) debug() << "==> saveP done for " << P->key()<< endmsg;
  return StatusCode::SUCCESS ;
  
}
//=========================================================================
//  
//=========================================================================
StatusCode HltSelChecker::twoBody(std::string part, const LHCb::Particle* P, 
                                  Tuple& tuple ) {
  if (msgLevel(MSG::DEBUG)) debug() << "==> twoBody " << P->key()<< endmsg;
  const Particle* D1 = P->daughters()[0];
  const Particle* D2 = P->daughters()[1];
  StatusCode sc = saveP("D1",D1,tuple);
  if (!sc) return sc;
  sc = saveP("D2",D2,tuple);
  if (!sc) return sc;
  double ca, cae;
  geomDispCalculator()->calcCloseAppr(*D1, *D2, ca, cae);
  tuple->column( part+"CA",  ca);
  tuple->column( part+"CAe", cae);
  info() << "Distance of " << D1->referencePoint() << " " << D1->momentum() 
         << endmsg ;
  info() << "        and " << D2->referencePoint() << " " << D2->momentum() 
         << " is " << ca << " +/- " << cae << endmsg ;  

  if (msgLevel(MSG::DEBUG)) debug() << "==> twoBody done " << P->key()<< endmsg;
  return StatusCode::SUCCESS ;
}
//=========================================================================
//  MC truth
//=========================================================================
StatusCode HltSelChecker::saveMC(std::string part, const LHCb::Particle* P, 
                                 Tuple& tuple) {
  if (msgLevel(MSG::DEBUG)) debug() << "==> saveMC " << P->key()<< endmsg;
  int cat = m_background->category(P);  
  tuple->column( part+"Bcat", cat);
  //  const MCParticle* MC = m_background->origin(P);
  double weight;
  const MCParticle* MC = m_pLinker->firstMCP(P, weight);
  if (NULL!=MC){
    tuple->column(part+"MCW",   weight);
    tuple->column(part+"MCPid", MC->particleID().pid());
    tuple->column(part+"MCP",   MC->momentum());
    tuple->column(part+"MCV",   extrapolatedMC(P->referencePoint().Z(),MC));
    tuple->column(part+"MCM",   MC->virtualMass());
  } else {
    tuple->column(part+"MCW",   -1. );
    tuple->column(part+"MCPid", 0 );
    tuple->column(part+"MCP",   Gaudi::XYZTVector(-1.,-1.,-1.,-1.));
    tuple->column(part+"MCV",   Gaudi::XYZVector(-1.,-1.,-1.));
    tuple->column(part+"MCM",   -1.);
  }
  if (msgLevel(MSG::DEBUG)) debug() << "==> saveMC  done" << P->key()<< endmsg;
  
  return StatusCode::SUCCESS;
}

//=========================================================================
// extrapolate MC point to particle Z assuming no field
//=========================================================================
Gaudi::XYZPoint HltSelChecker::extrapolatedMC(double z,
                                              const LHCb::MCParticle* MC) {
  if (NULL==MC->originVertex()) {
    return Gaudi::XYZPoint(-1.,-1.,-1.);
    Warning("No origin vertex").ignore();
  }
  Gaudi::XYZPoint p = MC->originVertex()->position();
  Gaudi::XYZVector vm = MC->momentum().Vect();
  double deltaZ = z-p.z();
  Gaudi::XYZVector v(vm.X()/vm.Z(),vm.Y()/vm.Z(),1.);
  p = p + deltaZ*v ;
  if (msgLevel(MSG::VERBOSE)) verbose() <<  MC->originVertex()->position() 
                                        << " and " << vm 
                                        << " is extrapolated to " 
                                        << p << endmsg ;
  return p;
  
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltSelChecker::finalize() {

  debug() << "==> Finalize" << endmsg;

  return DVAlgorithm::finalize(); //=== For DC04, return StatusCode::SUCCESS;
}

//=============================================================================
