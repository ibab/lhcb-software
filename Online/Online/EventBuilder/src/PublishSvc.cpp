// Framework include files


// Include files
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "EventBuilder/PublishSvc.h"

/*
 *   LHCb namespace declaration
 */
using namespace LHCb;
using namespace std;

// Factory for instantiation of service objects
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,PublishSvc)

StatusCode PublishSvc::queryInterface(const InterfaceID& riid, void** ppvIF) {
  if(IPublishSvc::interfaceID() == riid) {
    *ppvIF = dynamic_cast<IPublishSvc*> (this);
    addRef();
    return StatusCode::SUCCESS;
  }
  return Service::queryInterface(riid, ppvIF);
}

PublishSvc::PublishSvc(CSTR name, ISvcLocator* svcLoc): Service(name, svcLoc), m_started(false)
{
  declareProperty("ServicePrefix",m_Prefix="/Publisher/");
}

StatusCode PublishSvc::initialize() {
  StatusCode sc = Service::initialize();
  MsgStream msg(msgSvc(),name());
  if ( !sc.isSuccess() )  {
    msg << MSG::ERROR << "Cannot initialize base class." << endmsg;
    return sc;
  }
  undeclarePubAll();
  msg << MSG::INFO << " MonitorSvc of type PublishSvc initialized." << endmsg;
  return sc;
}

StatusCode PublishSvc::finalize() {
  undeclarePubAll();
  return Service::finalize();
}

StatusCode PublishSvc::start()
{
  PublishSvc::ITEM *itm;
  ItemMap::iterator it;
  DimServer::autoStartOn();
  for (it=this->m_items.begin();it!=m_items.end();it++)
  {
    itm = it->second;
    if (itm->m_srvc ==0)
    {
      itm->make_service();
    }
  }
  m_started = true;
  return StatusCode::SUCCESS;
}
void PublishSvc::regItem(CSTR item_name,const void* ptr, IType typ)
{
  ItemMap::iterator i = m_items.find(item_name);
  if ( i == m_items.end() )
  {
    ITEM *item = new ITEM();
    item->item_name = item_name;
    item->SrvName = m_Prefix+item_name;
    item->typ = typ;
    item->m_ptr = (void*)ptr;
    if (m_started)
    {
      item->make_service();
    }
    m_items.insert(make_pair(item_name,item));

  }
}

void PublishSvc::undeclarePubItem(CSTR nam)
{
  ItemMap::iterator it = m_items.find(nam);
  if (it!= m_items.end())
  {
    if (it->second->m_srvc != 0) delete (it->second->m_srvc);
    delete (it->second);
  }
  m_items.erase(it);
}

void PublishSvc::undeclarePubAll()
{
  for (ItemMap::iterator i = m_items.begin();i!=m_items.end();i++)
  {
    delete(i->second->m_srvc);
    delete (i->second);
  }
  m_items.clear();
}

//std::set<PublishSvc::ITEM *> *PublishSvc::getPubItems()
//{
//  static std::set<PublishSvc::ITEM *> infos;
//  infos.clear();
//  for (ItemMap::iterator i = m_items.begin();i!=m_items.end();i++)
//  {
//    infos.insert(i->second);
//  }
//  return &infos;
//}

void PublishSvc::updateItem(const std::string &name)
{
  ItemMap::iterator i = m_items.find(name);
  if (i!=m_items.end())
  {
    ITEM* it = i->second;
    switch (it->typ)
    {
      case IT_Bool:
      case IT_Long:
      case IT_Double:
      case IT_Int:
      {
        it->m_srvc->updateService();
        break;
      }
      case IT_String:
      {
        std::string s = *(std::string*)it->m_ptr +'\0';
        it->m_srvc->updateService((char*)((std::string*)(it->m_ptr))->c_str());
        break;
      }
      default :
      {
        break;
      }
    }
  }
}
void PublishSvc::updateAll()
{
  PublishSvc::ITEM *itm;
  for (ItemMap::iterator i = m_items.begin();i!=m_items.end();i++)
  {
    itm = i->second;
    updateItem(itm->item_name);
  }
}
