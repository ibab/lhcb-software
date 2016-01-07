#ifndef HLT2SELREPORTSMAKER_H
#define HLT2SELREPORTSMAKER_H 1

// STD & STL
#include <string>

// Gaudi
#include "GaudiKernel/AlgFactory.h"

// Include files
#include "HltSelReportsMaker.h"

// Forward declarations
namespace LHCb {
   class HltSelReports;
   class HltDecReports;
}

/** @class Hlt2SelReportsMaker Hlt2SelReportsMaker.h
 *
 *
 *  @author Roel Aaij
 *  @date   2015-10-27
 */
class Hlt2SelReportsMaker : public HltSelReportsMaker {
public:
   /// Standard constructor
   Hlt2SelReportsMaker(const std::string& name,
                       ISvcLocator* svcLocator);

   virtual ~Hlt2SelReportsMaker( ); ///< Destructor

protected:

   virtual StatusCode postExecute(LHCb::HltSelReports* outputSummary,
                                  const LHCb::HltDecReports* decReports);


private:

   std::string m_RecSummaryLoc;
   std::string m_summaryName;
   unsigned int m_numCand;

};
#endif // HLT2SELREPORTSMAKER_H
