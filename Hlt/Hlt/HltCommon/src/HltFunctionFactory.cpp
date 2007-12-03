// $Id: HltFunctionFactory.cpp,v 1.15 2007-12-03 16:47:22 hernando Exp $
// Include files
// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "HltFunctionFactory.h"
#include "HltBase/HltConfigurationHelper.h"
#include "Event/HltSummary.h"
#include "HltBase/EParser.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltFunctionFactory
//
// 2007-03-22 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( HltFunctionFactory );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltFunctionFactory::HltFunctionFactory( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IHltFunctionFactory>(this);

  declareProperty("DataSummaryLocation", 
                  m_dataSummaryLocation = LHCb::HltSummaryLocation::Default);

  declareProperty("Smart", m_smart = true);
}

StatusCode HltFunctionFactory::initialize() {
  
  IDataProviderSvc* hltsvc = NULL;
  StatusCode sc = serviceLocator()->service("HltDataSvc",hltsvc);
  if (!hltsvc) fatal() << " not able to create Hlt Svc provider " << endreq;

  m_conf = NULL;
  std::string loca = m_dataSummaryLocation+"/Configuration";
  Hlt::DataHolder<Hlt::Configuration>* holder = 
    get<Hlt::DataHolder<Hlt::Configuration> >(hltsvc,loca);
  if (!holder) fatal() << " not able to retrieve configuration " << endreq;

  m_conf = &(holder->object());
  
  return sc;
}

Hlt::TrackFunction* HltFunctionFactory::trackFunction(const std::string& fn) 
{
  Hlt::TrackFunction* fun = NULL;
  std::vector<std::string> cromos = EParser::parse(fn,",");
  std::string name = cromos[0];
  debug() << " function name " << name << endreq;
  if (name == "rIP") {
    if (!m_vertices) error() << " vertices not set in factory " << endreq;
    fun =  new Estd::binder_function<Track,RecVertex>(Hlt::rIP(),*m_vertices,
                                                      Estd::abs_min());
    debug() << " created function rIP " << name << endreq;
  } else if (name == "IP") {
    if (!m_vertices) error() << " vertices not set in factory " << endreq;
    fun =  new Estd::binder_function<Track,RecVertex>(Hlt::IP(), *m_vertices,
                                                      Estd::abs_min());
    debug() << " created function IP " << name << endreq;
  } else if (name == "IPKey") {
    if (!m_vertices) error() << " vertices not set in factory " << endreq;
    Estd::binder_function<Track,RecVertex>* bfun =
      new Estd::binder_function<Track,RecVertex>(Hlt::IP(), *m_vertices,
                                                 Estd::abs_min());
    fun = new Hlt::BinderKey<Track,RecVertex>(*bfun);
    delete bfun;
    debug() << " created function IPKey " << name << endreq;
  } else if (name == "PT") {
    fun = new Hlt::PT();
    debug() << " created function PT " << name << endreq;
  } else if (name == "DOCA") {
    if (!m_vertices) error() << " tracks not set in factory " << endreq;
    fun =  new Estd::binder_function<Track,Track>(Hlt::DOCA(),*m_tracks,
                                                  Estd::abs_min());
    debug() << " created function DOCA " << name << endreq;
  } else if (name == "DOCAKey") {
    if (!m_vertices) error() << " tracks not set in factory " << endreq;
    Estd::binder_function<Track,Track>* bfun = 
      new  Estd::binder_function<Track,Track> (Hlt::DOCA(),*m_tracks,
                                               Estd::abs_min());
    fun = new Hlt::BinderKey<Track,Track> (*bfun) ;
    delete bfun;
    debug() << " created function DOCAKey " << name << endreq;
  } else if (name == "IDsFraction") {
    if (!m_tracks) error() << " tracks [2] not set in factory " << endreq;
    fun =  new Estd::binder_function<Track,Track>(Hlt::MatchIDsFraction(), 
                                                  *m_tracks, Estd::abs_max());
    debug() << " created function IDsFrunction " << name << endreq;
  } else if (name == "IDsFractionKey") {
    if (!m_tracks) error() << " tracks [2] not set in factory " << endreq;
    Estd::binder_function<Track,Track>* bfun = 
      new Estd::binder_function<Track,Track> (Hlt::MatchIDsFraction(),
                                              *m_tracks, Estd::abs_max());
    fun = new Hlt::BinderKey<Track,Track>(*bfun);
    delete bfun;
    debug() << " created function IDsFrunction Key " << name << endreq;
  } else if (name == "Calo2DChi2") {
    if (!m_tracks) error() << " tracks [2] not set in factory " << endreq;
    ITrackMatch* imatch = tool<ITrackMatch>("HltRZVeloTCaloMatch");
    fun =  new Estd::binder_function<Track,Track>(Hlt::TrackMatch(*imatch), 
                                                  *m_tracks, Estd::abs_min());
  } else if (name == "Calo2DChi2Key") {
    if (!m_tracks) error() << " tracks [2] not set in factory " << endreq;
    ITrackMatch* imatch = tool<ITrackMatch>("HltRZVeloTCaloMatch");
    Estd::binder_function<Track,Track>* bfun = 
      new Estd::binder_function<Track,Track> (Hlt::TrackMatch(*imatch), 
                                              *m_tracks, Estd::abs_min());
    fun = new Hlt::BinderKey<Track,Track>(*bfun);
    delete bfun;
  } else if (name == "Calo3DChi2") {
    if (!m_tracks) error() << " tracks [2] not set in factory " << endreq;
    ITrackMatch* imatch = tool<ITrackMatch>("HltVeloTCaloMatch");
    fun =  new Estd::binder_function<Track,Track>(Hlt::TrackMatch(*imatch), 
                                                  *m_tracks, Estd::abs_min());

  } else if (name == "Calo3DChi2ETCalo") {
    if (!m_tracks) error() << " tracks [2] not set in factory " << endreq;
    ITrackMatch* imatch = tool<ITrackMatch>("HltVeloTCaloMatch");
    Estd::binder_function<Track,Track>*  bfun = 
      new Estd::binder_function<Track,Track>(Hlt::TrackMatch(*imatch), 
                                             *m_tracks, Estd::abs_min());
    fun = new Hlt::BinderValue<Track,Track>(*bfun, &Track::p);
    delete bfun;
  } else if (name == "Calo3DChi2Key") {
    if (!m_tracks) error() << " tracks [2] not set in factory " << endreq;
    ITrackMatch* imatch = tool<ITrackMatch>("HltVeloTCaloMatch");
    Estd::binder_function<Track,Track>* bfun = 
      new Estd::binder_function<Track,Track>(Hlt::TrackMatch(*imatch), 
                                             *m_tracks, Estd::abs_min());
    fun = new Hlt::BinderKey<Track,Track>(*bfun);
    delete bfun;
  } else if (name == "MatchOffTrack2OnCaloKey") {
    if (!m_tracks) error() << " tracks [2] not set in factory " << endreq;
    ITrackMatch* imatch = tool<ITrackMatch>("HltMatchOffTrack2OnCalo");
    fun =  new Estd::binder_function<Track,Track>(Hlt::TrackMatch(*imatch), 
                                                  *m_tracks, Estd::abs_max());
  } else if (name == "ETCalo"){
    fun = new Hlt::ETCalo();
  } else if (name == "DeltaE"){
    fun = new Hlt::DeltaE();
  } else if (name == "FlagL0Candidate"){
    fun = new Hlt::TrackFlag(Track::L0Candidate);
  }
  
  
  if (m_smart && fun) {
    int id = HltConfigurationHelper::getID(*m_conf,"InfoID",name);
    Hlt::TrackFunction* fun1 = fun;
    fun = new Hlt::SmartFunction<Track>(id,*fun1);
    debug() << " created smart function " << name << " id " << id << endreq;
    delete fun1;
  }
  if (!fun) fatal() << " requested track function " << name 
                    << " not in factory " << endreq;
  return fun;
}


Hlt::VertexFunction* HltFunctionFactory::vertexFunction(const std::string& fn) {
  Hlt::VertexFunction* fun = 0;
  std::vector<std::string> cromos = EParser::parse(fn,",");
  std::string name = cromos[0];
  if (name == "VertexDz") {
    if (!m_vertices) error() << " vertices not set in factory " << endreq;
    fun =  new Estd::binder_function<RecVertex,RecVertex>(Hlt::DZ(),*m_vertices,
                                                          Estd::abs_min());    
  } else if ( name == "VertexPointing") {
    if (!m_vertices) error() << " vertices not set in factory " << endreq;
    fun =  new Estd::binder_function<RecVertex,RecVertex>(Hlt::FC(),*m_vertices,
                                                          Estd::abs_min());
  } else if (name == "VertexMinPT") {
    fun = new Hlt::minPT();
  } else if (name == "VertexMaxPT") {
    fun = new Hlt::maxPT();
  } else if (name == "VertexDimuonMass") {
    fun = new Hlt::VertexDimuonMass();
  } else if (name == "VertexSumPT") {
    fun = new Hlt::VertexSumPT();
  }
  
  
  if (m_smart && fun) {
    int id = HltConfigurationHelper::getID(*m_conf,"InfoID",name);
    Hlt::VertexFunction* fun1 = fun;
    fun = new Hlt::SmartFunction<RecVertex>(id,*fun1);
    debug() << " created smart function " << name << " id " << id << endreq;
    delete fun1;
  }
  if (!fun) fatal() << " requested vertex function " << name
                    << " not in factory " << endreq;
  return fun;
}

Hlt::TrackBiFunction* HltFunctionFactory::trackBiFunction(const std::string& fn) {
  Hlt::TrackBiFunction* bfun = NULL;
  std::vector<std::string> cromos = EParser::parse(fn,",");
  std::string name = cromos[0];
  if (name == "DOCA")
    bfun = new Hlt::DOCA();
  else if (name == "Calo3DChi2") {
    ITrackMatch* imatch = tool<ITrackMatch>("HltVeloTCaloMatch");
    bfun =  new Hlt::TrackMatch(*imatch);
  } else if (name == "DeltaP") {
    bfun = new Hlt::DeltaP();
  } else if (name == "DimuonMass") {
    bfun = new Hlt::DimuonMass();
  } else if (name == "SumPT") {
    bfun = new Hlt::SumPT();
  }
  
  if (!bfun) fatal() << " requested track bifunction " << name
                     << " not in factory " << endreq;
  return bfun;
}

Hlt::TrackFilter* HltFunctionFactory::trackFilter(const std::string& name) {

  std::string funname= "";
  std::string mode= "";
  float x0 = 0.;
  float xf = 1e6;
  if (!EParser::parseFilter(name,funname,mode,x0,xf)) {
    fatal() << " not able to parse " << name << endreq;
    return NULL;  
  }
  if (m_smart) {
    int id = HltConfigurationHelper::getID(*m_conf,"InfoID",funname);
    Hlt::Info<Track> info(id);
    return makeFilter(info,mode,x0,xf);
  } 
  Hlt::TrackFunction* fun = trackFunction(funname);
  Hlt::TrackFilter* filter = makeFilter(*fun,mode,x0,xf);
  delete fun;
  return filter;
}

Hlt::VertexFilter* HltFunctionFactory::vertexFilter(const std::string& name) {

  std::string funname= "";
  std::string mode= "";
  float x0 = 0.;
  float xf = 1e6;
  if (!EParser::parseFilter(name,funname,mode,x0,xf)) {
    fatal() << " not able to parse " << name << endreq;
    return NULL;  
  }
  if (m_smart) {
    int id = HltConfigurationHelper::getID(*m_conf,"InfoID",funname);
    Hlt::Info<RecVertex> info(id);
    return makeFilter(info,mode,x0,xf);
  }
  Hlt::VertexFunction* fun = vertexFunction(funname);
  Hlt::VertexFilter* filter = makeFilter(*fun,mode,x0,xf);
  delete fun;  
  return filter;
}


//=============================================================================
// Destructor
//=============================================================================
HltFunctionFactory::~HltFunctionFactory() {} 

//=============================================================================
