// $Id: TutorialAlgorithm.cpp,v 1.1 2004-11-09 14:19:08 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "TutorialAlgorithm.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TutorialAlgorithm
//
// 2004-11-04 : Patrick KOPPENBURG
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<TutorialAlgorithm>          s_factory ;
const        IAlgFactory& TutorialAlgorithmFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TutorialAlgorithm::TutorialAlgorithm( const std::string& name,
                            ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator )
    , m_JPsiID(0)
    , m_JPsiMass(0.)
    , m_nJPsis(0)
    , m_nEvents(0)
{
  declareProperty("MassWindow",   m_JPsiMassWin = 10.*GeV); 
  declareProperty("MaxChi2",   m_JPsiChi2 = 1000.);
}
//=============================================================================
// Destructor
//=============================================================================
TutorialAlgorithm::~TutorialAlgorithm() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TutorialAlgorithm::initialize() {

  debug() << "==> Initialize" << endmsg;
  // particle property service
  ParticleProperty* m_psi = ppSvc()->find( "J/psi(1S)" );
  m_JPsiID = m_psi->pdgID();
  m_JPsiMass = m_psi->mass();
  info() << "Will reconstruct " << m_psi->particle() << " (ID=" << m_JPsiID
         << ") with mass " << m_JPsiMass << endreq ;
  info() << "Mass window is " << m_JPsiMassWin << " MeV" << endreq ;
  info() << "Max chi^2 is " << m_JPsiChi2 << endreq ;

  return StatusCode::SUCCESS;
};
//=============================================================================
// Main execution
//=============================================================================
StatusCode TutorialAlgorithm::execute() {

  debug() << "==> Execute" << endmsg;
  setFilterPassed(false);   // Mandatory. Set to true if event is accepted.
  ++m_nEvents;
  
  // get particles. Filter muons.
  const ParticleVector& parts = desktop()->particles();
  ParticleVector MuPlus, MuMinus;
  StatusCode sc = particleFilter()->filterNegative(parts,MuMinus);
  if (sc) sc = particleFilter()->filterPositive(parts,MuPlus);
  if (!sc) {
    err() << "Error while filtering" << endreq ;
    return sc ;
  } 
  verbose() << "Filtered " << MuMinus.size() << " mu- and " << MuPlus.size() 
            << " mu+" << endreq ;

  // combine mu+ and mu-
  ParticleVector::const_iterator imup, imum;
  for ( imum = MuMinus.begin() ; imum !=  MuMinus.end() ; ++imum ){
    plot((*imum)->momentum().perp(),"Mu Pt",0.*GeV,10.*GeV);
    for ( imup = MuPlus.begin() ; imup !=  MuPlus.end() ; ++imup ){
      if (imum == MuMinus.begin()) plot((*imup)->momentum().perp(),"Mu Pt",0.*GeV,10.*GeV);
      HepLorentzVector twoMu = (*imup)->momentum() + (*imum)->momentum() ;
      verbose() << "Two muon mass is " << twoMu.m()/MeV << endreq ;
      // mass cut
      plot(twoMu.m(),"DiMu mass",2.*GeV,4.*GeV);
      if ( fabs ( twoMu.m() - m_JPsiMass ) > m_JPsiMassWin ) continue ;
      // vertex fit
      Vertex MuMuVertex;
      sc = vertexFitter()->fitVertex(*(*imup),*(*imum),MuMuVertex);
      if (!sc){
        info() << "Failed to fit vertex" << endreq ; // no bid deal
        continue ;
      }  
      debug() << "Vertex fit at " << MuMuVertex.position()/cm 
              << " with chi2 " << MuMuVertex.chi2() << endreq;
      // chi2 cut
      plot(MuMuVertex.chi2(),"DiMu Chi^2",0.,200.);
      if ( MuMuVertex.chi2() > m_JPsiChi2 ) continue ;
      // make particle
      Particle Jpsi ;
      sc = particleStuffer()->fillParticle(MuMuVertex,Jpsi,ParticleID(m_JPsiID));
      Particle* pJpsi = desktop()->createParticle(&Jpsi);
      info() << "Created J/psi candidate with m=" << Jpsi.mass() << " and " \
             << "chi^2=" << MuMuVertex.chi2() << endreq ;      
      plot((*imum)->momentum().perp(),"Selected Mu Pt",0.*GeV,10.*GeV);
      plot((*imup)->momentum().perp(),"Selected Mu Pt",0.*GeV,10.*GeV);
      plot(twoMu.m(),"Selected DiMu mass",m_JPsiMass-m_JPsiMassWin,m_JPsiMass+m_JPsiMassWin);
      if (!pJpsi){
        err() << "Cannot save particle to desktop" << endreq ;
        return StatusCode::FAILURE;
      } 
      setFilterPassed(true);
      ++m_nJPsis ;
    } // imup
  } // imum
  // save desktop
  sc = desktop()->saveDesktop();
  return sc;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode TutorialAlgorithm::finalize() {

  debug() << "==> Finalize" << endmsg;
  info() << "Found " << m_nJPsis << " J/psi in " << m_nEvents << " events" << endreq;

  return  StatusCode::SUCCESS;
}

//=============================================================================
