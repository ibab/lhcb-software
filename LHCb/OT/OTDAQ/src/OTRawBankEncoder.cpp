// Include files 
// from std
#include <algorithm>
#include <cstring> // for memcpy with gcc 4.3

// from LHCbMath
// #include "LHCbMath/Bit.h"

// from Event
#include "Event/RawEvent.h"
#include "Event/RawBank.h"
#include "Event/OTBankVersion.h"

// from OTDAQ
#include "OTDAQ/OTSpecificHeader.h"
#include "OTDAQ/GolHeader.h"
#include "OTDAQ/RawHit.h"

// local
#include "OTRawBankEncoder.h"

//-----------------------------------------------------------------------------
// Implementation file for class : OTRawBankEncoder
//
// 2008-05-22 : Jan Amoraal
//-----------------------------------------------------------------------------

namespace OTDAQ {
  /// These simple classes are needed for sorting
  /// the channel ids according to Bank and Gol

 template<class T> 
 void pipeToBuffer( const T& t, std::vector<unsigned char>& buffer ) {
  size_t curSize    = buffer.size();
  size_t objectSize = sizeof( t );
  buffer.resize( objectSize +curSize );
  std::memcpy( &buffer.front() + curSize, &t, objectSize );
 }

 struct CompareChannel {
   bool operator()( const OTDAQ::RawHit& lhs, const OTDAQ::RawHit& rhs ) {
     return lhs.channel() < rhs.channel() ;
   }
 };

 /// Simple Gol class
 /// One gol per X modules
 class OTGol {
   public:

   OTGol( size_t id ):m_id( id ), 
                      m_channels(),
                      m_encode(false) {};

   size_t id() const { return m_id; };

   void addChannel( const LHCb::OTChannelID& channel ) { m_channels.push_back( channel ); };
   
   size_t nChannels() const { return m_channels.size(); }; ///< == Number of hits in gol

   bool encode() const { return m_encode; };
        
   void setEncode( const bool& encode ) { m_encode = encode; } ;
        
   std::vector<LHCb::OTChannelID>::const_iterator firstChannel() const { return m_channels.begin(); };
   
   std::vector<LHCb::OTChannelID>::const_iterator lastChannel() const { return m_channels.end(); };
     
   void clear() { 
     m_channels.clear();
     m_encode = false;
   };
        
   private:

   size_t                         m_id;
   std::vector<LHCb::OTChannelID> m_channels;
   bool                           m_encode;
 };

 /// Simple OT Bank class
 /// One bank per X gols
 class OTBank {
   public:
   
   OTBank( size_t id, size_t nGols ):m_id( id ), 
                                     m_nGols( nGols), 
                                     m_gols(),
                                     m_nChannels( 0u ),
                                     m_encode(false),
                                     m_nGolsToEncode( 0u ) {
     m_gols.reserve( m_nGols );
     /// Gols start from 1
     for (size_t i = 1u, iEnd = m_nGols+1u; i != iEnd; ++i) m_gols.push_back( OTDAQ::OTGol( i ) );
   };

   size_t id() const { return m_id; };
   
   void addChannel( const LHCb::OTChannelID& channel ) { 
     /// Gols start from 1
     unsigned int golID = channel.module(); /// == module id
     if ( golID == 0 || golID > m_nGols ) {
       /// Skip channel. Something went wrong
       std::cout << "Invalid gol id skipping channel!" << std::endl;
     } else {
       /// Remember gols start from 1
       m_encode = true;
       m_gols[golID-1u].addChannel( channel );
       ++m_nChannels;
       if ( !m_gols[golID-1u].encode() ) {
         m_gols[golID-1u].setEncode( true );
         ++m_nGolsToEncode;
       }
     }
   };

   size_t nChannels() const { return m_nChannels; }; ///< == Number of hits in bank

   bool encode() const { return m_encode; };
        
   size_t nGols() const { return m_nGols; };
   
   size_t nGolsToEncode() const { return m_nGolsToEncode; };
   
   std::vector<OTGol>::const_iterator begin() const { return m_gols.begin(); };
   
   std::vector<OTGol>::const_iterator end() const { return m_gols.end(); };

   void clear() { 
     /// Clear gols
     std::for_each( m_gols.begin(), m_gols.end(), [](OTGol& g) { return g.clear(); } );
     /// reset counters
     m_nChannels    = 0u;
     m_encode       = false;
     m_nGolsToEncode = 0u;
   };
      
   private:
   
   size_t             m_id;
   size_t             m_nGols;
   std::vector<OTGol> m_gols;
   size_t             m_nChannels;
   bool               m_encode;
   size_t             m_nGolsToEncode;
 };
 
}

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( OTRawBankEncoder )

OTRawBankEncoder::OTRawBankEncoder( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : GaudiTool ( type, name , parent ), m_channelmaptool(0)
{
  declareInterface<IOTRawBankEncoder>(this);
  declareProperty( "AddEmptyBanks"   , m_addEmptyBanks = true                               );
  declareProperty( "RawEventLocation", m_rawEventLocation = LHCb::RawEventLocation::Default );
}

OTRawBankEncoder::~OTRawBankEncoder() {}


StatusCode OTRawBankEncoder::initialize() {
  
  debug() << "Initialising OTRawbankEncoder" << endmsg;
  StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;

  // access to the channel map
  m_channelmaptool = tool<IOTChannelMapTool>("OTChannelMapTool");
 
  createBanks();
  
  return sc;
}

void OTRawBankEncoder::createBanks( ) {

  m_banks.reserve( nTell1s::v2008 );
  /// OK I assume we have one Tell1 per quarter, 48 in total, and 
  /// that the Tell1 id is 0x0TLQ where T={1,2,3}, L={0,1,2,3}, and Q={0,1,2,3}
  for ( unsigned t = 1; t < 4u; ++t ) {
    for ( unsigned l = 0; l < 4u; ++l ) {
      for (unsigned q = 0; q < 4u; ++q ) {
        //unsigned id = 100u*t + 10u*l + 1u*q; // dec
        // First four bits is Q next for four is L and next four is T
        unsigned id = ( t << 8 ) | ( l << 4 ) | ( q << 0 );
        //std::cout << "Created id " << std::dec << id << " in hex " << std::hex << id << std::endl;
        m_banks.push_back( OTDAQ::OTBank( id, nGols::v2008 ) );
      }
    }
  }
}

const OTRawBankEncoder::OTRawBank& OTRawBankEncoder::createRawBank(const OTDAQ::OTBank& bank) const {
  
  const bool isDebug   = msgLevel( MSG::DEBUG );
  const bool isVerbose = msgLevel( MSG::VERBOSE );

  if ( isDebug ) debug() << "Creating OTRawBank for OTBank with id = " << bank.id() 
                         << " containing " << bank.nGols() << " GOLS of which " << bank.nGolsToEncode() 
                         << " are non-empty and contain " << bank.nChannels() << " channels." << endmsg;

  if ( isDebug ) debug() << "Start: Size of bank = " << m_rawBank.size() << endmsg;
  
  /// The first 4 bytes contain the OT Specific header.
  /// Don't fill it for simulation.
  std::vector<unsigned char> buffer;
  if ( m_addEmptyBanks )
    pipeToBuffer( OTDAQ::OTSpecificHeader( 0, 0, 0, bank.nGols() ), buffer ); ///< The way it's done in the hardware
  else
    pipeToBuffer( OTDAQ::OTSpecificHeader( 0, 0, 0, bank.nGolsToEncode() ), buffer );

  /// Loop over ot gols and encode them
  /// Do this only for non-empty gols
  for ( const auto& gol : bank ) {
    
    if ( gol.encode() || m_addEmptyBanks ) {
      if ( !gol.encode() && m_addEmptyBanks ) { //Empty. Always add empty gols in hardware
        /// OK empty gol and we want to "add" it
        /// OK lot of assumptions here:
        /// 1) First we assume that module id is gol id
        /// 2) The bank id is of the format 0x0LTQ
        /// Get bank id in format 0x0TLQ
        if ( isDebug ) debug() << "Going to add empty gol with id " << gol.id() << " to bank with id " << bank.id() << endmsg;
        if ( isVerbose ) verbose() << "Empty bank id is " << std::hex << bank.id() << endmsg;
        const unsigned bankid  = bank.id();
        const unsigned mask    = 15;
        const unsigned station = ( ( bankid >> 8 ) & mask );  //bank.id()/100;
        const unsigned layer   = ( ( bankid >> 4 ) & mask );  //( bank.id()%100 )/10;
        const unsigned quarter = ( ( bankid >> 0 ) & mask );  //( bank.id()%10 );
        const unsigned module  = gol.id();
        if ( isDebug ) debug() << "Creating gol header with id = " << module << " station = " << station 
                               << " layer = " << layer << " quarter = " << quarter << endmsg;
        /// Create gol header.
        pipeToBuffer( OTDAQ::GolHeader( 0u, station, layer, quarter, module, 0u, 0u), buffer );
      } else
        /// Create gol header. ///Non-empty. Can create header from first channel
        pipeToBuffer( OTDAQ::GolHeader( 0u, 
                                        gol.firstChannel()->station(), 
                                        gol.firstChannel()->layer()  ,
                                        gol.firstChannel()->quarter(),
                                        gol.firstChannel()->module() ,
                                        0u,
                                        gol.nChannels() ), buffer );
      
      /// tmp vector of raw hits
      OTDAQ::RawHitContainer rawHits;
      rawHits.reserve( gol.nChannels() );
      for ( auto firstChannel = gol.firstChannel(); 
            firstChannel != gol.lastChannel(); ++firstChannel ) {
        if ( isVerbose ) verbose() << " Gol ID  = " << gol.id()
                                   << " Station = " << firstChannel->station()
                                   << " Layer   = " << firstChannel->layer()
                                   << " Quarter = " << firstChannel->quarter()
                                   << " Module  = " << firstChannel->module() 
                                   << " nHits   = " << gol.nChannels() << endmsg;
        
        size_t channel = m_channelmaptool->channel( (*firstChannel) ) ;
        rawHits.emplace_back( 0, channel, firstChannel->tdcTime() );
      }
      
      /// Sort according to channel in Tell1
      std::sort( rawHits.begin(), rawHits.end(), OTDAQ::CompareChannel() );
      
      // add padding i.e. empty hit
      if ( rawHits.size()%2 ) rawHits.emplace_back( );
      
      for ( const auto & i : rawHits ) pipeToBuffer( i, buffer );
  
      m_rawBank.resize( buffer.size()/sizeof( int ) );
      std::memcpy( &( m_rawBank.front() ), &( buffer.front() ), buffer.size() );
      
      if ( isDebug ) debug() << "End: Size of bank = " << m_rawBank.size() << endmsg;
    }
  }
  
  return m_rawBank;
}

StatusCode OTRawBankEncoder::encodeChannels( const std::vector<LHCb::OTChannelID>& channels ) const {
 
  const bool isDebug   = msgLevel( MSG::DEBUG );
  const bool isVerbose = msgLevel( MSG::VERBOSE );

  /// Raw event
  LHCb::RawEvent* rawEvent = get<LHCb::RawEvent>( m_rawEventLocation );
  // LHCb::RawEvent* rawEvent = 0;
  //   /// Check if raw event exists
  //   if ( exist<LHCb::RawEvent>( m_rawEventLocation ) ) {
  //       rawEvent = get<LHCb::RawEvent>( m_rawEventLocation );
  //   } else {
  //     /// else create it
  //     rawEvent = new LHCb::RawEvent();
  //     eventSvc()->registerObject(m_rawEventLocation, rawEvent);
  //   }
  
  /// Sort the channels into banks
  if ( isDebug ) debug() << "Going to encode " << channels.size() << " channels" << endmsg;
  for ( const auto& chan : channels ) {
    if ( isVerbose ) verbose() << "ChannelID = " << chan << endmsg;
    const size_t bankID = channelToBank( chan );
    if ( bankID == 0u || bankID > m_banks.size() ) {
      error() << "Trying to add channel to non-existent bank with index " << bankID <<  ", skipping!" << endmsg;
    } else {
      if ( isVerbose ) verbose() << "Adding channel " << chan << " to bank with index " << bankID << endmsg;
      /// Remember Tell1s start from 1
      m_banks[bankID-1u].addChannel( chan );
      if ( isVerbose ) verbose() << "Added channel " << chan << " to bank with id 0x0" << std::hex << m_banks[bankID-1u].id() << endmsg;
    }
  } 
  
  if ( isDebug ) {
    for ( const auto& bank : m_banks ) {
      debug() << "Bank id = " << bank.id() << " number of channels = " << bank.nChannels() << endmsg;
      for ( const auto& gol : bank ) {
        debug() << "  ===> Gol id = " << gol.id() << endmsg;
        for ( auto chan = gol.firstChannel(), chanEnd = gol.lastChannel(); chan != chanEnd; ++chan ) {
          if ( isVerbose ) verbose() << "   ++> Channel id = " << (*chan) << endmsg;
        }
      }
    }
  }

  /// Loop over ot banks and create raw banks
  /// We do this only for banks that contain hits
  for ( const auto& bank : m_banks ) {
    if ( bank.encode() || m_addEmptyBanks ) {
      // create, and put raw bank in raw event
      rawEvent->addBank( bank.id(), LHCb::RawBank::OT, OTBankVersion::SIM, createRawBank( bank ) );
      /// clear raw bank
      m_rawBank.clear();
    }
  }
  
  /// clear OTBanks
  std::for_each( m_banks.begin(), m_banks.end(), [](OTDAQ::OTBank& bank) { bank.clear(); } );

  return StatusCode::SUCCESS;
}

