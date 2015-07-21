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

   // Load all existing histograms from a file.
   void loadSavedHistograms(Monitoring::RunNumber run) const;

   // Saveset file name
   std::pair<boost::filesystem::path, bool> filename(Monitoring::RunNumber run) const;

   // properties
   std::string m_directory;
   std::string m_fileName;
   std::string m_dataCon;
   std::string m_infoCon;
   std::string m_normalize;
   int m_saveInterval;

   // Data members
   bool m_stopSaving;

   // Container typedef
   struct byRun{ };
   struct byDir{ };
   struct byName{ };

   using histoKey_t = std::pair<Monitoring::RunNumber, Monitoring::HistId>;

   struct entry_t {
      entry_t(Monitoring::RunNumber r, std::string t, std::string d, TH1D* h,
              std::chrono::time_point<std::chrono::high_resolution_clock> when
              = std::chrono::time_point<std::chrono::high_resolution_clock>{})
         : lastUpdate{when},
           run{r},
           type{t},
           dir{d},
           histo{h} { }

      virtual ~entry_t() { }

      std::string name() const { return dir + "/" + histo->GetName(); }

      // data members
      std::chrono::time_point<std::chrono::high_resolution_clock> lastUpdate;
      Monitoring::RunNumber run;
      std::string type;
      std::string dir;
      std::shared_ptr<TH1D> histo;
   };

   // Multi index container to hold the items.
   typedef boost::multi_index_container<
      entry_t,
      boost::multi_index::indexed_by<
         boost::multi_index::hashed_non_unique<
            boost::multi_index::tag<byRun>,
            boost::multi_index::member<entry_t, Monitoring::RunNumber, &entry_t::run>
            >,
         boost::multi_index::hashed_non_unique<
            boost::multi_index::tag<byName>,
            boost::multi_index::const_mem_fun<entry_t, std::string, &entry_t::name>
            >,
         boost::multi_index::hashed_non_unique<
            boost::multi_index::tag<byDir>,
            boost::multi_index::member<entry_t, std::string, &entry_t::dir>
            > >
      > histos_t;

   // typedefs
   using histosByName = histos_t::index<byName>::type;

   // data members
   mutable histos_t m_histos;
   mutable boost::unordered_map<Monitoring::RunNumber, Monitoring::RunInfo> m_runInfo;

};
#endif // HLT2SAVERSVC_H
