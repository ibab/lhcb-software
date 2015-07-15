#ifndef HLT2MONINFOSVC_H
#define HLT2MONINFOSVC_H

// STD & STL
#include <string>
#include <map>
#include <set>

// Gaudi
#include <GaudiKernel/Service.h>
#include <GaudiKernel/IIncidentListener.h>

// Hlt2Monitoring
#include <Hlt2Monitoring/RunInfo.h>

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

   bool receiveHistoInfo(zmq::socket_t& data) const;
   bool receiveRunInfo(zmq::socket_t& data) const;

   bool histoInfoRequest(zmq::socket_t& inf) const;
   bool runInfoRequest(zmq::socket_t& inf) const;
   
   // properties
   std::string m_hostRegex;

   // data members
   using histoKey_t = std::pair<Monitoring::RunNumber, Monitoring::HistId>;
   mutable boost::unordered_map<histoKey_t, Gaudi::Histo1DDef> m_histograms;
   mutable boost::unordered_map<histoKey_t, std::string> m_rates;

   mutable boost::unordered_map<Monitoring::RunNumber, int> m_startTimes;

   // Run Info
   mutable boost::unordered_map<Monitoring::RunNumber, Monitoring::RunInfo> m_runInfo;

};

#endif // HLT2MONINFOSVC_H
