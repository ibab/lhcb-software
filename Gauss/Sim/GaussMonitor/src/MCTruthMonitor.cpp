// $Id: MCTruthMonitor.cpp,v 1.5 2009-03-26 21:45:04 robbep Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

// from LHCb
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"

// local1
#include "MCTruthMonitor.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MCTruthMonitor
//
// 2005-08-12 : Gloria CORTI
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MCTruthMonitor )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCTruthMonitor::MCTruthMonitor( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator )
{
  declareProperty( "DetailedHistos", m_detailedHistos = false );  
  declareProperty( "MCParticles"   , 
                   m_mcParticles = LHCb::MCParticleLocation::Default ) ;
  declareProperty( "MCVertices" , 
                   m_mcVertices = LHCb::MCVertexLocation::Default ) ;
}
//=============================================================================
// Destructor
//=============================================================================
MCTruthMonitor::~MCTruthMonitor() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode MCTruthMonitor::initialize() {
  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  m_hNPart   = book(100, "Multiplicity MCparticles", 0., 1000., 100);
  m_hNVert   = book(200, "Multiplicity MCVertices", 0., 200., 100);

  m_hPOrigT   = book(101, "Vertex type of particles", 0., 20., 20);
  m_hPOrigZ   = book(102, "Z origin of all particles (mm)", -1000., 20000., 2100);  
  m_hPMom     = book(103, "Momentum of all particles (GeV)", 0., 1., 200);
  m_hPPrimMom = book(104, "Momentum of primary particles (GeV)", 0., 100., 50);
  m_hPProtMom = book(105, "Momentum of protons particles (GeV)", 0., 100., 50);

  m_hVType   = book(201, "Vertex type", 0., 20., 20);
  m_hVZpos   = book(202, "Z position of all vertices (mm)", -1000., 20000., 2100);
  m_hVZpos2  = book(203, "Z position of all vertices (mm)", -10000., 10000., 200);
  m_hVTime   = book(204, "Time of all vertices (ns)", -20., 100., 120);

  if( !detailedHistos() ) {
    return StatusCode::SUCCESS;
  }

  m_hNProtons   = book(111, "Number of protons", 0., 100., 50);
  m_hNNeutrons  = book(112, "Number of neutrons", 0., 100., 50);
  m_hNChPions   = book(113, "Number of charged pions", 0., 200., 100);
  m_hNPiZeros   = book(114, "Number of pi zeros", 0., 200., 100);
  m_hNChKaons   = book(115, "Number of charged kaons", 0., 50., 50);
  m_hNKs        = book(116, "Number of Ks", 0., 50., 50);
  m_hNElectrons = book(117, "Number of e+/e-", 0., 200., 100);
  m_hNMuons     = book(118, "Number of mu+mu-", 0., 100., 100);
  m_hNGammas    = book(119, "Number of gammas", 0., 200., 100);
  m_hNBeauty    = book(120, "Number of particles with b/b~ quark", 0., 10., 10);
  m_hNCharm     = book(121, "Number of particles with c/c~ quark", 0., 20., 20);
  m_hNNuclei    = book(122, "Number of nuclei", 0., 50., 50);

  return StatusCode::SUCCESS;
}
 
//=============================================================================
// Main execution
//=============================================================================
StatusCode MCTruthMonitor::execute() {

  debug() << "==> Execute" << endmsg;

  unsigned int nProtons = 0, nNeutrons = 0, nChPions = 0, nPiZeros = 0;
  unsigned int nChKaons = 0, nKs = 0, nElectrons = 0, nMuons = 0, nGammas = 0;
  unsigned int nBeauty = 0, nCharm = 0, nNuclei = 0;

  const LHCb::MCParticles* particles = 
    get<LHCb::MCParticles>( m_mcParticles );
  m_hNPart->fill(particles->size());
  LHCb::MCParticles::const_iterator ip;
  for( ip = particles->begin(); particles->end() != ip; ++ip ) {
    m_hPOrigZ->fill((*ip)->originVertex()->position().z()/Gaudi::Units::mm);
    int vtype = (*ip)->originVertex()->type();
    if( vtype >=100 ) { vtype = vtype - 100 + 10; }
    m_hPOrigT->fill(vtype);
    m_hPMom->fill((*ip)->p()/Gaudi::Units::GeV);
    if( (*ip)->originVertex()->type() == LHCb::MCVertex::ppCollision ) {
      m_hPPrimMom->fill((*ip)->p()/Gaudi::Units::GeV);
    }
    LHCb::ParticleID id = (*ip)->particleID();
    if( id.pid() == 2212 ) {
      if( (*ip)->p() >= 5.0*Gaudi::Units::TeV ) {
        m_hPProtMom->fill(10*((*ip)->p())/(Gaudi::Units::TeV/Gaudi::Units::GeV));
      } else {
        m_hPProtMom->fill((*ip)->p()/Gaudi::Units::GeV);
      } 
    }
    if( detailedHistos() ) {
      // Find number of different particles types
      if( id.abspid() == 2212 ) nProtons++;
      if( id.abspid() == 2112 ) nNeutrons++;
      if( id.abspid() == 211 )  nChPions++;
      if( id.abspid() == 111 )  nPiZeros++;
      if( id.abspid() == 321 )  nChKaons++;
      if( id.abspid() == 310 )  nKs++;
      if( id.abspid() == 11 )   nElectrons++;
      if( id.abspid() == 13 )   nMuons++;
      if( id.abspid() == 22 )   nGammas++;
      if( id.hasBottom() )      nBeauty++;
      if( id.hasCharm() )       nCharm++;
      if( id.isNucleus() )      nNuclei++;
    }
  }
  if( detailedHistos() ) {
    m_hNProtons->fill(nProtons);
    m_hNNeutrons->fill(nNeutrons);
    m_hNChPions->fill(nChPions);
    m_hNPiZeros->fill(nPiZeros);
    m_hNChKaons->fill(nChKaons);
    m_hNKs->fill(nKs);
    m_hNElectrons->fill(nElectrons);
    m_hNMuons->fill(nMuons);
    m_hNGammas->fill(nGammas);
    m_hNBeauty->fill(nBeauty);
    m_hNCharm->fill(nCharm);
    m_hNNuclei->fill(nNuclei);
  }
  
  const LHCb::MCVertices* vertices = 
    get<LHCb::MCVertices>( m_mcVertices );
  m_hNVert->fill(vertices->size());
  LHCb::MCVertices::const_iterator iv;
  for( iv = vertices->begin(); vertices->end()!= iv; ++iv ) {
    int vtype = (*iv)->type();
    if( vtype >=100 ) { vtype = vtype - 100 + 10; }
    m_hVType->fill(vtype);
    m_hVZpos->fill((*iv)->position().z()/Gaudi::Units::mm);
    m_hVZpos2->fill((*iv)->position().z()/Gaudi::Units::mm);
    m_hVTime->fill((*iv)->time()/Gaudi::Units::ns);
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode MCTruthMonitor::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
