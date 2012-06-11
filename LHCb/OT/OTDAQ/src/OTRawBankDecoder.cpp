
// Include files
#include <algorithm>
#include <numeric>
#include <sstream>

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

// OTDAQ
#include "OTDAQ/RawEvent.h"
#include "OTDAQ/IndexedModuleDataHolder.h"

// local
#include "GolHeaderDC06.h"

//#include "Event/DataWord.h"

// local
#include "Event/OTBankVersion.h"

// Tool interface
#include "OTDAQ/IOTReadOutWindow.h"
#include "OTDAQ/IOTChannelMapTool.h"

// local
#include "OTRawBankDecoder.h"


namespace OTRawBankDecoderHelpers
{
  

  class Module
  {
  public:
    Module() : m_detelement(0), m_channelmap(0), m_data(0), m_tdcconversion(0), m_size(0),
               m_bankversion(OTBankVersion::UNDEFINED), m_isdecoded(false) 
    { m_ottimes.reserve(16) ; }
    void clearevent() { m_isdecoded=false ; m_size=0; m_data=0 ; m_ottimes.clear() ; m_tdcconversion = 0; }
    void setData( unsigned int size, const unsigned short* data,
                  int bankversion, double tdcconversion,
                  const std::pair<double,double>& window) { 
      m_size = size;
      m_data = data;
      m_bankversion = bankversion;
      m_tdcconversion = tdcconversion;
      m_window = window;
    }
    bool isDecoded() const { return m_isdecoded ; }
    void setIsDecoded(bool b=true) { m_isdecoded = b ; }
    const unsigned short* data() const { return m_data ; }
    unsigned int size() const { return m_size ; }
    LHCb::OTLiteTimeContainer& ownedottimes() { return m_ottimes ; }
    LHCb::OTLiteTimeRange ottimes() const { return LHCb::OTLiteTimeRange(m_ottimes.begin(),m_ottimes.end()) ; }
    void setDetElement( const DeOTModule& e ) ;
    void setChannelMap( const OTDAQ::ChannelMap::Module& map ) { m_channelmap = &map ; }
    void setTimeWindow( double tmin, double tmax ) { m_window.first = tmin; m_window.second = tmax; }
    const DeOTModule& detelement() const { return *m_detelement ; }
    
    inline bool isHitInWindow(unsigned short data) const;
    size_t decodeDC06() ;
    size_t countHitsInWindowDC06() const;
    size_t decodeV3() ;
    size_t countHitsInWindowV3() const;
    size_t decode() ;
    size_t countHitsInWindow() const;
    const DeOTModule& detElement() const { return *m_detelement ; }
  private:
    void addHit(unsigned short data ) ;
   
    // optimize layout wrt. memory alignment requirements
    const DeOTModule* m_detelement ;
    const OTDAQ::ChannelMap::Module* m_channelmap ;
    const unsigned short* m_data ;
    std::pair<double,double> m_window;
    double m_tdcconversion;
    unsigned int m_station ;
    unsigned int m_layer ;
    unsigned int m_quarter ;
    unsigned int m_module ;
    unsigned int m_size ;
    int m_bankversion ;
    LHCb::OTLiteTimeContainer m_ottimes ;
    bool m_isdecoded ;
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
  
  inline bool Module::isHitInWindow(unsigned short data) const
  { 
    OTDAQ::RawHit hit(data) ;
    unsigned int straw = m_channelmap->straw( hit.channel() ) ;
    double t = hit.time()*m_tdcconversion - m_detelement->strawT0( straw );
    return m_window.first < t && t < m_window.second;
  } 

  inline void Module::addHit(unsigned short data ) 
  { 
    OTDAQ::RawHit hit(data) ;
    unsigned int straw = m_channelmap->straw( hit.channel() ) ;
    unsigned int tdctime = hit.time() ;
    double t0 = m_detelement->strawT0( straw ) ;
    double t = tdctime * m_tdcconversion - t0 ;
    if (m_window.second<m_window.first || (  m_window.first < t && t < m_window.second ) ) {
        LHCb::OTChannelID channelid(m_station,m_layer,m_quarter,m_module,straw,tdctime) ;
        m_ottimes.push_back( LHCb::OTLiteTime( channelid, t ) );
    }
  } 

  inline size_t Module::countHitsInWindowDC06()  const
  {
        if (m_isdecoded) return m_ottimes.size();
        if (m_window.second<m_window.first) return size();
        // now, DC06 has a padding problem: The padded hits appears before the last 
        // hit. So, we need to fix that. 
        bool haspaddinghit = m_data[m_size-2] == 0 ;
        const unsigned short* begin = m_data ;
        const unsigned short* end   = begin + (haspaddinghit ? m_size -2 : m_size) ;
        size_t n(0);
        for( const unsigned short* ihit = begin ; ihit != end ; ++ihit)
          if (isHitInWindow(*ihit)) ++n;
        if(haspaddinghit && isHitInWindow(m_data[m_size-1])) ++n;
        return n;
  }
  
  inline size_t Module::decodeDC06() 
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
          addHit(*ihit) ;
        if(haspaddinghit) addHit(m_data[m_size-1]) ;
      }
      m_isdecoded = true ;
    }
    return m_ottimes.size() ;
  }

  inline size_t Module::countHitsInWindowV3() const
  {
    if (m_isdecoded) return m_ottimes.size();
    if (m_window.second<m_window.first) return size();
    const unsigned short* begin = m_data ;
    const unsigned short* end   = begin + m_size ;
    size_t n(0);
    for( const unsigned short* ihit = begin ; ihit != end ; ++ihit)  
        if (isHitInWindow(*ihit) ) ++n;
    return n;
  }

  inline size_t Module::decodeV3() 
  {
    if(!m_isdecoded) {
      if( m_size != 0 ) {
        const unsigned short* begin = m_data ;
        const unsigned short* end   = begin + m_size ;
        m_ottimes.reserve( m_size ) ;
        for( const unsigned short* ihit = begin ; ihit != end ; ++ihit) addHit(*ihit) ;
      }
      m_isdecoded = true ;
    }
    return m_ottimes.size() ;
  }
  
  inline size_t Module::decode()
  {
    size_t rc(0) ;
    switch(m_bankversion) {
      case OTBankVersion::DC06: 
        rc = decodeDC06() ;
        break;
        // Note: SIM and v3 currently (22/07/2008) uses same decoding.
        //       If SIM changes w.r.t. to the real decoding then we'll need
        //       to change it here. 
      case OTBankVersion::SIM:
      case OTBankVersion::v3:
        rc = decodeV3() ;
        break;
    }
    return rc ;
  }
  inline size_t Module::countHitsInWindow() const
  {
    size_t rc(0) ;
    switch(m_bankversion) {
      case OTBankVersion::DC06: 
        rc = countHitsInWindowDC06() ;
        break;
        // Note: SIM and v3 currently (22/07/2008) uses same decoding.
        //       If SIM changes w.r.t. to the real decoding then we'll need
        //       to change it here. 
      case OTBankVersion::SIM:
      case OTBankVersion::v3:
        rc = countHitsInWindowV3() ;
        break;
    }
    return rc ;
  }

  struct add { 
      inline size_t operator()(size_t c, const Module& m) { return c + m.size(); } 
  };
  struct addInWindow{ 
      inline size_t operator()(size_t c, const Module& m) { return c + m.countHitsInWindow(); } 
  };
  
  class Detector : public OTDAQ::IndexedModuleDataHolder<Module>
  {
  public:
    Detector(const DeOTDetector& det,
             const OTDAQ::ChannelMap::Detector& channelmap) : m_event(0)
    {  
      for(DeOTDetector::Modules::const_iterator imod = det.modules().begin() ;
          imod != det.modules().end(); ++imod) {
        Module& amodule = module((*imod)->stationID(),(*imod)->layerID(),(*imod)->quarterID(),(*imod)->moduleID()) ;
        amodule.setDetElement(**imod) ;
        amodule.setChannelMap( channelmap.module((*imod)->stationID(),
                                                 (*imod)->layerID(),
                                                 (*imod)->quarterID(),
                                                 (*imod)->moduleID()   ) ) ;
      }
    }
    
    void clearevent() {
      m_event = 0 ;
      for(iterator imod = begin(); imod != end(); ++imod) imod->clearevent() ;
    }
    
    const LHCb::RawEvent* rawEvent() const { return m_event ; }
    void setRawEvent( const LHCb::RawEvent* ev) { m_event = ev ; }
    bool golHeadersLoaded() const { return m_event != 0 ; }
    
    size_t totalNumberOfHits() const { 
        //size_t ntot = std::accumulate( begin(), end(), size_t(0), add() ); 
        size_t nwin = totalNumberOfHitsInWindow(); // ns...
        //std::cout << " ntot = " << ntot << " nInWindow = " << nwin << std::endl;
        return nwin;

    }

    size_t totalNumberOfHitsInWindow() const {
        return std::accumulate( begin(), end(), size_t(0), addInWindow() ); 
    }

    bool isTotalNumberOfHitsLessThen(size_t nmax) const
    {
        size_t n = 0;
        for(const_iterator imod = begin(); imod!= end(); ++imod)
        {
            n += imod->countHitsInWindow();
            if(n >= nmax) return false;
        }
        return true;
    }
    
  private: 
    const LHCb::RawEvent* m_event ;
  } ;
  
}

//using namespace LHCb;

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( OTRawBankDecoder )
  

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
OTRawBankDecoder::OTRawBankDecoder( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_detectordata(0),
    m_otdet(0),
    m_channelmaptool(0),
    m_timewindow(999*Gaudi::Units::ns,-999*Gaudi::Units::ns),
    m_timePerBX(25*Gaudi::Units::ns),
    m_countsPerBX(64),
    m_numberOfBX(3),
    m_forcebankversion(OTBankVersion::UNDEFINED),
    m_nsPerTdcCount(m_timePerBX/m_countsPerBX)
{
  declareInterface<IOTRawBankDecoder>(this);
  declareProperty("countsPerBX", m_countsPerBX );
  declareProperty("numberOfBX", m_numberOfBX );
  declareProperty("timePerBX", m_timePerBX );
  declareProperty("ForceBankVersion", m_forcebankversion = OTBankVersion::UNDEFINED );
  declareProperty( "rawEventLocation",  m_rawEventLocation = "", 
                   "OBSOLETE. Use RawEventLocations instead" );
  declareProperty( "RawEventLocations", m_rawEventLocations,
                   "List of possible locations of the RawEvent object in the"
                   " transient store. By default it is LHCb::RawEventLocation::Other,"
                   " LHCb::RawEventLocation::Default.");
  declareProperty("TimeWindow", m_timewindow );
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
  incSvc()->addListener( this, IncidentType::BeginEvent );
  
  // Loading OT Geometry from XML
  m_otdet = getDet<DeOTDetector>(DeOTDetectorLocation::Default );

  // access to the channel map
  m_channelmaptool = tool<IOTChannelMapTool>("OTChannelMapTool");

  // initialize the decoder data. this things basically contains the decoded event
  m_detectordata = new OTRawBankDecoderHelpers::Detector(*m_otdet,m_channelmaptool->getChannelMap()) ;
  
  // Read out window tool
  IOTReadOutWindow* aReadOutWindow = tool<IOTReadOutWindow>("OTReadOutWindow");
  m_startReadOutGate  = aReadOutWindow->startReadOutGate();
  
  m_nsPerTdcCount = m_timePerBX/ double(m_countsPerBX);
  
  if( m_forcebankversion != OTBankVersion::UNDEFINED ) {
    warning() << "Forcing bank version to be " << m_forcebankversion << endmsg ;
  }

  // Initialise the RawEvent locations
  bool usingDefaultLocation = m_rawEventLocations.empty() && m_rawEventLocation.empty();
  if (! m_rawEventLocation.empty()) {
    warning() << "The rawEventLocation property is obsolete, use RawEventLocations instead" << endmsg;
    m_rawEventLocations.insert(m_rawEventLocations.begin(), m_rawEventLocation);
  }

  if (std::find(m_rawEventLocations.begin(), m_rawEventLocations.end(), LHCb::RawEventLocation::Default)
      == m_rawEventLocations.end()) {
    // append the defaults to the search path
    m_rawEventLocations.push_back(LHCb::RawEventLocation::Other);
    m_rawEventLocations.push_back(LHCb::RawEventLocation::Default);
  }

  if (!usingDefaultLocation) {
    info() << "Using '" << m_rawEventLocations << "' as search path for the RawEvent object" << endmsg;
  }
  
  info() << " countsPerBX = " << m_countsPerBX 
         << " numberOfBX  = " << m_numberOfBX 
         << " timePerBX = " << m_timePerBX 
         << " ForceBankVersion = " << m_forcebankversion ;
  if (m_timewindow.first<m_timewindow.second) 
         info() << " require window [" << m_timewindow.first <<","<<m_timewindow.second << "]";
  info() << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Finalize
//=============================================================================
StatusCode OTRawBankDecoder::finalize()
{
  if( m_detectordata ) {
    m_detectordata->clearevent();
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
  if ( IncidentType::BeginEvent == incident.type() ) m_detectordata->clearevent() ;
}


//=============================================================================
//Decode the gol headers in the raw banks. Each header is assigned
//to its corresponding module. The contents of the gol are only
//decoded once the module data is asked for. (That is the 'decoding
//on demand' part.)
//=============================================================================
StatusCode OTRawBankDecoder::decodeGolHeadersDC06(const LHCb::RawBank& bank, int bankversion) const
{
  // There is one word which contains no data. (In real data this is
  // the OTSpecificHeader). We just skip it.
  const unsigned int* begin = bank.data() + 1 ;
  const unsigned int* end   = bank.data() + bank.size()/4 ;
  const unsigned int* idata ;  
  unsigned int station,layer,quarter,module,numhits ;
  bool decodingerror(false) ;
  for( idata = begin ; idata < end ; ++idata ) {
    OTDAQ::GolHeaderDC06 golHeader(*idata) ;
    numhits = golHeader.numberOfHits() ;
    // We skip empty headers without issuing a warning
    if( numhits > 0 ) {
      station = golHeader.station();
      layer = golHeader.layer();
      quarter = golHeader.quarter();
      module = golHeader.module();
      if(!m_detectordata->isvalidID(station,layer,quarter,module) ) {
        warning() << "Invalid gol header "<< golHeader << endmsg ;
        decodingerror = true ;
      } else {
        const unsigned short* firsthit = reinterpret_cast<const unsigned short*>(idata+1) ;
        m_detectordata->module(station,layer,quarter,module).setData(numhits,firsthit,bankversion,m_nsPerTdcCount,m_timewindow) ; 
        if (msgLevel(MSG::DEBUG)) debug() << "Reading gol header " << golHeader << endmsg ;
      }
      idata += golHeader.hitBufferSize() ;
    }
  }
  
  if(idata != end) {
    warning() << "OTRawBankDecoder: gol headers do not add up to buffer size. " << idata << " " << end << endmsg ;
    decodingerror = true ;
  }
  
  return decodingerror ? StatusCode::FAILURE : StatusCode::SUCCESS ;
}

StatusCode OTRawBankDecoder::decodeGolHeadersV3(const LHCb::RawBank& bank, int bankversion) const
{
  bool decodingerror(false) ;
  // The first 4 bytes are the OTSpecificHeader
  OTDAQ::OTSpecificHeader otheader(*bank.data()) ;
  if( msgLevel(MSG::DEBUG)) 
    debug() << "OTSpecificHeader in bank:" << otheader << endmsg ;
  if( otheader.error() ) {
    std::ostringstream mess;
    mess << "OTSpecificHeader has error bit set in bank 0x0" << std::hex << bank.sourceID();
    Warning( mess.str(), StatusCode::FAILURE, 0 ).ignore();
    if ( msgLevel( MSG::DEBUG ) ) debug() << mess.str() << endmsg;
  }
 
  // The data starts at the next 4byte
  const unsigned int* begin = bank.begin<unsigned int>()+1;
  const unsigned int* end   = bank.end<unsigned int>();
  const unsigned int* idata ;
  unsigned int station,layer,quarter,module,numhits,numgols(0) ;
  for( idata = begin ; idata < end; ++idata) {
    // decode the header
    OTDAQ::GolHeader golHeader(*idata) ;
    numhits = golHeader.numberOfHits() ;
    // Decode the GOL ID
    station = golHeader.station();
    layer = golHeader.layer();
    quarter = golHeader.quarter();
    module = golHeader.module();
    // check that the GOL ID is valid  
    if(!m_detectordata->isvalidID(station,layer,quarter,module) ) {
      std::ostringstream mess;
      mess << "Invalid gol header "<< golHeader;
      Warning( mess.str(), StatusCode::FAILURE, 0 ).ignore();
      decodingerror = true ;
    } else {
      const unsigned short* firsthit = reinterpret_cast<const unsigned short*>(idata+1) ;
      m_detectordata->module(station,layer,quarter,module).setData(numhits,firsthit,bankversion,m_nsPerTdcCount,m_timewindow) ; 
      if (msgLevel(MSG::DEBUG)) debug() << "Reading gol header " << golHeader << endmsg ;
    }
    // skip the actual hits
    idata += golHeader.hitBufferSize() ;
    ++numgols ;
  }

  // check that everything is well aligned
  if(idata != end) {
    std::ostringstream mess;
    mess << "GOL headers do not add up to buffer size. " << idata << " " << end;
    Warning( mess.str(), StatusCode::FAILURE, 0 ).ignore();
    decodingerror = true ;
  }
  
  // check that we have read the right number of GOLs
  if( numgols != otheader.numberOfGOLs() ) {
    std::ostringstream mess;
    mess << "Found " << otheader.numberOfGOLs() << " in bank header, but read only " 
         << numgols << " from bank.";
    Warning( mess.str(), StatusCode::FAILURE, 0 ).ignore();
    decodingerror = true ;
  }
    
  return decodingerror ? StatusCode::FAILURE : StatusCode::SUCCESS ;
}

StatusCode OTRawBankDecoder::decodeGolHeaders(const LHCb::RawEvent& event) const
{
  if( &event != m_detectordata->rawEvent() ) {
    
    // clear the hit buffers
    m_detectordata->clearevent() ;
    m_detectordata->setRawEvent( &event ) ;
    
    // Get the buffers associated with OT
    const std::vector<LHCb::RawBank*>& OTBanks = 
      (const_cast<LHCb::RawEvent&>(event)).banks( LHCb::RawBank::OT );
    
    // Report the number of banks
    if (msgLevel(MSG::DEBUG)) 
      debug() << "Decoding GOL headers in OTRawBankDecoder. Number of OT banks is " 
              << OTBanks.size() << endmsg ;
    
    for (std::vector<LHCb::RawBank*>::const_iterator  ibank = OTBanks.begin();
         ibank != OTBanks.end() ; ++ibank) {

      if( LHCb::RawBank::MagicPattern != (*ibank)->magic() )
      {
        Error("Wrong 'magic' value: skip decoding this RawBank.", StatusCode::FAILURE, 0).ignore();
        continue;
      }

      // Report the bank size and version
      if (msgLevel(MSG::DEBUG))
        debug() << "OT Bank sourceID= " << (*ibank)->sourceID() 
                << " size=" << (*ibank)->size()/4 << " bankversion=" << (*ibank)->version() << endmsg;
      
      // Choose decoding based on bank version
      int bVersion = m_forcebankversion != OTBankVersion::UNDEFINED ? m_forcebankversion : (*ibank)->version();
      StatusCode sc ;
      switch( bVersion ) {
        case OTBankVersion::DC06:
          m_channelmaptool->setBankVersion( bVersion ) ;
          sc = decodeGolHeadersDC06(**ibank,bVersion) ;
          break ;
          // Note: SIM and v3 currently (22/07/2008) uses same decoding.
          //       If SIM changes w.r.t. to the real decoding then we'll need
          //       to change it here.
        case OTBankVersion::SIM:
          m_channelmaptool->setBankVersion( bVersion ) ;
        case OTBankVersion::v3:
          m_channelmaptool->setBankVersion( bVersion ) ;
          sc = decodeGolHeadersV3(**ibank,bVersion) ;
          break ;
        default:
          std::ostringstream mess;
          mess << "Cannot decode OT raw buffer bank version "
               << bVersion << " with this version of OTDAQ";
          Warning( mess.str(), StatusCode::FAILURE, 0 ).ignore();
      } ;
      // ignore errors
      sc.ignore() ;
    }
  }
  
  return StatusCode::SUCCESS ;
}

StatusCode OTRawBankDecoder::decodeGolHeaders() const
{
  // Retrieve the RawEvent:
  LHCb::RawEvent* raw = NULL;
  for (std::vector<std::string>::const_iterator p = m_rawEventLocations.begin(); p != m_rawEventLocations.end(); ++p) {
    if (exist<LHCb::RawEvent>(*p)){
      raw = get<LHCb::RawEvent>(*p);
      break;
    }
  }

  if( raw == NULL ) {
    Warning("Failed to find raw data").ignore();
  }
  else {
    decodeGolHeaders( *raw ).ignore() ; ///< Always returns SUCCESS. Might change in the future ;)
  }

  return StatusCode::SUCCESS ;
}

size_t OTRawBankDecoder::totalNumberOfHits() const 
{
  if( !m_detectordata->golHeadersLoaded() ) decodeGolHeaders().ignore() ;
  return m_detectordata->totalNumberOfHits() ;
}

size_t OTRawBankDecoder::decodeModule( OTRawBankDecoderHelpers::Module& moduledata ) const 
{
  if( !m_detectordata->golHeadersLoaded() ) decodeGolHeaders().ignore() ;
  if( !moduledata.isDecoded() ) moduledata.decode( ) ;
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

StatusCode OTRawBankDecoder::decode( OTDAQ::RawEvent& otrawevent ) const
{
  // this needs to be merged with the code above. this only works for
  // verison 3 banks. for now, let's keep it seperate, until we have a
  // real solution.

  // Retrieve the RawEvent:
  LHCb::RawEvent* event = NULL;
  for (std::vector<std::string>::const_iterator p = m_rawEventLocations.begin(); p != m_rawEventLocations.end(); ++p) {
    if (exist<LHCb::RawEvent>(*p)){
      event = get<LHCb::RawEvent>(*p);
      break;
    }
  }

  if( event == NULL ) return Error("Failed to find raw data");

  // Get the buffers associated with OT
  const std::vector<LHCb::RawBank*>& banks = event->banks(LHCb::RawBank::OT );
 
  // Reserve space in the otbankcontainer
  otrawevent.clear() ;
  OTDAQ::RawEvent::RawBankContainer& otbankcontainer = otrawevent.banks() ;
  otbankcontainer.reserve( banks.size() ) ;
  // Now copy the information from all banks
  bool decodingerror(false) ;
  for (std::vector<LHCb::RawBank*>::const_iterator  ibank = banks.begin();
       ibank != banks.end() ; ++ibank) {

    if( LHCb::RawBank::MagicPattern != (*ibank)->magic() )
    {
      Error("Wrong 'magic' value: skip decoding this RawBank.", StatusCode::FAILURE, 0).ignore();
      continue;
    }

    const unsigned int* idata = (*ibank)->data() ;
    otbankcontainer.push_back( OTDAQ::RawBank( (*ibank)->version(), (*ibank)->size(),
					       OTDAQ::OTSpecificHeader(*idata))) ;
    OTDAQ::RawBank& otspecificbank = otbankcontainer.back() ;
    otspecificbank.gols().reserve( otspecificbank.header().numberOfGOLs()) ;
    
    // The data starts at the next 4byte
    const unsigned int* begin = (*ibank)->begin<unsigned int>()+1;
    const unsigned int* end   = (*ibank)->end<unsigned int>();
    size_t numgols(0) ;
    for( idata = begin ; idata < end; ++idata) {
      // decode the header
      OTDAQ::GolHeader golheader(*idata) ;
      OTDAQ::Gol gol(golheader) ;
      const OTDAQ::RawHit* firsthit = reinterpret_cast<const OTDAQ::RawHit*>(idata+1) ;
      gol.hits().insert(gol.hits().end(),firsthit,firsthit+golheader.numberOfHits()) ;
      otspecificbank.gols().push_back( gol ) ;
      // increase the pointer with the gol size
      idata += golheader.hitBufferSize() ;
      ++numgols ;
    }
    
    // check that everything is well aligned
    if(idata != end) {
      std::ostringstream mess;
      mess << "GOL headers do not add up to buffer size. " << idata << " " << end;
      Warning( mess.str(), StatusCode::FAILURE, 0 ).ignore();
      decodingerror = true ;
    }
    
    // check that we have read the right number of GOLs
    if( numgols != otspecificbank.header().numberOfGOLs() ) {
      std::ostringstream mess;
      mess << "Found " << otspecificbank.header().numberOfGOLs() << " in bank header, but read only " 
           << numgols << " from bank.";
      Warning( mess.str(), StatusCode::FAILURE, 0 ).ignore();
      decodingerror = true ;
    }
  }

  return decodingerror ? StatusCode::FAILURE : StatusCode::SUCCESS ;
}

LHCb::OTLiteTime 
OTRawBankDecoder::time( LHCb::OTChannelID channel ) const
{
  const DeOTModule& module = m_detectordata->module(channel).detElement() ;
  return LHCb::OTLiteTime( channel, channel.tdcTime() * m_nsPerTdcCount - module.strawT0(channel.straw())) ;
}
