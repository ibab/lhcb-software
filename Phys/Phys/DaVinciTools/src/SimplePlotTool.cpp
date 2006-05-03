// $Id: SimplePlotTool.cpp,v 1.13 2006-05-03 18:33:49 pkoppenb Exp $
// Include files 
#include "gsl/gsl_math.h"
// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/IParticlePropertySvc.h"

#include "Kernel/IOnOffline.h"
#include "Kernel/IGeomDispCalculator.h"
#include "Event/PrimVertex.h"
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
  , m_onOfflineTool(0)
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

  debug() << m_minima << endmsg ;

  m_ppSvc = svc<IParticlePropertySvc>("ParticlePropertySvc", true);
  m_onOfflineTool = tool<IOnOffline>("OnOfflineTool",this );
  m_geomTool = tool<IGeomDispCalculator>(m_onOfflineTool->dispCalculator(),this);

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
StatusCode SimplePlotTool::fillPlots(const LHCb::Particle::ConstVector& PV,
                                     const std::string trailer) {
  for ( LHCb::Particle::ConstVector::const_iterator p = PV.begin() ; p != PV.end() ; ++p ){
    StatusCode sc = fillPlots(*p,trailer);
    if (!sc) return sc;
  }
  return StatusCode::SUCCESS;
}; 
//=============================================================================
// 
//=============================================================================
StatusCode SimplePlotTool::fillPlots(const LHCb::Particle* p,
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
StatusCode SimplePlotTool::doPlot(const LHCb::Particle* P, MyHisto& H, 
                                  std::string trailer) {
  std::string var = H.getVar();
  ParticleProperty *pp = m_ppSvc->findByPythiaID(abs(P->particleID().pid()));
  std::string name = "Unknown";
  if (pp) name = pp->particle();
  if (trailer!="") name+=" "+trailer;
  double hmin = H.getMin();
  double hmax = H.getMax();

  debug() << "Filling plot for " << var << " of " << name << endmsg;

  // -----------------------------------------
  // MASS 
  // -----------------------------------------
  if (( var == "M" ) || ( var == "WM" )){ // mass or wide mass
    if ( P->endVertex()){
      double factor = 0.04; // 4%
      if ( var == "WM" ){
        name += " (wide)" ;
        factor = 0.15; // 15%
      }
      if ( hmax < 0 ){
        double mn = pp->mass() - gsl_max(factor*pp->mass(),pp->maxWidth()); // at least 4%
        double mx = pp->mass() + gsl_max(factor*pp->mass(),pp->maxWidth());
        plot(P->measuredMass(),"Mass of "+name,mn,mx );
      } else {
        plot(P->measuredMass(),"Mass of "+name,pp->mass()-fabs(hmin),pp->mass()+hmax );
      }
    }
  // -----------------------------------------
  // MASS Difference 
  // -----------------------------------------
  } else if ( var == "DM" ){
    if (P->endVertex()){
      const LHCb::Particle::ConstVector pv = P->daughtersVector();
      double maxmass = 0. ;
      int dpid = 0 ;
      for (LHCb::Particle::ConstVector::const_iterator i=pv.begin();i!=pv.end();++i) {
        if ( (*i)->endVertex() ){ // care only about daughters with an endVertex
          double dm = (*i)->measuredMass(); 
          if (dm>maxmass) {
            maxmass = dm ; // compare with heaviest daughter
            dpid = (*i)->particleID().pid();
          }
        }
      }
      if ( dpid!=0 ) {
        ParticleProperty *dp = m_ppSvc->findByPythiaID(abs(dpid));
        double dmn = pp->mass() - dp->mass() ;
        plot(P->measuredMass()-maxmass,"Mass difference of "+name, dmn-fabs(hmin),dmn+hmax );
        debug() << "Mass of " << P->particleID().pid() << " is " << P->measuredMass() 
                 << ", mass of daughter (" << dpid << ") is " << maxmass
                 << " -> difference = " << P->measuredMass()-maxmass << endmsg;
      } 
    }
  // -----------------------------------------
  // Momenta
  // -----------------------------------------
  } else if ( var == "P" ){
    plot(P->p(),"Momentum of "+name,hmin,hmax);
  } else if ( var == "Pt" ){
    plot(P->pt(),"Pt of "+name,hmin,hmax);
  } else if ( var == "Chi2" ){
    if (P->endVertex()) plot(P->endVertex()->chi2(),"Chi2 of "+name,hmin,hmax);
  // -----------------------------------------
  // IP or flight significances
  // -----------------------------------------
  } else if ( var == "IP" || var == "IPs" || var == "DPV" || var == "FS"){
    std::string PVContainer = m_onOfflineTool->getPVLocation() ;
    verbose() << "Getting PV from " << PVContainer << endreq ;
    if ( exist<LHCb::PrimVertices>(PVContainer)){
      LHCb::PrimVertices* PV = get<LHCb::PrimVertices>(PVContainer);
      double bestf = -1. , bestfe = -1., minip = 9999999.;
      for (LHCb::PrimVertex::Vector::const_iterator iv=PV->begin();iv!=PV->end();++iv) {
        double ip = -1 ,ipe = -1.;
        StatusCode sc = m_geomTool->calcImpactPar(*P, *(*iv), ip, ipe);
        if (!sc) continue;
        if ( var == "IP" ){
          plot(ip,"IP of "+name,hmin,hmax);
        } else if (( var == "IPs" ) && ( pp->mass() < 5*GeV )){ // not B's
          if (ipe>0) plot(ip/ipe,"Any IP/err of "+name,hmin,hmax);       
          // DPV or FS, or B - to "best PV"
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
        } else if (( var == "IPs" ) && ( pp->mass() >= 5*GeV )){
          plot(minip,"Smallest IP/err of "+name,hmin,hmax); 
        }
      }
    }
    // -----------------------------------------
    // Vertex
    // -----------------------------------------
  } else if ( var == "Vz" ){
    if (P->endVertex()) plot(P->endVertex()->position().z(),"Vz of "+name,hmin,hmax);
  } else if ( var == "Vx" ){
    if (P->endVertex()) plot(P->endVertex()->position().x(),"Vx of "+name,hmin,hmax);
  } else if ( var == "Vy" ){
    if (P->endVertex()) plot(P->endVertex()->position().y(),"Vy of "+name,hmin,hmax);
  } else if ( var == "Vr" ){
    if (P->endVertex()) plot(sqrt(P->endVertex()->position().Perp2()),"Vr of "+name,hmin,hmax);
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
  } else if ( var == "WM" ){
    m_min = -1. ;
    m_max = -1. ;
  } else if ( var == "DM" ){
    m_min = 5.*MeV ;
    m_max = 5.*MeV ;
  } else if ( var == "P" ){
    m_min = 0. ;
    m_max = 50.*GeV ;
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
  } else if ( var == "Vz" ){
    m_min = -150.*mm ;
    m_max = 150.*mm ;
  } else if ( ( var == "Vr" ) || ( var == "Vx" ) || ( var == "Vy" )){
    m_min = -10.*mm ;
    m_max = 10.*mm ;
  } else {
    return false;
  }
  return true ;
}


//=============================================================================
