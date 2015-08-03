// $Id: FastL0DUFilter.cpp,v 1.3 2010-01-27 23:35:23 odescham Exp $
// Include files 

// local
#include "FastL0DUFilter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FastL0DUFilter
//
// 2008-11-12 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( FastL0DUFilter )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FastL0DUFilter::FastL0DUFilter( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : L0FromRawBase ( name , pSvcLocator )
    ,m_count(0)
    ,m_sel(0)
{
  declareProperty( "SourceID"                , m_source = 0 );
  declareProperty( "ForceNonZeroSupMuons"    , m_noMuonSup = false );
  declareProperty( "SumEtThreshold"          , m_sumCut   = 0 );
  declareProperty( "HadronThreshold"         , m_haCut    = 0 );
  declareProperty( "SpdThreshold"            , m_spdCut   = 0 );
  declareProperty( "ElectronThreshold"       , m_elCut    = 0 );
  declareProperty( "MuonThreshold"           , m_muCut    = 0 );
  declareProperty( "UseDecisionInBank"       , m_useDecInBank = true );
}

//=============================================================================
// Destructor
//=============================================================================
FastL0DUFilter::~FastL0DUFilter() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode FastL0DUFilter::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode FastL0DUFilter::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  setFilterPassed( false );
  m_count++;

  LHCb::RawEvent* rawEvt = NULL ;
  std::string rawLoc="";
  if( selectRawEventLocation(rawLoc).isFailure()){
    counter("RawEvent is missing")+=1;
    return StatusCode::SUCCESS;
  }
  rawEvt= get<LHCb::RawEvent>( rawLoc );
  for( std::vector<LHCb::RawBank*>::const_iterator itB = (&rawEvt->banks(   LHCb::RawBank::L0DU ))->begin();
       itB != (&rawEvt->banks(   LHCb::RawBank::L0DU ))->end() ; itB++){
    

    LHCb::RawBank* bank = *itB;
    if( NULL == bank || 0 == bank->size() )continue;
    if( m_source != bank->sourceID() )continue;
    if( 1 != bank->version() && 2!= bank->version() ){
      Warning( "Inconsistent L0DU bank version" ).ignore();
      continue;
    }    
    unsigned int* data = bank->data();

    data += 2;
    // get muon1
    long muPt = (*data & 0x0000007F);

    // jump to 2nd sub-block ...
    // ... assuming no 0-suppression for muon block
    unsigned int offset = 9;
    // ... computing the offset according to non 0-sup #muon
    if( !m_noMuonSup ){
      data++;
      unsigned int  nmu       = (*data & 0xF0000000 ) >> 28;
      offset = 2 + (int) ( (nmu+1)/2 ) + (int) ( (nmu+3)/4 ) ;
    }    
    // here we are
    data += offset;
    unsigned int rsda       = (*data & 0x0000FFFF );
    bool dec   = (1 == ((rsda >> 12) & 1)) ? true : false;
    // get SumEt, hadron and electron Et
    data += 4;
    long sumEt   = (*data & 0x00003FFF ) ;
    long spdMult = (*data & 0x0FFFC000 ) >> 14;
    data += 1;
    long elEt  = (*data & 0x000000FF ) ;
    data += 1;
    long haEt  = (*data & 0x000000FF ) ;    

    // HARDCODED algorithm --------------------------------------------------------
    bool newdec = ( haEt >= m_haCut && spdMult >= m_spdCut) || ( muPt >= m_muCut ) ;
    // ----------------------------------------------------------------------------

    bool decision = m_useDecInBank ? dec : newdec;
    
    if ( msgLevel( MSG::DEBUG) ){
      debug() << " decision : " << decision << endmsg;      
      debug() << " SumEt  : " << sumEt << " Threshold : " << m_sumCut << endmsg;
      debug() << " SpdMult: " << spdMult << " Threshold : " << m_spdCut << endmsg;
      debug() << " el(Et) : " << elEt  << " Threshold : " << m_elCut << endmsg;
      debug() << " ha(Et) : " << haEt  << " Threshold : " << m_haCut << endmsg; 
      debug() << " mu(Pt) : " << muPt  << " Threshold : " << m_muCut << endmsg; 
    }
    if ( decision  ){
      setFilterPassed(true);
      m_sel++;
      if ( msgLevel( MSG::DEBUG) )debug() << " ----> ACCEPT EVENT " << endmsg;
    }
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode FastL0DUFilter::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  info() << "Filtering : " << m_sel << " events among " << m_count << " processed " << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
