// $Id: VtxChecker.cpp,v 1.2 2008-02-12 14:24:32 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

#include "Event/MCParticle.h"
// local
#include "VtxChecker.h"

using namespace LHCb ;
using namespace Gaudi::Units;

//-----------------------------------------------------------------------------
// Implementation file for class : VtxChecker
//
// 2006-05-25 : Yuehong Xie
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( VtxChecker )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VtxChecker::VtxChecker( const std::string& name,
                      ISvcLocator* pSvcLocator)
  : DaVinciTupleAlgorithm ( name , pSvcLocator )
{
   declareProperty( "ParticlePath", m_particlePath );
   declareProperty( "pidToCheck", m_pidToCheck );

}
//=============================================================================
// Destructor
//=============================================================================
VtxChecker::~VtxChecker() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode VtxChecker::initialize() {
  StatusCode sc = DaVinciTupleAlgorithm::initialize();
  if (!sc) return sc;

  debug() << "==> Initialize" << endmsg;
  // For Particle -> MCParticle association
  m_pLinker = new Particle2MCLinker(this,
                                    Particle2MCMethod::Composite,
                                    std::vector<std::string>(1,m_particlePath));


  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode VtxChecker::execute() {

  debug() << "==> Execute" << endmsg;

  setFilterPassed(false); 

  StatusCode sc = StatusCode::SUCCESS ;

  // Retrieve the particles and vertices from PhysDesktop
  const Particle::Range parts = this->particles();

  verbose() << "Found " << parts.size() << " particles in local storage" << endmsg;

  Tuples::Tuple ntuple = GaudiTupleAlg::nTuple( 100, "VtxChecker");

  for ( LHCb::Particle::Range::const_iterator it = parts.begin() ; it!=parts.end() ; ++it){
    
    debug() << " (ID= " <<  (*it)->particleID().pid()
            << ") has momentum " << (*it)->momentum()/GeV
            << " and mass " <<  (*it)->measuredMass()/GeV << " GeV" << endmsg;


    if ( abs((*it)->particleID().pid())!=m_pidToCheck ) continue ;
    const LHCb::Particle* Part = *it;
    const LHCb::MCParticle* MCP = m_pLinker->firstMCP( Part ); ;
    if (0==MCP) debug() << "no MCParticle found " <<endmsg;
    else debug() << "MCParticle is found " << endmsg;
    if (0==MCP) continue ;
    sc = plotParameters(Part , MCP, "Particle ");

    ntuple->column("partTrue",1.);
    ntuple->column("partFromB",isBChild(MCP));
    ntuple->column("partMass",Part->measuredMass());
    ntuple->column("partMassErr",Part->measuredMassErr());
    ntuple->column("partChi2",Part->endVertex()->chi2());
    ntuple->column("partDoF",Part->endVertex()->nDoF());
    ntuple->column("partVx",Part->referencePoint().X());
    ntuple->column("partVy",Part->referencePoint().Y());
    ntuple->column("partVz",Part->referencePoint().Z());
    ntuple->column("partVxDiff",Part->referencePoint().X()-MCP->endVertices().front()->position().X());
    ntuple->column("partVyDiff",Part->referencePoint().Y()-MCP->endVertices().front()->position().Y());
    ntuple->column("partVzDiff",Part->referencePoint().Z()-MCP->endVertices().front()->position().Z());
    ntuple->column("partVxPull",(Part->referencePoint().X()-MCP->endVertices().front()->position().X())
                                 /sqrt(Part->posCovMatrix()(0,0)));
    ntuple->column("partVyPull",(Part->referencePoint().Y()-MCP->endVertices().front()->position().Y())
                                 /sqrt(Part->posCovMatrix()(1,1)));
    ntuple->column("partVzPull",(Part->referencePoint().Z()-MCP->endVertices().front()->position().Z())
                                 /sqrt(Part->posCovMatrix()(2,2)));

    ntuple->column("partPx",Part->momentum().X());
    ntuple->column("partPy",Part->momentum().Y());
    ntuple->column("partPz",Part->momentum().Z());
    ntuple->column("partPxDiff",Part->momentum().X()-MCP->momentum().X());
    ntuple->column("partPyDiff",Part->momentum().Y()-MCP->momentum().Y());
    ntuple->column("partPzDiff",Part->momentum().Z()-MCP->momentum().Z());
    ntuple->column("partPxPull",(Part->momentum().X()-MCP->momentum().X())
                                /sqrt(Part->momCovMatrix()(0,0)));
    ntuple->column("partPyPull",(Part->momentum().Y()-MCP->momentum().Y())
                                /sqrt(Part->momCovMatrix()(1,1)));
    ntuple->column("partPzPull",(Part->momentum().Z()-MCP->momentum().Z())
                                /sqrt(Part->momCovMatrix()(2,2)));


    sc = ntuple->write();
    if( sc.isFailure() )
    return Error( "Cannot fill ntuple" );


  }

  return sc;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode VtxChecker::finalize() {

  debug() << "==> Finalize" << endmsg;

  delete m_pLinker;

  return DaVinciTupleAlgorithm::finalize();
}

//=============================================================================
//  Nicely print a Particle
//=============================================================================
StatusCode 
VtxChecker::printParticle(const LHCb::Particle::ConstVector &parts) const 
{
  StatusCode sc = StatusCode::SUCCESS;
  for ( LHCb::Particle::ConstVector::const_iterator i = parts.begin();
        i != parts.end(); ++i )
  {
    sc = sc && printParticle(*i) ;
  }
  return sc ; 
}

//=============================================================================
//  Nicely print a Particle
//=============================================================================
StatusCode VtxChecker::printParticle(const LHCb::Particle* p) const {
  StatusCode sc = StatusCode::SUCCESS ;
  if ( 0==p ) return StatusCode::FAILURE ;
  if (msgLevel(MSG::INFO)){
    info() << "Going to print details for Particle " ;
    if ( p->hasKey () )  info() << p->key () << " " ;
    info() << " " << p->particleID().pid() << "\n" ;
    
    boost::format mom("%1$9.1f");
    boost::format pos("%1$9.3f");
    
    info() << "Vx " << pos % p->referencePoint().X() ; printMatrix<Gaudi::SymMatrix7x7>(p->covMatrix(),0) ;
    info() << "Vy " << pos % p->referencePoint().Y() ; printMatrix<Gaudi::SymMatrix7x7>(p->covMatrix(),1) ;
    info() << "Vz " << pos % p->referencePoint().Z() ; printMatrix<Gaudi::SymMatrix7x7>(p->covMatrix(),2) ;
    info() << "Px " << mom % p->momentum().X() ; printMatrix<Gaudi::SymMatrix7x7>(p->covMatrix(),3) ;
    info() << "Py " << mom % p->momentum().Y() ; printMatrix<Gaudi::SymMatrix7x7>(p->covMatrix(),4) ;
    info() << "Pz " << mom % p->momentum().Z() ; printMatrix<Gaudi::SymMatrix7x7>(p->covMatrix(),5) ;
    info() << "E  " << mom % p->momentum().E() ; printMatrix<Gaudi::SymMatrix7x7>(p->covMatrix(),6) ;
    info() << "M  " << mom % p->measuredMass() << "\n" ;
    info() << endmsg ;
  }
 
  return sc ;
  
}

//=============================================================================
// error matrix printing
//=============================================================================
template<class TYPE> 
StatusCode VtxChecker::printMatrix(const TYPE& m, int line, int cols) const {
  
  if (msgLevel(MSG::INFO)){
    boost::format posmom("%1$8.3f");
    boost::format pos2("%1$8.5f");
    boost::format mom2("%1$8.3f");
    boost::format fmt ;
  
    info() << " " ;
 
    for ( int i=0; i!=cols; i++){
      if ( m(line,i) != 0. ){
        if ( line<3 && i<3 ) fmt = pos2 ;
        else if ( line>=3 && i>=3 ) fmt = mom2 ;
        else fmt = posmom ;
        info() << fmt % m(line,i) ;
      } else info() << "        " ;
    }
    info() << "\n" ;
  }
  
  return  StatusCode::SUCCESS ;
  
}
//=============================================================================
//  Nicely print a Vertex
//=============================================================================
StatusCode VtxChecker::printVertex(const LHCb::Vertex* v) const {
  if ( 0==v ) return StatusCode::FAILURE ;
  if (msgLevel(MSG::INFO)){
    info() << "Going to print details for Vertex " ;
    if ( v->hasKey () )  info() << v->key () << " " ;
    info() << "\n" ;
  
    boost::format mom("%1$9.1f");
    boost::format pos("%1$9.3f");

    info() << "Vx " << pos % v->position().X() ; printMatrix<Gaudi::SymMatrix3x3>(v->covMatrix(),0,3) ;
    info() << "Vy " << pos % v->position().Y() ; printMatrix<Gaudi::SymMatrix3x3>(v->covMatrix(),1,3) ;
    info() << "Vz " << pos % v->position().Z() ; printMatrix<Gaudi::SymMatrix3x3>(v->covMatrix(),2,3) ;
    info() << "Chi2 " << v->chi2() << " products " << v->outgoingParticlesVector().size() ;
    info() << "\n" << endmsg ;
  }
  
  return StatusCode::SUCCESS ;
  
}

//=============================================================================
// Tests IPs with pions from PV
//=============================================================================
StatusCode VtxChecker::plotParameters(const LHCb::Particle* P, const LHCb::MCParticle* MCP, const std::string& head) const{

  StatusCode sc = StatusCode::SUCCESS;

  plot( P->endVertex()->chi2() ,head+"Chi2",0., 100.);
  plot( P->measuredMass() ,head+"Ks Mass", 400., 600.);
  plot( P->measuredMass() ,head+"Psi Mass",3000., 3300.);
  plot( P->measuredMass() ,head+"B Mass",4000., 6000.);


  plot(P->momentum().X(),head+"Px",-3.*GeV,3.*GeV);
  plot(P->momentum().Y(),head+"Py",-3.*GeV,3.*GeV);
  plot(P->momentum().Z(),head+"Pz",0,100.*GeV);
  plot(P->momentum().X()-MCP->momentum().X(),head+"Px diff",-0.1*GeV,0.1*GeV);
  plot(P->momentum().Y()-MCP->momentum().Y(),head+"Py diff",-0.1*GeV,0.1*GeV);
  plot(P->momentum().Z()-MCP->momentum().Z(),head+"Pz diff",-0.2*GeV,0.2*GeV);
  plot((P->momentum().X()-MCP->momentum().X())/sqrt(P->momCovMatrix()(0,0)),head+"Px pull",-5,5);
  plot((P->momentum().Y()-MCP->momentum().Y())/sqrt(P->momCovMatrix()(1,1)),head+"Py pull",-5,5);
  plot((P->momentum().Z()-MCP->momentum().Z())/sqrt(P->momCovMatrix()(2,2)),head+"Pz pull",-5,5);

  plot(P->referencePoint().X(),head+"Vx",-4*mm,4*mm);
  plot(P->referencePoint().Y(),head+"Vy",-4*mm,4*mm);
  plot(P->referencePoint().Z(),head+"Vz",-150*mm,700.*mm);

  plot(P->referencePoint().X()-MCP->endVertices().front()->position().X(),head+"Vx diff",-0.4*mm,0.4*mm);
  plot(P->referencePoint().Y()-MCP->endVertices().front()->position().Y(),head+"Vy diff",-0.4*mm,0.4*mm);
  plot(P->referencePoint().Z()-MCP->endVertices().front()->position().Z(),head+"Vz diff",-2.*mm,2.*mm);

  plot((P->referencePoint().X()-MCP->endVertices().front()->position().X())/sqrt(P->posCovMatrix()(0,0)),head+"Vx pull",-5,5);
  plot((P->referencePoint().Y()-MCP->endVertices().front()->position().Y())/sqrt(P->posCovMatrix()(1,1)),head+"Vy pull",-5,5);
  plot((P->referencePoint().Z()-MCP->endVertices().front()->position().Z())/sqrt(P->posCovMatrix()(2,2)),head+"Vz pull",-5,5);
    
  return sc;
  
}

//=============================================================================
// check if a MCParticle is B or from B
//=============================================================================
double VtxChecker::isBChild(const LHCb::MCParticle* MCP) const
{
  if (!MCP) return 0.;
  if(MCP->particleID().hasBottom()) return 1.;
  const MCParticle* mother=MCP->mother() ;
  return isBChild(mother);
}
