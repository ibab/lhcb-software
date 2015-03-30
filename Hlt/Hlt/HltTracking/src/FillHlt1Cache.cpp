// Include files 
// STD & STL
#include <set>
#include <map>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>

// from Gaudi
#include <GaudiKernel/AlgFactory.h>

// Event
#include <Event/Track.h>

// Relations
#include <Relations/Relation1D.h>

// local
#include "FillHlt1Cache.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FillHlt1Cache
//
// 2015-03-28 : Roel Aaij
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( FillHlt1Cache )
namespace {
   using Table = LHCb::Relation1D<LHCb::Track, LHCb::Track>;
   using std::to_string;
   using std::for_each;
   using std::copy_if;
   using std::make_pair;
   using std::string;
   using std::find;
   using std::set;
   using std::map;
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FillHlt1Cache::FillHlt1Cache( const string& name,
                              ISvcLocator* pSvcLocator)
   : GaudiAlgorithm ( name , pSvcLocator ),
     m_input{},
     m_cache{}
{
   declareProperty("FromTracks", m_fromTracks, "From track location.");
   declareProperty("ToTracks", m_toTracks, "To track location.");
   declareProperty("FromTypes", m_fromTypes, "List of from types to be cached. "
                   "Order indicates preference.");
   declareProperty("ToTypes", m_toTypes, "Type of to track to be cached.");
   declareProperty("Cache", m_cache, "Cache location.");
}

//=============================================================================
FillHlt1Cache::~FillHlt1Cache()
{

} 

//=============================================================================
StatusCode FillHlt1Cache::initialize() {
   StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
   if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

   if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

   // Check if from and to types are correctly defined.
   for (auto& types : {make_pair(&m_fromTypes, &m_fromTrackTypes),
                       make_pair(&m_toTypes,   &m_toTrackTypes)}) {
      const auto sts = types.first;
      auto tts = types.second;
      for (const auto& st : *sts) {
         auto tt = LHCb::Track::TypesToType(st);
         if (tt == LHCb::Track::TypeUnknown) {
            return Error(string("Track Type ") + st + " requested for input "
                         + m_input + " is not a valid track type.",
                         StatusCode::FAILURE);
         } else {
            tts->push_back(tt);
         }
      }
   }
   // Priority based on order of type in property.
   for (size_t i = 0; i < m_fromTrackTypes.size(); ++i) {
      m_priorities.emplace(m_fromTrackTypes[i], i);
   }

   return StatusCode::SUCCESS;
}

//=============================================================================
StatusCode FillHlt1Cache::execute() {

   if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

   auto checkTracks = [this] (const std::string& input,
                              const std::vector<LHCb::Track::Types>& types) {
      if (input.empty()) {
         return LHCb::Track::Range{};
      }
      // Check if the tracks exist on the TES
      if (!exist<LHCb::Track::Range>(evtSvc(), input)) {
         throw Error(string("Could not retrieve tracks from ") + input,
                     StatusCode::FAILURE);
      }
      // Get the tracks
      auto tracks = getIfExists<LHCb::Track::Range>(input);

      // Check if all tracks in the container are of allowed type and fail if
      // they are not.
      set<LHCb::Track::Types> diff;
      auto e = end(types);
      auto b = begin(types);
      for_each(begin(tracks), end(tracks),
               [b, e, &diff] (const LHCb::Track* track) {
                  if (find(b, e, track->type()) == e) diff.insert(track->type());
               } );
      if (!tracks.empty() && !diff.empty()) {
         string good, bad;
         for (auto t : diff) bad += LHCb::Track::TypesToString(t) + " ";
         for (auto t : types) good += LHCb::Track::TypesToString(t) + " ";
         throw Error(string("Track of other types in container than expected: ") +
                     bad + "instead of " + good + "for " + m_input, StatusCode::FAILURE);
      }
      return tracks;
   };
   
   LHCb::Track::Range fromTracks;
   LHCb::Track::Range toTracks;
   try {
      fromTracks = checkTracks(m_fromTracks, m_fromTrackTypes);
      toTracks   = checkTracks(m_toTracks, m_toTrackTypes);
   } catch (const StatusCode& sc) {
      return sc;
   }
      
   // Create a cache
   auto cache = getOrCreate<Table, Table>(m_cache);

   // List of from tracks that successfully upgraded tracks will be removed from.
   std::unordered_set<const LHCb::Track*> failed(begin(fromTracks), end(fromTracks));
   
   // Add the ancestors of each track as from and the track itself as to;
   // use fast insertion.
   for (auto track : toTracks) {
      if (m_priorities.empty()) {
         // Fill relations for all ancestors
         for (const auto& ancestor : track->ancestors()) {
            cache->i_push(ancestor.data(), track);
            failed.erase(ancestor.data());
         }
      } else {
         // Fill relations according to order of types in m_fromTypes
         const LHCb::Track* from = nullptr;
         size_t best = m_priorities.size();
         for (const auto& ancestor : track->ancestors()) {
            auto type = ancestor->type();
            if (m_priorities.count(type) && m_priorities[type] < best) {
               best = m_priorities[type];
               from = ancestor.data();
            }
         }
         if (from) {
            cache->i_push(from, track);
            failed.erase(from);
         }
      }
   }
   // Insert null pointers for all tracks that were not upgraded.
   for (auto from : failed) {
      cache->i_push(from, nullptr);
   }

   // Don't forget to sort the table.
   cache->i_sort();
   counter(m_input) += cache->i_relations().size();
   
   return StatusCode::SUCCESS;
}

//=============================================================================
StatusCode FillHlt1Cache::finalize() {

   if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

   return GaudiAlgorithm::finalize();  // must be called after all other actions
}
