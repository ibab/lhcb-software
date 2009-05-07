// $Id: DecayTreeFitterTestAlg.cpp,v 1.1 2009-05-07 11:29:36 wouter Exp $


#include "GaudiAlg/GaudiHistoAlg.h"
#include <string>

class DecayTreeFitterTestAlg: public 
			      //DVAlgorithm
			      GaudiHistoAlg 
{
public:

  // Constructors and destructor
  DecayTreeFitterTestAlg(const std::string& name,
			 ISvcLocator* pSvcLocator);
  virtual ~DecayTreeFitterTestAlg();

  virtual StatusCode initialize();
  virtual StatusCode execute();

private:
  std::string m_inputLocation;
};


// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "Event/Particle.h"
#include "DecayTreeFitter/VtkFitter.h"
#include "DecayTreeFitter/BtaFitParams.h"
#include "DecayTreeFitter/PidPdg.h"
#include <TMath.h>
#include <boost/foreach.hpp>


DECLARE_ALGORITHM_FACTORY( DecayTreeFitterTestAlg );

DecayTreeFitterTestAlg::DecayTreeFitterTestAlg(const std::string& name,
					       ISvcLocator* pSvcLocator)
  :
  //DVAlgorithm(name, pSvcLocator)
  GaudiHistoAlg(name, pSvcLocator)
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
  virtual bool analyse(IRegistry * pObject, int level)
  {
    std::cout << pObject->identifier() << std::endl ;
    return true ;
  }
} ;


const LHCb::Particle* findParticle( const LHCb::Particle& particle,
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

StatusCode DecayTreeFitterTestAlg::execute()
{
  static int first(true) ;
  if(first) {
    Browser b ;
    DataSvc* evtsvc = dynamic_cast<DataSvc*>(evtSvc()) ;
    evtsvc->traverseTree(&b) ;
    first = false ;
  }

  //vtxtreefit::vtxverbose = 2 ;

  LHCb::Particles* particles= get<LHCb::Particles>(m_inputLocation);
  //LHCb::Particles* particles = desktop()->particles();

  for( LHCb::Particles::iterator ip = particles->begin() ;
       ip != particles->end(); ++ip ) {
    LHCb::Particle* p = *ip ;
    vtxtreefit::Fitter fitter(*p)  ;
    //fitter.print() ;
    fitter.fit() ;
    //fitter.print() ;

    plot(fitter.nIter(),"numiter",-0.5,10.5,11) ;
    plot(fitter.status(),"status",-0.5,10.5,11) ;
    plot(fitter.chiSquare(),"chisq",0,100) ;
    plot(fitter.nDof(),"ndof",-0.5,20.5,21) ;
    double chisqprob = TMath::Prob(fitter.chiSquare(),fitter.nDof()) ;
    plot(chisqprob,"chisqprob",0,1) ;
    
    // let's plot some masses
    plot(fitter.btaFitParams(*p).mass(),"B mass",5200,5500) ;

    const LHCb::Particle* psi = findParticle( *p, LHCb::PidPdg::J_psi ) ;
    if(psi) {
      plot(fitter.btaFitParams(*psi).mass(),"psi mass",3000,3200) ;
      plot(psi->momentum().M(),"psi mass orig",3000,3200) ;
      if( chisqprob > 0.001 ) 
	plot(fitter.btaFitParams(*psi).mass(),"psi mass good",3000,3200) ;
    }

    const LHCb::Particle* phi = findParticle( *p, LHCb::PidPdg::phi ) ;
    if(phi) {
      plot(fitter.btaFitParams(*phi).mass(),"phi mass",1000,1040) ;
      plot(phi->momentum().M(),"phi mass orig",1000,1040) ;
      if( chisqprob > 0.001 ) 
	plot(fitter.btaFitParams(*phi).mass(),"phi mass good",1000,1040) ;
    }
    
    if(psi) fitter.setMassConstraint(*psi) ;
    if(phi) fitter.setMassConstraint(*phi) ;
    fitter.fit() ;

    plot(fitter.nIter(),"numiter (constraint daughters)",-0.5,10.5,11) ;
    plot(fitter.status(),"status (constraint daughters)",-0.5,10.5,11) ;
    plot(fitter.btaFitParams(*p).mass(),"B mass (constraint daughters)",
	 5200,5500) ;
    chisqprob = TMath::Prob(fitter.chiSquare(),fitter.nDof()) ;
    plot(chisqprob,"chisqprob (constraint daughters)",0,1) ;
    plot(fitter.nDof(),"ndof (constraint duaghters)",-0.5,20.5,21) ;
    if(psi)
      plot(fitter.btaFitParams(*psi).mass(),"psi mass (constraint daughters)",3000,3200) ;
    if(phi)
      plot(fitter.btaFitParams(*phi).mass(),"phi mass (constraint daughters)",1000,1040) ;
    
    if( TMath::Prob(fitter.chiSquare(),fitter.nDof())>0.001 )
       plot(fitter.btaFitParams(*p).mass(),"B mass (good)",
	 5200,5500) ;

    plot( p->momentum().M(),"B mass orig",5200,5500) ;
        
  }
  return StatusCode::SUCCESS;  
}
