#ifndef HLT2MONINFOSVC_H
#define HLT2MONINFOSVC_H

// STD & STL
#include <string>
#include <map>
#include <set>

// Gaudi
#include <GaudiKernel/Service.h>
#include <GaudiKernel/IIncidentListener.h>

// ZeroMQ
#include <ZeroMQ/IZeroMQSvc.h>

// local
#include "Hlt2MonBaseSvc.h"

class ISvcLocator;
class IIncidentSvc;

/** @class Hlt2MonInfoSvc

 */

class Hlt2MonInfoSvc : public Hlt2MonBaseSvc {
public:

   Hlt2MonInfoSvc(const std::string& name, ISvcLocator* sl);
   virtual ~Hlt2MonInfoSvc();

   // Service pure virtual member functions
   virtual StatusCode initialize();

   virtual void function() override;

private:

   void runTop();
   void runRelay();

   // properties
   std::string m_hostRegex;

};

#endif // HLT2MONINFOSVC_H
