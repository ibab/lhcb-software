#ifndef GAUDISVC_STAGERCHRONOSVC_H
#define GAUDISVC_STAGERCHRONOSVC_H 1

#include "GaudiKernel/Service.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include <string>
#include <vector>


class IIncidentSvc;

class StagerChronoSvc : public extends1 <Service,IIncidentListener>
{
public:

   StagerChronoSvc(const std::string& nam, ISvcLocator* svcLoc);

   virtual StatusCode initialize();

   void handle(const Incident& inc);

   StatusCode finalize();

   virtual ~StagerChronoSvc();

private:

   IIncidentSvc* m_incidentSvc;
   struct timeval tp;
   double sec, usec;
   double m_beginInputFile, m_endInputFile;
   double m_totalWaitTime, m_totalProcessingTime;

};

#endif
