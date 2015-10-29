// Include files
// boost
#include "boost/algorithm/string/replace.hpp"
#include "boost/algorithm/string/predicate.hpp"
#include "boost/format.hpp"

// from Gaudi
#include "GaudiKernel/StringKey.h"

// Event
#include "Event/HltSelReports.h"
#include "Event/HltObjectSummary.h"
#include "Event/HltDecReports.h"

// local
#include "Hlt1SelReportsMaker.h"

namespace {
   const  Gaudi::StringKey Hlt1SelectionID{"Hlt1SelectionID"};
}


//-----------------------------------------------------------------------------
// Implementation file for class : Hlt1SelReportsMaker
//
// 2015-10-27 : Roel Aaij
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY(Hlt1SelReportsMaker)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Hlt1SelReportsMaker::Hlt1SelReportsMaker(const std::string& name,
                                         ISvcLocator* svcLocator)
 : HltSelReportsMaker(name, svcLocator, Hlt1SelectionID) {

}

//=============================================================================
// Destructor
//=============================================================================
Hlt1SelReportsMaker::~Hlt1SelReportsMaker()
{

}

//=============================================================================
// Post execute
//=============================================================================
StatusCode Hlt1SelReportsMaker::postExecute(LHCb::HltSelReports* outputSummary,
                                            const LHCb::HltDecReports* decReports)
{
   // -------------------------------------------------------------------------------------
   // create global selections ------------------------------------------------------------
   // -------------------------------------------------------------------------------------
   if( !outputSummary->hasSelectionName("Hlt1Global") ){

	  HltObjectSummary selSumOut;
	  selSumOut.setSummarizedObjectCLID( 1 ); // use special CLID for selection summaries (lowest number for sorting to the end)

	  // integer selection id
	  selSumOut.addToInfo("0#SelectionID",float(kHlt1GlobalID));

	  // see which decisions contributed to it
	  for( const auto& it : *outputSummary) {
		 const std::string & selName = it.first;
		 if( !boost::algorithm::starts_with( selName, "Hlt1") ||
			 !boost::algorithm::ends_with(   selName, "Decision") ) continue;
		 const LHCb::HltObjectSummary& selRep = it.second;
		 // must find corresponding HltObjectSummary in the object store
		 auto i = selRep.numericalInfo().find("0#SelectionID");
		 if( i==selRep.numericalInfo().end() ){
			Error(" (1) HltObjectSummary of selection-summary-type has no SelectionID info ",StatusCode::SUCCESS,20);
			continue;
		 }
		 int id = (int)(i->second+0.1);
		 for( const auto& pObj : objectSummaries()) {
			if( pObj->summarizedObjectCLID()!=1 ) continue;
			auto j=pObj->numericalInfo().find("0#SelectionID");
			if( j==pObj->numericalInfo().end() ){
			   Error(" (2) HltObjectSummary of selection-summary-type has no SelectionID info ",StatusCode::SUCCESS,20);
			   continue;
			}
			if( id == (int)(j->second+0.1) ){
			   // must also check its decision in HltDecReports since it might have been killed by postscale
			   if( decReports ){
				  const auto decReport = decReports->decReport(selName);
				  if( decReport && !decReport->decision() ) break;
			   }
			   selSumOut.addToSubstructure( (const SmartRef<HltObjectSummary>)(pObj) );
			   break;
			}
		 }
	  }
	  if( !selSumOut.substructure().empty() ){

		 // insert selection into the container
		 if( outputSummary->insert("Hlt1Global", selSumOut) == StatusCode::FAILURE ){
			Error( "  Failed to add Hlt selection name Hlt1Global to its container ", StatusCode::SUCCESS, 10 );
		 }
		 objectSummaries().push_back(new HltObjectSummary{ selSumOut });
	  }
   }
   return StatusCode::SUCCESS;
}
                                     
