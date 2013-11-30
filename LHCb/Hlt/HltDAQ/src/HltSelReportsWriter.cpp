// $Id: HltSelReportsWriter.cpp,v 1.5 2010-06-03 18:59:27 graven Exp $
// Include files 

#include <memory>
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
DECLARE_ALGORITHM_FACTORY( HltSelReportsWriter )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltSelReportsWriter::HltSelReportsWriter( const std::string& name,
                                          ISvcLocator* pSvcLocator)
    : GaudiAlgorithm ( name , pSvcLocator )
{

  declareProperty("InputHltSelReportsLocation",
    m_inputHltSelReportsLocation= LHCb::HltSelReportsLocation::Default);  
  declareProperty("OutputRawEventLocation",
    m_outputRawEventLocation= LHCb::RawEventLocation::Default);
  declareProperty("SourceID",
    m_sourceID= kSourceID_Dummy );  

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
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_hltANNSvc = svc<IANNSvc>("HltANNSvc");

  if( m_sourceID > kSourceID_Max ){
    m_sourceID = m_sourceID & kSourceID_Max;
    return Error("Illegal SourceID specified; maximal allowed value is 7" , StatusCode::FAILURE, 50 );
  }


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

  // protection against too many objectSummaries to store
  if( objectSummaries->size() > 0xFFFFL ){
    std::ostringstream mess;
    mess << "Too many HltObjectSummaries to store " << objectSummaries->size() 
         << " HltSelReports RawBank cannot be created ";    
    return Error( mess.str(), StatusCode::SUCCESS, 50 );
  }

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

  if( lhcbidSequences.size()/2 + 1 + nHits >  0xFFFFL  ){
    // don't forget to clean lhcbidSequences; they own sets they are pointing to
    for( LhcbidSequences::const_iterator iSeq=lhcbidSequences.begin();
         iSeq != lhcbidSequences.end(); ++iSeq ){
      delete *iSeq;
    }
    std::ostringstream mess;
    mess << "Too many hits or hit-sequences to store hits=" << nHits << " seq=" << lhcbidSequences.size()
         << " HltSelReports RawBank cannot be created ";    
    return Error( mess.str(), StatusCode::SUCCESS, 50 );
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
    for( HltObjectSummary::Info::const_iterator i=hos.numericalInfo().begin();
         i!=hos.numericalInfo().end();++i){
      if( i->first.find("#")!=std::string::npos ){
        ++nStdInfo;        
      } else {
        ++nExtraInfo;        
      }
    }
  }  
  bool saveExtraInfo = extraInfoSubBank.initialize( sortedHosPtrs.size(), nExtraInfo );
  if( !saveExtraInfo ){
        std::ostringstream mess;
        mess << "ExtraInfoSubBank too large to store nObj=" << sortedHosPtrs.size()
             << " nInfo=" << nExtraInfo << " No Extra Info will be saved!";
        Error( mess.str(), StatusCode::SUCCESS, 50 );
        if( !extraInfoSubBank.initialize( sortedHosPtrs.size(), 0 ) ){
          Error( "Cannot save even empty ExtraInfoSubBank  - expect a fatal error", StatusCode::SUCCESS, 50 );
        }        
  }
  bool saveStdInfo = stdInfoSubBank.initialize( sortedHosPtrs.size(), nStdInfo );
  if( !saveStdInfo ){
        std::ostringstream mess;
        mess << "StdInfoSubBank too large to store nObj=" << sortedHosPtrs.size()
             << " nInfo=" << nStdInfo << " No Std Info will be saved!";        
        Error( mess.str(), StatusCode::SUCCESS, 50 );
        // save only selection IDs
        nStdInfo=0;  
        for( std::vector<const HltObjectSummary*>::const_iterator  iObj= sortedHosPtrs.begin();
             iObj != sortedHosPtrs.end(); ++iObj ){
          const HltObjectSummary & hos = **iObj;
          if( hos.summarizedObjectCLID() == 1 ){
            for( HltObjectSummary::Info::const_iterator i=hos.numericalInfo().begin();
                 i!=hos.numericalInfo().end();++i){
              if( i->first.find("#")!=std::string::npos ){
                ++nStdInfo;        
              }
            }
          }
        }  
        if( !stdInfoSubBank.initialize( sortedHosPtrs.size(), nStdInfo ) ){
          Error( "Cannot save even selectionIDs - expect a fatal error", StatusCode::SUCCESS, 50 );
        }        
  }

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

        if( saveStdInfo || ( hos.summarizedObjectCLID() == 1 ) ){
          
          // push floats as ints (allows for possible compression in future versions)
          union IntFloat { unsigned int mInt; float mFloat; };
          IntFloat a; a.mFloat = i->second;
          unsigned int intFloat = a.mInt;
          stdInfo.push_back( intFloat );
          
        }

      } else if(saveExtraInfo) {

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
      delete thisIDset;      
    }

    HltSelRepRBSubstr::Substr aS( sHitType, svect );
    if( !substrSubBank.push_back( aS ) ){
      // don't forget to clean lhcbidSequences; they own sets they are pointing to
      for( LhcbidSequences::const_iterator iSeq=lhcbidSequences.begin();
           iSeq != lhcbidSequences.end(); ++iSeq ){
        delete *iSeq;
      }
      hitsSubBank.deleteBank();    
      objTypSubBank.deleteBank();
      substrSubBank.deleteBank();
      stdInfoSubBank.deleteBank();
      extraInfoSubBank.deleteBank();
      return Error("Exceeded maximal size of substructure-subbank. HltSelReports RawBank cannot be created"
                   , StatusCode::SUCCESS, 50 );
    } 
      
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


  // delete any previously inserted sel reports with the same major sourceID
  const std::vector<RawBank*> hltselreportsRawBanks = rawEvent->banks( RawBank::HltSelReports );
  for( std::vector<RawBank*>::const_iterator b=hltselreportsRawBanks.begin();
       b!=hltselreportsRawBanks.end(); ++b){
    unsigned int sourceID=kSourceID_Hlt;
    if( (*b)->version() > 1 ){
      sourceID = (*b)->sourceID() >> kSourceID_BitShift;
    }
    if( m_sourceID != sourceID )continue;

    rawEvent->removeBank(*b);
    if ( msgLevel(MSG::VERBOSE) ){ verbose() << " Deleted previosuly inserted HltSelReports bank " << endmsg;

    }    
  }

  //    std::vector< unsigned int > bankBody( &(hltSelReportsBank.location()[0]), 
  //                                        &(hltSelReportsBank.location()[hltSelReportsBank.size()]) );
  //  rawEvent->addBank(  0, RawBank::HltSelReports, kVersionNumber, bankBody );
  //
  // RawBank is limited in size to 65535 bytes i.e. 16383 words; be conservative cut it off at a smaller limit.
  // Save in chunks if exceed this size.
  int nBank = (hltSelReportsBank.size()-1)/16300 + 1;
  if( nBank > kSourceID_MinorMask ){
    // delete the main bank
    hltSelReportsBank.deleteBank();
    // 
    return Error("HltSelReports too long to save", StatusCode::SUCCESS, 50 );    
  }
  for(int iBank=0; iBank < nBank; ++iBank ){
    int ioff=iBank*16300;
    int isize=hltSelReportsBank.size()-ioff;
    if( isize > 16300 )isize=16300;	
    std::vector< unsigned int > bankBody( &(hltSelReportsBank.location()[ioff]), 
                                          &(hltSelReportsBank.location()[ioff+isize]) );
    int sourceID = iBank | ( m_sourceID << kSourceID_BitShift );
    rawEvent->addBank(  sourceID, RawBank::HltSelReports, kVersionNumber, bankBody );
  }
  if( nBank>1 ){
    std::ostringstream mess;
    mess << "HltSelReports is huge. Saved in " << nBank << " separate RawBanks ";    
    Warning( mess.str(), StatusCode::SUCCESS, 10 );
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
  
    
  // delete the main bank
  hltSelReportsBank.deleteBank();

  return StatusCode::SUCCESS;
}

//=============================================================================
// this function effectively transfers ownership of set2 into lhcbidSequences

void  HltSelReportsWriter::addToLhcbidSequences( LhcbidSequence* set2,
                                                 LhcbidSequences & lhcbidSequences ) const
{
  if ( set2==0 ) return;
  if ( set2->empty() ) {
      delete set2; set2 = 0;
      return;
  }
  for(LhcbidSequences::iterator iSet1= lhcbidSequences.begin();
      iSet1!=lhcbidSequences.end();++iSet1){
    // first check full overlap -- no need to store, just skip
    // is this needed as a special case? Is always checking == faster then set_intersection??
    if( (**iSet1) == (*set2) ){
      delete set2; set2 = 0;
      break;
    }
    // check  for overlap
    std::auto_ptr<LhcbidSequence> setint(new LhcbidSequence());
    set_intersection( (*iSet1)->begin(),(*iSet1)->end(),
                      set2->begin(),set2->end(),
                      inserter(*setint,setint->begin()) );
    if( !setint->empty() ){
      // if any overlap, we replace the original with its subset not in the input
      std::auto_ptr<LhcbidSequence> set1p(new LhcbidSequence());
      set_difference(  (*iSet1)->begin(),(*iSet1)->end(),
                       setint->begin(),setint->end(),
                       inserter(*set1p,set1p->begin()) );
      delete *iSet1;                  // given that erase invalidates iSet1, we first delete
      lhcbidSequences.erase( iSet1 ); // and then erase...
      if( !set1p->empty() ) lhcbidSequences.push_back( set1p.release() );

      // and then we split the input into the overlap and unique parts
      std::auto_ptr<LhcbidSequence> set2p(new LhcbidSequence());
      set_difference(  set2->begin(),set2->end(),
                       setint->begin(),setint->end(),
                       inserter(*set2p,set2p->begin()) );
      // add the overlap
      lhcbidSequences.push_back( setint.release() );
      // don't need the original input anymore
      delete set2; set2 = 0;
      // and add the unique part of the input, if any
      if(!set2p->empty() ) addToLhcbidSequences( set2p.release(), lhcbidSequences );
      // Done!
      break;
    }
  }
  // and if no overlap, we just add everything in one shot...
  if( set2!=0 ) lhcbidSequences.push_back( set2 );
}
