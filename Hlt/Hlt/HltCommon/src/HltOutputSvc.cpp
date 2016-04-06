// STD & STL
#include <vector>
#include <string>
#include <map>

// Gaudi
#include <GaudiKernel/Service.h>

// local
#include "HltOutputSvc.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltOutputSvc
//
// 2016-03-26 : Roel Aaij
//-----------------------------------------------------------------------------

namespace {
   using std::vector;
   using std::string;
   using std::map;

   using OutputMap = map<string, vector<string>>;
   auto findEntry = [](const OutputMap& map, string entry) -> vector<string>
   {
      auto it = map.find(entry);
      if (it == end(map)) {
         return {};
      } else {
         return it->second;
      }
   };
}

// Declaration of the Service Factory
DECLARE_SERVICE_FACTORY(HltOutputSvc)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltOutputSvc::HltOutputSvc(const string& name, ISvcLocator* svcLocator)
   : base_class(name, svcLocator)
{
   declareProperty("StreamGroups", m_streams, "Groups of streams and the "
                   " logical streams they consist of.");
   declareProperty("Filters", m_filters, "Logical streams that are enabled, with "
                  " their filters, formatted as: {stream : [filter1, filter2, ...]");
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltOutputSvc::initialize()
{
   auto sc = Service::initialize();
   if (!sc.isSuccess()) return sc;

   debug() << "Initialized HltOutputSvc." << endmsg;
   debug() << "Streams: " << endmsg;
   for (const auto& entry : m_streams) {
      debug() << entry.first << " : {";
      bool firstStream = true;
      for (const auto& substream : entry.second) {
         if (!firstStream) debug() << ", ";
         if (firstStream) firstStream = false;
         debug() << substream << " :";
         auto filters = m_filters.find(substream);
         debug() << " [";
         bool first = true;
         for (const auto& filter : filters->second) {
            if (!first) debug() << ", ";
               if (first) first = false;
               debug() << filter;
         }
         debug() << "]";
      }
      debug() << "}" << endmsg;
   }

   return sc;
}

//=============================================================================
// Finalization
//=============================================================================
StatusCode HltOutputSvc::finalize()
{
   debug() << "Finalized HltOutputSvc." << endmsg;
   return Service::finalize();
}

//=============================================================================
vector<string> HltOutputSvc::groups() const
{
   vector<string> s;
   for (const auto& entry : m_streams) {
      s.push_back(entry.first);
   }
   return s;
}

//=============================================================================
vector<string> HltOutputSvc::logicalStreams(string group) const
{
   return findEntry(m_streams, group);
}

//=============================================================================
vector<string> HltOutputSvc::filters(string stream) const
{
   return findEntry(m_filters, stream);
}
