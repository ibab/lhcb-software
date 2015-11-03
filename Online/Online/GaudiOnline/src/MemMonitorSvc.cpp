// Include files
#include "GaudiKernel/Service.h"
#define NO_LONGLONG_TYPEDEF
#include "dis.hxx"

#include "GaudiKernel/IAlgorithm.h"
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiOnline/MemMonitorSvc.h"
#include "AIDA/IAxis.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IHistogram3D.h"

using namespace AIDA;
using namespace LHCb;
using namespace std;
enum { OFLW = IAxis::OVERFLOW_BIN, UFLW = IAxis::UNDERFLOW_BIN };

MonitoringEngine::Histogram::Histogram(const string& nam,IMessageSvc* msg,IHistogram* h)
: m_messageSvc(msg), m_hist(h)
{
  MsgStream log(m_messageSvc,nam);
  size_t size = 0;
  switch(m_hist.HIST->dimension())  {
    case 1:
      // 1Hd data: dimension,nXBins,xMin,xMax,entries,2*(UNDERFLOW,"in range" bins, OVERFLOW): entries and errors
      size = (5+2*(m_hist.h1->axis().bins()+2));
      m_data.resize(size);
      m_data[0] = Buffer::value_type(m_hist.h1->dimension());
      m_data[1] = Buffer::value_type(m_hist.h1->axis().bins());
      m_data[2] = Buffer::value_type(m_hist.h1->axis().lowerEdge());
      m_data[3] = Buffer::value_type(m_hist.h1->axis().upperEdge());
      break;
    case 2:
      // 2Hd data: dimension,nXBins,xMin,xMax,nYBins,yMin,yMax,entries,
      // 2*((UNDERFLOW,"in range" bins, OVERFLOW)_x*(UNDERFLOW,"in range" bins,OVERFLOW)_y)
      size = (8+2*(m_hist.h2->xAxis().bins()+2)*(m_hist.h2->yAxis().bins()+2));
      m_data.resize(size);
      m_data[0] = Buffer::value_type(m_hist.h2->dimension());
      m_data[1] = Buffer::value_type(m_hist.h2->xAxis().bins());
      m_data[2] = Buffer::value_type(m_hist.h2->xAxis().lowerEdge());
      m_data[3] = Buffer::value_type(m_hist.h2->xAxis().upperEdge());
      m_data[4] = Buffer::value_type(m_hist.h2->yAxis().bins());
      m_data[5] = Buffer::value_type(m_hist.h2->yAxis().lowerEdge());
      m_data[6] = Buffer::value_type(m_hist.h2->yAxis().upperEdge());
      break;
    default:
      log << MSG::ERROR << "Unable to instantiate a DimHisto for "
          << m_hist.HIST->dimension() << "D histogram " << nam << endmsg;
      throw std::runtime_error("Monitoring cannot handle histogram:"+nam);
  }
}

void MonitoringEngine::Histogram::fill_1d()  {
  IHistogram1D* h1 = m_hist.h1;
  int i, nx = h1->axis().bins();
  float *p = &m_data[5];

  *p++ = Buffer::value_type(h1->allEntries());
  // Entries (or should be height=entry*weight ?)
  *p++ = Buffer::value_type(h1->binHeight(UFLW));
  for(i=0; i<nx; i++) *p++ = Buffer::value_type(h1->binHeight(i));
  *p++ = Buffer::value_type(h1->binHeight(OFLW));
  // Errors
  *p++ = Buffer::value_type(h1->binError(UFLW));
  for(i=0; i<nx; i++) *p++ = Buffer::value_type(h1->binError(i));
  *p++ = Buffer::value_type(h1->binError(OFLW));
}

void MonitoringEngine::Histogram::fill_2d()  {
  float *p = &m_data[7];
  IHistogram2D* h2 = m_hist.h2;
  int i, j, nx = h2->xAxis().bins(), ny = h2->yAxis().bins();

  *p++ = Buffer::value_type(h2->allEntries());
  // Entries (in fact, height=entry*weight ?)
  *p++ = Buffer::value_type(h2->binHeight(UFLW,UFLW));
  for(i=0;i<ny; i++) *p++ = Buffer::value_type(h2->binHeight(UFLW,i));
  *p++ = Buffer::value_type(h2->binHeight(UFLW,OFLW));
  for(i=0; i<nx; i++) {
    *p++ = Buffer::value_type(h2->binHeight(i,UFLW));
    for (j=0; j<ny; j++)
      *p++= Buffer::value_type(h2->binHeight(i,i));
    *p++= Buffer::value_type(h2->binHeight(i,OFLW));
  }
  *p++= Buffer::value_type(h2->binHeight(OFLW,UFLW));
  for (i=0; i<ny; i++) *p++= Buffer::value_type(h2->binHeight(OFLW,i));
  *p++= Buffer::value_type(h2->binHeight(OFLW,OFLW));

  // Errors
  *p++= Buffer::value_type(h2->binError(UFLW,UFLW));
  for (i=0; i<ny; i++) *p++= Buffer::value_type(h2->binError(UFLW,i));
  *p++= Buffer::value_type(h2->binError(UFLW,OFLW));
  for (i=0; i<nx; i++){
    *p++= Buffer::value_type(h2->binError(i,UFLW));
    for (j=0; j<ny; j++)
      *p++= Buffer::value_type(h2->binError(i,j));
    *p++= Buffer::value_type(h2->binError(i,OFLW));
  }
  *p++= Buffer::value_type(h2->binError(OFLW,UFLW));
  for (i=0; i<ny; i++) *p++= Buffer::value_type(h2->binError(OFLW,i));
  *p++= Buffer::value_type(h2->binError(OFLW,OFLW));
}

void MonitoringEngine::Histogram::filldata()  {
  size_t size = 0;
  switch (m_hist.HIST->dimension()) {
  case 1:
    size = 5+2*(m_hist.h1->axis().bins()+2);
    if ( size == m_data.size() )  {
      fill_1d();
      return;
    }
    // Error: allocated space does not match!
    throw std::runtime_error("Histogram datasize mismatch while monitoring!");
  case 2:
    //Sum of the entries in all the IHistogram's bins, i.e in-range bins, UNDERFLOW and OVERFLOW.
    //This is equivalent to the number of times the method fill was invoked.
    size = (8+2*(m_hist.h2->xAxis().bins()+2)*(m_hist.h2->yAxis().bins()+2));
    if ( size == m_data.size() )  {
      return;
    }
    // Error: allocated space does not match!
    throw std::runtime_error("Histogram mismatch while monitoring!");
  default:
    throw std::runtime_error("Unknown Histogram type while monitoring! [Internal error]");
   }
}

StatusCode MonitoringEngine::initialize()  {
  StatusCode sc = Service::initialize();
  MsgStream msg(msgSvc(),name());
  if ( !sc.isSuccess() )  {
    msg << MSG::ERROR << "Cannot initialize service base class." << endmsg;
  }
  sc = allocatePubarea();
  if ( !sc.isSuccess() )  {
    msg << MSG::ERROR << "Cannot allocate publishing area." << endmsg;
  }
  return sc;
}

StatusCode MonitoringEngine::finalize()  {
  StatusCode sc = deallocatePubarea();
  if ( !sc.isSuccess() )  {
    MsgStream msg(msgSvc(),name());
    msg << MSG::ERROR << "Cannot deallocate publishing area." << endmsg;
  }
  return Service::finalize();
}

// Constructor
MemMonitorSvc::MemMonitorSvc(CSTR name, ISvcLocator* sl)
: Service(name, sl), m_engine(0)
{
  m_engineType = "MonitoringEngine";
  declareProperty("Engine", m_engineType);
}

StatusCode MemMonitorSvc::queryInterface(const InterfaceID& riid, void** ppvIF) {
  if(IMonitorSvc::interfaceID() == riid) {
    *ppvIF = dynamic_cast<IMonitorSvc*> (this);
    addRef();
    return StatusCode::SUCCESS;
  }
  return Service::queryInterface(riid, ppvIF);
}

StatusCode MemMonitorSvc::initialize() {
  MsgStream msg(msgSvc(),name());
  StatusCode sc = Service::initialize();
  if ( !sc.isSuccess() )  {
    msg << MSG::ERROR << "Cannot initialize base class." << endmsg;
  }
  sc = service(m_engineType, m_engine);
  if ( !sc.isSuccess() )  {
    msg << MSG::ERROR << "Cannot access monitoring engine:" << m_engineType << endmsg;
  }
  return sc;
}

StatusCode MemMonitorSvc::finalize() {
  MsgStream msg(msgSvc(),name());
  for(ClientMap::iterator i=m_clients.begin(); i != m_clients.end(); ++i)  {
    undeclareItems(msg,(*i).second);
  }
  m_clients.clear();
  if ( m_engine ) m_engine->release();
  m_engine = 0;
  return Service::finalize();
}

MemMonitorSvc::ClientMap::iterator MemMonitorSvc::regClient(Client c) {
  ClientMap::iterator i = m_clients.find(c);
  if( i == m_clients.end() )  {
    string owner = clientName(c);
    pair<ClientMap::iterator,bool> res = m_clients.insert(make_pair(c,Items(owner)));
    return res.first;
  }
  return i;
}

MemMonitorSvc::RegInfo MemMonitorSvc::regItem(CSTR nam,CSTR dsc,Client c) {
  ClientMap::iterator i = regClient(c);
  if ( i != m_clients.end() )  {
    MsgStream log(msgSvc(),name());
    auto_ptr<Item> itm(new Item(nam,dsc));
    pair<Items::iterator,bool> p = (*i).second.insert(itm.get());
    if( p.second ) {
      itm.release();
      log << MSG::DEBUG << "Declaring monitor item:" << (*i).second.owner
          << "/" << nam << endreq;
      return RegInfo(i,p.first);
    }
    // Insertion failed: Name already exists
    log << MSG::ERROR << "Already existing monitor item:" << (*i).second.owner
        << "/" << nam << " not published" << endreq;
    return RegInfo(m_clients.end(),Items::iterator());
  }
  return RegInfo(m_clients.end(),Items::iterator());
}

void MemMonitorSvc::undeclareItem(MsgStream& log, CSTR owner, Item* itm)  {
  if ( itm )  {
    log << MSG::DEBUG << "revoke Item:" << owner << "/" << itm->first << endmsg;
    m_engine->revokeItem(owner,itm->first);
    delete itm;
  }
}

void MemMonitorSvc::undeclareItems(MsgStream& log, Items& itms)  {
  for(Items::iterator j=itms.begin(); j != itms.end(); ++j)
    undeclareItem(log, itms.owner, (*j));
  itms.clear();
}

void MemMonitorSvc::undeclareInfo(CSTR nam, Client owner)  {
  MsgStream log(msgSvc(),name());
  ClientMap::iterator i = m_clients.find(owner);
  if( i != m_clients.end() )  {
    Items& itms = (*i).second;
    for(Items::iterator j=itms.begin(); j != itms.end(); ++j )  {
      if ( (*j)->first == nam )  {
        undeclareItem(log, itms.owner, *j);
        itms.erase(j);
        return;
      }
    }
    log << MSG::WARNING << "revoke Item:" << itms.owner << "/" << nam
        << " not found" << endreq;
    return;
  }
  log << MSG::WARNING << "undeclareInfo: Unknown client:" << clientName(owner) << endreq;
}

void MemMonitorSvc::undeclareAll(Client c)    {
  MsgStream log(msgSvc(),name());
  ClientMap::iterator i = (0==c) ? m_clients.begin() : m_clients.find(c);
  for( ; i != m_clients.end(); ++i)  {
    undeclareItems(log,(*i).second);
    if ( c ) {
      if ( (*i).second.empty() )  {
        m_clients.erase(i);
      }
      return;
    }
  }
  m_clients.clear();
}

set<string>* MemMonitorSvc::getInfos(Client owner)  {
  static set<string> infos;
  infos.clear();
  ClientMap::iterator i = m_clients.find(owner);
  if ( i != m_clients.end() )  {
    for(Items::const_iterator j=(*i).second.begin(); j != (*i).second.end(); ++j)
      infos.insert((*j)->first);
  }
  return &infos;
}

string MemMonitorSvc::clientName(Client owner)  {
  const INamedInterface* c = dynamic_cast<const INamedInterface*>(owner);
  if ( c ) return c->name();
  // NULL pointer or Unknown interface:
  return "";
}

void MemMonitorSvc::declareInfo(CSTR nam, const AIDA::IBaseHistogram* var,CSTR dsc,Client own)
{ i_publish(regItem(nam,dsc,own),dynamic_cast<const AIDA::IHistogram*>(var)); }
