// Include files

#if !(defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L)
#include <boost/assign/list_of.hpp>
#endif
#include <cmath>
#include <sstream>

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ToolHandle.h"

// from Gaudi
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/SystemOfUnits.h"

// Event
#include "Event/RawEvent.h"
#include "Event/OTLiteTime.h"

// OTDAQ
#include "OTDAQ/IOTRawBankDecoder.h"

// local
#include "OTDAQ/IndexedModuleDataHolder.h"


// forward declarations
namespace LHCb
{
  class RawBank ;
}

namespace LocalHelpers
{
  class DetectorHitData ;
}

class OTMultiBXRawBankDecoder : public GaudiTool,
				virtual public IOTRawBankDecoder,
				virtual public IIncidentListener
{

public:

  /// Standard constructor
  OTMultiBXRawBankDecoder( const std::string& type,
			   const std::string& name,
			   const IInterface* parent);

  /// Destructor
  virtual ~OTMultiBXRawBankDecoder( ) ; ///< Destructor

  /// Tool initialization
  virtual StatusCode initialize();

  /// Tool finalize
  virtual StatusCode finalize();

  /// Decode data for a single module
  virtual LHCb::OTLiteTimeRange decodeModule( const LHCb::OTChannelID& moduleId ) const ;

  /// Decode all gol headers
  virtual StatusCode decodeGolHeaders() const { return m_decoder->decodeGolHeaders() ; }

  /// Decode all gol headers.
  virtual StatusCode decodeGolHeaders(const LHCb::RawEvent& rawevent) const { return m_decoder->decodeGolHeaders(rawevent) ;}

  /// Decode all modules
  virtual StatusCode decode( LHCb::OTLiteTimeContainer& ottimes ) const ;

  /// Translate the raw bank in an ot-specific raw bank.
  virtual StatusCode decode( OTDAQ::RawEvent& otevent ) const ;

  /// Retrieve the total number of hits in the OT without actually
  /// decoding the modules. Useful in pattern reco to remove full
  /// events.
  virtual size_t totalNumberOfHits() const ;

  /// Get the conversion factor
  virtual double nsPerTdcCount() const { return m_decoder->nsPerTdcCount() ; }

  /// Create a single OTLiteTime
  virtual LHCb::OTLiteTime time( LHCb::OTChannelID channel ) const ;

protected:
  virtual void handle ( const Incident& incident );
  StatusCode decodeAll() const ;

private:
  ToolHandle<IOTRawBankDecoder> m_decoder ;
  std::vector<std::string> m_rawEventLocations ;
  bool m_selectEarliestHit ;
  mutable LocalHelpers::DetectorHitData* m_hitdata ;
};


namespace LocalHelpers
{

  struct ModuleHitData : public LHCb::OTLiteTimeContainer
  {
  public:
    LHCb::OTLiteTimeRange range() const { return LHCb::OTLiteTimeRange(begin(),end()) ; }
  } ;

  class DetectorHitData : public OTDAQ::IndexedModuleDataHolder<ModuleHitData>
  {
  public:
    DetectorHitData() : m_isDecoded(false) {}
    void clear() {
      m_isDecoded = false ;
      for(iterator imod = begin(); imod!= end(); ++imod)
        imod->clear() ;
    }
    size_t totalNumberOfHits() const {
      size_t rc(0) ;
      for(const_iterator imod = begin() ; imod != end(); ++imod) rc += imod->size() ;
      return rc ;
    }
    bool isDecoded() const { return m_isDecoded ; }
    void setIsDecoded(bool b=true) { m_isDecoded = b ; }

  private:
    bool m_isDecoded ;

  } ;

}

using namespace LHCb;

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( OTMultiBXRawBankDecoder )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
OTMultiBXRawBankDecoder::OTMultiBXRawBankDecoder( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_decoder("OTRawBankDecoder/OTSingleBXRawBankDecoder"),
    m_hitdata(0)
{
  declareInterface<IOTRawBankDecoder>(this);
  declareProperty("RawBankDecoder",m_decoder);
  declareProperty("RawEventLocations",m_rawEventLocations=
#if defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L
		  {LHCb::RawEventLocation::Default,
		   "Prev1/DAQ/RawEvent",
		   "Next1/DAQ/RawEvent"}
#else
                  boost::assign::list_of(LHCb::RawEventLocation::Default)
                  ("Prev1/DAQ/RawEvent")("Next1/DAQ/RawEvent")
#endif
		  );
  declareProperty("SelectEarliestHit",m_selectEarliestHit=true) ;
}

//=============================================================================
// Destructor
//=============================================================================
OTMultiBXRawBankDecoder::~OTMultiBXRawBankDecoder() {}

//=============================================================================

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode OTMultiBXRawBankDecoder::initialize()
{
  std::stringstream msg ;
  msg <<  "Merging raw events from following locations: " ;
  for( std::vector<std::string>::const_iterator ilocation = m_rawEventLocations.begin() ;
       ilocation != m_rawEventLocations.end(); ++ilocation)
    msg << "\'" << *ilocation << "\', " ;
  info() << msg.str() << endmsg ;

  StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  sc = m_decoder.retrieve() ;
  if( sc.isFailure() ) {
    error() << "Failed to retieve decoder" << endmsg ;
    return sc ;
  } else {
    info() << "retrieved single BX decoder: " << m_decoder->type() << "/" << m_decoder->name() << endmsg ;
  }

  // Setup incident services
  incSvc()->addListener( this, IncidentType::BeginEvent );

  // initialize the decoder data. this things basically contains the decoded event
  m_hitdata = new LocalHelpers::DetectorHitData() ;

  return sc ;
}

//=============================================================================
// Finalize
//=============================================================================
StatusCode OTMultiBXRawBankDecoder::finalize()
{
  if( m_hitdata ) delete m_hitdata ;
  m_decoder.release().ignore() ;
  return GaudiTool::finalize() ;
}

//=============================================================================
// Handle for incident service
//=============================================================================

void OTMultiBXRawBankDecoder::handle ( const Incident& incident )
{
  if ( IncidentType::BeginEvent == incident.type() ) m_hitdata->clear() ;
}


size_t OTMultiBXRawBankDecoder::totalNumberOfHits() const
{
  if( !m_hitdata->isDecoded() ) decodeAll() ;
  return m_hitdata->totalNumberOfHits() ;
}

LHCb::OTLiteTimeRange OTMultiBXRawBankDecoder::decodeModule( const LHCb::OTChannelID& moduleid ) const
{
  if( !m_hitdata->isDecoded() ) decodeAll() ;
  return m_hitdata->module(moduleid.station(),moduleid.layer(),moduleid.quarter(),moduleid.module()).range() ;
}

StatusCode OTMultiBXRawBankDecoder::decode( LHCb::OTLiteTimeContainer& ottimes ) const
{
  if( !m_hitdata->isDecoded() ) decodeAll() ;
  size_t numhits = m_hitdata->totalNumberOfHits() ;
  ottimes.clear() ;
  ottimes.reserve( numhits ) ;
  for( LocalHelpers::DetectorHitData::const_iterator imod = m_hitdata->begin();
       imod != m_hitdata->end(); ++imod)
    ottimes.insert(ottimes.end(), imod->begin(), imod->end() ) ;
  return StatusCode::SUCCESS ;
}

StatusCode OTMultiBXRawBankDecoder::decodeAll() const
{
  StatusCode sc = StatusCode::SUCCESS ;
  for( std::vector<std::string>::const_iterator ilocation = m_rawEventLocations.begin() ;
       ilocation != m_rawEventLocations.end() && sc.isSuccess(); ++ilocation) {
    // Get the raw event if it exists at this location. Issue a warning otherwise
    const LHCb::RawEvent* event = NULL;
    event = getIfExists<LHCb::RawEvent>(evtSvc(),*ilocation);
    if( NULL == event ) {
      Warning(std::string("No raw event buffer at location ") + *ilocation,
	      StatusCode::SUCCESS,0).ignore() ;
    } else {
      // from the name of the location, get the time offset
      double eventoffset(0) ;
      size_t pos(0) ;
      if(       (pos= ilocation->find("Prev") ) != std::string::npos) {
        int index = atoi( &((*ilocation)[pos+4]) ) ;
        eventoffset = index * -25*Gaudi::Units::ns ;
      } else if( (pos= ilocation->find("Next") ) != std::string::npos) {
        int index = atoi( &((*ilocation)[pos+4]) ) ;
        eventoffset = index * 25*Gaudi::Units::ns ;
      }
      debug() << *ilocation << " ----> time offset = " << eventoffset << endmsg ;

      // now set up the decoder for this event. without this call, it would just take the default event location.
      sc = m_decoder->decodeGolHeaders( *event ) ;
      if( sc.isSuccess() ) {
	// then get the hits
        LHCb::OTLiteTimeContainer ottimes ;
        sc = m_decoder->decode(ottimes) ;
        if( sc.isSuccess() ) {
          // we could optimize this by asking for the hits per module,
          // but that's not worth it right now.
          for( LHCb::OTLiteTimeContainer::const_iterator ihit = ottimes.begin() ;
               ihit != ottimes.end(); ++ihit ) {
            // create a new hit, adjusting for the offset
            LHCb::OTLiteTime newhit( ihit->channel(), eventoffset + ihit->calibratedTime() ) ;
            // find the module
            LocalHelpers::ModuleHitData& module = m_hitdata->module( ihit->channel() ) ;
            // check that this hit is not yet there. if 'select earliest
            // is set', we replace the previous hit if the new hit is
            // earlier. if not, we allow hits with different time to
            // co-exist.
            LHCb::OTLiteTimeContainer::iterator jhit = module.begin() ;
            if( m_selectEarliestHit ) {
              for( ; jhit != module.end() &&
                     newhit.channel().straw() != jhit->channel().straw() ; ++jhit) {}
              if( jhit != module.end() &&
                  jhit->calibratedTime() > newhit.calibratedTime() )
                *jhit = newhit ;
            } else {
              for( ; jhit != module.end() &&
                     !(newhit.channel().straw() == jhit->channel().straw() &&
                       std::abs( jhit->calibratedTime() - newhit.calibratedTime() )<1) ; ++jhit){}
            }
            if( jhit == module.end() )
              module.push_back( newhit ) ;
          }
        }
      }
    }
  }
  m_hitdata->setIsDecoded( true ) ;
  return sc ;
}

StatusCode OTMultiBXRawBankDecoder::decode( OTDAQ::RawEvent& otrawevent) const
{
  return m_decoder->decode( otrawevent ) ;
}

LHCb::OTLiteTime OTMultiBXRawBankDecoder::time( LHCb::OTChannelID channel ) const
{
  if( !m_hitdata->isDecoded() ) decodeAll() ;
  const LocalHelpers::ModuleHitData& moduledata = m_hitdata->module( channel ) ;
  // if we would sort them, we could use a binary search ...
  LHCb::OTLiteTimeContainer::const_iterator jhit =  moduledata.begin() ;
  for( ; jhit != moduledata.end() && channel.straw() != jhit->channel().straw(); ++jhit) {}
  LHCb::OTLiteTime rc ;
  if( jhit != moduledata.end() ) rc = *jhit ;
  else {
    error() << "Cannot find channel in list of hits" << endmsg ;
  }
  return rc ;
}
