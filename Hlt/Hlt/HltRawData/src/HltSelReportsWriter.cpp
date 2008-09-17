// $Id: HltSelReportsWriter.cpp,v 1.2 2008-09-17 16:14:56 tskwarni Exp $
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
#include "HltSelReportsWriter.h"


using namespace LHCb;


//-----------------------------------------------------------------------------
// Implementation file for class : HltSelReportsWriter
//
// 2008-07-25 : Tomasz Skwarnicki
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltSelReportsWriter );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltSelReportsWriter::HltSelReportsWriter( const std::string& name,
                                          ISvcLocator* pSvcLocator)
  : HltBaseAlg( name , pSvcLocator )
  //  : GaudiAlgorithm ( name , pSvcLocator )
{

  declareProperty("InputHltSelReportsLocation",
    m_inputHltSelReportsLocation= LHCb::HltSelReportsLocation::Default);  
  declareProperty("OutputRawEventLocation",
    m_outputRawEventLocation= LHCb::RawEventLocation::Default);

    m_hltANNSvc = 0;

}
//=============================================================================
// Destructor
//=============================================================================
HltSelReportsWriter::~HltSelReportsWriter() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltSelReportsWriter::initialize() {
  //  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  StatusCode sc = HltBaseAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_hltANNSvc = svc<IANNSvc>("HltANNSvc");

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltSelReportsWriter::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;


  // get inputs

  // ------ actually  don't really need this container since its content is duplicated in object store 
  //if( !exist<HltSelReports>(m_inputHltSelReportsLocation) ){    
  //  warning() << " No HltSelReports at " << m_inputHltSelReportsLocation << endmsg;
  //  return StatusCode::SUCCESS;  
  //}  
  //const HltSelReports* inputSummary = get<HltSelReports>(m_inputHltSelReportsLocation);

  const std::string objectsLocation = m_inputHltSelReportsLocation.value() + "/Candidates" ;  
  if( !exist<HltObjectSummary::Container>( objectsLocation ) )
  {
    return Warning( " No HltSelReports objects at " + objectsLocation, StatusCode::SUCCESS, 20 );
  }
  const HltObjectSummary::Container* objectSummaries = get<HltObjectSummary::Container>( objectsLocation );

  // get output
  if( !exist<RawEvent>(m_outputRawEventLocation) ){    
    return Error(" No RawEvent at " + m_outputRawEventLocation.value(), StatusCode::SUCCESS, 20 );
  }  
  RawEvent* rawEvent = get<RawEvent>(m_outputRawEventLocation);


  // --------------------------------------------------------------------------------------
  // ------------ create hit sequence bank -------------------------------------------------
  // --------------------------------------------------------------------------------------

  // lhcbid sequences - recognize common hit sequences
  LhcbidSequences lhcbidSequences;  
  for( HltObjectSummary::Container::const_iterator iObj=objectSummaries->begin();
       iObj!=objectSummaries->end(); ++iObj ){
    const HltObjectSummary & hos = **iObj;
    if( !(hos.lhcbIDs().size()) )continue;
    LhcbidSequence* thisIDset=new LhcbidSequence();    
    for( std::vector<LHCbID>::const_iterator iHit=hos.lhcbIDs().begin();
         iHit!=hos.lhcbIDs().end(); ++iHit){
      thisIDset->insert( iHit->lhcbID() );
    }
    addToLhcbidSequences( thisIDset, lhcbidSequences );
  }


  unsigned int nHits(0);
  for( LhcbidSequences::const_iterator iSeq=lhcbidSequences.begin();
       iSeq != lhcbidSequences.end(); ++iSeq ){
    nHits += (*iSeq)->size();
  }

  HltSelRepRBHits hitsSubBank;
  hitsSubBank.initialize( lhcbidSequences.size(),nHits );
  for( LhcbidSequences::const_iterator iSeq=lhcbidSequences.begin();
       iSeq != lhcbidSequences.end(); ++iSeq ){
    const LhcbidSequence & hitset = **iSeq;
    std::vector<unsigned int> hitvec( hitset.begin(), hitset.end() );    
    hitsSubBank.push_back( hitvec );
  }

  // --------------------------------------------------------------------------------------
  //  ---------------- in storage banks are ordered by summarizedClassCLID ----------------
  // -------------------------------- sort them -------------------------------------------

  std::vector<const HltObjectSummary*> sortedHosPtrs( objectSummaries->begin(),objectSummaries->end() );

  std::sort( sortedHosPtrs.begin(), sortedHosPtrs.end(), sortByCLID() );

  // inverse mapping
  std::vector< unsigned int > fromIndexToNewIndex(sortedHosPtrs.size());
  for( std::vector<const HltObjectSummary*>::const_iterator  ppHos= sortedHosPtrs.begin();
       ppHos != sortedHosPtrs.end(); ++ppHos ){
    fromIndexToNewIndex[ (*ppHos)->index() ] = int(ppHos - sortedHosPtrs.begin());
  }

  // --------------------------------------------------------------------------------------
  //   create substructure bank -------------------------------------------------
  //   and object type bank
  //   and extra  info bank
  //   and std    info bank
  // --------------------------------------------------------------------------------------

  HltSelRepRBSubstr substrSubBank;
  HltSelRepRBObjTyp objTypSubBank;
  HltSelRepRBExtraInfo extraInfoSubBank;
  HltSelRepRBStdInfo stdInfoSubBank;

  std::vector<IANNSvc::minor_value_type> hltinfos = m_hltANNSvc->items("InfoID"); 

  objTypSubBank.initialize();
  substrSubBank.initialize();  
  // some banks require initialization with size info 
  unsigned int nExtraInfo=0;  
  unsigned int nStdInfo=0;  
  for( std::vector<const HltObjectSummary*>::const_iterator  iObj= sortedHosPtrs.begin();
       iObj != sortedHosPtrs.end(); ++iObj ){
    const HltObjectSummary & hos = **iObj;
    HltSelRepRBExtraInfo::ExtraInfo extraInfo;
    for( HltObjectSummary::Info::const_iterator i=hos.numericalInfo().begin();
         i!=hos.numericalInfo().end();++i){
      if( i->first.find("#")!=std::string::npos ){
        ++nStdInfo;        
      } else {
        ++nExtraInfo;        
      }
    }
  }  
  extraInfoSubBank.initialize( sortedHosPtrs.size(), nExtraInfo );
  stdInfoSubBank.initialize( sortedHosPtrs.size(), nStdInfo );
  

  // associate objects with hit sequences, or do simple substructure
  // do info banks, and object type bank
  for( std::vector<const HltObjectSummary*>::const_iterator  iObj= sortedHosPtrs.begin();
       iObj != sortedHosPtrs.end(); ++iObj ){
    const HltObjectSummary & hos = **iObj;
    
    objTypSubBank.push_back( hos.summarizedObjectCLID() );

    HltSelRepRBExtraInfo::ExtraInfo extraInfo;
    HltSelRepRBStdInfo::StdInfo stdInfo;

    for( HltObjectSummary::Info::const_iterator i=hos.numericalInfo().begin();
         i!=hos.numericalInfo().end();++i){

      if( i->first.find("#")!=std::string::npos ){

        // push floats as ints (allows for possible compression in future versions)
        union IntFloat { unsigned int mInt; float mFloat; };
        IntFloat a; a.mFloat = i->second;
        unsigned int intFloat = a.mInt;
        stdInfo.push_back( intFloat );

      } else {

        bool found=false;        
        // convert string-id to a short
        for( std::vector<IANNSvc::minor_value_type>::const_iterator j= hltinfos.begin();j!=hltinfos.end();++j){
          if( j->first == i->first  ){
            HltSelRepRBExtraInfo::Inf a( j->second, i->second );            
            extraInfo.push_back( a  );
            found=true;          
            break;          
          }
        }
        // this is very unexpected but shouldn't be fatal
        if( !found ) {
          std::ostringstream mess;
          mess << "Int key for string info key=" << i->first << " not found ";
          Error( mess.str(), StatusCode::SUCCESS, 50 );
        }

      
      }
    }
    stdInfoSubBank.push_back( stdInfo );
    extraInfoSubBank.push_back( extraInfo );
    

    // substructure or hit sequence link vector
    HltSelRepRBSubstr::Substrv svect;
    unsigned int sHitType=0;    

    if( hos.substructure().size() ){
      // substructure 
      for( SmartRefVector<LHCb::HltObjectSummary>::const_iterator iS=hos.substructure().begin();
           iS!=hos.substructure().end(); ++iS ){
        svect.push_back( fromIndexToNewIndex[ (*iS)->index() ] );
      }
    } else if( hos.lhcbIDs().size() ) {
      // hits
      sHitType=1;
      
      LhcbidSequence* thisIDset=new LhcbidSequence();    
      for( std::vector<LHCbID>::const_iterator iHit=hos.lhcbIDs().begin();
           iHit!=hos.lhcbIDs().end(); ++iHit){
        thisIDset->insert( iHit->lhcbID() );
      }
    
      unsigned int iSeqID(0);  
      for( LhcbidSequences::const_iterator iSeq=lhcbidSequences.begin();
           iSeq != lhcbidSequences.end(); ++iSeq, ++iSeqID ){
        const LhcbidSequence & hitset = **iSeq;
        if( thisIDset->find( *(hitset.begin()) ) != thisIDset->end() ){
          svect.push_back( iSeqID );          
        }
      }
    }

    HltSelRepRBSubstr::Substr aS( sHitType, svect );
    substrSubBank.push_back( aS );

  }
  
  // don't forget to clean lhcbidSequences; they own sets they are pointing to
  for( LhcbidSequences::const_iterator iSeq=lhcbidSequences.begin();
       iSeq != lhcbidSequences.end(); ++iSeq ){
    delete *iSeq;
  }

  // ----------- create HltSelReports bank body and insert subbanks to it
  HltSelRepRawBank hltSelReportsBank;

  // order of sub-banks in the main bank is arbitrary

  //    lhcbids      
  hltSelReportsBank.push_back( HltSelRepRBEnums::kHitsID, hitsSubBank.location(), hitsSubBank.size() );
  hitsSubBank.deleteBank();    

  //    object types
  objTypSubBank.saveSize();  
  hltSelReportsBank.push_back( HltSelRepRBEnums::kObjTypID, objTypSubBank.location(), objTypSubBank.size() );
  objTypSubBank.deleteBank();

  // their substructure
  substrSubBank.saveSize();  
  hltSelReportsBank.push_back( HltSelRepRBEnums::kSubstrID, substrSubBank.location(), substrSubBank.size() );
  substrSubBank.deleteBank();

  // std info
  stdInfoSubBank.saveSize();  
  hltSelReportsBank.push_back( HltSelRepRBEnums::kStdInfoID, stdInfoSubBank.location(), stdInfoSubBank.size() );
  stdInfoSubBank.deleteBank();

  // extra info
  hltSelReportsBank.push_back( HltSelRepRBEnums::kExtraInfoID, extraInfoSubBank.location(), extraInfoSubBank.size() );
  extraInfoSubBank.deleteBank();

  // insert the bank into the RawEvent
  hltSelReportsBank.saveSize();
  std::vector< unsigned int > bankBody( &(hltSelReportsBank.location()[0]), 
                                        &(hltSelReportsBank.location()[hltSelReportsBank.size()]) );  
  rawEvent->addBank(  kSourceID, RawBank::HltSelReports, kVersionNumber, bankBody );

  if ( msgLevel(MSG::VERBOSE) ){

    // print created bank and subbanks inside
    verbose() << hltSelReportsBank << endmsg;
    
    verbose() << HltSelRepRBHits( hltSelReportsBank.subBankFromID( HltSelRepRBEnums::kHitsID ) ) << endmsg;
    verbose() << HltSelRepRBObjTyp( hltSelReportsBank.subBankFromID( HltSelRepRBEnums::kObjTypID ) ) << endmsg;
    verbose() << HltSelRepRBSubstr( hltSelReportsBank.subBankFromID( HltSelRepRBEnums::kSubstrID ) ) << endmsg;
    verbose() << HltSelRepRBStdInfo( hltSelReportsBank.subBankFromID( HltSelRepRBEnums::kStdInfoID ) ) << endmsg;
    verbose() << HltSelRepRBExtraInfo( hltSelReportsBank.subBankFromID( HltSelRepRBEnums::kExtraInfoID ) ) << endmsg;

  } 

  // delete the main bank
  hltSelReportsBank.deleteBank();



  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltSelReportsWriter::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  //  return GaudiAlgorithm::finalize();  // must be called after all other actions
  return HltBaseAlg::finalize();  // must be called after all other actions
}

//=============================================================================



void  HltSelReportsWriter::addToLhcbidSequences( LhcbidSequence* set2,
                                                 LhcbidSequences & lhcbidSequences ) const
{
  if( set2->empty() )return;
  bool found=false;
  for(LhcbidSequences::iterator iSet1= lhcbidSequences.begin();
      iSet1!=lhcbidSequences.end();++iSet1){
    LhcbidSequence* set1 = *iSet1;
    if( (*set1) == (*set2) ){
      found=true;
      break;
    }
    LhcbidSequence* setint=new LhcbidSequence();
    set_intersection( set1->begin(),set1->end(),
                      set2->begin(),set2->end(),
                      inserter(*setint,setint->begin()) );
    if( setint->size() ){
      found=true;
      LhcbidSequence* set1p=new LhcbidSequence();
      set_difference(  set1->begin(),set1->end(),
                       setint->begin(),setint->end(),
                       inserter(*set1p,set1p->begin()) );
      lhcbidSequences.erase( iSet1 );
      delete set1;      
      if( set1p->size() ){
        lhcbidSequences.push_back( set1p );
      } else {
        delete set1p;
      }      
      lhcbidSequences.push_back( setint );
      LhcbidSequence* set2p=new LhcbidSequence();
      set_difference(  set2->begin(),set2->end(),
                       setint->begin(),setint->end(),
                       inserter(*set2p,set2p->begin()) );
      delete set2;      
      if( set2p->size() ){
        addToLhcbidSequences( set2p, lhcbidSequences );
      } else {
        delete set2p;
      }      
      break;      
    } else {
      delete setint;
    }    
  }
  if( !found ){
    lhcbidSequences.push_back( set2 );
  }  
}


      
bool HltSelReportsWriter::sortByCLID::operator()( const LHCb::HltObjectSummary*  elem1,
                                                  const LHCb::HltObjectSummary*  elem2 ) const 
{
  if( (elem1==0) || (elem2==0) )return false;
  if( elem1==elem2 )return false;
  int i1=elem1->summarizedObjectCLID();
  int i2=elem2->summarizedObjectCLID();
  if( i1 > i2 )return true;
  return false;
}
    
  
