#ifndef FILLHLT1CACHE_H 
#define FILLHLT1CACHE_H 1

// Include files 
// STD & STL
#include <map>
#include <unordered_map>

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// Event
#include <Event/Track.h>

/** @class FillHlt1Cache FillHlt1Cache.h
 *  Fill Hlt1 track upgrade relations tables from ancestor information.
 *  Every cache should contain unique from -> to relations, so the type 
 *  of from track must be selected to distinguish between multiple ancestors.
 *
 *  @author Roel Aaij
 *  @date   2015-03-28
 */
class FillHlt1Cache : public GaudiAlgorithm {
public: 
   /// Standard constructor
   FillHlt1Cache( const std::string& name, ISvcLocator* pSvcLocator );

   virtual ~FillHlt1Cache( ); ///< Destructor

   virtual StatusCode initialize();    ///< Algorithm initialization
   virtual StatusCode execute   ();    ///< Algorithm execution
   virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

   std::string m_fromTracks;
   std::string m_toTracks;
   std::string m_input;
   std::string m_cache;
   std::vector<std::string> m_fromTypes;
   std::vector<std::string> m_toTypes;

   std::vector<LHCb::Track::Types> m_fromTrackTypes;
   std::vector<LHCb::Track::Types> m_toTrackTypes;
   std::unordered_map<int, size_t> m_priorities;
   
};
#endif // FILLHLT1CACHE_H// Include files
