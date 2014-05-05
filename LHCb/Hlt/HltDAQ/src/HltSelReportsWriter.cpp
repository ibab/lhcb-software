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

namespace {
  const Gaudi::StringKey InfoID{"InfoID"};


template <typename Input1,  typename Input2,
          typename Output1, typename Output2, typename Output3>
void sets_decompose(Input1 first1, Input1 last1,
                           Input2 first2, Input2 last2,
                           Output1 result1, Output2 result2, Output3 result3)
{
    while (first1 != last1 && first2 != last2) {
        if (*first1 < *first2) {
            *result1++ = *first1++;
        } else if (*first2 < *first1) {
            *result2++ = *first2++;
        } else {
            *result3++ = *first1++;
            ++first2; // skip common value in set2
        }
    }
    std::copy(first1, last1, result1);
    std::copy(first2, last2, result2);
}

template <typename S> class intersects {
        const S& m_s;
    public:
        intersects(const S& s) : m_s(s) {};

        bool operator()(const S& s) const {
            auto first1 = std::begin(m_s);
            auto last1  = std::end(m_s);
            auto first2 = std::begin(s);
            auto last2  = std::end(s);
            while( first1!=last1 && first2!=last2) {
                if      (*first1<*first2)  ++first1; 
                else if (*first2<*first1)  ++first2; 
                else return true;
            }
            return false;
        }
};
template <typename S> intersects<S> intersects_( const S& s ) { return {s}; }

}

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
    : GaudiAlgorithm ( name , pSvcLocator ),  m_hltANNSvc{ nullptr }
{

  declareProperty("InputHltSelReportsLocation",
    m_inputHltSelReportsLocation= LHCb::HltSelReportsLocation::Default);  
  declareProperty("OutputRawEventLocation",
    m_outputRawEventLocation= LHCb::RawEventLocation::Default);
  declareProperty("SourceID",
    m_sourceID= kSourceID_Dummy );  
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
  for( const auto&  hos : *objectSummaries ) {
    if( hos->lhcbIDs().empty() ) continue;
    LhcbidSequence thisIDset; // in C++14 set will have  a constructor from a range...
    for( const auto&  hit : hos->lhcbIDs() ) thisIDset.emplace( hit.lhcbID() );
    addToLhcbidSequences( std::move(thisIDset), lhcbidSequences );
  }

  auto nHits = std::accumulate(
      std::begin(lhcbidSequences), std::end(lhcbidSequences), 0u,
      [](unsigned int n, const LhcbidSequence &s) { return n += s.size(); });

  if( lhcbidSequences.size()/2 + 1 + nHits >  0xFFFFL  ){
    std::ostringstream mess;
    mess << "Too many hits or hit-sequences to store hits=" << nHits << " seq=" << lhcbidSequences.size()
         << " HltSelReports RawBank cannot be created ";    
    return Error( mess.str(), StatusCode::SUCCESS, 50 );
  }

  HltSelRepRBHits hitsSubBank;
  hitsSubBank.initialize( lhcbidSequences.size(), nHits );
  std::copy(std::begin(lhcbidSequences), std::end(lhcbidSequences),
            std::back_inserter(hitsSubBank));

  // --------------------------------------------------------------------------------------
  //  ---------------- in storage banks are ordered by summarizedClassCLID ----------------
  // -------------------------------- sort them -------------------------------------------

  std::vector<const HltObjectSummary*> sortedHosPtrs( objectSummaries->begin(),objectSummaries->end() );
  std::sort( std::begin(sortedHosPtrs), std::end(sortedHosPtrs), sortByCLID() );
  // inverse mapping
  std::vector< unsigned int > fromIndexToNewIndex(sortedHosPtrs.size());
  for( auto ppHos= std::begin(sortedHosPtrs); ppHos != std::end(sortedHosPtrs); ++ppHos ){
    fromIndexToNewIndex[ (*ppHos)->index() ] = int(ppHos - std::begin(sortedHosPtrs));
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

  std::vector<IANNSvc::minor_value_type> hltinfos = m_hltANNSvc->items(InfoID); 

  objTypSubBank.initialize();
  substrSubBank.initialize();  
  // some banks require initialization with size info 
  unsigned int nExtraInfo=0;  
  unsigned int nStdInfo=0;  
  for( const auto&  hos : sortedHosPtrs ) {
    for( const auto&  i : hos->numericalInfo() ) {
      if( i.first.find("#")!=std::string::npos ){
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
        for( auto  iObj= sortedHosPtrs.begin(); iObj != sortedHosPtrs.end(); ++iObj ){
          const HltObjectSummary & hos = **iObj;
          if( hos.summarizedObjectCLID() == 1 ){
            for( const auto&  i : hos.numericalInfo() ) {
              if( i.first.find("#")!=std::string::npos ){
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
  for( const auto& hos : sortedHosPtrs ) {
    
    objTypSubBank.push_back( hos->summarizedObjectCLID() );

    HltSelRepRBExtraInfo::ExtraInfo extraInfo;
    HltSelRepRBStdInfo::StdInfo stdInfo;

    for(const auto&  i : hos->numericalInfo() ){

      if( i.first.find("#")!=std::string::npos ){

        if( saveStdInfo || ( hos->summarizedObjectCLID() == 1 ) ){
          
          // push floats as ints (allows for possible compression in future versions)
          union IntFloat { unsigned int mInt; float mFloat; };
          IntFloat a; a.mFloat = i.second;
          unsigned int intFloat = a.mInt;
          stdInfo.push_back( intFloat );
          
        }

      } else if(saveExtraInfo) {

        bool found=false;        
        // convert string-id to a short
        for( const auto&  j : hltinfos) {
          if( j.first == i.first  ){
            extraInfo.emplace_back(  j.second, i.second );
            found=true;
            break;
          }
        }
        // this is very unexpected but shouldn't be fatal
        if( !found ) {
          std::ostringstream mess;
          mess << "Int key for string info key=" << i.first << " not found ";
          Error( mess.str(), StatusCode::SUCCESS, 50 );
        }
      }
    }
    stdInfoSubBank.push_back( stdInfo );
    extraInfoSubBank.push_back( extraInfo );

    // substructure or hit sequence link vector
    HltSelRepRBSubstr::Substrv svect;
    unsigned int sHitType=0;    

    if( hos->substructure().size() ){
      // substructure 
      for( auto iS=hos->substructure().begin(); iS!=hos->substructure().end(); ++iS ){
        svect.push_back( fromIndexToNewIndex[ (*iS)->index() ] );
      }
    } else if( !hos->lhcbIDs().empty() ) {
      // hits
      sHitType=1;
      
      LhcbidSequence thisIDset;
      for( const auto&  hit : hos->lhcbIDs() ) thisIDset.insert( hit.lhcbID() );
    
      unsigned int iSeqID(0);  
      for( const auto&  hitset :  lhcbidSequences) {
        if( thisIDset.find( *hitset.begin() ) != std::end(thisIDset) ){
          svect.push_back( iSeqID );          
        }
        ++iSeqID;
      }
    }

    HltSelRepRBSubstr::Substr aS( sHitType, svect );
    if( !substrSubBank.push_back( aS ) ){
      hitsSubBank.deleteBank();    
      objTypSubBank.deleteBank();
      substrSubBank.deleteBank();
      stdInfoSubBank.deleteBank();
      extraInfoSubBank.deleteBank();
      return Error("Exceeded maximal size of substructure-subbank. HltSelReports RawBank cannot be created"
                   , StatusCode::SUCCESS, 50 );
    } 
      
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
// this function moves (the relevant parts of) set2 into lhcbidSequences

void  HltSelReportsWriter::addToLhcbidSequences( LhcbidSequence&& set2,
                                                 LhcbidSequences& lhcbidSequences ) const
{
  LhcbidSequences::difference_type offset{ 0 };
  while ( !set2.empty() ) {
     auto iSet1 = std::find_if(std::begin(lhcbidSequences)+offset,std::end(lhcbidSequences), intersects_( set2 ));
     if (iSet1==std::end(lhcbidSequences)) {
        // no overlap, add everything in one shot...
       lhcbidSequences.emplace_back( std::move(set2) );
       assert(set2.empty());
       break;
     }
     offset = std::distance( std::begin(lhcbidSequences), iSet1 );  // start of the next iteration
     LhcbidSequence set1p,set2p,setin;
     sets_decompose( std::begin(*iSet1), std::end(*iSet1),
                     std::begin(set2),   std::end(set2),
                     std::inserter( set1p, std::end(set1p) ),  // subset of set1, not in set2
                     std::inserter( set2p, std::end(set2p) ),  // subset of set2, not in set1 
                     std::inserter( setin, std::end(setin) ) ) ; // intersection of set1 and set2
     if (iSet1->size()!=setin.size()) iSet1->swap(setin); // shrink set1 to its intersection with set2 
     if (!set1p.empty()) lhcbidSequences.emplace_back( std::move(set1p) ); // add the non-overlapping part of set1
     set2.swap(set2p);   // shrink set2 to its non-overlapping part
  }
}
