// $Id: TutorialAlgorithm.cpp,v 1.3 2005-12-15 16:06:06 pkoppenb Exp $
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
    , m_jPsiID(0)
    , m_jPsiMass(0.)
    , m_nJPsis(0)
    , m_nEvents(0)
{
  declareProperty("MassWindow",   m_jPsiMassWin = 10.*GeV); 
  declareProperty("MaxChi2",   m_jPsiChi2 = 1000.);
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
  if ( !m_psi ) { // 
    err() << "Cannot find particle property for J/psi(1S)" << endmsg ; 
    return StatusCode::FAILURE;
  }
  m_jPsiID = m_psi->pdgID();
  m_jPsiMass = m_psi->mass();
  info() << "Will reconstruct " << m_psi->particle() << " (ID=" << m_jPsiID
         << ") with mass " << m_jPsiMass << endmsg ;
  info() << "Mass window is " << m_jPsiMassWin << " MeV" << endmsg ;
  info() << "Max chi^2 is " << m_jPsiChi2 << endmsg ;

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
    err() << "Error while filtering" << endmsg ;
    return sc ;
  } 
  verbose() << "Filtered " << MuMinus.size() << " mu- and " << MuPlus.size() 
            << " mu+" << endmsg ;

  // combine mu+ and mu-
  ParticleVector::const_iterator imup, imum;
  for ( imum = MuMinus.begin() ; imum !=  MuMinus.end() ; ++imum ){
    plot((*imum)->momentum().perp(),"Mu Pt",0.*GeV,10.*GeV);
    for ( imup = MuPlus.begin() ; imup !=  MuPlus.end() ; ++imup ){
      if (imum == MuMinus.begin()) plot((*imup)->momentum().perp(),"Mu Pt",0.*GeV,10.*GeV);
      HepLorentzVector twoMu = (*imup)->momentum() + (*imum)->momentum() ;
      verbose() << "Two muon mass is " << twoMu.m()/MeV << endmsg ;
      // mass cut
      plot(twoMu.m(),"DiMu mass",2.*GeV,4.*GeV);
      if ( fabs ( twoMu.m() - m_jPsiMass ) > m_jPsiMassWin ) continue ;
      // vertex fit
      Vertex MuMuVertex;
      sc = vertexFitter()->fitVertex(*(*imup),*(*imum),MuMuVertex);
      if (!sc){
        info() << "Failed to fit vertex" << endmsg ; // no big deal
        continue ;
      }  
      debug() << "Vertex fit at " << MuMuVertex.position()/cm 
              << " with chi2 " << MuMuVertex.chi2() << endmsg;
      // chi2 cut
      plot(MuMuVertex.chi2(),"DiMu Chi^2",0.,200.);
      if ( MuMuVertex.chi2() > m_jPsiChi2 ) continue ;
      // make particle
      Particle Jpsi ;
      sc = particleStuffer()->fillParticle(MuMuVertex,Jpsi,ParticleID(m_jPsiID));
      Particle* pJpsi = desktop()->createParticle(&Jpsi);
      info() << "Created J/psi candidate with m=" << Jpsi.mass() << " and " \
             << "chi^2=" << MuMuVertex.chi2() << endmsg ;      
      plot((*imum)->momentum().perp(),"Selected Mu Pt",0.*GeV,10.*GeV);
      plot((*imup)->momentum().perp(),"Selected Mu Pt",0.*GeV,10.*GeV);
      plot(twoMu.m(),"Selected DiMu mass",m_jPsiMass-m_jPsiMassWin,m_jPsiMass+m_jPsiMassWin);
      if (!pJpsi){
        err() << "Cannot save particle to desktop" << endmsg ;
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
  info() << "Found " << m_nJPsis << " J/psi in " << m_nEvents << " events" << endmsg;

  return  StatusCode::SUCCESS;
}

//=============================================================================
