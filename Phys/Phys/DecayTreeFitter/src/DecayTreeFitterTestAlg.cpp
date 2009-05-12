// $Id: DecayTreeFitterTestAlg.cpp,v 1.5 2009-05-12 07:58:19 wouter Exp $


#include "GaudiAlg/GaudiHistoAlg.h"
#include <string>
#include "LoKi/AlgoMC.h"
#include "LoKi/ParticleProperties.h"
#include "LoKi/MCParticles.h"

using namespace LoKi ;

class DecayTreeFitterTestAlg: public AlgoMC
{
public:

  // Constructors and destructor
  DecayTreeFitterTestAlg(const std::string& name,
			 ISvcLocator* pSvcLocator);
  virtual ~DecayTreeFitterTestAlg() ;
  LHCb::MCParticle* findMCParticle(const LHCb::Particle& particle);
  const LHCb::Particle* findParticle( const LHCb::Particle& particle,
				      int pid ) ;
  virtual StatusCode initialize();
  virtual StatusCode execute();

private:
  std::string m_inputLocation;
};


// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "Event/Particle.h"
#include "DecayTreeFitter/Fitter.h"
#include "DecayTreeFitter/VtxFitParams.h"
#include "DecayTreeFitter/PidPdg.h"
#include <TMath.h>
#include <boost/foreach.hpp>

#include "LoKi/VertexCuts.h"
#include "LoKi/ParticleCuts.h"
#include "LoKi/MCParticleCuts.h"
#include "LoKi/PhysMCParticleCuts.h"

#include "Event/TrackStateVertex.h"
#include "Event/TwoProngVertex.h"


DECLARE_ALGORITHM_FACTORY( DecayTreeFitterTestAlg );

DecayTreeFitterTestAlg::DecayTreeFitterTestAlg(const std::string& name,
					       ISvcLocator* pSvcLocator)
  :
  //DVAlgorithm(name, pSvcLocator)
  AlgoMC(name, pSvcLocator)
{
  // constructor
  declareProperty( "inputLocation",  m_inputLocation  = "" );
}

DecayTreeFitterTestAlg::~DecayTreeFitterTestAlg()
{
  // destructor
}

StatusCode DecayTreeFitterTestAlg::initialize()
{
  // Initializes TsaInitialization at the begin of program execution.

  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()){
     return Error("Failed to initialize");
  }

  // here I need to get the LoKiSvc

  return StatusCode::SUCCESS;
}

#include "GaudiKernel/IDataStoreAgent.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/DataSvc.h"

class Browser : public IDataStoreAgent
{
public:
  virtual bool analyse(IRegistry * pObject, int /*level*/)
  {
    std::cout << pObject->identifier() << std::endl ;
    return true ;
  }
} ;


const LHCb::Particle* DecayTreeFitterTestAlg::findParticle( const LHCb::Particle& particle,
								int pid )
{
  const LHCb::Particle* rc(0) ;
  if( particle.particleID().pid() == pid ) 
    rc = &particle ;
  else 
    BOOST_FOREACH( const LHCb::Particle* daughter, particle.daughters() ) 
      if( (rc = findParticle(*daughter,pid) ) ) break ;
  return rc ;
}

LHCb::MCParticle* DecayTreeFitterTestAlg::findMCParticle(const LHCb::Particle& particle)
{
 
  const LHCb::MCParticles* mcps = 
    get<LHCb::MCParticles>( LHCb::MCParticleLocation::Default );
  
  //MCRange mcrange( *mcps ) ;
  MCMatch matcher = mcTruth ( "Bs->PsiPhi") ;
  
  //Cut mcBs  =  MCTRUTH ( matcher , mcbs   ) ;

  LHCb::MCParticle* rc(0) ;
  for( LHCb::MCParticles::const_iterator imc = mcps->begin() ;
       imc != mcps->end() && ! rc ; ++imc) {
    if( (*imc)->particleID() == particle.particleID() &&
	matcher->match(&particle,*imc) ) {
      rc = *imc ;
    }
  }
  return rc ;
}

void printpars( const decaytreefit::Fitter& fitter,
		const LHCb::Particle& p)
{
  LHCb::LorentzVectorErr p4( p.momentum(), p.momCovMatrix() ) ;
  LHCb::LorentzVectorErr p4b = fitter.fitParams(p).p4Err() ;
  std::string name = fitter.name(p) ;
  std::cout << name << " momentum/mass error before fitting: "
	    << p4.PxErr() << " " << p4.PyErr() << " " << p4.PzErr()  << " " << p4.MErr() << std::endl ;
  std::cout << name << " momentum/mass error after fitting: "
	    << p4b.PxErr() << " " << p4b.PyErr() << " " << p4b.PzErr()  << " " << p4b.MErr() << std::endl ;
}

StatusCode DecayTreeFitterTestAlg::execute()
{
  setFilterPassed(true) ;

  static int first(true) ;
  if(first) {
    Browser b ;
    DataSvc* evtsvc = dynamic_cast<DataSvc*>(evtSvc()) ;
    evtsvc->traverseTree(&b) ;
    first = false ;
  }

  LHCb::Particles* particles= get<LHCb::Particles>(m_inputLocation);
  const LHCb::RecVertices* pvcontainer = get<LHCb::RecVertices>( LHCb::RecVertexLocation::Primary) ;

  for( LHCb::Particles::iterator ip = particles->begin() ;
       ip != particles->end(); ++ip ) {
    LHCb::Particle* p = *ip ;

    LHCb::MCParticle* mcp = findMCParticle(*p) ;
    if(mcp==0) continue ;

    const LHCb::Particle* phi = findParticle( *p, LHCb::PidPdg::phi ) ;
    if(phi)
      (const_cast<LHCb::Particle*>(phi))->setParticleID( LHCb::ParticleID(LHCb::PidPdg::D0 )) ;
 
    //decaytreefit::vtxverbose = 2 ;

    decaytreefit::Fitter fitter(*p)  ;
    //fitter.print() ;
    fitter.fit() ;
    //fitter.print() ;

    plot(fitter.nIter(),"numiter",-0.5,10.5,11) ;
    plot(fitter.status(),"status",-0.5,10.5,11) ;
    plot(fitter.chiSquare(),"chisq",0,100) ;
    plot(fitter.nDof(),"ndof",-0.5,20.5,21) ;
    double chisqprob = TMath::Prob(fitter.chiSquare(),fitter.nDof()) ;
    plot(chisqprob,"chisqprob",0,1) ;
    
    counter("chisqprob") += chisqprob ;
    counter("success rate") += fitter.status() == LHCb::VtxFitStatus::Success ;

    // let's plot some masses
    plot(fitter.fitParams(*p).p4().M(),"B mass",5200,5500) ;

    const LHCb::Particle* mup = findParticle( *p, LHCb::PidPdg::mu_plus ) ;
    //if(mup) printpars(fitter,*mup) ;
    const LHCb::Particle* Kp = findParticle( *p, LHCb::PidPdg::K_plus ) ;
    if(Kp) {
      //printpars(fitter,*Kp) ;
      LHCb::MCParticle* mc = findMCParticle(*Kp) ;
      if(mc) {
	LHCb::LorentzVectorErr p4b = fitter.fitParams(*Kp).p4Err() ;
	Gaudi::LorentzVector truep4 = mc->momentum() ;
	plot((p4b.Px() - truep4.Px())/p4b.PxErr(),"K+ px pull",-5,5) ;
	plot((p4b.Py() - truep4.Py())/p4b.PyErr(),"K+ py pull",-5,5) ;
	plot((p4b.Pz() - truep4.Pz())/p4b.PzErr(),"K+ pz pull",-5,5) ;
      }
    }

    const LHCb::Particle* Km = findParticle( *p, LHCb::PidPdg::K_minus ) ;
    //if(Km) printpars(fitter,*Km) ;
    
    if(phi) {
      decaytreefit::Fitter phifitter(*phi) ;
      phifitter.fit() ;
      LHCb::LorentzVectorErr p4 = phifitter.fitParams(*phi).p4Err() ;
      //std::cout << "phi fit vertex momentum/mass : "
      //		<< p4.M() << " " 
      //		<< p4.PxErr() << " " << p4.PyErr() << " " << p4.PzErr()  << " " << p4.MErr() << std::endl ;
      //std::cout << "phi fit chisq: " << phifitter.chiSquare() << std::endl ;
      if(Kp && Km) {
	const LHCb::Track* postrk = Kp->proto()->track() ;
	const LHCb::Track* negtrk = Km->proto()->track() ;
	std::vector<const LHCb::State*> states ;
	states.push_back(&(postrk->firstState())) ;
	states.push_back(&(negtrk->firstState())) ;
	LHCb::TrackStateVertex vertex( states ) ;
	vertex.fit() ;
	//std::cout << "Chi2 of psi tp vertex fit : "
	//	  << vertex.chi2() << std::endl ;
	LHCb::TwoProngVertex* tpvertex = vertex.createTwoProngVertex(true) ;
	double kmass = LoKi::Particles::ppFromPID(Kp->particleID())->mass() ;
	LHCb::VtxFitParams fitpars( 0,
				    tpvertex->position(),
				    tpvertex->momentum(kmass,kmass),
				    tpvertex->covMatrix7x7(kmass,kmass) ) ;
	LHCb::LorentzVectorErr p4 = fitpars.p4Err() ;
	//std::cout << "tp vertex momentum/mass : "
	//	  << p4.M() << " "
	//	  << p4.PxErr() << " " << p4.PyErr() << " " << p4.PzErr()  << " " << p4.MErr() << std::endl ;
	delete tpvertex ;
      } ;
    }
    

    const LHCb::Particle* psi = findParticle( *p, LHCb::PidPdg::J_psi ) ;
    if(psi) {
      LHCb::LorentzVectorErr p4b = fitter.fitParams(*psi).p4Err() ;
      plot(fitter.fitParams(*psi).p4().M(),"psi mass",3000,3200) ;
      plot(psi->momentum().M(),"psi mass orig",3000,3200) ;
      if( chisqprob > 0.001 ) 
	plot(fitter.fitParams(*psi).p4().M(),"psi mass good",3000,3200) ;
      double pdgmass = LoKi::Particles::ppFromPID(psi->particleID())->mass() ;
      double mpull = (p4b.M()-pdgmass)/p4b.MErr() ;
      plot(mpull,"psi mass pull",-5,5) ;
      //std::cout << "mass chi2: " << mpull*mpull << std::endl ;
    }

    if(phi) {
      
      LHCb::VtxFitParams phiparams = fitter.fitParams(*phi) ;
      plot(phiparams.p4().M(),"phi mass",1000,1040) ;
      LHCb::VtxDoubleErr dl = phiparams.decayLengthErr() ;
      plot(dl.value(),"phi decay length",-10,10) ;
      plot(dl.value()/std::sqrt(dl.covariance()),"phi decay length pull",-5,5) ;

      plot(phi->momentum().M(),"phi mass orig",1000,1040) ;
      if( chisqprob > 0.001 ) 
	plot(phiparams.p4().M(),"phi mass good",1000,1040) ;
      LHCb::LorentzVectorErr p4b = phiparams.p4Err() ;
      
      double pdgmass = LoKi::Particles::ppFromPID(phi->particleID())->mass() ;
      double recoerr = p4b.MErr() ;
      double width = LoKi::Particles::ppFromPID(phi->particleID())->width() ;
      double err = std::sqrt( recoerr*recoerr + width*width ) ;

      double mpull = (p4b.M()-pdgmass)/err ;
      plot(mpull,"phi mass pull",-5,5) ;
      //std::cout << "phi mass chi2: " << mpull*mpull << std::endl ;

      LHCb::LorentzVectorErr p4( mup->momentum(), mup->momCovMatrix() ) ;
      //std::cout << "phi momentum/mass error before fitting: "
      //<< p4.PxErr() << " " << p4.PyErr() << " " << p4.PzErr()  << " " << p4.MErr() << std::endl ;
      //std::cout << "phi momentum/mass error after fitting: "
      //<< p4b.PxErr() << " " << p4b.PyErr() << " " << p4b.PzErr()  << " " << p4b.MErr() << std::endl ;

      LHCb::MCParticle* mcphi = findMCParticle(*phi) ;
      if(mcphi) {
	Gaudi::LorentzVector truep4 = mcphi->momentum() ;
	plot((p4b.Px() - truep4.Px())/p4b.PxErr(),"phi px pull",-5,5) ;
	plot((p4b.Py() - truep4.Py())/p4b.PyErr(),"phi py pull",-5,5) ;
	plot((p4b.Pz() - truep4.Pz())/p4b.PzErr(),"phi pz pull",-5,5) ;
      }
    }
    
    //std::cout << "chi2 before mass constraint: " << fitter.chiSquare() << std::endl ;
    if(psi) fitter.setMassConstraint(*psi) ;
    fitter.setMassConstraint(*p) ;

    fitter.fit() ;
    {
      Gaudi::LorentzVector truep4 = mcp->momentum() ;
      Gaudi::LorentzVector p4orig = p->momentum() ;
      LHCb::LorentzVectorErr p4 = fitter.fitParams(*p).p4Err() ;
     //  std::cout << "Pz: "
      // 		<< truep4.Pz() << " "
      // 		<< p4orig.Pz() << " "
      // 		<< p4.Pz() << std::endl ;
      // std::cout << p4.Pz()/truep4.Pz()-1 << std::endl ;
      plot(p4.Pz()/truep4.Pz()-1,"delta Pz / Pz (constraint daughters)",-0.01,0.01) ;
      plot(p4orig.Pz()/truep4.Pz()-1,"delta Pz / Pz (orig)",-0.01,0.01) ;
    }

    //std::cout << "chi2 after mass constraint: " << fitter.chiSquare() << std::endl ;
    //fitter.print() ;

    plot(fitter.nIter(),"numiter (constraint daughters)",-0.5,10.5,11) ;
    plot(fitter.status(),"status (constraint daughters)",-0.5,10.5,11) ;
    plot(fitter.fitParams(*p).p4().M(),"B mass (constraint daughters)",
	 5200,5500) ;
    chisqprob = TMath::Prob(fitter.chiSquare(),fitter.nDof()) ;
    counter("chisqprob mass constraint fit") += chisqprob ; 
    counter("success rate mass constraint fit") += fitter.status() == LHCb::VtxFitStatus::Success ;

    plot(chisqprob,"chisqprob (constraint daughters)",0,1) ;
    plot(fitter.nDof(),"ndof (constraint duaghters)",-0.5,20.5,21) ;
    if(psi) 
      plot(fitter.fitParams(*psi).p4().M(),"psi mass (constraint daughters)",3000,3200) ;
    if(phi)
      plot(fitter.fitParams(*phi).p4().M(),"phi mass (constraint daughters)",1000,1040) ;
    
    if( TMath::Prob(fitter.chiSquare(),fitter.nDof())>0.001 )
      plot(fitter.fitParams(*p).p4().M(),"B mass (good)",
	   5200,5500) ;

    plot( p->momentum().M(),"B mass orig",5200,5500) ;

    if(pvcontainer->size()==1 ) {
      const LHCb::RecVertex* pv = *(pvcontainer->begin()) ;
      decaytreefit::Fitter lifetimefit( *p,*pv,true) ;
      lifetimefit.fit() ;
      //lifetimefit.print() ;
      LHCb::VtxFitParams fitpars = lifetimefit.fitParams(*p) ;
      plot(fitpars.decayLength(),"B decay length",-10,20) ;
      double truetau = LoKi::MCParticles::ProperLifeTime()(mcp) ;
      const LHCb::MCVertex* endVertex = mcp->endVertices()[0] ;
      const LHCb::MCVertex* originVertex = mcp->originVertex() ;
      double truelen = endVertex && originVertex ?
	(endVertex->position() - originVertex->position()).R() : 0 ;

      LHCb::VtxDoubleErr recotau = fitpars.properDecayTime() ;
      LHCb::VtxDoubleErr recolen = fitpars.decayLengthErr() ;
//       std::cout << "len: "
// 		<< fitpars.decayLength().value() << " "
// 		<< std::sqrt(fitpars.decayLength().covariance()) << std::endl ;

//       std::cout << "tau: "
// 		<< truetau/Gaudi::Units::ns << " " << recotau.value()/Gaudi::Units::ns << " " 
// 		<< Gaudi::Units::ns << std::endl ;

      plot(recolen.value()-truelen,"B decay length resolution",-2,2) ;
      plot((recolen.value()-truelen)/std::sqrt(recolen.covariance()),"B decay length pull",-5,5) ;
      plot(recotau.value()-truetau,"B life time resolution",-0.2,0.2) ;
      plot((recotau.value()-truetau)/std::sqrt(recotau.covariance()),
	   "B life time pull",-5,5) ;
      const double pdtmass = LoKi::Particles::ppFromPID(p->particleID())->mass() ;
      recotau = fitpars.properDecayTime(pdtmass) ;
      plot(recotau.value()-truetau,"B life time resolution (mass constrained)",-0.2,0.2) ;
      plot((recotau.value()-truetau)/std::sqrt(recotau.covariance()),
	   "B life time pull (mass constrained)",-5,5) ;
      
    }
    
  }
  return StatusCode::SUCCESS;  
}
