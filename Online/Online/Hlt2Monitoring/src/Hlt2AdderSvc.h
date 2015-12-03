#ifndef HLT2ADDER_H
#define HLT2ADDER_H 1

// Include files
#include <string>
#include <unordered_map>
#include <chrono>

// boost
#include <boost/unordered_map.hpp>

// Gaudi
#include <GaudiKernel/Service.h>
#include <GaudiKernel/IIncidentListener.h>

// ZeroMQ
#include <ZeroMQ/IZeroMQSvc.h>

// Local
#include "Hlt2Monitoring/Common.h"
#include "Hlt2MonBaseSvc.h"

/** @class Hlt2Adder Hlt2Adder.h
 *
 *
 *  @author Roel Aaij
 *  @date   2015-06-13
 */
class Hlt2AdderSvc : public Hlt2MonBaseSvc {
public:
   /// Standard constructor
   Hlt2AdderSvc(const std::string& name, ISvcLocator* sl);

   virtual ~Hlt2AdderSvc( ); ///< Destructor

   // Service pure virtual member functions
   virtual StatusCode initialize() override;

   // The function that does the work
   void function() override;

private:

   // properties
   double m_sendInterval;

   // data members
   typedef std::pair<Monitoring::RunNumber, Monitoring::HistId> key_t;
   boost::unordered_map<key_t, Monitoring::Histogram> m_histograms;
   boost::unordered_map<key_t, std::chrono::time_point<std::chrono::high_resolution_clock>> m_updates;

};
#endif // HLT2ADDER_H
