// $Id: HltFunctionFactory.cpp,v 1.1 2007-06-20 12:17:37 hernando Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "HltFunctionFactory.h"
#include "HltBase/HltFunctions.h"

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
 
  declareProperty("Smart", m_smart = true);
}

Hlt::TrackFunction* HltFunctionFactory::trackFunction(int id) {
  Hlt::TrackFunction* fun = NULL;
  if (id == HltEnums::rIP) {
    if (!m_vertices) error() << " vertices not set in factory " << endreq;
    fun =  new Estd::binder_function<Track,RecVertex>(Hlt::rIP(),*m_vertices,
                                                      Estd::abs_min());
    info() << " created function rIP " << id << endreq;
  } else if (id == HltEnums::IP) {
    if (!m_vertices) error() << " vertices not set in factory " << endreq;
    fun =  new Estd::binder_function<Track,RecVertex>(Hlt::IP(), *m_vertices,
                                                      Estd::abs_min());
    info() << " created function IP " << id << endreq;
  } else if (id == HltEnums::PT) {
    fun = new Hlt::PT();
    info() << " created function PT " << id << endreq;
  } else if (id == HltEnums::DOCA) {
    if (!m_vertices) error() << " tracks not set in factory " << endreq;
    fun =  new Estd::binder_function<Track,Track>(Hlt::DOCA(),*m_tracks,
                                                  Estd::abs_min());
    info() << " created function DOCA " << id << endreq;
  } else if (id == HltEnums::IDsFraction) {
    if (!m_tracks) error() << " tracks [2] not set in factory " << endreq;
    fun =  new Estd::binder_function<Track,Track>(Hlt::MatchIDsFraction(), 
                                                  *m_tracks, Estd::abs_max());
    info() << " created function IDsFrunction " << id << endreq;
  }
  
  if (m_smart && fun) {
    Hlt::TrackFunction* fun1 = fun;
    fun = new Hlt::SmartFunction<Track>(id,*fun1);
    info() << " created smart function " << id << endreq;
    delete fun1;
  }
  if (!fun) error() << " requested track function not in factory " << endreq;
  return fun;
}

Hlt::VertexFunction* HltFunctionFactory::vertexFunction(int id) {
  Hlt::VertexFunction* fun = NULL;
  if (id == HltEnums::VertexDz) {
    if (!m_vertices) error() << " vertices not set in factory " << endreq;
    fun =  new Estd::binder_function<RecVertex,RecVertex>(Hlt::DZ(),*m_vertices,
                                                          Estd::abs_min());    
  } else if ( id == HltEnums::VertexPointing ) {
    if (!m_vertices) error() << " vertices not set in factory " << endreq;
    fun =  new Estd::binder_function<RecVertex,RecVertex>(Hlt::FC(),*m_vertices,
                                                          Estd::abs_min());
  } else if (id == HltEnums::VertexMinPT) {
    fun = new Hlt::minPT();
  } else if (id == HltEnums::VertexMaxPT) {
    fun = new Hlt::maxPT();
  }
  if (!fun) error() << " requested vertex function not in factory " << endreq;
  if (m_smart && fun) {
    Hlt::VertexFunction* fun1 = fun;
    fun = new Hlt::SmartFunction<RecVertex>(id,*fun1);
    delete fun1;
  }
  return fun;
}

Hlt::TrackFilter* HltFunctionFactory::trackFilter(int id,
                                             const std::string& mode,
                                             double x0, double xf) {
  Hlt::Info<Track> info(id);
  return trackFilter(info,mode,x0,xf);
}


Hlt::TrackFilter* HltFunctionFactory::trackFilter(const Hlt::TrackFunction& fun,
                                                  const std::string& mode,
                                                  double x0, double xf) {
  if (mode == "<") return (fun < x0).clone();
  else if (mode == ">") return (fun > x0).clone();
  else if (mode == "[]") return ((fun > x0) && (fun < xf)).clone();
  else if (mode == "||>") {
    Hlt::AbsFun<Track> afun(fun);
    return (afun > x0).clone();
  } else if ( mode == "||<") {
    Hlt::AbsFun<Track> afun(fun);
    return (afun < x0).clone();
  } else if (mode == "||[]") {
    Hlt::AbsFun<Track> afun(fun);
    return ((afun > x0) && (afun < xf)).clone();
  } 
  error() << " not able to create filter " << mode << x0 << xf << endreq;
  return NULL;
}

Hlt::VertexFilter* HltFunctionFactory::vertexFilter(int id,
                                                    const std::string& mode,
                                                    double x0, double xf) {
  Hlt::Info<RecVertex> info(id);
  return vertexFilter(info,mode,x0,xf);
}


Hlt::VertexFilter* HltFunctionFactory::vertexFilter(const Hlt::VertexFunction& fun,
                                                    const std::string& mode,
                                                    double x0, double xf) {
  if (mode == "<") return (fun < x0).clone();
  else if (mode == ">") return (fun > x0).clone();
  else if (mode == "[]") return ((fun > x0) && (fun < xf)).clone();
  else if (mode == "||>") {
    Hlt::AbsFun<RecVertex> afun(fun);
    return (afun > x0).clone();
  } else if ( mode == "||<") {
    Hlt::AbsFun<RecVertex> afun(fun);
    return (afun < x0).clone();
  } else if (mode == "||[]") {
    Hlt::AbsFun<RecVertex> afun(fun);
    return ((afun > x0) && (afun < xf)).clone();
  } 
}

Hlt::TrackBiFunction* HltFunctionFactory::trackBiFunction(int id) {
  Hlt::TrackBiFunction* bfun = NULL;
  switch(id) {
  case(HltEnums::DOCA): bfun = new Hlt::DOCA();
  }
  if (!bfun)
    error() << " track bifunction not in factory, id = " << id << endreq;
  return bfun;
}


//=============================================================================
// Destructor
//=============================================================================
HltFunctionFactory::~HltFunctionFactory() {} 

//=============================================================================
