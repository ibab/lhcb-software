// $Id: CaloTriggerBitsFromRaw.cpp,v 1.16 2007-05-01 22:24:27 odescham Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "CaloTriggerBitsFromRaw.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloTriggerBitsFromRaw
//
// 2005-01-06 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( CaloTriggerBitsFromRaw );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloTriggerBitsFromRaw::CaloTriggerBitsFromRaw( const std::string& type,
                                                const std::string& name,
                                                const IInterface* parent )
  : CaloReadoutTool ( type, name , parent )
{
  declareInterface<ICaloTriggerBitsFromRaw>(this);
  (m_data.first).clear();
  (m_data.second).clear();  
};


//=============================================================================
// Destructor
//=============================================================================
CaloTriggerBitsFromRaw::~CaloTriggerBitsFromRaw() {};

//=========================================================================
//  Initialisation
//=========================================================================
StatusCode CaloTriggerBitsFromRaw::initialize ( ) {
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiTool

  debug() << "==> Initialize " << name() << endmsg;

  m_calo     = getDet<DeCalorimeter>( DeCalorimeterLocation::Prs );
  m_packedType = LHCb::RawBank::PrsPacked;
  m_shortType  = LHCb::RawBank::PrsTrig;

  long nCells = m_calo->numberOfCells();
  (m_data.first).reserve( nCells );
  (m_data.second).reserve( nCells );
  return StatusCode::SUCCESS;
}


//=========================================================================
//  Return prs or spd cells independently
//  Warning if both method are invoqued consecutively 
//  the FULL decoding is run twice
//  --> in this case it's better to invoque prsSpdCells() method
//=========================================================================
LHCb::Calo::FiredCells& CaloTriggerBitsFromRaw::prsCells () {
  LHCb::Calo::PrsSpdFiredCells& allCells = prsSpdCells();
  return allCells.first;
}
LHCb::Calo::FiredCells& CaloTriggerBitsFromRaw::spdCells () {
  LHCb::Calo::PrsSpdFiredCells& allCells = prsSpdCells();  
  return allCells.second;
}
//=========================================================================
//  Decode the fired Cells for ALL banks of one event
//=========================================================================
LHCb::Calo::PrsSpdFiredCells& CaloTriggerBitsFromRaw::prsSpdCells () {
  return prsSpdCells( -1 );
}
//=========================================================================
//  Decode the TriggerBit for a single bank (given by sourceID)
//  Decode ALL banks if source < 0
//=========================================================================
LHCb::Calo::PrsSpdFiredCells& CaloTriggerBitsFromRaw::prsSpdCells (int source ) {

  (m_data.first).clear();
  (m_data.second).clear();  
  StatusCode sc = StatusCode::SUCCESS;
  if(m_getRaw)sc = getCaloBanksFromRaw();
  if( 0 == m_banks ){
    error() << "banks containter is not defined" << endreq;
    sc = StatusCode::FAILURE;
  }
  bool ok=false;
  if( sc.isSuccess() ){
    for( std::vector<LHCb::RawBank*>::const_iterator itB = m_banks->begin(); 
         itB != m_banks->end() ; ++itB ) {
      int sourceID       = (*itB)->sourceID();
      if( source >= 0 && source != sourceID )continue;
      ok = true;
      sc = getData ( *itB );
      if( !sc.isSuccess() ) break;
    } 
  }
  if( !ok ){
    error() << " Expected bank source " << source << " has not been found " << endreq;
    sc = StatusCode::FAILURE;
  }
  if( !sc.isSuccess() ) {
    error() << " Error when decoding data -> return empty container " << endreq;
    (m_data.first).clear();
    (m_data.second).clear();  
  }
  return m_data;
}

//=========================================================================
//  Return appropriate containers (prs.or.spd) for a single bank (given by pointer)
//=========================================================================

LHCb::Calo::PrsSpdFiredCells& CaloTriggerBitsFromRaw::prsSpdCells (  LHCb::RawBank* bank ) {
  (m_data.first).clear();
  (m_data.second).clear();  
  StatusCode sc = getData ( bank );    
  return m_data;
}

//=========================================================================
//  Main decoding method fill both m_prsCells and m_spdCells containers.
//=========================================================================
StatusCode CaloTriggerBitsFromRaw::getData(  LHCb::RawBank* bank ) {
  unsigned int* data = bank->data();
  int size           = bank->size()/4;  // size in byte
  int version        = bank->version();
  int sourceID       = bank->sourceID();
  int lastData       = 0;
  debug() << "Decode bank " << bank << " source " << sourceID 
          << " version " << version << " size " << size << endreq;
  


  // -----------------------------------------------
  // skip detector specific header line 
  if(m_extraHeader){
    ++data ; 
    --size;
  }
  // -----------------------------------------------

  //=== Offline coding: a CellID, 8 SPD bits, 8 Prs bits
  if ( 1 == version ) {
    while ( 0 != size ) {
      int spdData = (*data >> 8 ) & 0xFF;
      int prsData = (*data) & 0xFF;
      int lastID  = (*data) >> 16;
        ++data;
        --size;
        for ( unsigned int kk = 0; 8 > kk; ++kk ) {

          LHCb::CaloCellID id( lastID+kk );
          LHCb::CaloCellID spdId( (lastID+kk) & 0x3FFF );

          if ( spdData & 1 ) m_data.second.push_back( spdId);
          if ( prsData & 1 ) m_data.first.push_back( id ); 
 

          //event dump
          if ( msgLevel( MSG::DEBUG) ) {
            debug() << " |  SourceID : " << sourceID
                    << " |  FeBoard : " << m_calo->cardNumber(id)
                    << " |  CaloCell " << id
                    << " |  valid ? " << m_calo->valid(id)
                    << " |  Prs/Spd  = " << (prsData & 1) << "/" << (spdData & 1) << endreq;
          }

          spdData = spdData >> 1;
          prsData = prsData >> 1;
        }
    }
    //=== Compact coding: a CellID, and its Prs/SPD bits
  } else if ( 2 == version ) {
    while ( 0 != size ) {
      int word = *data;
      while ( 0 != word ) {
        int item = word & 0xFFFF;
        word = ( word >> 16 ) & 0xFFFF;
        int spdId =  (item&0xFFFC) >> 2;

        //event dump
        LHCb::CaloCellID prsId( spdId + 0x4000 );   // Prs
        if ( msgLevel( MSG::DEBUG) ) {
          debug() << " |  SourceID : " << sourceID
                  << " |  FeBoard : " << m_calo->cardNumber( prsId )
                  << " |  CaloCell " << prsId
                  << " |  valid ? " << m_calo->valid( prsId )
                  << " |  Prs/Spd  = " << (item&1) << "/" << (item&2) << endreq;
        }
        
        
        if ( 0 != (item & 2) ) {
          LHCb::CaloCellID id ( spdId );   // SPD
          m_data.second.push_back( id );
        }
        if ( 0 != (item & 1) ) {
          m_data.first.push_back( prsId );
        }
      }
      ++data;
      --size;
    }
    //==== Codage for 1 MHz
  } else if ( 3 == version ) {

    // Get the FE-Cards associated to that bank (via condDB)
    std::vector<int> feCards = m_calo->tell1ToCards( sourceID );
    int nCards = feCards.size();
    debug() << nCards << " FE-Cards are expected to be readout : " 
            << feCards << " in Tell1 bank " << sourceID << endreq;
    
    int offset   = 0;
    int lenAdc   = 0;
    int lenTrig  = 0;
    while( 0 != size ) {
      int word = *data++;
      size--;
      lenTrig = word & 0x7F;
      lenAdc  = (word >> 7 ) & 0x7F;
      if ( msgLevel( MSG::DEBUG) ) {
        debug() << format( "  Header data %8x size %4d lenAdc%3d lenTrig%3d",
                           word, size, lenAdc, lenTrig )
                << endreq;
      }
      int code  = (word >>14 ) & 0x1FF;
      debug() << "Read FE-board ["<< code << "] linked to TELL1 bank " << sourceID << endreq;      
      // access chanID via condDB
      std::vector<LHCb::CaloCellID> chanID  ;
      // look for the FE-Card in the Tell1->cards vector
      int card = findCardbyCode(feCards,code);
      if( 0 <= card ){
        chanID = m_calo->cardChannels( feCards[card] );
        feCards.erase(feCards.begin()+card);
      }else{
        error() << " FE-Card w/ [code : " << code << "] not associated with TELL1 bank " << sourceID
                << " in condDB :  Cannot read that bank" << endreq;
        return StatusCode::FAILURE;
      }

      // Process FE-data decoding
      offset   = 32;      
      while ( 0 < lenTrig ) {
        if ( 32 == offset ) {
          lastData = *data++;
          size--;
          offset = 0;
        }
        int num   = ( lastData >> offset ) & 0x3F;
        int isPrs = ( lastData >> (offset +6)) & 1;
        int isSpd = ( lastData >> (offset +7)) & 1;
        offset += 8;
        lenTrig--;
        LHCb::CaloCellID id =  chanID[ num ];


        // event dump
        if ( msgLevel( MSG::DEBUG) ) {
          debug() << " |  SourceID : " << sourceID
                  << " |  FeBoard : " << code 
            //<< " |  Channel : " << bitNum
                  << " |  CaloCell " << id
                  << " |  valid ? " << m_calo->valid(id)
                  << " |  Prs/Spd  = " << isPrs << "/" << isSpd << endreq;
        }


        if ( 0 != isPrs ) m_data.first.push_back( id );
        if ( 0 != isSpd ) {
          LHCb::CaloCellID spdId( 0, id.area(), id.row(), id.col() );
          m_data.second.push_back( spdId );
        }
      }
      int nSkip = (lenAdc+1 ) / 2;  // Length in number of words
      size     -= nSkip;
      data     += nSkip;
    } //== DataSize
    // Check All cards have been read
    checkCards(nCards,feCards);
  } //== versions
  
  
  return StatusCode::SUCCESS;
}
//=============================================================================
