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
#include "Event/RecSummary.h"

// local
#include "Hlt2SelReportsMaker.h"

namespace {
   const  Gaudi::StringKey Hlt2SelectionID{"Hlt2SelectionID"};
}

//-----------------------------------------------------------------------------
// Implementation file for class : Hlt2SelReportsMaker
//
// 2015-10-27 : Roel Aaij
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY(Hlt2SelReportsMaker)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Hlt2SelReportsMaker::Hlt2SelReportsMaker(const std::string& name,
                                         ISvcLocator* svcLocator)
 : HltSelReportsMaker(name, svcLocator, Hlt2SelectionID)
{
   declareProperty("RecSummaryLocation", m_RecSummaryLoc = "Hlt2/RecSummary" );
   declareProperty("SummaryName", m_summaryName = "Hlt2RecSummary");
}

//=============================================================================
// Destructor
//=============================================================================
Hlt2SelReportsMaker::~Hlt2SelReportsMaker()
{

}

//=============================================================================
// Post execute
//=============================================================================
StatusCode Hlt2SelReportsMaker::postExecute(LHCb::HltSelReports* outputSummary,
											const LHCb::HltDecReports* decReports)
{

   for(auto name : decReports->decisionNames()){
     const LHCb::HltDecReport* report = decReports->decReport(name);
     if(report){
       if(report->numberOfCandidates()>14) Warning( name+" selects 15 or more candidates. If you see this a lot there is a problem." ,
           StatusCode::SUCCESS, 10 );
     }
   }
   
   const LHCb::RecSummary* recsummary = getIfExists<LHCb::RecSummary>(m_RecSummaryLoc);

   if( !recsummary && !m_RecSummaryLoc.empty() ) {
	  Warning( "No RecSummary object found, you will not get event level information in the reports",
			   StatusCode::SUCCESS, 10 );
   } else if (recsummary) {
	  HltObjectSummary* eventObjectSummary = new HltObjectSummary();
	  HltObjectSummary* eventObjectSummarySub = new HltObjectSummary();
	  eventObjectSummarySub->setSummarizedObjectCLID( recsummary->clID() );
	  eventObjectSummary->setSummarizedObjectCLID( 1 ); // use special CLID for selection summaries (lowest number for sorting to the end)

	  // integer selection id
	  auto value = hltANNSvc()->value(idKey(), m_summaryName);
	  if (!value) {
		 error() << "Could not obtain Hlt2 selection ID for " << m_summaryName << endmsg;
		 return StatusCode::FAILURE;
	  }

	  eventObjectSummary->addToInfo("0#SelectionID", float(value->second));
	  HltObjectSummary::Info EventInfo;
	  reportConverter()->RecSummaryObject2Summary(&EventInfo,recsummary);
	  eventObjectSummarySub->setNumericalInfo(EventInfo);
	  //
	  objectSummaries().push_back(eventObjectSummarySub);
	  eventObjectSummary->addToSubstructure(eventObjectSummarySub);
	  if( outputSummary->insert(m_summaryName,*eventObjectSummary) == StatusCode::FAILURE ){
		 Warning(" Failed to add RecSummary to its container ", StatusCode::SUCCESS, 10 );
	  }
	  objectSummaries().push_back(eventObjectSummary);
   }

   if( !outputSummary->hasSelectionName("Hlt2Global") ){

	  HltObjectSummary selSumOut;
	  selSumOut.setSummarizedObjectCLID( 1 ); // use special CLID for selection summaries (lowest number for sorting to the end)

	  // integer selection id
	  selSumOut.addToInfo("0#SelectionID",float(kHlt2GlobalID));

	  // see which decisions contributed to it
	  for(const  auto&  it : *outputSummary ) {
		 const std::string & selName = it.first;
		 if( !boost::algorithm::starts_with(selName, "Hlt2")  ||
			 !boost::algorithm::ends_with( selName, "Decision") ) continue;
		 auto& selRep = it.second;
		 // must find corresponding HltObjectSummary in the object store
		 auto i = selRep.numericalInfo().find("0#SelectionID");
		 if( i==selRep.numericalInfo().end() ){
			Error(" (1) HltObjectSummary of selection-summary-type has no SelectionID info ",StatusCode::SUCCESS,20);
			continue;
		 }
		 int id = (int)(i->second+0.1);
		 for( const auto& pObj : objectSummaries() ){
			if( pObj->summarizedObjectCLID()!=1 )continue;
			auto j=pObj->numericalInfo().find("0#SelectionID");
			if( j==pObj->numericalInfo().end() ){
			   Error(" (2) HltObjectSummary of selection-summary-type has no SelectionID info ",StatusCode::SUCCESS,20);
			   continue;
			}
			if( id == (int)(j->second+0.1) ){
			   // must also check its decision in HltDecReports since it might have been killed by postscale
			   if( decReports ){
				  const auto decReport = decReports->decReport(selName);
				  if( decReport && !(decReport->decision()) )break;
			   }
			   selSumOut.addToSubstructure( pObj );
			   break;
			}
		 }
	  }
	  if( selSumOut.substructure().size() ){

		 // insert selection into the container
		 if( outputSummary->insert("Hlt2Global",selSumOut) == StatusCode::FAILURE ){
			Error( "  Failed to add Hlt selection name Hlt2Global to its container ", StatusCode::SUCCESS, 10 );
		 }
		 objectSummaries().push_back( new HltObjectSummary{ selSumOut } );
	  }
   }
   return StatusCode::SUCCESS;
}
