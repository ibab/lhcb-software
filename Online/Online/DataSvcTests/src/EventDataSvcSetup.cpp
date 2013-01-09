#include "DataSvcTests/EventDataSvcSetup.h"
#include "GaudiKernel/SmartIF.h"

// C/C++ header files
#include <iostream>
#include <stdexcept>
#include <fstream>

using namespace Gaudi;
using namespace Tests;
using namespace std;

EventDataSvcSetup::EventDataSvcSetup() 
: m_pIS(0), m_pIDM(0) 
{
}

EventDataSvcSetup::~EventDataSvcSetup() 
{
}

void EventDataSvcSetup::setup(IEventDataSvc* p)  {
  SmartIF<IProperty> prp(p);
  prp->setProperty("EnableFaultHandler","true");
  prp->setProperty("ForceLeaves","true");
  prp->setProperty("RootName","/Event");
  prp->setProperty("RootCLID","1");
}

void EventDataSvcSetup::setup(IDataProviderSvc* p)  {
  SmartIF<IProperty> prp(p);
  prp->setProperty("EnableFaultHandler","true");
  prp->setProperty("ForceLeaves","true");
  prp->setProperty("RootName","/Event");
  prp->setProperty("RootCLID","1");
}

void EventDataSvcSetup::create_objects() {
  delete_objects();
  for(size_t n=0; n<tags.size();++n) {
    DataObject* pObj = new DataObject();
    objects.push_back(pObj);
    pObj->addRef();
  }
}

void EventDataSvcSetup::delete_objects() {
  for(size_t n=0; n<objects.size();++n) {
    delete objects[n];
  }
  objects.clear();
}

void EventDataSvcSetup::load_tags(const std::string& file_name) {
  ifstream input(file_name.c_str());
  string item;
  tags.clear();
  leaves.clear();
  while(input.good() ) {
    getline(input,item,'\n');
    if ( item.find('/') != string::npos ) {
      leaves.push_back(item);
      //cout << item << endl;
      tags.push_back(EventTag_t(item.c_str()));
    }
  }
  cout << "Read " << tags.size() << " tags." << endl;
}

void EventDataSvcSetup::load_commands(const std::string& file_name) {
  ifstream input(file_name.c_str());
  size_t idx;
  string item;
  tags.clear();
  leaves.clear();
  tags.clear();
  while(input.good() ) {
    getline(input,item,'\n');
    if ( (idx=item.find(' ')) != string::npos ) {
      string cmd = item.substr(0,idx);
      string leaf = item.substr(++idx);
      if      ( cmd == "RETRIEVE"  ) commands.push_back(RETRIEVE);
      else if ( cmd == "REGOBJ"    ) commands.push_back(REGOBJ);
      else if ( cmd == "SETROOT"   ) commands.push_back(SETROOT);
      else commands.push_back(NO_COMMAND);
      leaves.push_back(leaf);
      string tag = leaf[0] == '/' ? leaf : "/Event/"+leaf;
      tags.push_back(EventTag_t(tag.c_str()));
    }
  }
  cout << "Read " << tags.size() << " tags." << endl;
}

size_t EventDataSvcSetup::insert_leaves(IEventDataSvc* ds)   {
  size_t num_actions = 0;
  for(size_t i=0; i<leaves.size(); ++i, ++num_actions) {
    const EventTag_t& tag = tags[i];
    DataObject* pObj = objects[i];
    StatusCode sc = ds->put(tag,pObj);
    if ( !sc.isSuccess() ) {
      throw runtime_error("Fatal exception. cannot populate EventDataSvc.");
    }
  }
  m_pIDM->clearStore();
  return num_actions;
}

size_t EventDataSvcSetup::insert_leaves(IDataProviderSvc* ds)   {
  size_t num_actions = 0;
  StatusCode sc = m_pIDM->setRoot("/Event",new DataObject());
  for(size_t i=0; i<leaves.size(); ++i, ++num_actions) {
    const string& tag = leaves[i];
    DataObject* pObj = objects[i];
    StatusCode sc = ds->registerObject(tag,pObj);
    if ( !sc.isSuccess() ) {
      throw runtime_error("Fatal exception. cannot populate EventDataSvc.");
    }
  }
  m_pIDM->clearStore();
  return num_actions;
}

size_t EventDataSvcSetup::excute_commands(IEventDataSvc* ds)   {
  size_t num_actions = 0;
  for(size_t i=0; i<tags.size(); ++i, ++num_actions) {
    DataObject* pObj = 0;
    Command cmd = commands[i];
    const EventTag_t& tag = tags[i];
    StatusCode sc = StatusCode::SUCCESS;
    switch(cmd) {
    case RETRIEVE:
      pObj = ds->retrieve<DataObject>(tag);
      break;
    case REGOBJ:
      sc = ds->put(tag,objects[i]);
      break;
    case SETROOT:
      sc = ds->put(tag,objects[i]);
      break;
    default:
      --num_actions;
      break;
    }
    if ( !sc.isSuccess() ) {
      throw runtime_error("Fatal exception. cannot populate EventDataSvc.");
    }
  }
  m_pIDM->clearStore();
  return num_actions;
}

size_t EventDataSvcSetup::excute_commands(IDataProviderSvc* ds)  {
  size_t num_actions = 0;
  for(size_t i=0; i<tags.size(); ++i, ++num_actions) {
    DataObject* pObj = 0;
    Command cmd = commands[i];
    const string& tag = leaves[i];
    StatusCode sc = StatusCode::SUCCESS;
    switch(cmd) {
    case RETRIEVE:
      sc = ds->retrieveObject(tag,pObj);
      if ( sc == IDataProviderSvc::INVALID_ROOT ) sc = StatusCode::SUCCESS;
      if ( sc == IDataProviderSvc::INVALID_OBJ_PATH ) sc = StatusCode::SUCCESS;
      break;
    case REGOBJ:
      sc = ds->registerObject(tag,objects[i]);
      break;
    case SETROOT:
      sc = m_pIDM->setRoot(tag,objects[i]);
      break;
    default:
      --num_actions;
      break;
    }
    if ( !sc.isSuccess() ) {
      throw runtime_error("Fatal exception. cannot populate EventDataSvc.");
    }
  }
  m_pIDM->clearStore();
  return num_actions;
}
