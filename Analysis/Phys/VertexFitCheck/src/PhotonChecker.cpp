// $Id: PhotonChecker.cpp,v 1.2 2008-02-12 14:24:32 jpalac Exp $
// Include files

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

#include "Event/MCParticle.h"
// local
#include "PhotonChecker.h"

using namespace LHCb ;
using namespace Gaudi::Units;

//-----------------------------------------------------------------------------
// Implementation file for class : PhotonChecker
//
// 2006-05-25 : Yuehong Xie
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PhotonChecker::PhotonChecker( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : DaVinciTupleAlgorithm ( name , pSvcLocator )
{
  declareProperty( "ParticlePath", m_particlePath );
}

//=============================================================================
// Destructor
//=============================================================================
PhotonChecker::~PhotonChecker() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode PhotonChecker::initialize()
{
  StatusCode sc = DaVinciTupleAlgorithm::initialize();
  if (!sc) return sc;

  debug() << "==> Initialize" << endmsg;
  // For Particle -> MCParticle association
  m_pLinker = new Particle2MCLinker(this,
                                    Particle2MCMethod::Composite,
                                    std::vector<std::string>(1, m_particlePath));


  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PhotonChecker::execute() {

  debug() << "==> Execute" << endmsg;

  setFilterPassed(false);

  StatusCode sc = StatusCode::SUCCESS ;

  // Retrieve the particles and vertices from PhysDesktop
  const Particle::Range parts = this->particles();

  verbose() << "Found " << parts.size() << " particles in desktop" << endmsg;

  Tuples::Tuple ntuple = GaudiTupleAlg::nTuple( 500, "PhotonChecker");

  for ( LHCb::Particle::Range::const_iterator it = parts.begin() ; it!=parts.end() ; ++it){

    debug() << " (ID= " <<  (*it)->particleID().pid()
            << ") has momentum " << (*it)->momentum()/GeV
            << " and mass " <<  (*it)->measuredMass()/GeV << " GeV" << endmsg;


    if ( abs((*it)->particleID().pid())!=22 ) continue ;
    const LHCb::Particle* Part = *it;
    const LHCb::MCParticle* MCP = m_pLinker->firstMCP( Part ); ;
    if (0==MCP) debug() << "no MCParticle found " <<endmsg;
    else debug() << "MCParticle is found " << endmsg;
    if (0==MCP) continue ;

    ntuple->column("partTrue",1.);
    ntuple->column("partMass",Part->measuredMass());
    ntuple->column("partE",Part->momentum().T());
    ntuple->column("partEDiff",Part->momentum().T()-MCP->momentum().T());
    ntuple->column("partEPull",(Part->momentum().T()-MCP->momentum().T())
                   /sqrt(Part->momCovMatrix()(3,3)));

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

    double zc=-9999.;
    Gaudi::Vector3 gammapara;
    Gaudi::SymMatrix3x3 gammacov;
    sc=getPhotonParameter(*Part, zc, gammapara, gammacov);

    double xc = gammapara[0];
    double yc = gammapara[1];
    double xerr = sqrt(gammacov(0,0));
    double yerr = sqrt(gammacov(1,1));

    double mcx0 = MCP->originVertex()->position().X();
    double mcy0 = MCP->originVertex()->position().Y();
    double mcz0 = MCP->originVertex()->position().Z();

    double mcpx = MCP->momentum().X();
    double mcpy = MCP->momentum().Y();
    double mcpz = MCP->momentum().Z();

    double mcx = mcx0 + mcpx/mcpz*(zc-mcz0);
    double mcy = mcy0 + mcpy/mcpz*(zc-mcz0);

    ntuple->column("clustZ",zc);
    ntuple->column("clustX",xc);
    ntuple->column("clustY",yc);
    ntuple->column("clustXDiff",xc-mcx);
    ntuple->column("clustYDiff",yc-mcy);
    ntuple->column("clustXPull",(xc-mcx)/xerr);
    ntuple->column("clustYPull",(yc-mcy)/yerr);

    sc = ntuple->write();
    if( sc.isFailure() )
      return Error( "Cannot fill ntuple" );


  }

  return sc;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode PhotonChecker::finalize()
{

  debug() << "==> Finalize" << endmsg;

  delete m_pLinker;
  m_pLinker = NULL;

  return DaVinciTupleAlgorithm::finalize();
}

StatusCode PhotonChecker::getPhotonParameter(const LHCb::Particle& photon,
                                             double& zg,
                                             Gaudi::Vector3& para ,
                                             Gaudi::SymMatrix3x3& cov ) const
{
  StatusCode sc = StatusCode::SUCCESS;

  int pid=photon.particleID().pid();
  if(pid!=22) {
    err() <<"Particle is not a photon!"<<endmsg;
    return StatusCode::FAILURE;
  }

  const LHCb::ProtoParticle*   proto  = photon.proto() ;
  if( 0 == proto  ) {
    err() <<"ProtoParticle points to NULL!"<<endmsg;
    return StatusCode::FAILURE;
  }

  if( proto->calo().empty() ) {
    err() <<"ProtoParticle has no CaloHypos "<<endmsg;
    return StatusCode::FAILURE;
  }
  typedef const SmartRefVector<CaloHypo> Hypos;
  const Hypos& hypos = proto->calo();
  Hypos::const_iterator ihypo =
    std::find_if( hypos.begin () , hypos.end () , IsHypo( CaloHypo::Photon ) ) ;

  if( hypos.end() == ihypo )  {
    err() <<" CaloHypothesis 'Photon' is not found ";
    return StatusCode::FAILURE;
  }
  const CaloHypo* hypo = *ihypo ;

  // get the position
  const CaloPosition* pos = hypo->position() ;
  if( 0 == pos    ) {
    err() <<"CaloPosition* points to NULL! "<<endmsg;
    return StatusCode::FAILURE;
  }
  zg=pos->z();
  para(0)=pos->x();
  para(1)=pos->y();
  para(2)=pos->e();
  cov=pos -> covariance();
  verbose() <<"Photon parameters: " <<para<<endmsg;
  verbose() <<"Photon cov : " <<cov<<endmsg;

  return sc;
}

// ===========================================================================
// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PhotonChecker )
// ===========================================================================
