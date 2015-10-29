#ifndef HLT1SELREPORTSMAKER_H
#define HLT1SELREPORTSMAKER_H 1

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

/** @class Hlt1SelReportsMaker Hlt1SelReportsMaker.h
 *
 *
 *  @author Roel Aaij
 *  @date   2015-10-27
 */
class Hlt1SelReportsMaker : public HltSelReportsMaker {
public:

   /// Standard constructor
   Hlt1SelReportsMaker(const std::string& name,
					   ISvcLocator* svcLocator);

   virtual ~Hlt1SelReportsMaker( ); ///< Destructor

protected:

   virtual StatusCode postExecute(LHCb::HltSelReports* outputSummary,
								  const LHCb::HltDecReports* decReports);
   
};
#endif // HLT1SELREPORTSMAKER_H
