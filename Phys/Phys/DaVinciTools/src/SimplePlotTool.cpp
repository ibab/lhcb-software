// $Id: SimplePlotTool.cpp,v 1.1 2005-01-06 10:37:47 pkoppenb Exp $
// Include files 
#include "gsl/gsl_math.h"
// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/IParticlePropertySvc.h"

#include "Kernel/IPVLocator.h"
#include "Kernel/IGeomDispCalculator.h"
// local
#include "SimplePlotTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : SimplePlotTool
//
// 2005-01-05 : Patrick KOPPENBURG
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<SimplePlotTool>          s_factory ;
const        IToolFactory& SimplePlotToolFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SimplePlotTool::SimplePlotTool( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : GaudiHistoTool ( type, name , parent )
  , m_histos()
  , m_allDefault(true)
  , m_ppSvc(0)
{
  declareInterface<IPlotTool>(this);

  declareProperty( "Variables",    m_variables  );
  declareProperty( "Minima",       m_minima   );
  declareProperty( "Maxima",       m_maxima   );
};
//=============================================================================
// Destructor
//=============================================================================
SimplePlotTool::~SimplePlotTool() {}; 

//=============================================================================
// Initilisation
//=============================================================================
StatusCode SimplePlotTool::initialize () {
  StatusCode sc = GaudiHistoTool::initialize();
  if (!sc) return sc;
  
  m_ppSvc = svc<IParticlePropertySvc>("ParticlePropertySvc", true);
  if( !m_ppSvc ) {
    err() << "    Unable to locate Particle Property Service" << endreq;
    return StatusCode::FAILURE;
  }                                              

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
// 
//=============================================================================
StatusCode SimplePlotTool::setPath(const std::string& path) {
  debug() << "Setting path to " << path << endmsg;
  setHistoDir(path);
  return StatusCode::SUCCESS;  
}; 
//=============================================================================
// 
//=============================================================================
StatusCode SimplePlotTool::fillPlots(const ParticleVector& PV) {
  for ( ParticleVector::const_iterator p = PV.begin() ; p != PV.end() ; ++p ){
    StatusCode sc = fillPlots(*p);
    if (!sc) return sc;
  }
  return StatusCode::SUCCESS;
}; 
//=============================================================================
// 
//=============================================================================
StatusCode SimplePlotTool::fillPlots(const Particle* p) {

  for ( std::vector<MyHisto>::iterator H = m_histos.begin() ; 
        H != m_histos.end() ; ++H ){
      StatusCode sc = doPlot(p,(*H));
      if (!sc) return sc ;
  }
  return StatusCode::SUCCESS;
}; 

//=============================================================================
//  Plot
//=============================================================================
StatusCode SimplePlotTool::doPlot(const Particle* P, MyHisto& H) {
  std::string var = H.getVar();
  ParticleProperty *pp = m_ppSvc->findByPythiaID(abs(P->particleID().pid()));
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
  } else if ( var == "IP" || var == "IPs"){
    if ( var == "IP" ){
      
    } else if ( var == "IPs" ){
      
    }
  } else {
    err() << "Unknown variable " << var << endreq ;
    return  StatusCode::FAILURE;
  }
  return  StatusCode::SUCCESS;
}
//=============================================================================
//  Defaults for variables
//=============================================================================
bool SimplePlotTool::MyHisto::setHisto(const std::string& var ){
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
