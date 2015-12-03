#ifndef HLT2ROOTPUBLISH_H
#define HLT2ROOTPUBLISH_H 1

// Include files
#include <string>
#include <unordered_map>
#include <chrono>

// boost
#include <boost/unordered_map.hpp>

// Gaudi
#include <GaudiKernel/Service.h>
#include <GaudiKernel/HistoDef.h>
#include <GaudiKernel/IIncidentListener.h>

// Local
#include "Hlt2Monitoring/Common.h"
#include "Hlt2MonBaseSvc.h"

class TFile;
class TH1D;

/** @class HltRootPublishSvc HltRootPublishSvc.h
 *
 *
 *  @author Roel Aaij
 *  @date   2015-06-13
 */
class Hlt2RootPublishSvc : public Hlt2MonBaseSvc {
public:
   /// Standard constructor
   Hlt2RootPublishSvc(const std::string& name, ISvcLocator* sl);

   virtual ~Hlt2RootPublishSvc( ); ///< Destructor

   // Service pure virtual member functions
   virtual StatusCode initialize() override;

   virtual void function() override;

private:

   void publishHistograms(zmq::socket_t& socket) const;


   // properties
   std::string m_infoCon;
   double m_publishInterval;
   double m_rateStart;
   double m_runDuration;
   double m_rateInterval;

   // data members
   typedef std::pair<Monitoring::RunNumber, Monitoring::HistId> histoKey_t;
   typedef std::pair<std::string, Gaudi::Histo1DDef> histoValue_t;
   typedef boost::unordered_map<histoKey_t, histoValue_t> histoMap_t;

   histoMap_t m_defs;
   histoMap_t m_rates;
   boost::unordered_map<Monitoring::RunNumber, int> m_startTimes;

   // Histograms are kept as {(run, id) : (directory, histo)}
   mutable boost::unordered_map<histoKey_t, std::pair<std::string, TH1D*>> m_histos;

};
#endif // HLT2ROOTPUBLISH_H
