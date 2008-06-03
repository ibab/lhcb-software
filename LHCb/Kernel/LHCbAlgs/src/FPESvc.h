#include <vector>
#include <string>
#include "GaudiKernel/Service.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IIncidentSvc.h"

class FPESvc : public Service,virtual public IIncidentListener  {
public:
    FPESvc(const std::string& name, ISvcLocator* pSvcLocator);
    virtual StatusCode initialize();
    virtual void handle(const Incident&);
private:
   std::vector<std::string> m_mask;
   std::string              m_when;
   mutable MsgStream m_log;

   StatusCode enable(); 
};

