// Include files 

// from Gaudi
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "Event/HltSelReports.h"
#include "Event/HltDecReports.h"
#include "Event/RawEvent.h"
#include "Event/HltObjectSummary.h"

// bank structure
#include "HltDAQ/HltSelRepRawBank.h"
#include "HltDAQ/HltSelRepRBHits.h"
#include "HltDAQ/HltSelRepRBSubstr.h"
#include "HltDAQ/HltSelRepRBObjTyp.h"
#include "HltDAQ/HltSelRepRBExtraInfo.h"
#include "HltDAQ/HltSelRepRBStdInfo.h"

// boost
#include "boost/format.hpp"

// local
#include "HltRawDataMonitor.h"
#include "HltSelReportsWriter.h"

#include "Event/Track.h"
#include "Event/Particle.h"
#include "Event/RecVertex.h"
#include "Event/CaloCluster.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltRawDataMonitor
//
// Data                          : 2008-10-06 
// Author                        : Chun-Min Jen, jencmhep@gmail.com
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltRawDataMonitor )

using namespace LHCb;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltRawDataMonitor::HltRawDataMonitor( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator )
{
  declareProperty("InputRawEventLocation", m_inputRawEventLocation ="");
  // <0 never 0=at finalize >0 event frequency
  declareProperty("DiagnosticsFrequency",  m_diagnosticsFrequency = 0);

  declareProperty("SourceID",
		  m_sourceID= HltSelReportsWriter::kSourceID_Hlt );  


}


//=============================================================================
// Initialization
//=============================================================================
StatusCode HltRawDataMonitor::initialize() {
  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  
  m_rawEventLocations.clear();
  if( m_inputRawEventLocation != "" )m_rawEventLocations.push_back(m_inputRawEventLocation);
  m_rawEventLocations.push_back(LHCb::RawEventLocation::Trigger);
  m_rawEventLocations.push_back(LHCb::RawEventLocation::Copied);
  m_rawEventLocations.push_back(LHCb::RawEventLocation::Default);

  m_bankSize    = book(100, "Size of HltSelReports Raw Bank (kB)", 0., 5., 500); 
  m_hitSize     = book(200, "Size of hits sub-bank in HltSelReports (kB)", 0., 5., 500);
  m_objtypSize  = book(300, "Size of object-type sub-bank in HltSelReports (kB)", 0., 5., 500);
  m_substrSize  = book(400, "Size of substructure sub-bank in HltSelReports (kB)", 0., 5., 500);
  m_stdinfoSize = book(500, "Size of Standard-Info sub-bank in HltSelReports (kB)", 0., 5., 500);
  m_extinfoSize = book(600, "Size of Extra-Info sub-bank in HltSelreports (kB)", 0., 5., 500);

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltRawDataMonitor::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // get inputs
 LHCb::RawEvent* rawEvent = 0;
  std::vector<std::string>::const_iterator iLoc = m_rawEventLocations.begin();
  for (; iLoc != m_rawEventLocations.end() ; ++iLoc ) {
    //    try RootInTES independent path first
    if (exist<LHCb::RawEvent>(*iLoc, false)) {
      rawEvent = get<LHCb::RawEvent>(*iLoc, false);
      break;
    }
    //   now try RootInTES dependent path
    if (exist<LHCb::RawEvent>(*iLoc)) {
      rawEvent = get<LHCb::RawEvent>(*iLoc);
      break;
    }
  }
 if( ! rawEvent ){
    return Warning(" No RawEvent found at any location.");
  }  


  ++m_event; // event counter
  StatusCode sc;
  sc = fillRawBank();
  sc = fillHltSelRep();

  if( m_diagnosticsFrequency>0  && (m_event % m_diagnosticsFrequency) == 0 ){
      selectionDiagnostics();
  }

  return StatusCode::SUCCESS;

}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltRawDataMonitor::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  if ( m_diagnosticsFrequency>=0 ) selectionDiagnostics();

  return GaudiHistoAlg::finalize();  // must be called after all other actions
}

//=============================================================================
//  Get the bank from RawEvent
//=============================================================================
StatusCode HltRawDataMonitor::fillRawBank()
{ 
  debug() << "==> FillRawBank " << endmsg;

  LHCb::RawEvent* rawEvent = get<LHCb::RawEvent>(m_inputRawEventLocation);

  // ----------------------------------------------------------
  // get the bank from RawEvent
  // ----------------------------------------------------------

  const std::vector<RawBank*> hltselreportsRawBanks = rawEvent->banks( RawBank::HltSelReports );
  if( !hltselreportsRawBanks.size() ){
    return Warning( " No HltSelReports RawBank in RawEvent. Quiting. ",StatusCode::SUCCESS, 10 );
  }
  const RawBank* hltselreportsRawBank0 = *(hltselreportsRawBanks.begin());
  if( hltselreportsRawBank0->version() > kVersionNumber ){
    Warning( " HltSelReports RawBank version is higher than expected. Will try to decode it anyway." ,StatusCode::SUCCESS, 20 );
  }

  unsigned int bankCounterMax = 0;
  unsigned int bankSize =0;
  std::vector<const RawBank*> orderedBanks(hltselreportsRawBanks.size(),(const RawBank*)0);
  for( std::vector<RawBank*>::const_iterator hltselreportsRawBankP=hltselreportsRawBanks.begin();
	 hltselreportsRawBankP!=hltselreportsRawBanks.end(); ++hltselreportsRawBankP ){    
    const RawBank* hltselreportsRawBank = *hltselreportsRawBankP;

    unsigned int sourceID=HltSelReportsWriter::kSourceID_Hlt;
    if( hltselreportsRawBank->version() > 1 ){
      sourceID = hltselreportsRawBank->sourceID() >> HltSelReportsWriter::kSourceID_BitShift;
    }
    if( m_sourceID != sourceID )continue;

    if( hltselreportsRawBank->magic() != RawBank::MagicPattern ){
      Error(" HltSelReports RawBank has wrong magic number. Skipped ",StatusCode::SUCCESS, 20 );
      continue;
    }
    unsigned int bankCounter = hltselreportsRawBank->sourceID();
    if( hltselreportsRawBank->version() > 1 ){
      bankCounter = bankCounter & HltSelReportsWriter::kSourceID_MinorMask;
    }
    if( bankCounter < hltselreportsRawBanks.size() ){
      orderedBanks[bankCounter]= hltselreportsRawBank;
      if( bankCounter > bankCounterMax ) bankCounterMax = bankCounter;
    } else {
      Error( " Illegal Source ID HltSelReports bank skipped ", StatusCode::SUCCESS, 20 );
    }
    bankSize += hltselreportsRawBank->size();
  }
  //      if new Hlt1,Hlt2 reports not found try for Hlt1+Hlt2 reports
  if( !bankSize ){
    if( ( m_sourceID == HltSelReportsWriter::kSourceID_Hlt1 ) ||
        ( m_sourceID == HltSelReportsWriter::kSourceID_Hlt2 ) ){

      for( std::vector<RawBank*>::const_iterator hltselreportsRawBankP=hltselreportsRawBanks.begin();
	   hltselreportsRawBankP!=hltselreportsRawBanks.end(); ++hltselreportsRawBankP ){    
	const RawBank* hltselreportsRawBank = *hltselreportsRawBankP;

	unsigned int sourceID= HltSelReportsWriter::kSourceID_Hlt;
	if( hltselreportsRawBank->version() > 1 ){
	  sourceID = hltselreportsRawBank->sourceID() >> HltSelReportsWriter::kSourceID_BitShift;
	}
	if( HltSelReportsWriter::kSourceID_Hlt != sourceID )continue;

	if( hltselreportsRawBank->magic() != RawBank::MagicPattern ){
	  Error(" HltSelReports RawBank has wrong magic number. Skipped ",StatusCode::SUCCESS, 20 );
	  continue;
	}
	unsigned int bankCounter = hltselreportsRawBank->sourceID();
	if( hltselreportsRawBank->version() > 1 ){
	  bankCounter = bankCounter & HltSelReportsWriter::kSourceID_MinorMask;
	}
	if( bankCounter < hltselreportsRawBanks.size() ){
	  orderedBanks[bankCounter]= hltselreportsRawBank;
	  if( bankCounter > bankCounterMax ) bankCounterMax = bankCounter;
	} else {
	  Error( " Illegal Source ID HltSelReports bank skipped ", StatusCode::SUCCESS, 20 );
	}
	bankSize += hltselreportsRawBank->size();
      }
    }
  }
  if( !bankSize ){
    return Warning( " No HltSelReports RawBank for requested SourceID in RawEvent. Quiting. ",StatusCode::SUCCESS, 10 );
  }    
  bankSize = (bankSize+3)/4; // from bytes to words
  // need to copy it to local array to get rid of const restriction
  unsigned int* pBank = new unsigned int[bankSize];
  HltSelRepRawBank hltSelReportsBank( pBank );

  ++bankCounterMax;
  for( unsigned int iBank=0; iBank<bankCounterMax; ++iBank){
    const RawBank* hltselreportsRawBank = orderedBanks[iBank];
    if( !hltselreportsRawBank ){
      hltSelReportsBank.deleteBank();
      return Error("Missing HltSelReports RawBank part - quiting.", StatusCode::SUCCESS, 100 );
    }
    unsigned int bankSize1 =  (hltselreportsRawBank->size()+3)/4; // from bytes to words
    for(unsigned int i=0; i!=bankSize1; ++i){
      (*pBank) = hltselreportsRawBank->data()[i]; ++pBank;
    }
  }


  HltSelRepRBHits hitsSubBank( hltSelReportsBank.subBankFromID( HltSelRepRBEnums::kHitsID ) );
  HltSelRepRBObjTyp objTypSubBank( hltSelReportsBank.subBankFromID( HltSelRepRBEnums::kObjTypID ) );
  HltSelRepRBSubstr substrSubBank( hltSelReportsBank.subBankFromID( HltSelRepRBEnums::kSubstrID ) );
  HltSelRepRBStdInfo stdInfoSubBank( hltSelReportsBank.subBankFromID( HltSelRepRBEnums::kStdInfoID ) );
  HltSelRepRBExtraInfo extraInfoSubBank( hltSelReportsBank.subBankFromID( HltSelRepRBEnums::kExtraInfoID ) );

  // ----------------------------------------- integrity checks -------------------------
  bool errors=false;
  //  bool exInfOn=true; // not used
  unsigned int ic;
  unsigned int nObj = objTypSubBank.numberOfObj();

  if( bankSize < hltSelReportsBank.size() ){
    Error( " HltSelReportsRawBank internally reported size " + std::to_string( hltSelReportsBank.size() )
           + " less than bank size delivered by RawEvent " + std::to_string(bankSize),
          StatusCode::SUCCESS, 100 );
    errors=true;

  } else {

    ic = hltSelReportsBank.integrityCode();
    if( ic ){
      Error( " HltSelReportsRawBank fails integrity check with code " 
            + std::to_string( ic ) + " " +  HltSelRepRBEnums::IntegrityCodesToString(ic) ,
             StatusCode::SUCCESS, 100 );
      errors=true;
    }

    ic = hitsSubBank.integrityCode();
    if( ic ){
      Error( " HltSelRepRBHits fails integrity check with code " 
             + std::to_string(ic) +  " " +  HltSelRepRBEnums::IntegrityCodesToString(ic),
             StatusCode::SUCCESS, 100 );
      errors=true;
    }

    ic = objTypSubBank.integrityCode();
    if( ic ){
      Error( " HltSelRepRBObjTyp fails integrity check with code " 
             + std::to_string(ic) +  " " + HltSelRepRBEnums::IntegrityCodesToString(ic),
             StatusCode::SUCCESS, 100 );
      errors=true;
    }

    ic = substrSubBank.integrityCode();
    if( ic ){
      Error( " HltSelRepRBSubstr fails integrity check with code " 
             + std::to_string(ic) + " " +  HltSelRepRBEnums::IntegrityCodesToString(ic),
             StatusCode::SUCCESS, 100 );
      errors=true;
    }
    if( nObj != substrSubBank.numberOfObj() ){
      Error( " HltSelRepRBSubstr has number of objects " 
             + std::to_string( substrSubBank.numberOfObj( ))
             +  " which is different than HltSelRepRBObjTyp " + std::to_string(nObj),
             StatusCode::SUCCESS, 100 );
      errors=true;
    }

    ic = stdInfoSubBank.integrityCode();
    if( ic ){
      Error( " HltSelRepRBStdInfo fails integrity check with code " 
             + std::to_string(ic) +  " " + HltSelRepRBEnums::IntegrityCodesToString(ic),
             StatusCode::SUCCESS, 100 );
      errors=true;
    }
    if( nObj != stdInfoSubBank.numberOfObj() ){
      Error( " HltSelRepRBStdInfo has number of objects " 
             + std::to_string(stdInfoSubBank.numberOfObj())
             + " which is different than HltSelRepRBObjTyp " + std::to_string(nObj),
             StatusCode::SUCCESS, 100 );
      errors=true;
    }

    ic = extraInfoSubBank.integrityCode();
    if( ic ){
      Error( " HltSelRepRBExtraInfo fails integrity check with code " 
             + std::to_string(ic) +  " " + HltSelRepRBEnums::IntegrityCodesToString(ic),
             StatusCode::SUCCESS, 100 );
      //      exInfOn=false; // the only non-fatal info corruption. Set but not used!
    }
    if( nObj != extraInfoSubBank.numberOfObj() ){
      Error(  " HltSelRepRBExtraInfo has number of objects " 
              + std::to_string(extraInfoSubBank.numberOfObj())
              +  " which is different than HltSelRepRBObjTyp " + std::to_string( nObj),
             StatusCode::SUCCESS, 100 );
      //      exInfOn=false; // Set but not used!
    }
  }

  
  if( errors ){
    hltSelReportsBank.deleteBank();
    return Error("Quiting because of the possible data corruption", StatusCode::SUCCESS, 10 );
  }
  
  fill(m_bankSize, (hltSelReportsBank.size()*4)/1000., 1);
  fill(m_hitSize, (hitsSubBank.size()*4)/1000., 1);
  fill(m_objtypSize, (objTypSubBank.size()*4)/1000., 1);
  fill(m_substrSize, (substrSubBank.size()*4)/1000., 1);
  fill(m_stdinfoSize, (stdInfoSubBank.size()*4)/1000., 1);
  fill(m_extinfoSize, (extraInfoSubBank.size()*4)/1000., 1);

  fillDiag("HLTSELREPORTS TOTAL SIZE (Bytes)",hltSelReportsBank.size()*4);
  fillDiag("HLTSELREPORTS HIT SUBBANK",       hitsSubBank.size()*4);
  fillDiag("HLTSELREPORTS STD INFO SUBBANK",  stdInfoSubBank.size()*4);
  fillDiag("HLTSELREPORTS EXTRA INFO SUBBANK",extraInfoSubBank.size()*4);
  fillDiag("HLTSELREPORTS SUBSTR SUBBANK",    substrSubBank.size()*4);

  // clean-up
  hltSelReportsBank.deleteBank();

  return StatusCode::SUCCESS;
}


void HltRawDataMonitor::fillDiag(const char* trigName, double length )
{
  const double maxlen=20000.0;
  double lenn = (length<maxlen)?length:(maxlen-1.);
  for (std::vector<std::pair<std::string, AIDA::IHistogram1D*> >::const_iterator i = m_hltSelNameList.begin(); 
       i != m_hltSelNameList.end(); ++i) { 
    if (  i->first == trigName ) { 
      fill(i->second, lenn, 1); 
      return;        
    }
  } 
  std::pair<std::string, AIDA::IHistogram1D*> hltHis(trigName, book(trigName, 0., maxlen, 2000));
  m_hltSelNameList.push_back(hltHis);
  fill(hltHis.second, lenn , 1);     
}


//=============================================================================
//  Selected Candidates for HltSelReports    
//=============================================================================
StatusCode HltRawDataMonitor::fillHltSelRep()
{
  debug() << "==> FillHltSelReport " << endmsg;
  if ( exist<LHCb::HltSelReports>( LHCb::HltSelReportsLocation::Default ) ) { // exist --> get content
    const LHCb::HltSelReports* selReports = get<LHCb::HltSelReports>( LHCb::HltSelReportsLocation::Default );

    // individual L0/Hlt1/Hlt2 trigger line
    for (  LHCb::HltSelReports::Container::const_iterator it = selReports->begin(); it != selReports->end(); ++it ) {
      //      info() << "trigger:" << it->first << endmsg;
      fillDiag( it->first.c_str(),double(nLength(it->second)));
    }
  }
  return StatusCode::SUCCESS; 
}

//=============================================================================
//  the size of HltObjectSummary    
//=============================================================================
int HltRawDataMonitor::nLength (const LHCb::HltObjectSummary& p)
{
  unsigned int nlength    = (2+4*(p.lhcbIDs().size())) + (2+2*(p.substructure().size()));
  // loop over numericalInfo
  for ( LHCb::HltObjectSummary::Info::const_iterator k = p.numericalInfo().begin(); k != p.numericalInfo().end(); ++k) {
    if ( k->first.find("#") != std::string::npos ) { 
      nlength += 2;
    } else { 
      nlength += 4;
    }
  }
  // loop over substructure
  for ( SmartRefVector< LHCb::HltObjectSummary >::const_iterator j = p.substructure().begin(); 
        j != p.substructure().end(); ++j){
    nlength += nLength(**j);
  }
  return nlength;
}

//===============================================================================
//  the Sum, the Average, the Median, the Root Mean Square (for w/o trigger-off)
//===============================================================================
double HltRawDataMonitor::nMedian(const AIDA::IHistogram1D& q)
{
  unsigned int i         = 0;
  unsigned int sum       = 0;
  unsigned int midEvt    = q.allEntries()/2;
  unsigned int nBinX     = q.axis().bins();

  for ( i = 0; i < nBinX; i++ ) {
    sum += q.binEntries(i);
    if ( sum > midEvt ) break;
  }
  return q.axis().binLowerEdge(i);
}

//==========================================================================
//  the Median (for w/ trigger-Off)
//==========================================================================
double HltRawDataMonitor::nMedian2(unsigned int NumOfEvt, const AIDA::IHistogram1D& q)
{
  unsigned int i         = 0;

  unsigned int nBinX     = q.axis().bins();
  unsigned int allEvt    = q.allEntries(); // trigger-on
  unsigned int sum       = NumOfEvt - allEvt; // trigger-off

  for ( i = 0; i < nBinX; i++ ) { // trigger-on 
    sum +=  q.binEntries(i);
    if ( sum > NumOfEvt/2 ) {// trigger-on & trigger-off
      break;
    }
  }
  return q.axis().binLowerEdge(i);
}

//==========================================================================
//  the Mean (for w/ trigger-Off)
//==========================================================================
double HltRawDataMonitor::nMean2(unsigned int NumOfEvt, const AIDA::IHistogram1D& q)
{
  unsigned int i         = 0;

  unsigned int nBinX     = q.axis().bins();
  double       sum       = 0;
  double       mean      = 0;

  for ( i = 0; i < nBinX; i++ ) { // trigger-on 
    sum += q.binEntries(i) * q.axis().binLowerEdge(i);
  }
  mean = sum / NumOfEvt;
  return mean;
}

//==========================================================================
//  the RMS (for w/ trigger-Off)
//==========================================================================
double HltRawDataMonitor::nRMS2(unsigned int NumOfEvt, const AIDA::IHistogram1D& q)
{
  unsigned int i         = 0;

  unsigned int nBinX     = q.axis().bins();
  double       sum       = 0;
  double       rms       = 0;

  for ( i = 0; i < nBinX; i++ ) { // trigger-on 
    sum += q.axis().binLowerEdge(i) * q.axis().binLowerEdge(i) * q.binEntries(i);
  }
  double mean=nMean2(NumOfEvt,q);
  rms = sqrt(fabs(sum/NumOfEvt -mean*mean));
  return rms;
}

//================================================================================================
//  descending the mean value of every selected trigger function (for w/ trigger-off) 
//================================================================================================   
bool HltRawDataMonitor::sort::operator()(const HltRawDataMonitor::HltSortedSelName & q1, 
                                      const HltRawDataMonitor::HltSortedSelName & q2) const
{  
  return  q1.second > q2.second ;
}

//=============================================================================
//  Printout selection-by-selection diagnostics
//=============================================================================
StatusCode HltRawDataMonitor::selectionDiagnostics()
{ 
  // analyze histograms
  // calculate mean, median, rms here
  for (std::vector<std::pair<std::string, AIDA::IHistogram1D*> >::const_iterator k = m_hltSelNameList.begin();
       k != m_hltSelNameList.end(); ++k) {
    std::pair<std::string, AIDA::IHistogram1D*> m_HltSelected(k->first,k->second);
    m_hltRankedSelName.push_back(HltSortedSelName(m_HltSelected,nMean2(m_event,*k->second)));
   }

  std::ostringstream mess;

  mess << boost::format( " ==== HltSelReports selection-by-selections diagnostics #-of-events= %1$=10i \n" )
    % m_event;

  // sorting based on the mean value for w/o trigger-off case
  std::sort( m_hltRankedSelName.begin(), m_hltRankedSelName.end(), sort() );

  // print-out results
  mess << boost::format( " | %1$=20.20s || %2$=15.10s  %3$=15.10s  %4$=15.10s  %5$=15.10s | \n" ) 
    % "" % "entry" % "mean" % "RMS" % "median";

  mess << boost::format( " | %1$=20.20s || %2$=15.1f  %3$=15.1f  %4$=15.1f  %5$=15.1f | \n" ) 
    % "TotalSize" %  m_bankSize->allEntries() %  m_bankSize->mean() %  m_bankSize->rms() % nMedian(*m_bankSize);

  mess << boost::format( " | %1$=20.20s  || %2$=15.1f  %3$=15.1f  %4$=15.1f  %5$=15.1f | \n" ) 
    % "HitSize" %  m_hitSize->allEntries() %  m_hitSize->mean() %  m_hitSize->rms() % nMedian(*m_hitSize);

  mess << boost::format( " | %1$=20.20s  || %2$=15.1f  %3$=15.1f  %4$=15.1f  %5$=15.1f | \n" ) 
    % "ObjTypSize" %  m_objtypSize->allEntries() %  m_objtypSize->mean() %  m_objtypSize->rms() % nMedian(*m_objtypSize);

  mess << boost::format( " | %1$=20.20s  || %2$=15.1f  %3$=15.1f  %4$=15.1f  %5$=15.1f | \n" ) 
    % "SubStrSize" %  m_substrSize->allEntries() %  m_substrSize->mean() %  m_substrSize->rms() % nMedian(*m_substrSize);

  mess << boost::format( " | %1$=20.20s  || %2$=15.1f  %3$=15.1f  %4$=15.1f  %5$=15.1f | \n" ) 
    % "StdInfoSize" %  m_stdinfoSize->allEntries() %  m_stdinfoSize->mean() %  m_stdinfoSize->rms() % nMedian(*m_stdinfoSize);

  mess << boost::format( " | %1$=20.20s  || %2$=15.1f  %3$=15.1f  %4$=15.1f  %5$=15.1f | \n" ) 
    % "ExtInfoSize" %  m_extinfoSize->allEntries() %  m_extinfoSize->mean() %  m_extinfoSize->rms() % nMedian(*m_extinfoSize);

  // print-out results 
  mess << boost::format( " | %1$=35.15s | | %2$=16.16s | %3$=16.16s | | %4$=14.12s | %5$=14.12s | | %6$=8s | | %7$=14.8s | %8$=14.8s | \n" ) 
    % "trigger name" % "mean(per event)" % "mean(per trigger)" % "median(evt)" % "median(trg)" % "entries" % "rms(evt)" % "rms(trg)";
  
  for ( std::vector< HltSortedSelName >::const_iterator j = m_hltRankedSelName.begin(); 
        j != m_hltRankedSelName.end(); ++j) {
    mess << boost::format( " | %1$-35.35s | | %2$16.1f | %3$16.1f | | %4$14.1f | %5$14.1f | | %6$8.1f | | %7$14.1f | %8$14.1f | \n" ) 
      % j->first.first %  j->second % (j->first.second)->mean() % nMedian2(m_event,*j->first.second) 
      % nMedian(*j->first.second) % j->first.second->allEntries() %  nRMS2(m_event,*j->first.second) % j->first.second->rms();
  }

  info() <<  mess.str() << endmsg;
  return StatusCode::SUCCESS;

}
