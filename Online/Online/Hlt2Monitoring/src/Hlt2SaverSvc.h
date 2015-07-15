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

   void saveHistograms() const;
   boost::optional<Monitoring::RunInfo> getRunInfo(Monitoring::RunNumber run) const;
   
   std::array<std::string, 6> timeInfo(unsigned int run) const;

   // properties
   std::string m_directory;
   std::string m_fileName;
   std::string m_dataCon;
   std::string m_infoCon;
   std::string m_normalize;
   double m_saveInterval;

   // Container typedef
   struct byRun{ };
   struct byKey{ };

   using histoKey_t = std::pair<Monitoring::RunNumber, Monitoring::HistId>;

   struct entry_t {
      entry_t(histoKey_t k, std::string t, std::string d, TH1D* h)
         : key{k},
           type{t},
           dir{d},
           histo{h} {
      }

      virtual ~entry_t() { }

      Monitoring::RunNumber run() const { return key.first; }

      // data members
      histoKey_t key;
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
            boost::multi_index::const_mem_fun<entry_t, Monitoring::RunNumber, &entry_t::run>
            >,
         boost::multi_index::hashed_non_unique<
            boost::multi_index::tag<byKey>,
            boost::multi_index::member<entry_t, histoKey_t, &entry_t::key>
            > >
      > histos_t;

   typedef histos_t::index<byRun>::type histosByRun_t;
   typedef histos_t::index<byKey>::type histosByKey_t;

   // data members
   boost::unordered_map<Monitoring::RunNumber, int> m_startTimes;
   mutable histos_t m_histos;
   mutable boost::unordered_map<Monitoring::RunNumber, Monitoring::RunInfo> m_runInfo;
};
#endif // HLT2SAVERSVC_H
