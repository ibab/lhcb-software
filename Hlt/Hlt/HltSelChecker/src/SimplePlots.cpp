// $Id: SimplePlots.cpp,v 1.1.1.1 2004-11-18 12:38:36 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "gsl/gsl_math.h"

// local
#include "SimplePlots.h"

//-----------------------------------------------------------------------------
// Implementation file for class : SimplePlots
//
// 2004-10-22 : Patrick KOPPENBURG
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<SimplePlots>          s_factory ;
const        IAlgFactory& SimplePlotsFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SimplePlots::SimplePlots( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator )
    , m_histos()
    , m_allDefault(true)
{
  declareProperty( "Variables",    m_variables  );
  declareProperty( "Minima",       m_minima   );
  declareProperty( "Maxima",       m_maxima   );
}
//=============================================================================
// Destructor
//=============================================================================
SimplePlots::~SimplePlots() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode SimplePlots::initialize() {

  debug() << "==> Initialize" << endmsg;
  if ( m_minima.empty() && m_maxima.empty() ){
    m_allDefault = true ;
  } else {
    m_allDefault = false ;
    if (( m_minima.size() != m_variables.size() ) || 
        ( m_maxima.size() != m_variables.size())) {
      err() << "There are " << m_variables.size() << " variables, " 
            << m_minima.size() << " mimima and " 
            << m_maxima.size() << " maxima. Must be equal" << endreq ;
      return StatusCode::FAILURE ;
    } 
  }

  for ( unsigned int i = 0; i < m_variables.size(); ++i ) {
    MyHisto H;
    bool ok ;
    if ( m_allDefault ) ok = H.setHisto(m_variables[i]);
    else ok = H.setHisto(m_variables[i],m_minima[i],m_maxima[i]);
    if (!ok){
      err() << "Unknown variable " << m_variables[i] << endreq ;
      return StatusCode::FAILURE;
    }
    m_histos.push_back(H);
    info() << "Histogram with variable " << H.getVar() << " with boundaries" 
           << H.getMin() << " " << H.getMax() << endreq ;
  }
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode SimplePlots::execute() {

  debug() << "==> Execute" << endmsg;

  // code goes here  
  const ParticleVector PP = desktop()->particles() ;
  ParticleVector::const_iterator p ;
  std::vector<MyHisto>::iterator H ;
  for ( p = PP.begin() ; p != PP.end() ; ++p ){
    for ( H = m_histos.begin() ; H != m_histos.end() ; ++H ){
      StatusCode sc = doPlot((*p),(*H));
      if (!sc) return sc ;
    }
  }

  setFilterPassed(true);   // Mandatory. Set to true if event is accepted.
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode SimplePlots::finalize() {

  debug() << "==> Finalize" << endmsg;

  return  StatusCode::SUCCESS;
}
//=============================================================================
//  Plot
//=============================================================================
StatusCode SimplePlots::doPlot(const Particle* P, MyHisto& H) {
  std::string var = H.getVar();
  ParticleProperty *pp = ppSvc()->findByPythiaID(abs(P->particleID().pid()));
  if ( var == "M" ){
    if ( H.getMin() < 0 ){
      double mn = pp->mass() - gsl_max(pp->mass()/50.,pp->maxWidth()); // at least 2%
      double mx = pp->mass() + gsl_max(pp->mass()/50.,pp->maxWidth());
      plot(P->mass(),"Mass of "+pp->particle(),mn,mx );
    } else {
      plot(P->mass(),"Mass of "+pp->particle(),H.getMin(),H.getMax() );
    }
  } else if ( var == "P" ){
    plot(P->p(),"Momentum of "+pp->particle(),H.getMin(),H.getMax());
  } else if ( var == "Pt" ){
    plot(P->pt(),"Pt of "+pp->particle(),H.getMin(),H.getMax());
  } else if ( var == "Chi2" ){
    if (P->endVertex()){
      plot(P->endVertex()->chi2(),"Chi2 of "+pp->particle(),H.getMin(),H.getMax());
    }
    
    //  } else if ( var == "IP" ){
    //  } else if ( var == "IPs" ){
  } else {
    err() << "Unknown variable " << var << endreq ;
    return  StatusCode::FAILURE;
  }
  return  StatusCode::SUCCESS;
}
//=============================================================================
//  Defaults for variables
//=============================================================================
bool SimplePlots::MyHisto::setHisto(const std::string& var ){
  m_var = var ;
  if ( var == "M" ){
    m_min = -1 ;
    m_max = -1 ;
  } else if ( var == "P" ){
    m_min = 0. ;
    m_max = 100.*GeV ;
  } else if ( var == "Pt" ){
    m_min = 0. ;
    m_max = 10.*GeV ;
  } else if ( var == "Chi2" ){
    m_min = 0. ;
    m_max = 100. ;
    //  } else if ( var == "IP" ){
    //    m_min = 0. ;
    //    m_max = 1. ;
    //  } else if ( var == "IPs" ){
    //    m_min = 0. ;
    //    m_max = 10. ;
  } else {
    return false;
  }
  return true ;
}


//=============================================================================
