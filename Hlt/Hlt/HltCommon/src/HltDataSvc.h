#ifndef HLTDATASVC_H
#define HLTDATASVC_H
// ============================================================================
#include "HltBase/IHltDataSvc.h"
#include "Kernel/IANNSvc.h"
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
  
  StatusCode addSelection
  ( Hlt::Selection*   sel            ,
    const IAlgorithm* parent         ,
    const bool        useTES = false ) ;
  
  bool hasSelection(const stringKey& id) const;

  
  Hlt::Selection& selection
  ( const stringKey&  id     ,
    const IAlgorithm* parent ) ;
  
  std::vector<stringKey> selectionKeys();
  
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
  IDataProviderSvc& evtSvc() const;
  IANNSvc&          annSvc() const;
  //

  mutable IDataProviderSvc* m_evtSvc;
  mutable IANNSvc*          m_annSvc;
  std::auto_ptr<Hlt::Configuration> m_hltConf;
  
  std::map<stringKey,Hlt::Selection*> m_mapselections;
  std::vector<Hlt::Selection*>        m_ownedSelections; //owner of HltSelection
  
  std::vector<const IAlgorithm*>            m_parents;
  
  //  std::multimap<Hlt::Selection*,IAlgorithm*> m_users;     // which algorithms use a given selection?
  //  std::map<Hlt::Selection*,IAlgorithm*>      m_producers; // which algorithm produced a given selection?
};
#endif
