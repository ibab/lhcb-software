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
#include <GaudiKernel/HistoDef.h>
#include <GaudiKernel/IIncidentListener.h>

// Local
#include "Hlt2Monitoring/Common.h"
#include "Hlt2MonBaseSvc.h"

class TFile;
class TH1D;

/** @class Hlt2Saver Hlt2Saver.h
 *
 *
 *  @author Roel Aaij
 *  @date   2015-06-13
 */
class Hlt2SaverSvc : public Hlt2MonBaseSvc {
public:
   /// Standard constructor
   Hlt2SaverSvc(const std::string& name, ISvcLocator* sl);

   virtual ~Hlt2SaverSvc( ); ///< Destructor

   // Service pure virtual member functions
   virtual StatusCode initialize();

   virtual void function() override;

private:

   void saveHistograms() const;

   // properties
   std::string m_directory;
   std::string m_fileName;
   std::string m_dataCon;
   std::string m_infoCon;
   double m_saveInterval;
   double m_rateStart;
   double m_runDuration;
   double m_rateInterval;

   // data members
   typedef std::pair<Monitoring::RunNumber, Monitoring::HistId> histoKey_t;
   boost::unordered_map<histoKey_t, std::pair<std::string, Gaudi::Histo1DDef>> m_defs;
   boost::unordered_map<histoKey_t, std::pair<std::string, std::string>> m_rates;

   // Histograms are kept as {(run, id) : (directory, histo)}
   mutable boost::unordered_map<histoKey_t, std::pair<std::string, TH1D*>> m_histos;

};
#endif // HLT2ADDER_H
