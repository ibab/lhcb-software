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
: HltRawBankDecoderBase( name, pSvcLocator )
{
  declareProperty("OutputHltSelReportsLocation",
    m_outputHltSelReportsLocation= LHCb::HltSelReportsLocation::Default);  
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltSelReportsDecoder::execute() {
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // ----------------------------------------------------------
  // get the bank(s) from RawEvent
  // ----------------------------------------------------------
  std::vector<const RawBank*> hltselreportsRawBanks = selectRawBanks(RawBank::HltSelReports );
  if( hltselreportsRawBanks.empty() ){
    return Warning( " No appropriate HltSelReports RawBank in RawEvent. Quiting. ",StatusCode::SUCCESS, 10 );
  }

  const RawBank* hltselreportsRawBank0 = *(hltselreportsRawBanks.begin());
  if( hltselreportsRawBank0->version() > kVersionNumber ){
    Warning( " HltSelReports RawBank version is higher than expected. Will try to decode it anyway." ,StatusCode::SUCCESS, 20 );
  }
  // put the banks into the right order (in case the data was split across multiple banks...
  std::sort( std::begin(hltselreportsRawBanks), std::end(hltselreportsRawBanks), 
             [](const RawBank* lhs, const RawBank* rhs) { 
                    auto l = lhs->sourceID() & HltSelReportsWriter::kSourceID_MinorMask;
                    auto r = rhs->sourceID() & HltSelReportsWriter::kSourceID_MinorMask;
                    return l < r; 
  } );
  // verify no duplicates...
  auto adj = std::adjacent_find( std::begin(hltselreportsRawBanks), std::end(hltselreportsRawBanks), 
             [](const RawBank* lhs, const RawBank* rhs) { 
                    auto l = lhs->sourceID() & HltSelReportsWriter::kSourceID_MinorMask;
                    auto r = rhs->sourceID() & HltSelReportsWriter::kSourceID_MinorMask;
                    return l == r; 
  } );
  if ( adj != std::end(hltselreportsRawBanks) ) {
      return Error( " Duplicate sequential Source ID HltSelReports. Aborting decoder ", StatusCode::SUCCESS, 20 );
  }

  unsigned int nLastOne =  hltselreportsRawBanks.back()->sourceID() & HltSelReportsWriter::kSourceID_MinorMask;
  if ( nLastOne+1 != hltselreportsRawBanks.size() ) {
      return Error( " Did not find the expected number of HltSelReports raw banks. Aborting decoder ", StatusCode::SUCCESS, 20 );
  }

  unsigned int bankSize = std::accumulate( std::begin(hltselreportsRawBanks), std::end(hltselreportsRawBanks), 
                                            0, [](unsigned int s, const RawBank* bank) { 
                                            return s+std::distance( bank->begin<unsigned int>(), bank->end<unsigned int>());  }
  );

  if( !bankSize ){
    return Warning( " No HltSelReports RawBank for requested SourceID in RawEvent. Quiting. ",StatusCode::SUCCESS, 10 );
  }    

  // need to copy it to local array to concatenate  --- TODO: we could run a decompression such as LZMA at this point as well...
  unsigned int* pBank = new unsigned int[bankSize];
  HltSelRepRawBank hltSelReportsBank( pBank );
  std::accumulate( std::begin(hltselreportsRawBanks), std::end(hltselreportsRawBanks),
                   pBank, [](unsigned int *p, const LHCb::RawBank* bank) {
                   return std::copy( bank->begin<unsigned int>(), bank->end<unsigned int>(), p);
  } );

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
  // create output container and put it on TES
  // TODO: check consistency of output location and source ID!!!!
  HltSelReports* outputSummary = new HltSelReports();
  put( outputSummary, m_outputHltSelReportsLocation );

  // create output container for Object Summaries and put it on TES
  LHCb::HltObjectSummary::Container* objectSummaries = new HltObjectSummary::Container();
  put( objectSummaries, m_outputHltSelReportsLocation.value() + "/Candidates" );

  auto mytck = tck();
  const auto& idmap = id2string(mytck);
  const auto& infomap = info2string(mytck);

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
            if (stdInfo.size()>=4) {
                infoPersistent.insert( "3#RecVertex.chi2", floatFromInt(stdInfo[3]) ); 
            }
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
            auto iselName = idmap.find(id);
            if (iselName == std::end(idmap)) {
              std::ostringstream mess;
              mess << " Did not find string key for PV-selection-ID in trigger selection in storage id=" << id;
              Error( mess.str(),  StatusCode::SUCCESS, 10 ); 
              infoPersistent.insert( "10#Unknown" , floatFromInt(id) );        
            } else 
              infoPersistent.insert( "10#" + std::string(iselName->second), floatFromInt(stdInfo[1]) );        
            }
            for( unsigned int ipvkeys=2; ipvkeys< stdInfo.size(); ++ipvkeys ){
              std::stringstream ss;
              ss << "11#" << boost::format("%1$=08X") % (ipvkeys-2) ;
              infoPersistent.insert( ss.str(), floatFromInt( stdInfo[ipvkeys] ) );        
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
      for(  const auto &i : extraInfo ) {
        // convert int to string
        auto infos = infomap.find( i.first );
        if ( infos!=std::end(infomap) ) {
          infoPersistent.insert( infos->second, i.second );
        } else {
          std::ostringstream mess;
          mess << " String key for Extra Info item in storage not found id=" << i.first;
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

    auto selName = std::end(idmap);
    auto i = std::find_if( std::begin(hos->numericalInfo()) , std::end(hos->numericalInfo()), 
                           [](const std::pair<std::string,double>& info) {
                               return info.first == "0#SelectionID";
                           } );
    if ( i != std::end( hos->numericalInfo() ) ) {
        int id = (int)(i->second+0.1);
        selName = idmap.find( id );
    }
    if( selName != std::end(idmap) ){

      // skip reports of the wrong type
      if (!selName->second)  continue;

      // clone hos
      HltObjectSummary selSumOut;
      selSumOut.setSummarizedObjectCLID( hos->summarizedObjectCLID() );
      selSumOut.setNumericalInfo( hos->numericalInfo() );
      selSumOut.setSubstructure( hos->substructure() ); 

      // insert selection into the container
      if( outputSummary->insert(selName->second,selSumOut) == StatusCode::FAILURE ){
        Error( "  Failed to add Hlt selection name " 
               + std::string(selName->second)
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
