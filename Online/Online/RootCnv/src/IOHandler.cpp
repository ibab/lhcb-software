// $Id: IOHandler.cpp,v 1.2 2010-01-11 17:13:39 frankb Exp $
//====================================================================
//
//	Package    : System (The POOL project)
//
//  Description: Generate neccessary infrastructure for the
//               SEAL plugin manager modules
//
//	@author      M.Frank
//====================================================================
#include "GaudiKernel/ObjectContainerBase.h"
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/System.h"
#include <stdexcept>
#include <iostream>
#include "TROOT.h"
#include "TFile.h"
#include "TClass.h"
#include "TSystem.h"
#include "TInterpreter.h"
#include "TClassStreamer.h"
#include "TStreamerInfo.h"
#include "Cintex/Cintex.h"
#include <memory>

using namespace ROOT::Reflex;
using namespace ROOT::Cintex;

template <class T> class IOHandler : public TClassStreamer {
protected:
  typedef ROOT::Reflex::Type TypeH;
  /// LCG Reflection type
  TypeH     m_type;
  /// ROOT persistent class description
  TClass*   m_root;
public:
  /// Initializing constructor
  IOHandler(const TypeH& typ, TClass* c) : m_type(typ), m_root(c) {  }
  /// Standard destructor
  virtual ~IOHandler() {  }
  /// ROOT I/O callback
  virtual void operator()(TBuffer &b, void *obj);
  /// Callback for reading the object
  virtual void get(TBuffer &b, void* obj);
  /// Callback for writing the object
  virtual void put(TBuffer &b, void* obj);
};


namespace ROOT { namespace Cintex  {
  bool IsTypeOf(Type& typ, const std::string& base_name);
}}
static const DataObject* last_link_object = 0;
static int               last_link_hint = -1;

void resetLastLink() {
  last_link_object = 0;
  last_link_hint   = -1;
}

using Gaudi::getCurrentDataObject;

namespace GaudiRoot {
  void pushCurrentDataObject(DataObject** pobjAddr) {
    Gaudi::pushCurrentDataObject(pobjAddr);
    resetLastLink();
  }

  void popCurrentDataObject() {
    Gaudi::popCurrentDataObject();
    resetLastLink();
  }
}

template <class T>
void IOHandler<T>::operator()(TBuffer &b, void *obj)  {
  try {
    if ( b.IsReading() )   {
      get(b,obj);
    }
    else  {
      put(b,obj);
    }
  }
  catch( const std::exception& e )    {
    std::string err = "Class:" + std::string(m_root->GetName()) + "> Exception in object I/O";
    err += e.what();
    throw std::runtime_error(err);
  }
  catch( ... )    {
    std::string err = "Class:" + std::string(m_root->GetName()) + "> Exception in object I/O";
    throw std::runtime_error(err);
  }
}

template <>
void IOHandler<SmartRefBase>::get(TBuffer &b, void* obj) {
  UInt_t start, count;
  SmartRefBase* ref = (SmartRefBase*)obj;
  Version_t version = b.ReadVersion(&start, &count, m_root);
  m_root->ReadBuffer(b, obj, version, start, count);
  switch( ref->objectType() ) {
   case SmartRefBase::DATAOBJECT:
    {
      SmartRef<DataObject>* r = (SmartRef<DataObject>*)obj;
      (*r)(getCurrentDataObject());
    }
    break;
  case SmartRefBase::CONTAINEDOBJECT:
   {
     SmartRef<ContainedObject>* r = (SmartRef<ContainedObject>*)obj;
     (*r)(getCurrentDataObject());
   }
   break;
  default:
    std::cout << "Hit uninitialized smartRef!!!!" << std::endl;
    break;
  }
}

template <>
void IOHandler<SmartRefBase>::put(TBuffer &b, void* obj) {
  SmartRefBase* ref = (SmartRefBase*)obj;
  SmartRef<DataObject>* r1 = (SmartRef<DataObject>*)ref;
  DataObject* curr = getCurrentDataObject();
  DataObject* pDO  = r1->data();
  int hint = r1->hintID();
  int link = r1->linkID();
  if ( pDO )  {
    switch( ref->objectType() ) {
    case SmartRefBase::CONTAINEDOBJECT:
      {
        SmartRef<ContainedObject>* r2 = (SmartRef<ContainedObject>*)ref;
        ContainedObject* p = r2->data();
        if ( p )  {
          const ObjectContainerBase* parent = p->parent();
          if ( parent )  {
            link = p->index();
            pDO  = const_cast<ObjectContainerBase*>(parent);
            break;
          }
        }
      }
      pDO = 0;
      std::cout << "IOHandler<SmartRefBase>::onWrite> "
                << "Found invalid smart reference with object "
                << "having no parent."
                << std::endl;
      throw std::runtime_error("IOHandler<SmartRefBase>::onWrite> "
                               "Found invalid smart reference with object "
                               "having no parent.");
      break;
    case SmartRefBase::DATAOBJECT:
      link = StreamBuffer::INVALID;
      break;
    default:
      break;
    }
    //if ( pDO == last_link_object && last_link_hint == -1 )  {
    //  std::cout << "IOHandler<SmartRefBase>::onWrite> "
    //            << "Found invalid smart reference."
    //            << std::endl;
    //}
    if ( pDO == last_link_object )  {
      ref->set(curr, last_link_hint, link);
      m_root->WriteBuffer(b, obj);
      return;
    }
    else if ( pDO ) {
      LinkManager* mgr = curr->linkMgr();
      IRegistry*   reg = pDO->registry();
      if ( reg && mgr )  {
        hint = mgr->addLink(reg->identifier(), pDO);
        last_link_hint   = hint;
        last_link_object = pDO;
      }
    }
    else {
      hint = link = StreamBuffer::INVALID;
    }
  }
  //if ( hint == StreamBuffer::INVALID )  {
  //  std::cout << "IOHandler<SmartRefBase>::onWrite> "
  //            << "Found invalid smart reference."
  //            << std::endl;
  //}
  ref->set(curr, hint, link);
  m_root->WriteBuffer(b, obj);
}

template <>
void IOHandler<ContainedObject>::get(TBuffer &b, void* obj) {
  UInt_t start, count;
  Version_t version = b.ReadVersion(&start, &count, m_root);
  m_root->ReadBuffer(b, obj, version, start, count);
  ContainedObject* p = (ContainedObject*)obj;
  p->setParent((ObjectContainerBase*)getCurrentDataObject());
}

template <>
void IOHandler<ContainedObject>::put(TBuffer &b, void* obj) {
  m_root->WriteBuffer(b, obj);
}

template <class T> static bool makeStreamer(MsgStream& log)  {
  std::string cl_name = System::typeinfoName(typeid(T));
  ROOT::Reflex::Type t = ROOT::Reflex::Type::ByName(cl_name);
  if ( t )  {
    TClass* c = gROOT->GetClass(cl_name.c_str());
    if ( c ) {
      TClassStreamer* s = new IOHandler<T>(t,c);
      c->AdoptStreamer(s);
      log << MSG::DEBUG << "Installed IOHandler for class " << cl_name << endmsg;
      return true;
    }
    else if ( !c )  {
      log << MSG::ERROR << "[No ROOT TClass] Failed to install IOHandler for class " << cl_name << endmsg;
    }
  }
  else  {
    log << MSG::ERROR << "[No Reflection Type] Failed to install IOHandler for class " << cl_name << endmsg;
  }
  return false;
}

namespace GaudiRoot  {
  bool patchStreamers(MsgStream& s)  {
    static bool first = true;
    if ( first ) {
      first = false;
      gSystem->Load("libCintex");
      gROOT->ProcessLine("#include <vector>");
      gInterpreter->EnableAutoLoading();
      gInterpreter->AutoLoad("DataObject");
      gROOT->ProcessLine("Cintex::Cintex::Enable()");    

      bool b2 = makeStreamer<SmartRefBase>(s);
      bool b3 = makeStreamer<ContainedObject>(s);
      return b2 && b3;
    }
    return true;
   }
}
