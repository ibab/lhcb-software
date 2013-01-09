#ifndef DATASVCTESTS_EVENTDATASVCSETUP_H
#define DATASVCTESTS_EVENTDATASVCSETUP_H

#include "GaudiKernel/IService.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IEventDataSvc.h"

#include "GaudiKernel/EventItemID.h"
#include "GaudiKernel/DataObject.h"
#include <vector>
#include <string>

namespace Tests {

  class EventDataSvcSetup {
  public:
    typedef Gaudi::EventTag_t EventTag_t;
    enum Command {
      SETROOT=1,
      RETRIEVE,
      REGOBJ,
      NO_COMMAND
    };

    /// Pointer to IService interface
    IService*                m_pIS;
    /// Pointer to data manager interface
    IDataManagerSvc*         m_pIDM;
    std::vector<EventTag_t>  tags;
    std::vector<std::string> leaves;
    std::vector<Command>     commands;
    std::vector<DataObject*> objects;

    EventDataSvcSetup();
    virtual ~EventDataSvcSetup();
    void load_tags(const std::string& file_name);
    void load_commands(const std::string& file_name);
    void create_objects();
    void delete_objects();
    void setup(IDataProviderSvc* p);
    void setup(Gaudi::IEventDataSvc* p);
    size_t excute_commands(Gaudi::IEventDataSvc* ds);
    size_t excute_commands(IDataProviderSvc* ds);
    size_t insert_leaves(Gaudi::IEventDataSvc* ds);
    size_t insert_leaves(IDataProviderSvc* ds);
  };
}
#endif // DATASVCTESTS_EVENTDATASVCSETUP_H
