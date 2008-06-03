// Include files 
// from std
#include <algorithm>

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// from Event
#include "Event/RawEvent.h"
#include "Event/RawBank.h"

// from Boost
#include "boost/lambda/bind.hpp"
#include "boost/lambda/lambda.hpp"

// local
#include "OTRawBankEncoder.h"
#include "OTDAQ/OTSpecificHeader.h"
#include "OTDAQ/GolHeader.h"
#include "OTDAQ/RawHit.h"

//-----------------------------------------------------------------------------
// Implementation file for class : OTRawBankEncoder
//
// 2008-05-22 : Jan Amoraal
//-----------------------------------------------------------------------------

namespace {

 template<class T> 
 void pipeToBuffer( const T& t, std::vector<unsigned char>& buffer ) {
  size_t curSize    = buffer.size();
  size_t objectSize = sizeof( t );
  buffer.resize( objectSize +curSize );
  std::memcpy( &buffer.front() + curSize, &t, objectSize );
 };

 struct CompareChannel {
   bool operator()( const OTDAQ::RawHit& lhs, const OTDAQ::RawHit& rhs ) {
     return lhs.channel() < rhs.channel() ;
   }
 };

 /// These simple classes are needed for sorting
 /// the channel ids according to Bank and Gol

 /// Simple Gol class
 /// One gol per X modules
 class OTGol {
   public:

   OTGol( size_t id ):m_id( id ), 
                      m_channels(),
                      m_encode(false) {};

   const size_t id() const { return m_id; };

   void addChannel( const LHCb::OTChannelID& channel ) { m_channels.push_back( channel ); };
   
   const size_t nChannels() const { return m_channels.size(); }; ///< == Number of hits in gol

   const bool encode() const { return m_encode; };
        
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
     for (size_t i = 1u, iEnd = m_nGols+1u; i != iEnd; ++i) m_gols.push_back( i );
   };

   const size_t id() const { return m_id; };
   
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

   const size_t nChannels() const { return m_nChannels; }; ///< == Number of hits in bank

   const bool encode() const { return m_encode; };
        
   const size_t nGols() const { return m_nGols; };
   
   const size_t nGolsToEncode() const { return m_nGolsToEncode; };
   
   std::vector<OTGol>::const_iterator firstGol() const { return m_gols.begin(); };
   
   std::vector<OTGol>::const_iterator lastGol() const { return m_gols.end(); };

   void clear() { 
     /// Clear gols
     std::for_each( m_gols.begin(), m_gols.end(), boost::lambda::bind( &OTGol::clear, boost::lambda::_1 ) ); 
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
DECLARE_TOOL_FACTORY( OTRawBankEncoder );

OTRawBankEncoder::OTRawBankEncoder( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IOTRawBankEncoder>(this);
  declareProperty( "NumberOfBanks"   , m_numberOfBanks = 48u                                );
  declareProperty( "NumberOfGols"    , m_numberOfGols  =  9u                                );
  declareProperty( "RawEventLocation", m_rawEventLocation = LHCb::RawEventLocation::Default );
}

OTRawBankEncoder::~OTRawBankEncoder() {}


StatusCode OTRawBankEncoder::initialize() {
  
  debug() << "Initialising OTRawbankEncoder" << endmsg;
  StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;

  // access to the channel map
  m_channelmaptool = tool<IOTChannelMapTool>("OTChannelMapTool");
  
  /// Create banks
  m_banks.reserve( m_numberOfBanks );
  /// Note Tell1s start from 1
  for ( size_t bank = 1u, bankEnd = m_numberOfBanks+1u; bank != bankEnd ; ++bank )  
    m_banks.push_back( OTBank( bank , m_numberOfGols ) );
  
  return sc;
}

const OTRawBankEncoder::OTRawBank& OTRawBankEncoder::createRawBank(const OTBank& bank) const {
  
  const bool isDebug = msgLevel( MSG::DEBUG );
  
  if ( isDebug ) debug() << "Creating OTRawBank for OTBank with id = " << bank.id() 
                         << " containing " << bank.nGolsToEncode() << " GOLs to decode and a total of " 
                         << bank.nChannels() << " channels." << endmsg;

  if ( isDebug ) debug() << "Start: Size of bank = " << m_rawBank.size() << endmsg;
  
  /// The first 4 bytes contain the OT Specific header.
  /// Don't fill it for simulation.
  std::vector<unsigned char> buffer;
  pipeToBuffer( OTDAQ::OTSpecificHeader( 0, 0, 0, bank.nGolsToEncode() ), buffer );
  
  /// Loop over ot gols and encode them
  /// Do this only for non-empty gols
  for ( std::vector<OTGol>::const_iterator gol = bank.firstGol(), golEnd = bank.lastGol(); 
        gol != golEnd ; ++gol ) {
    
    if ( gol->encode() ) {
      /// get the first channel
      std::vector<LHCb::OTChannelID>::const_iterator firstChannel = (*gol).firstChannel();
      
      /// Create gol header.
      pipeToBuffer( OTDAQ::GolHeader( 0, 
                                        firstChannel->station(), 
                                        firstChannel->layer()  ,
                                        firstChannel->quarter(),
                                        firstChannel->module() ,
                                        0,
                                        gol->nChannels() ), buffer );
      
      /// tmp vector of raw hits
      OTDAQ::RawHitContainer rawHits;
      rawHits.reserve( (*gol).nChannels() );
      for ( ; firstChannel != (*gol).lastChannel(); ++firstChannel ) {
        if ( isDebug ) debug() << " Gol ID  = " << gol->id()
                               << " Station = " << firstChannel->station()
                               << " Layer   = " << firstChannel->layer()
                               << " Quarter = " << firstChannel->quarter()
                               << " Module  = " << firstChannel->module() 
                               << " nHits   = " << gol->nChannels() << endmsg;
        
        size_t channel = m_channelmaptool->channel( (*firstChannel) ) ;
        rawHits.push_back( OTDAQ::RawHit( 0, channel, firstChannel->tdcTime() ) );
      }
      
      /// Sort according to channel in Tell1
      std::sort( rawHits.begin(), rawHits.end(), CompareChannel() );
      
      // add padding i.e. empty hit
      if ( rawHits.size()%2 ) rawHits.push_back( OTDAQ::RawHit() );
      
      for ( OTDAQ::RawHitContainer::const_iterator it = rawHits.begin(); it != rawHits.end(); ++it ) 
        pipeToBuffer( *it, buffer );
  
      m_rawBank.resize( buffer.size()/sizeof( int ) );
      std::memcpy( &( m_rawBank.front() ), &( buffer.front() ), buffer.size() );
      
      if ( isDebug ) debug() << "End: Size of bank = " << m_rawBank.size() << endmsg;
    }
  }
  
  return m_rawBank;
}

StatusCode OTRawBankEncoder::encodeChannels( const std::vector<LHCb::OTChannelID>& channels ) const {
 
  const bool isDebug = msgLevel( MSG::DEBUG );
      
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
  for ( std::vector<LHCb::OTChannelID>::const_iterator chan = channels.begin(), chanEnd = channels.end();
        chan != chanEnd; ++chan ) {
    if ( isDebug ) debug() << "ChannelID = " << (*chan) << endmsg;
    const size_t bankID = channelToBank( (*chan) );
    if ( bankID == 0u || bankID > m_numberOfBanks ) {
      error() << "Trying to add channel to non-existent bank with id " << bankID <<  ", skipping!" << endmsg;
    } else {
      if ( isDebug ) debug() << "Adding channel " << (*chan) << " to bank with id " << bankID << endmsg;
      /// Remember Tell1s start from 1
      m_banks[bankID-1u].addChannel( (*chan) );
      if ( isDebug ) debug() << "Added channel " << (*chan) << " to bank with id " << m_banks[bankID-1u].id() << endmsg;
    }
  } 
  
  if ( isDebug ) {
    for ( OTBanks::const_iterator bank = m_banks.begin(), bankEnd = m_banks.end(); bank != bankEnd; ++bank ) {
      debug() << "Bank id = " << bank->id() << " number of channels = " << bank->nChannels() << endmsg;
      for ( std::vector<OTGol>::const_iterator gol = bank->firstGol(), golEnd = bank->lastGol(); gol != golEnd; ++gol ) {
        debug() << "  ===> Gol id = " << gol->id() << endmsg;
        for ( std::vector<LHCb::OTChannelID>::const_iterator chan = gol->firstChannel(), chanEnd = gol->lastChannel(); chan != chanEnd; ++chan ) {
          debug() << "   ++> Channel id = " << (*chan) << endmsg;
        }
      }
    }
  }

  /// Loop over ot banks and create raw banks
  /// We do this only for banks that contain hits
  for ( OTBanks::const_iterator bank = m_banks.begin(), bankEnd = m_banks.end(); bank != bankEnd; ++bank ) {
    if ( bank->encode() ) {
      /// create RawBank
      const OTRawBank& rawBank = createRawBank( (*bank) );
      // put raw bank in raw event
      rawEvent->addBank( bank->id(), LHCb::RawBank::OT, 3, rawBank );
      /// clear raw bank
      m_rawBank.clear();
    }
  }
  
  /// clear OTBanks
  std::for_each( m_banks.begin(), m_banks.end(), boost::lambda::bind( &OTBank::clear, boost::lambda::_1 ) );

  return StatusCode::SUCCESS;
}

