// $Id: HltSelReportsReader.cpp,v 1.4 2008-12-15 21:41:25 tskwarni Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/HltSelReports.h"
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
#include "HltSelReportsReader.h"

#include "Event/Track.h"
#include "Event/Particle.h"
#include "Event/RecVertex.h"
#include "Event/CaloCluster.h"


using namespace LHCb;


//-----------------------------------------------------------------------------
// Implementation file for class : HltSelReportsReader
//
// 2008-08-01 : Tomasz Skwarnicki
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltSelReportsReader );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltSelReportsReader::HltSelReportsReader( const std::string& name,
                                          ISvcLocator* pSvcLocator)
    : GaudiAlgorithm ( name , pSvcLocator )
  //  : HltBaseAlg( name , pSvcLocator )
{

  declareProperty("OutputHltSelReportsLocation",
    m_outputHltSelReportsLocation= LHCb::HltSelReportsLocation::Default);  
  declareProperty("InputRawEventLocation",
    m_inputRawEventLocation= LHCb::RawEventLocation::Default);

    m_hltANNSvc = 0;

}
//=============================================================================
// Destructor
//=============================================================================
HltSelReportsReader::~HltSelReportsReader() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltSelReportsReader::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  // StatusCode sc = HltBaseAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_hltANNSvc = svc<IANNSvc>("HltANNSvc");

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltSelReportsReader::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;


  // get inputs
  if( !exist<RawEvent>(m_inputRawEventLocation) ){    
    return Error(" No RawEvent at "+ m_inputRawEventLocation.value());
  }  
  RawEvent* rawEvent = get<RawEvent>(m_inputRawEventLocation);

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

  unsigned int bankSize =0;
  const RawBank* orderedBanks[hltselreportsRawBanks.size()];
  for( unsigned int iBank=0; iBank<hltselreportsRawBanks.size(); ++iBank){
    orderedBanks[iBank]=0;
  }
  for( std::vector<RawBank*>::const_iterator hltselreportsRawBankP=hltselreportsRawBanks.begin();
	 hltselreportsRawBankP!=hltselreportsRawBanks.end(); ++hltselreportsRawBankP ){    
    const RawBank* hltselreportsRawBank = *hltselreportsRawBankP;
    unsigned int sourceId = hltselreportsRawBank->sourceID();
    if( sourceId < hltselreportsRawBanks.size() ){
      orderedBanks[sourceId]= hltselreportsRawBank;
    } else {
      Error( " Illegal Source ID HltSelReports bank skipped ", StatusCode::SUCCESS, 20 );
    }
    bankSize += hltselreportsRawBank->size();
  }
  bankSize = (bankSize+3)/4; // from bytes to words
  // need to copy it to local array to get rid of const restriction
  unsigned int* pBank = new unsigned int[bankSize];
  HltSelRepRawBank hltSelReportsBank( pBank );

  for( unsigned int iBank=0; iBank<hltselreportsRawBanks.size(); ++iBank){
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


  std::vector<IANNSvc::minor_value_type> hltinfos = m_hltANNSvc->items("InfoID"); 
  // get string-to-int selection ID map
  std::vector<IANNSvc::minor_value_type> selectionNameToIntMap;  
  //    std::vector<IANNSvc::minor_value_type> hlt = m_hltANNSvc->items("SelectionID"); // old style
  //    selectionNameToIntMap.insert( selectionNameToIntMap.end(),hlt.begin(),hlt.end() );
  std::vector<IANNSvc::minor_value_type> hlt1 = m_hltANNSvc->items("Hlt1SelectionID"); // new style
  selectionNameToIntMap.insert( selectionNameToIntMap.end(),hlt1.begin(),hlt1.end() );
  std::vector<IANNSvc::minor_value_type> hlt2 = m_hltANNSvc->items("Hlt2SelectionID");
  selectionNameToIntMap.insert( selectionNameToIntMap.end(),hlt2.begin(),hlt2.end() );


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
          if( stdInfo.size()==6 ){

            infoPersistent.insert( "0#Track.firstState.z", floatFromInt(stdInfo[0]) ); 
            infoPersistent.insert( "1#Track.firstState.x", floatFromInt(stdInfo[1]) ); 
            infoPersistent.insert( "2#Track.firstState.y", floatFromInt(stdInfo[2]) ); 
            infoPersistent.insert( "3#Track.firstState.tx", floatFromInt(stdInfo[3]) ); 
            infoPersistent.insert( "4#Track.firstState.ty", floatFromInt(stdInfo[4]) ); 
            infoPersistent.insert( "5#Track.firstState.qOverP", floatFromInt(stdInfo[5]) ); 
	    
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
          if( stdInfo.size()==3 ){    
 
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
          if( stdInfo.size()==8 ){    
            
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
          
          if( stdInfo.size()==4 ){    
 
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

          if( stdInfo.size()!=1 ){    
            std::ostringstream mess;
            mess << " wrong number of StdInfo on Trigger Selection " << stdInfo.size();
            Error( mess.str(),  StatusCode::SUCCESS, 20 );
            for(HltSelRepRBStdInfo::StdInfo::const_iterator i=(stdInfo.begin()+1);
                i!=stdInfo.end(); ++i){
              std::stringstream ss;
              ss << "z#unknown" << int(i-stdInfo.begin() );
              infoPersistent.insert( ss.str(), floatFromInt(*i) );        
            }      
          } 
          infoPersistent.insert( "0#SelectionID", floatFromInt(stdInfo[0]) );
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
        bool found=false;
        for( std::vector<IANNSvc::minor_value_type>::const_iterator j= hltinfos.begin();j!=hltinfos.end();++j){
          if( j->second == i->first  ){
            infoPersistent.insert( j->first, i->second );
            found=true;
            break;
          }
        }
        if( !found ){
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
          hits.insert( hits.end(), hitseq.begin(), hitseq.end() );
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

    std::string selName="Dummy";
    for( HltObjectSummary::Info::const_iterator i=hos->numericalInfo().begin();
         i!=hos->numericalInfo().end();++i ){
      if( i->first == "0#SelectionID" ){
        int id = (int)(i->second+0.1);

        for( std::vector<IANNSvc::minor_value_type>::const_iterator si=selectionNameToIntMap.begin();
             si!=selectionNameToIntMap.end();++si){
          if( si->second == id ){
            selName = si->first;
            break;
          }
        }
        break;
      }
    }
    if( selName != "Dummy" ){
      
      // clone hos
      HltObjectSummary selSumOut;
      selSumOut.setSummarizedObjectCLID( hos->summarizedObjectCLID() );
      selSumOut.setNumericalInfo( hos->numericalInfo() );
      selSumOut.setSubstructure( hos->substructure() ); 

      // insert selection into the container
      if( outputSummary->insert(selName,selSumOut) == StatusCode::FAILURE ){
          Error( "  Failed to add Hlt selection name " + selName
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
    for( HltObjectSummary::Container::const_iterator ppHos=objectSummaries->begin();
         ppHos!=objectSummaries->end();++ppHos){
      const HltObjectSummary* pHos=*ppHos;    
      verbose() << " key " << pHos->index() << *pHos << endmsg;    
    }
   
  }

 return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltSelReportsReader::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
  //  return HltBaseAlg::finalize();  // must be called after all other actions
}

//=============================================================================
    
  
float HltSelReportsReader::floatFromInt(unsigned int i)
{
        union IntFloat { unsigned int mInt; float mFloat; };
        IntFloat a; a.mInt=i;
        return a.mFloat;
}

