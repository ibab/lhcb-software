#ifndef HLTDATASVC_H
#define HLTDATASVC_H

#include "HltBase/IHltDataSvc.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include <string>

class IDataManagerSvc;

class HltDataSvc : public Service
                 , virtual public IHltDataSvc  
                 , virtual public IIncidentListener
{

public:
  /// Standard constructor
    HltDataSvc(const std::string& name, ISvcLocator* pSvcLocator );
    ~HltDataSvc();

    StatusCode initialize();
    StatusCode finalize();
    StatusCode queryInterface(const InterfaceID& riid,
                                    void** ppvUnknown) ;

    void resetData();
    StatusCode addSelection(Hlt::Selection* sel);
    bool hasSelection(const stringKey& id) const;
    Hlt::Selection& selection(const stringKey& id);
    std::vector<stringKey> selectionKeys();
    void clean();

    Hlt::Configuration& config();

    void handle(const Incident&);
private:
  MsgStream& msg(MSG::Level level) const;

  MsgStream& verbose() const { return msg(MSG::VERBOSE); }
  MsgStream& debug() const { return msg(MSG::DEBUG); }
  MsgStream& info() const { return msg(MSG::INFO); }
  MsgStream& warning() const { return msg(MSG::WARNING); }
  MsgStream& error() const { return msg(MSG::ERROR); }
  MsgStream& fatal() const { return msg(MSG::FATAL); }
  MsgStream& always() const { return msg(MSG::ALWAYS); }

  mutable std::auto_ptr<MsgStream>     m_msg;
  IDataProviderSvc& hltSvc() const;
  IDataProviderSvc& evtSvc() const;
  //
  // return the location of the data
  const std::string& hltConfigurationLocation() const {return m_hltConfigurationLocation;}

     IDataManagerSvc* m_hltMan;
     mutable IDataProviderSvc* m_hltSvc;
     mutable IDataProviderSvc* m_evtSvc;
     Hlt::Configuration*        m_hltConf;
     std::string m_hltConfigurationLocation;

    std::map<stringKey,Hlt::Selection*> m_mapselections; //owner of HltSelection
};
#endif
