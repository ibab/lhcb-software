// $Id: HltMonitorAlgorithm.h,v 1.2 2006-12-20 09:32:44 hernando Exp $
#ifndef HLTBASE_HLTMONITORALGORITHM_H 
#define HLTBASE_HLTMONITORALGORITHM_H 1

// Include files

#include "HltBase/HltAlgorithm.h"
#include "Event/HltSummary.h"
#include "Event/HltNames.h"

/** @class HltMonitorAlgorithm 
 *  
 *  Base class for HltSummaryAlg monitor
 *
 *  @author Jose A. Hernando 
 *  @date   2006-10-19
 */

typedef HltAlgorithm HltMonitorAlgorithm;


//  // fill histogram with the info from the keys
//   // (histos should have the same name)
//   template <class CONTAINER>
//   void monitor(const CONTAINER& con, 
//                const std::vector<int& keys) {
//     for (std::vector<std::string>::const_iterator it = keyNames.begin();
//          it != keyNames.end(); ++it) {
//       const std::string& title = (*it);
//       int key = HltNames::particleInfoID(title);
//       if (key <0) continue;
//       for (typename CONTAINER::const_iterator it2 = con.begin();
//            it2 != con.end(); it2++) {
//         if ((*it2)->hasInfo(key))
//           fill( histo1D(title), (*it2)->info(key,0), 1.);
//       }
//     } 
//   }

#endif // HLTBASE_HLTALGORITHM_H
