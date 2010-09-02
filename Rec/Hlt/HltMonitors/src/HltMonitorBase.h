// $Id: $
#ifndef HLTMONITORBASE_H 
#define HLTMONITORBASE_H 1

// Include files
// Gaudi
#include <GaudiAlg/GaudiHistoAlg.h>

// boost
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>

namespace LHCb {
   class HltDecReports;
}

/** @class HltMonitorBase HltMonitorBase.h
 *  Base class for Offline Hlt Monitoring algorithms. It contains a 
 *  map of decisions to the regex which they match. This container can
 *  only be filled when the first event is processed.
 *
 *  @author Roel Aaij
 *  @date   2010-08-24
 */
class HltMonitorBase : public GaudiHistoAlg {
public: 
   /// Standard constructor
   HltMonitorBase( const std::string& name, ISvcLocator* pSvcLocator );

   virtual ~HltMonitorBase( ); ///< Destructor

   virtual StatusCode initialize();    ///< Algorithm initialization
   virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

   // Tags for the container.
   struct regexTag { };
   struct decisionTag { };

   std::vector< std::string > m_regexes;

   typedef std::pair< std::string, std::string > pair_t;

   // Multi index container to hold the items.
   typedef boost::multi_index_container<
      pair_t,
      boost::multi_index::indexed_by<
         boost::multi_index::ordered_non_unique<
            boost::multi_index::tag< regexTag >,
            boost::multi_index::member< pair_t, std::string, &pair_t::first >
            >,
         boost::multi_index::ordered_unique<
            boost::multi_index::tag< decisionTag >,
            boost::multi_index::member< pair_t, std::string, &pair_t::second >
            > >
      > decMap_t;
   
   typedef decMap_t::index< regexTag >::type decByRegex_t;
   typedef decMap_t::index< decisionTag >::type decByDec_t;

   decMap_t m_decisions;

   bool m_filledDecisions;

   void fillDecisions( const LHCb::HltDecReports* decReports );

};
#endif // HLTMONITORBASE_H
