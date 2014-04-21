// Include files 
#include "boost/format.hpp"

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/HltSelReports.h"
#include "Event/HltDecReports.h"
#include "Event/HltObjectSummary.h"
#include "Event/RawEvent.h"

// bank structure
#include "HltSelRepRawBank.h"
#include "HltSelRepRBHits.h"
#include "HltSelRepRBSubstr.h"
#include "HltSelRepRBObjTyp.h"
#include "HltSelRepRBExtraInfo.h"
#include "HltSelRepRBStdInfo.h"

// local
#include "HltSelReportsDecoder.h"
#include "HltSelReportsWriter.h"

#include "Event/Track.h"
#include "Event/Particle.h"
#include "Event/RecVertex.h"
#include "Event/CaloCluster.h"


using namespace LHCb;

namespace {
float floatFromInt(unsigned int i)
{
        union IntFloat { unsigned int mInt; float mFloat; };
        IntFloat a; a.mInt=i;
        return a.mFloat;
}

const Gaudi::StringKey  Hlt1SelectionID{"Hlt1SelectionID"};
const Gaudi::StringKey  Hlt2SelectionID{"Hlt2SelectionID"};
const Gaudi::StringKey  InfoID{"InfoID"};
}

//-----------------------------------------------------------------------------
// Implementation file for class : HltSelReportsDecoder
//
// 2008-08-01 : Tomasz Skwarnicki
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltSelReportsDecoder )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltSelReportsDecoder::HltSelReportsDecoder( const std::string& name,
                                          ISvcLocator* pSvcLocator)
: Decoder::AlgBase ( name , pSvcLocator ), m_hltANNSvc{nullptr}
{
  //new for decoders, initialize search path, and then call the base method
  m_rawEventLocations = {LHCb::RawEventLocation::Trigger, LHCb::RawEventLocation::Copied, LHCb::RawEventLocation::Default};
  initRawEventSearch();

  declareProperty("OutputHltSelReportsLocation",
    m_outputHltSelReportsLocation= LHCb::HltSelReportsLocation::Default);  

  declareProperty("SourceID",
		  m_sourceID= HltSelReportsWriter::kSourceID_Dummy );  
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltSelReportsDecoder::initialize() {
  StatusCode sc = Decoder::AlgBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  
  m_hltANNSvc = svc<IANNSvc>("ANNDispatchSvc");

  if( m_sourceID > HltSelReportsWriter::kSourceID_Max ){
    m_sourceID = m_sourceID & HltSelReportsWriter::kSourceID_Max;
    return Error("Illegal SourceID specified; maximal allowed value is 7" , StatusCode::FAILURE, 50 );
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltSelReportsDecoder::execute() {
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // get inputs
  LHCb::RawEvent* rawEvent = findFirstRawEvent();
  
  if( !rawEvent ) {
    return Error(" No RawEvent found at any location. No HltSelReports created.");
  }  

  // create output container and put it on TES
  HltSelReports* outputSummary = new HltSelReports();
  put( outputSummary, m_outputHltSelReportsLocation );

  // create output container for Object Summaries and put it on TES
  LHCb::HltObjectSummary::Container* objectSummaries = new HltObjectSummary::Container();
  put( objectSummaries, m_outputHltSelReportsLocation.value() + "/Candidates" );


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
  std::vector<const RawBank*> orderedBanks(hltselreportsRawBanks.size(),nullptr);
  for( const auto& hltselreportsRawBank : hltselreportsRawBanks ) {

    unsigned int sourceID = HltSelReportsWriter::kSourceID_Hlt;
    if( hltselreportsRawBank->version() > 1 ){
      sourceID = hltselreportsRawBank->sourceID() >> HltSelReportsWriter::kSourceID_BitShift;
    }
    if( m_sourceID != sourceID ) continue;

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

      for( const auto& hltselreportsRawBank : hltselreportsRawBanks ) {
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
      *pBank = hltselreportsRawBank->data()[i]; 
      ++pBank;
    }
  }

  HltSelRepRBHits hitsSubBank( hltSelReportsBank.subBankFromID( HltSelRepRBEnums::kHitsID ) );
  HltSelRepRBObjTyp objTypSubBank( hltSelReportsBank.subBankFromID( HltSelRepRBEnums::kObjTypID ) );
  HltSelRepRBSubstr substrSubBank( hltSelReportsBank.subBankFromID( HltSelRepRBEnums::kSubstrID ) );
  HltSelRepRBStdInfo stdInfoSubBank( hltSelReportsBank.subBankFromID( HltSelRepRBEnums::kStdInfoID ) );
  HltSelRepRBExtraInfo extraInfoSubBank( hltSelReportsBank.subBankFromID( HltSelRepRBEnums::kExtraInfoID ) );

  // ----------------------------------------- integrity checks -------------------------
  bool errors=false;
  bool exInfOn=true;
  unsigned int ic;
  unsigned int nObj = objTypSubBank.numberOfObj();

  if( bankSize < hltSelReportsBank.size() ){
    std::ostringstream mess;
    mess   << " HltSelReportsRawBank internally reported size " << hltSelReportsBank.size()
           << " less than bank size delivered by RawEvent " << bankSize;
    Error( mess.str(), StatusCode::SUCCESS, 100 );
    errors=true;

  } else {

  ic = hltSelReportsBank.integrityCode();
  if( ic ){
    std::ostringstream mess;
    mess << " HltSelReportsRawBank fails integrity check with code " 
         << ic << " " << HltSelRepRBEnums::IntegrityCodesToString(ic) ;
    Error( mess.str(), StatusCode::SUCCESS, 100 );
    errors=true;
  }

  ic = hitsSubBank.integrityCode();
  if( ic ){
    std::ostringstream mess;
    mess << " HltSelRepRBHits fails integrity check with code " 
         << ic << " " << HltSelRepRBEnums::IntegrityCodesToString(ic) ;
    Error( mess.str(), StatusCode::SUCCESS, 100 );
    errors=true;
  }

  ic = objTypSubBank.integrityCode();
  if( ic ){
    std::ostringstream mess;
    mess << " HltSelRepRBObjTyp fails integrity check with code " 
         << ic << " " << HltSelRepRBEnums::IntegrityCodesToString(ic) ;
    Error( mess.str(), StatusCode::SUCCESS, 100 );
    errors=true;
  }

  ic = substrSubBank.integrityCode();
  if( ic ){
    std::ostringstream mess;
    mess << " HltSelRepRBSubstr fails integrity check with code " 
         << ic << " " << HltSelRepRBEnums::IntegrityCodesToString(ic) ;
    Error( mess.str(), StatusCode::SUCCESS, 100 );
    errors=true;
  }
  if( nObj != substrSubBank.numberOfObj() ){
    std::ostringstream mess;
    mess << " HltSelRepRBSubstr has number of objects " 
         << substrSubBank.numberOfObj()
         << " which is different than HltSelRepRBObjTyp " << nObj ;
    Error( mess.str(), StatusCode::SUCCESS, 100 );
    errors=true;
  }

  ic = stdInfoSubBank.integrityCode();
  if( ic ){
    std::ostringstream mess;
    mess << " HltSelRepRBStdInfo fails integrity check with code " 
         << ic << " " << HltSelRepRBEnums::IntegrityCodesToString(ic) ;
    Error( mess.str(), StatusCode::SUCCESS, 100 );
    errors=true;
  }
  if( nObj != stdInfoSubBank.numberOfObj() ){
    std::ostringstream mess;
    mess << " HltSelRepRBStdInfo has number of objects " 
         << stdInfoSubBank.numberOfObj()
         << " which is different than HltSelRepRBObjTyp " << nObj ;
    Error( mess.str(), StatusCode::SUCCESS, 100 );
    errors=true;
  }

  ic = extraInfoSubBank.integrityCode();
  if( ic ){
    std::ostringstream mess;
    mess << " HltSelRepRBExtraInfo fails integrity check with code " 
         << ic << " " << HltSelRepRBEnums::IntegrityCodesToString(ic) ;
    Error( mess.str(), StatusCode::SUCCESS, 100 );
    exInfOn=false; // the only non-fatal info corruption
  }
  if( nObj != extraInfoSubBank.numberOfObj() ){
    std::ostringstream mess;
    mess << " HltSelRepRBExtraInfo has number of objects " 
         << extraInfoSubBank.numberOfObj()
         << " which is different than HltSelRepRBObjTyp " << nObj ;
    Error( mess.str(), StatusCode::SUCCESS, 100 );
    exInfOn=false;
  }

  }

  if ( msgLevel(MSG::VERBOSE) ){

    // print created bank and subbanks inside
    verbose() << hltSelReportsBank << endmsg;
    
    verbose() << HltSelRepRBHits( hltSelReportsBank.subBankFromID( HltSelRepRBEnums::kHitsID ) ) << endmsg;
    verbose() << HltSelRepRBObjTyp( hltSelReportsBank.subBankFromID( HltSelRepRBEnums::kObjTypID ) ) << endmsg;
    verbose() << HltSelRepRBSubstr( hltSelReportsBank.subBankFromID( HltSelRepRBEnums::kSubstrID ) ) << endmsg;
    verbose() << HltSelRepRBStdInfo( hltSelReportsBank.subBankFromID( HltSelRepRBEnums::kStdInfoID ) ) << endmsg;
    verbose() << HltSelRepRBExtraInfo( hltSelReportsBank.subBankFromID( HltSelRepRBEnums::kExtraInfoID ) ) << endmsg;

  } 


  if( errors ){
    hltSelReportsBank.deleteBank();
    return Error("Quiting becasue of the possible data corruption", StatusCode::SUCCESS, 100 );
  }


  // -----------------------------------------------------------------
  // create object summaries
  // -----------------------------------------------------------------

  // put them in local vector until they are finished
  std::vector< HltObjectSummary* > objects;

  for(unsigned int iObj=0; iObj!= nObj; ++iObj){
  
    HltObjectSummary* hos = new HltObjectSummary();

    // =========== class ID
    hos->setSummarizedObjectCLID( objTypSubBank.next() );


    // =========== numerical info
    HltObjectSummary::Info infoPersistent;


    //           ============== standard 
    HltSelRepRBStdInfo::StdInfo stdInfo = stdInfoSubBank.next();
    if( stdInfo.size() )
    switch( hos->summarizedObjectCLID() )
      {
      case LHCb::CLID_Track:
        {      
         if( stdInfo.size()>=6 ){

            infoPersistent.insert( "0#Track.firstState.z", floatFromInt(stdInfo[0]) );
            infoPersistent.insert( "1#Track.firstState.x", floatFromInt(stdInfo[1]) );
            infoPersistent.insert( "2#Track.firstState.y", floatFromInt(stdInfo[2]) );
            infoPersistent.insert( "3#Track.firstState.tx", floatFromInt(stdInfo[3]) );
            infoPersistent.insert( "4#Track.firstState.ty", floatFromInt(stdInfo[4]) );
            infoPersistent.insert( "5#Track.firstState.qOverP", floatFromInt(stdInfo[5]) );  
            
            if (stdInfo.size()>=8) {
                  infoPersistent.insert( "6#Track.chi2PerDoF", floatFromInt( stdInfo[6] ) );
                  infoPersistent.insert( "7#Track.nDoF", floatFromInt( stdInfo[7] ) );
            }
        
         } else {


            std::ostringstream mess;
            mess  << " wrong number of StdInfo on Track " << stdInfo.size();
            Warning(mess.str(),StatusCode::SUCCESS, 20 );
            for(HltSelRepRBStdInfo::StdInfo::const_iterator i=stdInfo.begin();
                i!=stdInfo.end(); ++i){
              std::stringstream ss;
              ss << "z#Track.unknown" << int(i-stdInfo.begin() );
              infoPersistent.insert( ss.str(), floatFromInt(*i) );        
            }      
          }
        }
        break;
      case LHCb::CLID_RecVertex:
        {  
          if( stdInfo.size()>=3 ){    
 
            infoPersistent.insert( "0#RecVertex.position.x", floatFromInt(stdInfo[0]) ); 
            infoPersistent.insert( "1#RecVertex.position.y", floatFromInt(stdInfo[1]) ); 
            infoPersistent.insert( "2#RecVertex.position.z", floatFromInt(stdInfo[2]) ); 

          } else {
            
            std::ostringstream mess;
            mess  << " wrong number of StdInfo on RecVertex " << stdInfo.size();
            Warning(mess.str(),StatusCode::SUCCESS, 20 );
            for(HltSelRepRBStdInfo::StdInfo::const_iterator i=stdInfo.begin();
                i!=stdInfo.end(); ++i){
              std::stringstream ss;
              ss << "z#RecVertex.unknown" << int(i-stdInfo.begin() );
              infoPersistent.insert( ss.str(), floatFromInt(*i) );        
            }      
            
          }
        }
	break;
      case LHCb::CLID_Particle:
        {      
          if( stdInfo.size()>=8 ){    
            
            infoPersistent.insert( "0#Particle.particleID.pid", floatFromInt(stdInfo[0]) ); 
            infoPersistent.insert( "1#Particle.measuredMass", floatFromInt(stdInfo[1]) ); 
            infoPersistent.insert( "2#Particle.referencePoint.z", floatFromInt(stdInfo[2]) ); 
            infoPersistent.insert( "3#Particle.referencePoint.x", floatFromInt(stdInfo[3]) ); 
            infoPersistent.insert( "4#Particle.referencePoint.y", floatFromInt(stdInfo[4]) );
            infoPersistent.insert( "5#Particle.slopes.x", floatFromInt(stdInfo[5]) );
            infoPersistent.insert( "6#Particle.slopes.y", floatFromInt(stdInfo[6]) );
            infoPersistent.insert( "7#Particle.1/p", floatFromInt(stdInfo[7]) );
            
          } else {

            std::ostringstream mess;
            mess  << " wrong number of StdInfo on Particle " << stdInfo.size();
            Warning(mess.str(),StatusCode::SUCCESS, 20 );
            for(HltSelRepRBStdInfo::StdInfo::const_iterator i=stdInfo.begin();
                i!=stdInfo.end(); ++i){
              std::stringstream ss;
              ss << "z#Particle.unknown" << int(i-stdInfo.begin() );
              infoPersistent.insert( ss.str(), floatFromInt(*i) );        
            }      
            
          }
        }
        break;
      case LHCb::CLID_CaloCluster:
        {      
          
          if( stdInfo.size()>=4 ){    
 
            infoPersistent.insert( "0#CaloCluster.e", floatFromInt(stdInfo[0]) );
            infoPersistent.insert( "1#CaloCluster.position.x", floatFromInt(stdInfo[1]) );
            infoPersistent.insert( "2#CaloCluster.position.y", floatFromInt(stdInfo[2]) );
            infoPersistent.insert( "3#CaloCluster.position.z", floatFromInt(stdInfo[3]) );
          } else {

            std::ostringstream mess;
            mess  << " wrong number of StdInfo on CaloCluster " << stdInfo.size();
            Warning(mess.str(),StatusCode::SUCCESS, 20 );
            for(HltSelRepRBStdInfo::StdInfo::const_iterator i=stdInfo.begin();
                i!=stdInfo.end(); ++i){
              std::stringstream ss;
              ss << "z#CaloCluster.unknown" << int(i-stdInfo.begin() );
              infoPersistent.insert( ss.str(), floatFromInt(*i) );        
            }      
            
          }
        }
        break;
      case 1:
        {
          infoPersistent.insert( "0#SelectionID", floatFromInt(stdInfo[0]) );
          if( stdInfo.size()>1 ){
            int id = (int)(  floatFromInt(stdInfo[1])+0.1 );            
            boost::optional<IANNSvc::minor_value_type> selName = m_hltANNSvc->value(Hlt1SelectionID,id);
            if (!selName) selName = m_hltANNSvc->value(Hlt2SelectionID,id);
            if (selName) {
              infoPersistent.insert( "10#" + std::string(selName->first), floatFromInt(stdInfo[1]) );        
            } else {
              std::ostringstream mess;
              mess << " Did not find string key for PV-selection-ID in trigger selection in storage id=" << id;
              Error( mess.str(),  StatusCode::SUCCESS, 10 ); 
              infoPersistent.insert( "10#Unknown" , floatFromInt(id) );        
            }
            for( unsigned int ipvkeys=2; ipvkeys< stdInfo.size(); ++ipvkeys ){
              std::stringstream ss;
              ss << "11#" << boost::format("%1$=08X") % (ipvkeys-2) ;
              infoPersistent.insert( ss.str(), floatFromInt( stdInfo[ipvkeys] ) );        
            }
          } 

        }
        break;
      default:
        { 

          std::ostringstream mess;
          mess << " StdInfo on unsupported class type " << hos->summarizedObjectCLID();
          Warning( mess.str(),  StatusCode::SUCCESS, 20 );
          for(HltSelRepRBStdInfo::StdInfo::const_iterator i=stdInfo.begin();
              i!=stdInfo.end(); ++i){
            std::stringstream ss;
            ss << "z#Unknown.unknown" << int(i-stdInfo.begin() );
            infoPersistent.insert( ss.str(), floatFromInt(*i) );        
          }      
        }    
      }

    //           ============== extra 

    if( exInfOn ){
      HltSelRepRBExtraInfo::ExtraInfo extraInfo = extraInfoSubBank.next();
      for( HltSelRepRBExtraInfo::ExtraInfo::const_iterator i=extraInfo.begin();
           i!=extraInfo.end(); ++i){
        // convert int to string
        boost::optional<IANNSvc::minor_value_type> infos = m_hltANNSvc->value(InfoID,i->first); 
        if ( infos ) {
          infoPersistent.insert( infos->first, i->second );
        } else {
          std::ostringstream mess;
          mess << " String key for Extra Info item in storage not found id=" << i->first;
          Warning( mess.str(), StatusCode::SUCCESS, 20 );
        }

      }
    }
    hos->setNumericalInfo( infoPersistent );

    objects.push_back(hos);
  }

  // -----------------------------------------------------------------
  // reloop to add substructure or hits
  // -----------------------------------------------------------------
  for( unsigned int iObj=0; iObj!= nObj; ++iObj){
    
    HltObjectSummary* & hos = objects[iObj];
    
    HltSelRepRBSubstr::Substr sub=substrSubBank.next();

    if( sub.first ){
      // hits
      unsigned int nSeq = hitsSubBank.numberOfSeq();
      std::vector< LHCb::LHCbID > hits;
      for( HltSelRepRBSubstr::Substrv::const_iterator i=sub.second.begin();i!=sub.second.end();++i){
        unsigned int iSeq=*i;
        if( iSeq<nSeq ){
          std::vector< LHCb::LHCbID > hitseq = hitsSubBank.sequence( iSeq );
          //   for bank version zero, first hit in the first sequence was corrupted ------
          //                   for odd number of sequences saved - omit this hit
          if( iSeq==0 ){
            if( hltselreportsRawBank0->version()==0 ){
              if( nSeq%2 == 1 ){
                hitseq.erase( hitseq.begin() );
              }
            }
          }
          // ------------------------- end fix --------------------------------------------
          if( hitseq.size() ){            
            hits.insert( hits.end(), hitseq.begin(), hitseq.end() );
          }          
        } else {
          Error(  "Hit sequence index out of range", StatusCode::SUCCESS, 10 );
        }
      }
      hos->setLhcbIDs( hits );

    } else {
      // pointers
      SmartRefVector<LHCb::HltObjectSummary> thisSubstructure; 
      for( HltSelRepRBSubstr::Substrv::const_iterator i=sub.second.begin();i!=sub.second.end();++i){
        unsigned int jObj=*i;     
        if( jObj<nObj ){
          thisSubstructure.push_back( &(*(objects[jObj])) );
        } else {
          Error(  " Substructure object index out of range ", StatusCode::SUCCESS, 10 );
        }
      }
      hos->setSubstructure( thisSubstructure );
    }

    // give ownership to TES
    objectSummaries->push_back(hos);
  }

  // clean-up
  hltSelReportsBank.deleteBank();

  // ---------------------------------------------------------
  // ------- special container for selections ----------------
  // ---------------------------------------------------------

  for( unsigned int iObj=0; iObj!= nObj; ++iObj){
    
    HltObjectSummary* & hos = objects[iObj];        
    if( hos->summarizedObjectCLID()!=1 )continue; 

    boost::optional<IANNSvc::minor_value_type> selName;
    unsigned int hltType(HltSelReportsWriter::kSourceID_Hlt2); 
    for( HltObjectSummary::Info::const_iterator i=hos->numericalInfo().begin();
         i!=hos->numericalInfo().end();++i ){
      if( i->first == "0#SelectionID" ){
        int id = (int)(i->second+0.1);
        selName = m_hltANNSvc->value(Hlt1SelectionID,id);
        if (!selName) {
          selName = m_hltANNSvc->value(Hlt2SelectionID,id);
	} else {
	  hltType=HltSelReportsWriter::kSourceID_Hlt1;
	}
        break;
      }
    }
    if( selName  ){

      //   skip reports of the wrong type
      if( ( m_sourceID == HltSelReportsWriter::kSourceID_Hlt1 ) ||
	  ( m_sourceID == HltSelReportsWriter::kSourceID_Hlt2 ) ){
	if( hltType != m_sourceID )continue;
      } 

      // clone hos
      HltObjectSummary selSumOut;
      selSumOut.setSummarizedObjectCLID( hos->summarizedObjectCLID() );
      selSumOut.setNumericalInfo( hos->numericalInfo() );
      selSumOut.setSubstructure( hos->substructure() ); 

      // insert selection into the container
      if( outputSummary->insert(selName->first,selSumOut) == StatusCode::FAILURE ){
        Error( "  Failed to add Hlt selection name " 
               + std::string(selName->first)
                + " to its container ", StatusCode::SUCCESS, 10 );
      }

    } else {    

      std::ostringstream mess;
      mess << " Did not find string key for trigger selection in storage";
      Error( mess.str(),  StatusCode::SUCCESS, 50 ); 

    }

  }

  if ( msgLevel(MSG::VERBOSE) ){

    verbose() << " ======= HltSelReports size= " << outputSummary->size() << endmsg;  
    verbose() << *outputSummary << endmsg;

    verbose() << " ======= HltObjectSummary container size= " << objectSummaries->size() << endmsg;
    for( const auto& pHos : *objectSummaries ) {
      verbose() << " key " << pHos->index();
      auto selby = outputSummary->selectedAsCandidateBy(pHos);
      if( !selby.empty() ){
        verbose() << " selectedAsCandidateBy= ";       
        for( const auto&  i : selby ) verbose() << i << " ";
        auto pvInfo = outputSummary->pvSelectionNameAndKey(pHos);
        if( pvInfo.second > -1 ){
          verbose() << " pvSelectionName= " << pvInfo.first << " pvKey= " << pvInfo.second << " ";
        }
      }     
      verbose() << *pHos << endmsg;    
    }
  }

 return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltSelReportsDecoder::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return Decoder::AlgBase::finalize();  // must be called after all other actions
}

//=============================================================================
    
  

