// $Id: SimplePlotTool.cpp,v 1.4 2005-01-13 12:28:01 pkoppenb Exp $
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
  , m_geomTool(0)
  , m_pvLocator(0)
  , m_isInitialised(false)
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
// Initialisation
//=============================================================================
StatusCode SimplePlotTool::initialize () {
  if (!m_isInitialised){
    StatusCode sc = firstInitialize(); // major unique initialization
    if (!sc) return sc;
  }
  return reInitialize(); //  re-initialization in case options are overwritten
}
//=============================================================================
// Initialize everything
//=============================================================================
StatusCode SimplePlotTool::firstInitialize () {

  StatusCode sc = GaudiHistoTool::initialize();
  if (!sc) return sc;

  m_ppSvc = svc<IParticlePropertySvc>("ParticlePropertySvc", true);
  if( !m_ppSvc ) {
    err() << "Unable to locate Particle Property Service" << endreq;
    return StatusCode::FAILURE;
  }          

  m_geomTool = tool<IGeomDispCalculator>("GeomDispCalculator",this);
  if( !m_geomTool ) {
    err() << "Unable to get GeomDispCalculator" << endreq;
    return StatusCode::FAILURE;
  }          

  m_pvLocator = tool<IPVLocator>("PVLocator");
  if( !m_pvLocator ) {
    err() << "Unable to get PVLocator" << endreq;
    return StatusCode::FAILURE;
  }          
  m_isInitialised = true ; // don't re-do this
  return StatusCode::SUCCESS;
}
//=============================================================================
// Re-Initialize whatever can be
//=============================================================================
StatusCode SimplePlotTool::reInitialize () {
  
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
  if (!m_histos.empty()){ // do not re-initialise if already done 
    err() << "Variables have already been overwritten by another tool/algorithm" 
          << endreq;
    return StatusCode::FAILURE;
  }
  // clear and fill histograms
  m_histos.clear();
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
  info() << "Done initialization for " <<  m_variables.size() 
         << " variables" << endmsg ;

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
StatusCode SimplePlotTool::fillPlots(const ParticleVector& PV,
                                     const std::string trailer) {
  for ( ParticleVector::const_iterator p = PV.begin() ; p != PV.end() ; ++p ){
    StatusCode sc = fillPlots(*p,trailer);
    if (!sc) return sc;
  }
  return StatusCode::SUCCESS;
}; 
//=============================================================================
// 
//=============================================================================
StatusCode SimplePlotTool::fillPlots(const Particle* p,
                                     const std::string trailer) {

  debug() << "Filling plots for " << p->particleID().pid() << endmsg;
  for ( std::vector<MyHisto>::iterator H = m_histos.begin() ; 
        H != m_histos.end() ; ++H ){
    StatusCode sc = doPlot(p,(*H),trailer);
    if (!sc) return sc ;
  }
  return StatusCode::SUCCESS;
}; 

//=============================================================================
//  Plot
//=============================================================================
StatusCode SimplePlotTool::doPlot(const Particle* P, MyHisto& H, 
                                  std::string trailer) {
  std::string var = H.getVar();
  ParticleProperty *pp = m_ppSvc->findByPythiaID(abs(P->particleID().pid()));
  std::string name = "Unknown";
  if (pp) name = pp->particle();
  if (trailer!="") name+=" "+trailer;
  double hmin = H.getMin();
  double hmax = H.getMax();

  debug() << "Filling plot for " << var << " of " << name << endmsg;

  if ( ( var == "M" ) && (P->endVertex())){
    if ( hmin < 0 ){
      double mn = pp->mass() - gsl_max(pp->mass()/50.,pp->maxWidth()); // at least 2%
      double mx = pp->mass() + gsl_max(pp->mass()/50.,pp->maxWidth());
      plot(P->mass(),"Mass of "+name,mn,mx );
    } else {
      plot(P->mass(),"Mass of "+name,hmin,hmax );
    }
  } else if ( var == "P" ){
    plot(P->p(),"Momentum of "+name,hmin,hmax);
  } else if ( var == "Pt" ){
    plot(P->pt(),"Pt of "+name,hmin,hmax);
  } else if ( var == "Chi2" ){
    if (P->endVertex()) plot(P->endVertex()->chi2(),"Chi2 of "+name,hmin,hmax);
  } else if ( var == "IP" || var == "IPs" || var == "DPV" || var == "FS"){
    std::string PVContainer = m_pvLocator->getPVLocation() ;
    verbose() << "Getting PV from " << PVContainer << endreq ;
    Vertices* PV = get<Vertices>(PVContainer);
    if ( !PV ) {
      err() << "Could not find primary vertex location " 
            << PVContainer << endreq;
      return false ;
    }
// IP or IPs
    double bestf = -1. , bestfe = -1., minip = 9999999.;
    for (VertexVector::const_iterator iv=PV->begin();iv!=PV->end();++iv) {
      double ip = -1 ,ipe = -1.;
      StatusCode sc = m_geomTool->calcImpactPar(*P, *(*iv), ip, ipe);
      if (!sc) continue;
      if ( var == "IP" ){
        plot(ip,"IP of "+name,hmin,hmax);
      } else if ( var == "IPs" ){
        if (ipe>0) plot(ip/ipe,"IP/err of "+name,hmin,hmax);       
// DPV or FS - to "best PV"
      } else if ( (P->endVertex()) && ( ip/ipe < minip ) ) {
        minip = ip/ipe ; // new best PV
        StatusCode sc = m_geomTool->calcVertexDis(*(*iv), (*P->endVertex()), bestf ,bestfe );
        if (!sc) continue;  // ignore
      }
    } // PV loop
    if (minip < 9999999.){      
      if ( var == "DPV" ){
        plot(bestf,"PV distance of "+name,hmin,hmax);
      } else if ( var == "FS" ){
        if (bestfe>0) plot(bestf/bestfe,"Flight signif. of "+name,hmin,hmax);       
      }
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
  } else if ( var == "IP" ){
    m_min = 0. ;
    m_max = 1. ;
  } else if ( var == "IPs" ){
    m_min = 0. ;
    m_max = 10. ;
  } else if ( var == "DPV" ){
    m_min = 0. ;
    m_max = 1. ;
  } else if ( var == "FS" ){
    m_min = 0. ;
    m_max = 20. ;
  } else {
    return false;
  }
  return true ;
}


//=============================================================================
