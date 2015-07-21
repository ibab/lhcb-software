/*
 * PublishSvc.h
 *
 *  Created on: May 2, 2014
 *      Author: Beat Jost
 */

#ifndef PUBLISHSVC_H_
#define PUBLISHSVC_H_
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IPublishSvc.h"
#include <map>
#include "dis.hxx"

namespace LHCb
{

  /** @class PublishSvc PublishSvc.h GaudiOnline/PublishSvc.h
    *
    *  This class implements the IMonitorSvc interface, and publishes Gaudi variables
    *  into a memory area.
    *
    *  @author Markus Frank
    */
  typedef enum
  {
    IT_None = 0,
    IT_Bool,
    IT_Int,
    IT_Long,
    IT_Double,
    IT_String,
    IT_Float
  }IType;
  class PublishSvc : public Service, virtual public IPublishSvc
  {
    typedef const std::string& CSTR;
    class ITEM
    {
      public:
      std::string item_name;
      void *m_ptr;
      IType typ;
      DimService *m_srvc;
      std::string SrvName;
      ITEM() :item_name(""),m_ptr(0),typ(IT_None),m_srvc(0), SrvName("") {}
      ITEM(std::string &nam, void *ptr, IType t) : item_name(nam),m_ptr(ptr),typ(t),m_srvc(0), SrvName("") {};
      void make_service()
      {
        switch(typ)
        {
          case IT_Bool:
          case IT_Int:
          {
            m_srvc = new DimService(SrvName.c_str(),*((int*)m_ptr));
            break;
          }
          case IT_Long:
          {
            m_srvc = new DimService(SrvName.c_str(),*((long long *)m_ptr));
            break;
          }
          case IT_String:
          {
            m_srvc = new DimService(SrvName.c_str(),(char*)"");
            break;
          }
          case IT_Float:
          {
            m_srvc = new DimService(SrvName.c_str(),*((float*)m_ptr));
            break;
          }
          case IT_Double:
          {
            m_srvc = new DimService(SrvName.c_str(),*((double*)m_ptr));
          }
          break;
          case IT_None:
          {

          }
          break;
        };
      };

    };
    class Items : public std::map<std::string,ITEM*>
    {
      public:
      Items() : std::map<std::string,ITEM*>() {}
      Items(const Items& c) : std::map<std::string,ITEM*>(c) {}
      ITEM* getItem(std::string n){map<std::string,ITEM*>::iterator it =find(n);return (it==end()) ? 0: it->second;}
    };
    typedef std::map<std::string,ITEM*> ItemMap;
//    typedef std::pair<ClientMap::iterator,Items::iterator> PubInfo;

    /// Internal helper to declare monitoring items
    void regItem(CSTR item_name,const void* ptr, IType typ);

  public:
    /// Service constructor
    PublishSvc(CSTR name, ISvcLocator* svcLoc);
    /// Standard destructor
    virtual ~PublishSvc() {}
    /// IInterface pure virtual member functions
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIF);
    /// Initialize service
    virtual StatusCode initialize();
    /// Finalize service
    virtual StatusCode finalize();
    virtual StatusCode start();
    std::string m_Prefix;
#define _DECL(t,typ)  virtual void declarePubItem(CSTR n,t& v) {regItem(n,&v,typ); }
//#define _DECLP(t) virtual void declarePubItem(CSTR n,t  v,CSTR d,Client c) {regItem(n,v,d,c);  }
    typedef std::pair<double,double> Rate;
      _DECL(const bool,IT_Bool)
      _DECL(const int,IT_Int)
      _DECL(const long,IT_Long)
      _DECL(const double,IT_Double)
      _DECL(const std::string,IT_String)
      _DECL(bool,IT_Bool)
      _DECL(int,IT_Int)
      _DECL(long,IT_Long)
      _DECL(double,IT_Double)
      _DECL(float,IT_Float)
      _DECL(std::string,IT_String)


    /** Undeclare monitoring information
      * @param name Monitoring information name knwon to the external system
      * @param owner Owner identifier of the monitoring information
      */
    virtual void undeclarePubItem(CSTR nam);

    /** Undeclare monitoring information
      * @param owner Owner identifier of the monitoring information
      */
    virtual void undeclarePubAll() ;

    /** Get the names for all declared monitoring informations for a given
      * owner. If the owner is NULL, then it returns for all owners
      */
    virtual void updateItem(const std::string &name);
    virtual void updateAll();
    bool m_started;
  private:
    /// Map associating to each algorithm name a set with the info items
    ItemMap         m_items;
  };
}      // End namespace gaudi



#endif /* PUBLISHSVC_H_ */
