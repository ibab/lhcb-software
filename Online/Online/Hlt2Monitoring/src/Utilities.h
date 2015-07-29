#ifndef UTILITIES_H
#define UTILITIES_H 1

// STD & STL
#include <chrono>

// boost
#include <boost/unordered_map.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/filesystem.hpp>

// ROOT
#include <TH1D.h>

// Local
#include <Hlt2Monitoring/Common.h>


namespace Monitoring {
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
using histos_t = boost::multi_index_container<
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
   >;

// typedefs
using histosByName = histos_t::index<byName>::type;

void loadSavedHistograms(histos_t& histos, boost::filesystem::path file, Monitoring::RunNumber run,
                         const std::string skip = std::string{});
}

#endif // UTILITIES_H
