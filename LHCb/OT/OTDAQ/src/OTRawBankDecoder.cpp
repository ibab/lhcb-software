// $Id: OTRawBankDecoder.cpp,v 1.7 2007-11-26 11:08:30 wouter Exp $
// Include files
#include <algorithm>

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/SystemOfUnits.h"

// from Detector
#include "OTDet/DeOTDetector.h"
#include "OTDet/DeOTModule.h"

// Event
#include "Event/RawBank.h"
#include "Event/RawEvent.h"

// local
#include "RawHit.h"
#include "GolHeaderDC06.h"
#include "GolHeaderV3.h"

//#include "Event/DataWord.h"

// GSL
#include <algorithm>

// local
#include "Event/OTBankVersion.h"

// Tool interface
#include "OTDAQ/IOTReadOutWindow.h"

// local
#include "OTRawBankDecoder.h"


namespace OTRawBankDecoderHelpers
{


  class Module
  {
  public:
    Module() : m_detelement(0), m_isdecoded(false), m_data(0) { m_ottimes.reserve(16) ; }
    void clearevent() { m_isdecoded=false ; m_size=0; m_data=0 ; m_ottimes.clear() ; }
    void setData( unsigned int size, const unsigned short* data, unsigned int bankversion) { 
      m_size = size ;  m_data = data ; m_bankversion = bankversion ; }
    bool isDecoded() const { return m_isdecoded ; }
    void setIsDecoded(bool b=true) { m_isdecoded = b ; }
    const unsigned short* data() const { return m_data ; }
    unsigned int size() const { return m_size ; }
    LHCb::OTLiteTimeContainer& ownedottimes() { return m_ottimes ; }
    LHCb::OTLiteTimeRange ottimes() const { return LHCb::OTLiteTimeRange(m_ottimes.begin(),m_ottimes.end()) ; }
    void setDetElement( const DeOTModule& e) ;
    const DeOTModule& detelement() const { return *m_detelement ; }
    
    size_t decodeDC06(double tdcconversion) ;
    size_t decodeV3(double tdcconversion) ;
    size_t decode(double tdcconversion) ;
    
  private:
    void addHitDC06(unsigned short data, float tdcconversion) ;
     
  private:
    const DeOTModule* m_detelement ;
    unsigned int m_station ;
    unsigned int m_layer ;
    unsigned int m_quarter ;
    unsigned int m_module ;
    
    bool m_isdecoded ;
    unsigned int m_size ;
    const unsigned short* m_data ;
    unsigned int m_bankversion ;
    LHCb::OTLiteTimeContainer m_ottimes ;
  } ;
  
  void Module::setDetElement( const DeOTModule& e) 
  {
    // cache for speed
    m_detelement = &e ;
    LHCb::OTChannelID moduleid = e.elementID() ;
    m_station = moduleid.station() ;
    m_layer   = moduleid.layer() ;
    m_quarter = moduleid.quarter() ;
    m_module  = moduleid.module() ;
  }
  
  inline void Module::addHitDC06(unsigned short data, float tdcconversion) 
  { 
    OTDAQ::RawHit hit(data) ;
    unsigned int otis    = hit.otis() ;
    unsigned int channel = hit.channel() ;
    unsigned int straw   = otis/2 ? 32 * (6-otis) - channel : 32 * otis + channel + 1 ;
    LHCb::OTChannelID channelid(m_station,m_layer,m_quarter,m_module,straw,hit.time()) ;
    float t0 = m_detelement->strawT0( straw ) ;
    m_ottimes.push_back( LHCb::OTLiteTime( channelid, channelid.tdcTime() * tdcconversion - t0) );
  }
  
  inline size_t Module::decodeDC06(double tdcconversion) 
  {
    if(!m_isdecoded) {
      if( m_size != 0 ) {
        // now, DC06 has a padding problem: The padded hits appears before the last 
        // hit. So, we need to fix that. 
        bool haspaddinghit = m_data[m_size-2] == 0 ;
        const unsigned short* begin = m_data ;
        const unsigned short* end   = begin + (haspaddinghit ? m_size -2 : m_size) ;
        m_ottimes.reserve( m_size ) ;
        for( const unsigned short* ihit = begin ; ihit != end ; ++ihit)
          addHitDC06(*ihit,tdcconversion) ;
        if(haspaddinghit) addHitDC06(m_data[m_size-1],tdcconversion) ;
      }
      m_isdecoded = true ;
    }
    return m_ottimes.size() ;
  }

  inline size_t Module::decodeV3(double tdcconversion) 
  {
    if(!m_isdecoded) {
      if( m_size != 0 ) {
        const unsigned short* begin = m_data ;
        const unsigned short* end   = begin + m_size ;
        m_ottimes.reserve( m_size ) ;
        for( const unsigned short* ihit = begin ; ihit != end ; ++ihit)
          addHitDC06(*ihit,tdcconversion) ;
      }
      m_isdecoded = true ;
    }
    return m_ottimes.size() ;
  }
  
  inline size_t Module::decode(double tdcconversion)
  {
    size_t rc(0) ;
    switch(m_bankversion) {
    case OTBankVersion::DC06: 
      rc = decodeDC06(tdcconversion) ;
      break;
    case OTBankVersion::v3:
      rc = decodeV3(tdcconversion) ;
      break;
    }
    return 0 ;
  }
  
  class Detector
  {
  public:
    Detector(const DeOTDetector& det) : m_golHeadersLoaded(false) 
    {  
      for(DeOTDetector::Modules::const_iterator imod = det.modules().begin() ;
          imod != det.modules().end(); ++imod) 
        module((*imod)->stationID(),(*imod)->layerID(),(*imod)->quarterID(),(*imod)->moduleID()).setDetElement(**imod) ;
    }
    
    void clearevent() {
      m_golHeadersLoaded = false ;
      for(iterator imod = begin(); imod!= end(); ++imod)
        imod->clearevent() ;
    }
    Module& module(const unsigned int station, const unsigned int layer, 
                   const unsigned int quarter, const unsigned int module) {
      return m_modules[station-1][layer][quarter][module-1] ;
    }
    bool golHeadersLoaded() const { return m_golHeadersLoaded ; }
    void setGolHeadersLoaded(bool b=true) { m_golHeadersLoaded = b ; }
    typedef Module* iterator  ;
    iterator begin() { return &(m_modules[0][0][0][0]) ; }
    iterator end() { return &(m_modules[NumStations-1][NumLayers-1][NumQuadrants-1][NumModules]); }
    
  private:
    enum { NumStations=3, NumLayers=4, NumQuadrants=4, NumModules=9 } ;
    Module m_modules[NumStations][NumLayers][NumQuadrants][NumModules];
    bool m_golHeadersLoaded ;
  } ;
  
}

using namespace LHCb;

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( OTRawBankDecoder );
  

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
OTRawBankDecoder::OTRawBankDecoder( const std::string& type,
				    const std::string& name,
				    const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_countsPerBX(64),
    m_numberOfBX(3),
    m_timePerBX(25*Gaudi::Units::ns),
    m_forcebankversion(OTBankVersion::UNDEFINED),
    m_otdet(0),
    m_nsPerTdcCount(m_timePerBX/m_countsPerBX),
    m_detectordata(0)
{
  declareInterface<IOTRawBankDecoder>(this);
  declareProperty("countsPerBX", m_countsPerBX );
  declareProperty("numberOfBX", m_numberOfBX );
  declareProperty("timePerBX", m_timePerBX );
  declareProperty("ForceBankVersion", m_forcebankversion = OTBankVersion::UNDEFINED) ;
}
//=============================================================================
// Destructor
//=============================================================================
OTRawBankDecoder::~OTRawBankDecoder() {}

//=============================================================================

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode OTRawBankDecoder::initialize()
{
  
  debug()<<"initializing OTRawBankDecoder"<<endmsg;
  
  StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  // Setup incident services
  incSvc()->addListener( this, IncidentType::EndEvent );
  
  // Loading OT Geometry from XML
  m_otdet = getDet<DeOTDetector>(DeOTDetectorLocation::Default );
  m_detectordata = new OTRawBankDecoderHelpers::Detector(*m_otdet) ;
  
  // Read out window tool
  IOTReadOutWindow* aReadOutWindow = tool<IOTReadOutWindow>("OTReadOutWindow");
  m_startReadOutGate  = aReadOutWindow->startReadOutGate();
  
  m_nsPerTdcCount = m_timePerBX/ double(m_countsPerBX);
  
  if( m_forcebankversion != OTBankVersion::UNDEFINED )
    warning() << "Forcing bank version to be " << m_forcebankversion << endreq ;

  return StatusCode::SUCCESS;
};

//=============================================================================
// Finalize
//=============================================================================
StatusCode OTRawBankDecoder::finalize()
{
  if( m_detectordata ) {
    delete m_detectordata ;
    m_detectordata=0;
  }
  return GaudiTool::finalize() ;
}

//=============================================================================
// Handle for incident service
//=============================================================================

void OTRawBankDecoder::handle ( const Incident& incident )
{
  if ( IncidentType::EndEvent == incident.type() ) m_detectordata->clearevent() ;
}


//=============================================================================
//Decode the gol headers in the raw banks. Each header is assigned
//to its corresponding module. The contents of the gol are only
//decoded once the module data is asked for. (That is the 'decoding
//on demand' part.)
//=============================================================================

StatusCode OTRawBankDecoder::decodeGolHeaders() const
{
  // info() << "OTRawBankDecoder::decodeGolHeaders()" << endreq ;
  if (msgLevel(MSG::DEBUG)) debug() << "Decoding GOL headers in OTRawBankDecoder" << endreq ;
  
  // Retrieve the RawEvent:
  RawEvent* event = get<RawEvent>("DAQ/RawEvent" );
  
  // Get the buffers associated with OT
  const std::vector<RawBank*>& OTBanks = event->banks(RawBank::OT );
  
  // Loop over vector of banks (The Buffer)
  bool decodingerror(false) ;
  for (std::vector<RawBank*>::const_iterator  ibank = OTBanks.begin();
       ibank != OTBanks.end() ; ++ibank) {
    // get bank version
    bool bankdecodingerror(false) ;
    int bVersion = m_forcebankversion != OTBankVersion::UNDEFINED ? m_forcebankversion : (*ibank)->version();
    unsigned int nTell1 = 0u;
    switch( bVersion ) {
    case OTBankVersion::v1:
      nTell1 = 3 ;
      break ;
    case OTBankVersion::DC06:
    case OTBankVersion::v3:
      nTell1 = 1 ;
      break ;
    default:
      error() << "Cannot decode OT raw buffer bank version "
              << bVersion << " with this version of OTDAQ" << endmsg;
      bankdecodingerror = true ;
    } ;
    
    if( !bankdecodingerror ) {
      
      if (msgLevel(MSG::DEBUG))
        debug() << " Bank Nr " << (*ibank)->sourceID() << " with Size "
                << (*ibank)->size()/4 << " " << (*ibank)->version() << endmsg;
      
      const unsigned int* begin = (*ibank)->data() + nTell1 ;
      const unsigned int* end   = (*ibank)->data() + (*ibank)->size()/4 ;
      const unsigned int* idata ;
      unsigned int buffersize,station,layer,quarter,module,numhits ;
      
      for( idata = begin ; idata < end && !bankdecodingerror; ++idata ) {
        
	// I'd rather do this with a function pointer, such that we don't need the fork.
        if( bVersion == OTBankVersion::v2 ) {
          // in DC06 'size' is the size of the data-block (in units of 4 bytes). in real data it is the number of hits.
          OTDAQ::GolHeaderDC06 golHeader(*idata) ;
          buffersize = golHeader.size();
          station = golHeader.station();
          layer = golHeader.layer();
          quarter = golHeader.quarter();
          module = golHeader.module();
          numhits = 2*buffersize ;
        } else {
          OTDAQ::GolHeaderV3 golHeader(*idata) ;
          numhits = golHeader.size();
          station = golHeader.station();
          layer = golHeader.layer();
          quarter = golHeader.quarter();
          module = golHeader.module();
          buffersize = numhits/2 + numhits%2 ;
        } 
        
        if(station<1 || station>3 || layer>3 || quarter>3 || module<1 || module >9) {
          error() << "OTRawBankDecoder: invalid gol header. data = " << *idata << " module ID=("
                  << station << "," << layer << "," << quarter << "," << module << "). " 
		  << endmsg ;
        } else {
          const unsigned short* firsthit = reinterpret_cast<const unsigned short*>(idata+1) ;
          m_detectordata->module(station,layer,quarter,module).setData(numhits,firsthit,bVersion) ;
        }
        idata += buffersize ;
      }
      
      if(!bankdecodingerror && idata != end) {
        error() << "OTRawBankDecoder: gol headers do not add up to buffer size. " << idata << " " << end << endreq ;
        bankdecodingerror = true ;
      }
    }
    decodingerror |= bankdecodingerror ;
  }
  
  // make sure we don't call this until the next event
  m_detectordata->setGolHeadersLoaded(true) ; 
  
  return decodingerror ? StatusCode::FAILURE : StatusCode::SUCCESS ;
}

size_t OTRawBankDecoder::decodeModule( OTRawBankDecoderHelpers::Module& moduledata ) const 
{
  if( !m_detectordata->golHeadersLoaded() ) decodeGolHeaders().ignore() ;
  if( !moduledata.isDecoded() ) moduledata.decode( m_nsPerTdcCount ) ;
  return moduledata.ottimes().size() ;
}

LHCb::OTLiteTimeRange OTRawBankDecoder::decodeModule( const LHCb::OTChannelID& moduleid ) const
{
  OTRawBankDecoderHelpers::Module& moduledata =
    m_detectordata->module(moduleid.station(),moduleid.layer(),moduleid.quarter(),moduleid.module()) ;
  if( !moduledata.isDecoded() ) OTRawBankDecoder::decodeModule(moduledata) ;
  return moduledata.ottimes() ;
}

StatusCode OTRawBankDecoder::decode( LHCb::OTLiteTimeContainer& ottimes ) const
{
  // decode all modules. keep track of total number of hits.
  size_t numhits(0) ;
  for( OTRawBankDecoderHelpers::Module* imod = m_detectordata->begin(); 
       imod != m_detectordata->end(); ++imod)
    numhits += decodeModule( *imod ) ;
  // reserve and copy
  ottimes.clear() ;
  ottimes.reserve( numhits ) ;
  for( OTRawBankDecoderHelpers::Module* imod = m_detectordata->begin(); 
       imod != m_detectordata->end(); ++imod)
    ottimes.insert(ottimes.end(), imod->ottimes().begin(), imod->ottimes().end() ) ;
  // We'll need some proper error handling.
  return StatusCode::SUCCESS ;
}
