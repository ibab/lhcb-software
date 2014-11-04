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
    Error( std::string{ " HltSelReportsRawBank internally reported size " } 
         + std::to_string( hltSelReportsBank.size() )
         + " less than bank size delivered by RawEvent " 
         + std::to_string(bankSize),
         StatusCode::SUCCESS, 100 );
    errors=true;

  } else {

  ic = hltSelReportsBank.integrityCode();
  if( ic ){
    Error( std::string{ " HltSelReportsRawBank fails integrity check with code "}
         + std::to_string(ic) +  " " + HltSelRepRBEnums::IntegrityCodesToString(ic),
           StatusCode::SUCCESS, 100 );
    errors=true;
  }

  ic = hitsSubBank.integrityCode();
  if( ic ){
    Error( std::string{" HltSelRepRBHits fails integrity check with code " }
         + std::to_string(ic) + " " + HltSelRepRBEnums::IntegrityCodesToString(ic),
           StatusCode::SUCCESS, 100 );
    errors=true;
  }

  ic = objTypSubBank.integrityCode();
  if( ic ){
    Error( std::string{ " HltSelRepRBObjTyp fails integrity check with code " }
         + std::to_string(ic) + " " + HltSelRepRBEnums::IntegrityCodesToString(ic),
           StatusCode::SUCCESS, 100 );
    errors=true;
  }

  ic = substrSubBank.integrityCode();
  if( ic ){
    Error( std::string{ " HltSelRepRBSubstr fails integrity check with code " }
         + std::to_string(ic) +  " " + HltSelRepRBEnums::IntegrityCodesToString(ic),
           StatusCode::SUCCESS, 100 );
    errors=true;
  }
  if( nObj != substrSubBank.numberOfObj() ){
    Error( std::string{ " HltSelRepRBSubstr has number of objects "  }
         + std::to_string(substrSubBank.numberOfObj())
         + " which is different than HltSelRepRBObjTyp " 
         + std::to_string(nObj),
           StatusCode::SUCCESS, 100 );
    errors=true;
  }

  ic = stdInfoSubBank.integrityCode();
  if( ic ){
    Error( std::string{ " HltSelRepRBStdInfo fails integrity check with code " }
         + std::to_string(ic ) +  " " + HltSelRepRBEnums::IntegrityCodesToString(ic),
           StatusCode::SUCCESS, 100 );
    errors=true;
  }
  if( nObj != stdInfoSubBank.numberOfObj() ){
    Error( std::string{ " HltSelRepRBStdInfo has number of objects " }
         + std::to_string( stdInfoSubBank.numberOfObj() )
         + " which is different than HltSelRepRBObjTyp " 
         + std::to_string( nObj ),
           StatusCode::SUCCESS, 100 );
    errors=true;
  }

  ic = extraInfoSubBank.integrityCode();
  if( ic ){
    Error( std::string{ " HltSelRepRBExtraInfo fails integrity check with code " }
         + std::to_string(ic) + " " + HltSelRepRBEnums::IntegrityCodesToString(ic),
           StatusCode::SUCCESS, 100 );
    exInfOn=false; // the only non-fatal info corruption
  }
  if( nObj != extraInfoSubBank.numberOfObj() ){
    Error( std::string{  " HltSelRepRBExtraInfo has number of objects " }
         + std::to_string( extraInfoSubBank.numberOfObj() )
         + " which is different than HltSelRepRBObjTyp " 
         + std::to_string( nObj ),
           StatusCode::SUCCESS, 100 );
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
         if( (stdInfo.size()>=6) && (stdInfo.size()<9) ){

            infoPersistent.insert( "0#Track.firstState.z", floatFromInt(stdInfo[0]) );
            infoPersistent.insert( "1#Track.firstState.x", floatFromInt(stdInfo[1]) );
            infoPersistent.insert( "2#Track.firstState.y", floatFromInt(stdInfo[2]) );
            infoPersistent.insert( "3#Track.firstState.tx", floatFromInt(stdInfo[3]) );
            infoPersistent.insert( "4#Track.firstState.ty", floatFromInt(stdInfo[4]) );
            infoPersistent.insert( "5#Track.firstState.qOverP", floatFromInt(stdInfo[5]) ); 
            if( stdInfo.size()==8 ){
              infoPersistent.insert( "6#Track.chi2PerDoF", floatFromInt( stdInfo[6] ) );
              infoPersistent.insert( "7#Track.nDoF", floatFromInt( stdInfo[7] ) );
            }
         }
         else if (stdInfo.size()>8) {
              infoPersistent.insert( "0#Track.firstState.z", floatFromInt(stdInfo[0]) );
              infoPersistent.insert( "1#Track.firstState.x", floatFromInt(stdInfo[1]) );
              infoPersistent.insert( "2#Track.firstState.y", floatFromInt(stdInfo[9]) );
              infoPersistent.insert( "3#Track.firstState.tx", floatFromInt(stdInfo[10]) );
              infoPersistent.insert( "4#Track.firstState.ty", floatFromInt(stdInfo[11]) );
              infoPersistent.insert( "5#Track.firstState.qOverP", floatFromInt(stdInfo[12]) );  
              infoPersistent.insert( "6#Track.chi2PerDoF", floatFromInt( stdInfo[13] ) );
              infoPersistent.insert( "7#Track.nDoF", floatFromInt( stdInfo[14] ) );
              infoPersistent.insert( "8#Track.Likelihood", floatFromInt(stdInfo[15] ) );
              infoPersistent.insert( "9#Track.GhostProb", floatFromInt(stdInfo[16] ) );
              infoPersistent.insert( "10#Track.flags", floatFromInt(stdInfo[2] ) );
              infoPersistent.insert( "11#Track.lastState.z", floatFromInt(stdInfo[3] ) );
              infoPersistent.insert( "12#Track.lastState.x", floatFromInt(stdInfo[4] ) );
              infoPersistent.insert( "13#Track.lastState.y", floatFromInt(stdInfo[5] ) );
              infoPersistent.insert( "14#Track.lastState.tx", floatFromInt(stdInfo[6] ) );
              infoPersistent.insert( "15#Track.lastState.ty", floatFromInt(stdInfo[7] ) );
              infoPersistent.insert( "16#Track.lastState.qOverP", floatFromInt(stdInfo[8] ) );
         } 
         else {
            Warning( std::string{ " wrong number of StdInfo on Track "} + std::to_string( stdInfo.size() ),
                    StatusCode::SUCCESS, 20 );
            int e = 0;
            for(const auto& i : stdInfo ) { 
              infoPersistent.insert( std::string{ "z#Track.unknown" } + std::to_string(e++),
                                     floatFromInt(i) );        
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
           } 
          else {
            Warning( std::string{  " wrong number of StdInfo on RecVertex " } + std::to_string( stdInfo.size() ),
                    StatusCode::SUCCESS, 20 );
            int e = 0;
            for(const auto& i : stdInfo ) {
              infoPersistent.insert( std::string{ "z#RecVertex.unknown" } + std::to_string( e++ ),
                                     floatFromInt(i) );        
            }
          } 
        }
	break;
      case LHCb::CLID_Vertex:
        {  
          if( stdInfo.size()>=12 ){    
 
            infoPersistent.insert( "0#Vertex.chi2", floatFromInt(stdInfo[0]) ); 
            infoPersistent.insert( "1#Vertex.ndf", floatFromInt(stdInfo[1]) ); 
            infoPersistent.insert( "2#Vertex.position.x", floatFromInt(stdInfo[4]) ); 
            infoPersistent.insert( "3#Vertex.position.y", floatFromInt(stdInfo[5]) ); 
            infoPersistent.insert( "4#Vertex.position.z", floatFromInt(stdInfo[6]) ); 
            infoPersistent.insert( "5#Vertex.technique", floatFromInt(stdInfo[7]) ); 
            infoPersistent.insert( "6#Vertex.cov00", floatFromInt(stdInfo[8]) ); 
            infoPersistent.insert( "7#Vertex.cov11", floatFromInt(stdInfo[9]) ); 
            infoPersistent.insert( "8#Vertex.cov22", floatFromInt(stdInfo[10]) ); 
            infoPersistent.insert( "9#Vertex.cov10", floatFromInt(stdInfo[11]) ); 
            infoPersistent.insert( "10#Vertex.cov20", floatFromInt(stdInfo[2]) ); 
            infoPersistent.insert( "11#Vertex.cov21", floatFromInt(stdInfo[3]) ); 

          } else {
            
            Warning( std::string{ " wrong number of StdInfo on Vertex " } + std::to_string(stdInfo.size()),
                    StatusCode::SUCCESS, 20 );
            int e = 0;
            for(const auto& i : stdInfo ) { 
              infoPersistent.insert( std::string{ "z#Vertex.unknown" } + std::to_string(e++),
                                     floatFromInt(i) );        
            }
          }
        }
	break;
      case LHCb::CLID_RichPID:
        {  
          if( stdInfo.size()>=6 ){    
 
            infoPersistent.insert( "0#Rich.pidResultCode", floatFromInt(stdInfo[0] ) );
            infoPersistent.insert( "1#Rich.DLLe", floatFromInt(stdInfo[1] ) );
            infoPersistent.insert( "2#Rich.DLLmu", floatFromInt(stdInfo[2] ) );
            infoPersistent.insert( "3#Rich.DLLpi", floatFromInt(stdInfo[3] ) );
            infoPersistent.insert( "4#Rich.DLLK", floatFromInt(stdInfo[4] ) );
            infoPersistent.insert( "5#Rich.DLLp", floatFromInt(stdInfo[5] ) );

          } else {
            
            Warning( std::string{ " wrong number of StdInfo on RichPID " } + std::to_string(stdInfo.size()),
                    StatusCode::SUCCESS, 20 );
            int e = 0;
            for(const auto& i : stdInfo ) { 
              infoPersistent.insert( std::string{ "z#Rich.unknown" } + std::to_string(e++),
                                     floatFromInt(i) );        
            }
            
          }
        }
	break;
      case LHCb::CLID_MuonPID:
        {  
          if( stdInfo.size()>=7 ){    
 
            infoPersistent.insert( "0#Muon.MuonLLMu", floatFromInt(stdInfo[0] ) );
            infoPersistent.insert( "1#Muon.MuonLLBg", floatFromInt(stdInfo[1] ) );
            infoPersistent.insert( "2#Muon.NShared", floatFromInt(stdInfo[2] ) );
            infoPersistent.insert( "3#Muon.Status", floatFromInt(stdInfo[3] ) );
            infoPersistent.insert( "4#Muon.IsMuon", floatFromInt(stdInfo[4] ) );
            infoPersistent.insert( "5#Muon.IsMuonLoose", floatFromInt(stdInfo[5] ) );
            infoPersistent.insert( "6#Muon.IsMuonTight", floatFromInt(stdInfo[6] ) );

          } else {
            
            Warning( std::string{ " wrong number of StdInfo on MuonPID " } + std::to_string(stdInfo.size()),
                    StatusCode::SUCCESS, 20 );
            int e = 0;
            for(const auto& i : stdInfo ) { 
              infoPersistent.insert( std::string{ "z#Muon.unknown" } + std::to_string(e++),
                                     floatFromInt(i) );        
            }
            
          }
        }
	break;
      case LHCb::CLID_ProtoParticle:
        {  
          if( stdInfo.size()>=1 ){    
 
            infoPersistent.insert( "0#Proto.extraInfo.IsPhoton", floatFromInt(stdInfo[0]) ); 

          } else {
            
            Warning( std::string{ " wrong number of StdInfo on ProtoParticle " } + std::to_string(stdInfo.size()),
                    StatusCode::SUCCESS, 20 );
            int e = 0;
            for(const auto& i : stdInfo ) { 
              infoPersistent.insert( std::string{ "z#Proto.unknown" } + std::to_string(e++),
                                     floatFromInt(i) );        
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
          }
          else if( stdInfo.size()>8 ){
              infoPersistent.insert( "0#Particle.particleID.pid", floatFromInt(stdInfo[0]) ); 
              infoPersistent.insert( "1#Particle.measuredMass", floatFromInt(stdInfo[1]) ); 
              infoPersistent.insert( "2#Particle.referencePoint.z", floatFromInt(stdInfo[12]) ); 
              infoPersistent.insert( "3#Particle.referencePoint.x", floatFromInt(stdInfo[23]) ); 
              infoPersistent.insert( "4#Particle.referencePoint.y", floatFromInt(stdInfo[32]) );
              infoPersistent.insert( "5#Particle.slopes.x", floatFromInt(stdInfo[33]) );
              infoPersistent.insert( "6#Particle.slopes.y", floatFromInt(stdInfo[34]) );
              infoPersistent.insert( "7#Particle.1/p", floatFromInt(stdInfo[35]) );
              infoPersistent.insert( "8#Particle.conflevel", floatFromInt(stdInfo[36]) );
              infoPersistent.insert( "9#Particle.massErr", floatFromInt(stdInfo[37] ) );
              infoPersistent.insert( "10#Particle.momCov00", floatFromInt(stdInfo[2]) );
              infoPersistent.insert( "11#Particle.momCov11", floatFromInt(stdInfo[3] ) );
              infoPersistent.insert( "12#Particle.momCov22", floatFromInt(stdInfo[4] ) );
              infoPersistent.insert( "13#Particle.momCov33", floatFromInt(stdInfo[5] ) );
              infoPersistent.insert( "14#Particle.momCov10", floatFromInt(stdInfo[6] ) );
              infoPersistent.insert( "15#Particle.momCov20", floatFromInt(stdInfo[7] ) );
              infoPersistent.insert( "16#Particle.momCov21", floatFromInt(stdInfo[8] ) );
              infoPersistent.insert( "17#Particle.momCov30", floatFromInt(stdInfo[9] ) );
              infoPersistent.insert( "18#Particle.momCov31", floatFromInt(stdInfo[10] ) );
              infoPersistent.insert( "19#Particle.momCov32", floatFromInt(stdInfo[11] ) );
              infoPersistent.insert( "20#Particle.posmomCov00", floatFromInt(stdInfo[13] ) );
              infoPersistent.insert( "21#Particle.posmomCov11", floatFromInt(stdInfo[14] ) );
              infoPersistent.insert( "22#Particle.posmomCov22", floatFromInt(stdInfo[15] ) );
              infoPersistent.insert( "23#Particle.posmomCov10", floatFromInt(stdInfo[16] ) );
              infoPersistent.insert( "24#Particle.posmomCov01", floatFromInt(stdInfo[17] ) );
              infoPersistent.insert( "25#Particle.posmomCov20", floatFromInt(stdInfo[18] ) );
              infoPersistent.insert( "26#Particle.posmomCov02", floatFromInt(stdInfo[19] ) );
              infoPersistent.insert( "27#Particle.posmomCov21", floatFromInt(stdInfo[20] ) );
              infoPersistent.insert( "28#Particle.posmomCov12", floatFromInt(stdInfo[21] ) );
              infoPersistent.insert( "29#Particle.posmomCov30", floatFromInt(stdInfo[22] ) );
              infoPersistent.insert( "30#Particle.posmomCov31", floatFromInt(stdInfo[24] ) );
              infoPersistent.insert( "31#Particle.posmomCov32", floatFromInt(stdInfo[25] ) );
              infoPersistent.insert( "32#Particle.posCov00", floatFromInt(stdInfo[26] ) );
              infoPersistent.insert( "33#Particle.posCov11", floatFromInt(stdInfo[27] ) );
              infoPersistent.insert( "34#Particle.posCov22", floatFromInt(stdInfo[28] ) );
              infoPersistent.insert( "35#Particle.posCov10", floatFromInt(stdInfo[29] ) );
              infoPersistent.insert( "36#Particle.posCov20", floatFromInt(stdInfo[30] ) );
              infoPersistent.insert( "37#Particle.posCov21", floatFromInt(stdInfo[31] ) );
            } 
          else {
            Warning( std::string{ " wrong number of StdInfo on Particle " } + std::to_string(stdInfo.size()),
                     StatusCode::SUCCESS, 20 );
            int e = 0;
            for( const auto& i : stdInfo ) {
              infoPersistent.insert( std::string{ "z#Particle.unknown" } + std::to_string(e++),
                                     floatFromInt(i) );
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
          } 
          else {
            Warning( std::string{ " wrong number of StdInfo on CaloCluster " } + std::to_string(stdInfo.size()),
                    StatusCode::SUCCESS, 20 );
            int e = 0;
            for(const auto& i : stdInfo ) { 
              infoPersistent.insert( std::string{ "z#CaloCluster.unknown" } + std::to_string(e++),
                                     floatFromInt(i) );        
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
              Error( std::string{  " Did not find string key for PV-selection-ID in trigger selection in storage id=" } + std::to_string(id),
                     StatusCode::SUCCESS, 10 ); 
              infoPersistent.insert( "10#Unknown" , floatFromInt(id) );        
            } else 
              infoPersistent.insert( "10#" + iselName->second.str(), floatFromInt(stdInfo[1]) );
            }
            for( unsigned int ipvkeys=2; ipvkeys< stdInfo.size(); ++ipvkeys ){
              infoPersistent.insert( std::string{ "11#" }+ boost::str(boost::format("%1$=08X") % (ipvkeys-2)),
                                     floatFromInt( stdInfo[ipvkeys] ) );        
            }

        }
        break;
      default:
        { 

          Warning( std::string{ " StdInfo on unsupported class type "}+ std::to_string(hos->summarizedObjectCLID()),
                   StatusCode::SUCCESS, 20 );
          int e = 0;
          for (const auto& i : stdInfo) { 
            infoPersistent.insert( std::string { "z#Unknown.unknown" } + std::to_string( e++ ),
                                   floatFromInt(i) );        
          }      
        }    
      }

    //           ============== extra 

    if( exInfOn ){
      HltSelRepRBExtraInfo::ExtraInfo extraInfo = extraInfoSubBank.next();
      for(  const auto &i : extraInfo ) {
        auto infos = infomap.find( i.first );
        if ( infos!=std::end(infomap) ) {
          infoPersistent.insert( infos->second, i.second );
        } else {
          Warning( std::string{ " String key for Extra Info item in storage not found id=" } + std::to_string(i.first),
                   StatusCode::SUCCESS, 20 );
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
      for( const auto& iSeq : sub.second ) {
        if( iSeq<nSeq ){
          std::vector< LHCb::LHCbID > hitseq = hitsSubBank.sequence( iSeq );
          //   for bank version zero, first hit in the first sequence was corrupted ------
          //                   for odd number of sequences saved - omit this hit
          if( iSeq==0 && hltselreportsRawBank0->version()==0 && nSeq%2 == 1 ){
                hitseq.erase( hitseq.begin() );
          }
          // ------------------------- end fix --------------------------------------------
          if( hitseq.size() ){            
            hits.insert( std::end(hits), std::begin(hitseq), std::end(hitseq) );
          }          
        } else {
          Error(  "Hit sequence index out of range", StatusCode::SUCCESS, 10 );
        }
      }
      hos->setLhcbIDs( hits );

    } else {
      // pointers
      SmartRefVector<LHCb::HltObjectSummary> thisSubstructure; 
      for( const auto& jObj : sub.second ) { 
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
      Error( " Did not find string key for trigger selection in storage",
             StatusCode::SUCCESS, 50 ); 
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
