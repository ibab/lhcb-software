#ifndef HLT2SAVERSVC_H
#define HLT2SAVERSVC_H 1

// Include files
#include <string>
#include <unordered_map>
#include <chrono>

// boost
#include <boost/unordered_map.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/mem_fun.hpp>

// Gaudi
#include <GaudiKernel/Service.h>
#include <GaudiKernel/HistoDef.h>
#include <GaudiKernel/IIncidentListener.h>

// Local
#include "Hlt2Monitoring/Common.h"
#include "Hlt2MonBaseSvc.h"
#include "Utilities.h"

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
   virtual StatusCode initialize() override;

   virtual void function() override;

private:

   // Function used by thread to trigger saving of histograms
   void save();

   // Save all known histograms to file
   void saveHistograms() const;

   // Prune histograms that have not been updated for a while
   void pruneHistograms();

   // Get the run info for a given run
   boost::optional<Monitoring::RunInfo> runInfo(Monitoring::RunNumber run) const;

   // Convert the start time to something usable in the file names
   std::array<std::string, 6> timeInfo(unsigned int run) const;

   // Saveset file name
   std::pair<boost::filesystem::path, bool> filename(Monitoring::RunNumber run, bool byRun) const;

   // properties
   std::string m_directory;
   std::string m_fileName;
   std::string m_runFileName;
   std::string m_dataCon;
   std::string m_infoCon;
   std::string m_normalize;
   int m_saveInterval;

   // Data members
   bool m_stopSaving;

   // data members
   mutable Monitoring::histos_t m_histos;
   mutable boost::unordered_map<Monitoring::RunNumber, Monitoring::RunInfo> m_runInfo;

};
#endif // HLT2SAVERSVC_H
