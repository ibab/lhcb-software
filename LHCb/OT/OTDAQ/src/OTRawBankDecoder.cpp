// Include files
#include <algorithm>
#include <numeric>
#include <sstream>
#include <limits>

// from Gaudi
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
  class Module final
  {
  public:
    Module() = default;
    void setParent(Detector& parent) { m_parent = &parent; }
    void clearevent() { m_isdecoded=false ; m_size=0; m_data=0 ; m_tdcconversion = 0; m_idx = {0, 0}; }
    void setData( unsigned int size, const unsigned short* data,
                  int bankversion, double tdcconversion,
                  const std::pair<double, double>& window)
    {
      m_size = size;
      m_data = data;
      m_bankversion = bankversion;
      m_tdcconversion = tdcconversion;
      m_window = window;
    }
    bool isDecoded() const { return m_isdecoded; }
    void setIsDecoded(bool b=true) { m_isdecoded = b; }
    const unsigned short* data() const { return m_data; }
    unsigned int size() const { return m_size; }
    inline LHCb::OTLiteTimeRange ottimes() const;
    void setDetElement( const DeOTModule& e );
    void setChannelMap( const OTDAQ::ChannelMap::Module& map ) { m_channelmap = &map; }
    void setTimeWindow( double tmin, double tmax ) { m_window.first = tmin; m_window.second = tmax; }
    const DeOTModule& detElement() const { return *m_detelement; }

    inline bool isHitInWindow(unsigned short data, bool vetoOOTPairs) const;
    size_t decodeDC06();
    size_t countHitsInWindowDC06() const;
    size_t decodeV3();
    size_t countHitsInWindowV3() const;
    size_t decode();
    size_t countHitsInWindow() const;
  private:
    void addHit(unsigned short data, const bool vetoOOTPairs);

    // optimize layout wrt. memory alignment requirements
    const DeOTModule* m_detelement = nullptr;
    const OTDAQ::ChannelMap::Module* m_channelmap = nullptr;
    const unsigned short* m_data = nullptr;
    Detector* m_parent = nullptr;
    std::pair<double,double> m_window;
    double m_tdcconversion = 0;
    unsigned int m_station = 0;
    unsigned int m_layer = 0;
    unsigned int m_quarter = 0;
    unsigned int m_module = 0;
    unsigned int m_size = 0;
    int m_bankversion = OTBankVersion::UNDEFINED;
    std::pair<unsigned short, unsigned short> m_idx = { 0,0 };
    bool m_isdecoded = false;

    friend class OTRawBankDecoderHelpers::Detector;
  };

  class Detector : public OTDAQ::IndexedModuleDataHolder<Module>
  {
  public:
    Detector(const OTRawBankDecoder& parent,
	     const DeOTDetector& det,
             const OTDAQ::ChannelMap::Detector& channelmap) :
	m_parent(parent)
    {
      size_t nChannels = 0, maxidx = std::numeric_limits<size_t>::max();
      for(const auto& mod : det.modules()) {
	    nChannels += mod->nChannels();
        Module& amodule = module(mod->stationID(),mod->layerID(),mod->quarterID(),mod->moduleID());
	    amodule.setParent(*this);
        amodule.setDetElement(*mod);
        amodule.setChannelMap( channelmap.module(mod->stationID(),
                                                 mod->layerID(),
                                                 mod->quarterID(),
                                                 mod->moduleID()   ) );
	    maxidx = std::min(maxidx, size_t(std::min(
		std::numeric_limits<decltype(amodule.m_idx.first)>::max(),
		std::numeric_limits<decltype(amodule.m_idx.second)>::max())));
      }
      // make sure we throw if some joker plays with the data type of the
      // m_ottimes pair in Module
      if (nChannels > maxidx) {
	    throw std::length_error("OT module with more than 65536 channels?! "
	    "Adapt data type in modules!");
      }
      // we will rarely need to enlarge this as a result of an emplace_back in
      // Module (if ever), and if we do need to enlarge this, the indices in
      // the Modules stay valid (no iterators!)...
      m_ottimes.reserve(nChannels / 4);
    }

    void clearevent() {
      m_event = nullptr;
      for(auto imod = begin(); imod != end(); ++imod) imod->clearevent();
      m_ottimes.clear();
    }

    const LHCb::RawEvent* rawEvent() const { return m_event; }
    void setRawEvent( const LHCb::RawEvent* ev) { m_event = ev; }
    bool golHeadersLoaded() const { return m_event; }

    size_t totalNumberOfHits() const {
        //size_t ntot = std::accumulate( begin(), end(), size_t(0),
	//	[] (size_t acc, const Module& m)
	//	{ return acc + m.size(); });
        return totalNumberOfHitsInWindow(); // ns...
    }

    size_t totalNumberOfHitsInWindow() const
    {
      return std::accumulate( begin(), end(), size_t(0),
	                          [] (size_t acc, const Module& m) {
                                 return acc + m.countHitsInWindow();
      });
    }

    bool isTotalNumberOfHitsLessThen(size_t nmax) const
    {
        size_t n = 0;
        for(auto imod = begin(); imod!= end(); ++imod) {
            n += imod->countHitsInWindow();
            if(n >= nmax) return false;
        }
        return true;
    }

  private:
    const OTRawBankDecoder& m_parent;
    const LHCb::RawEvent* m_event = nullptr;
    LHCb::OTLiteTimeContainer m_ottimes;

    friend class OTRawBankDecoderHelpers::Module;
  };

  inline LHCb::OTLiteTimeRange Module::ottimes() const
  { return { m_parent->m_ottimes.begin() + m_idx.first,
             m_parent->m_ottimes.begin() + m_idx.second};
  }

  void Module::setDetElement( const DeOTModule& e)
  {
    // cache for speed
    m_detelement = &e;
    LHCb::OTChannelID moduleid = e.elementID();
    m_station = moduleid.station();
    m_layer   = moduleid.layer();
    m_quarter = moduleid.quarter();
    m_module  = moduleid.module();
  }

  inline bool Module::isHitInWindow(unsigned short data, bool vetoOOTPairs) const
  {
    if (m_window.second < m_window.first) return true;
    const OTDAQ::RawHit hit(data);
    const unsigned straw = m_channelmap->straw( hit.channel() );
    const auto t = hit.time()*m_tdcconversion - m_detelement->strawT0( straw );
    // if the hit itself is outside the window, we're done
    const auto wa = 0.5 * (m_window.first + m_window.second);
    const auto wd = 0.5 * (m_window.second - m_window.first);
    if (std::abs(wa - t) > wd) return false;
    // if we are not after vetoing pairs of hits, we're done, too
    if (!vetoOOTPairs) return true;
    // look for a matching hit to form a pair of neighbouring hits
    //
    // we drop hits according to the following algorithm
    // - keep hit if pair exists with both hits inside the window (overrides
    //   other conditions)
    // - drop hit if pair exits with one hit inside and one outside the window,
    //   and both hits in the pair are either both early or both late
    // - else: keep hit (no pair found)
    //
    // rationale is that we don't want to kill of a hit because a neighbouring
    // hit is outside the time window if there is also a neighbouring hit such
    // that both hits fall inside the window
    //
    // to identify a pair of neibouring hits, we need to figure out correct
    // remainders of hardware channel numbers modulo 64 for neighbouring
    // channels in different monolayers inside the same module
    //
    // this whole scheme works because the hardware straw numbering helps the
    // task along (it's either what is sketched below, or the corresponding
    // variant where staggering is such that straw 0 comes before straw 127):
    //
    // long modules:		short modules:
    //     0   1 ... 63		   0   1 ... 31
    //  127 126 ... 64		127 126 ... 96
    //
    // one can see that the sum of straw numbers in a pair (modulo 64) has to
    // be 62, 63, 0, 1, or 2
    //
    // a detailed survey of module geometry reveals that 62 and 63 (63 and 0)
    // are the only possible values in quarters 0 and 1 (2 and 3)
    //
    // moreover, it turns out the OT readout delivers hits sorted by straw
    // number
    const unsigned rem1 = 0x40u + (m_quarter < 2 ? 62 : 63);
    const unsigned rem2 = (1u + rem1) & 0x7fu;
    unsigned ostraw1 = (rem1 - hit.channel()) & 0x7fu;
    unsigned ostraw2 = (rem2 - hit.channel()) & 0x7fu;
    // in all but the "edge" straw case, we should have two consecutive
    // numbers; swap if needed to have them in increasing order
    if (ostraw1 > ostraw2) std::swap(ostraw1, ostraw2);
    // catch "edge" straws with only one neighbour (edge means edge of the
    // module where there is no more neighbouring straws to one side)
    if (UNLIKELY(hit.channel() == ostraw1)) ostraw1 = ostraw2;
    else if (UNLIKELY(hit.channel() == ostraw2)) ostraw2 = ostraw1;
    bool haveOutOfTimePair = false;
    for (const unsigned short* ptr =
	std::lower_bound(m_data, m_data + m_size, ostraw1,
	  [] (const unsigned short data, unsigned straw)
	  { OTDAQ::RawHit hit(data); return hit.channel() < straw; });
	m_data + m_size != ptr; ++ptr) {
      const OTDAQ::RawHit otherhit(*ptr);
      if (ostraw2 < otherhit.channel()) {
	if (LIKELY(OTBankVersion::DC06 != m_bankversion)) break;
	// don't stop early for DC06 decoding... (this is a hackish hack, but
	// it is a hack that works)
	else if (UNLIKELY(ptr == (m_data + m_size - 2) && *ptr == 0)) continue;
	else break;
      }
      const unsigned otherstraw = m_channelmap->straw(otherhit.channel());
      const auto othert = otherhit.time()*m_tdcconversion - m_detelement->strawT0( otherstraw );
      if (std::abs(wa - othert) > wd) {
	// kill the pair if the other hit is out of time, and they're either
	// both early, or both late
       	if ((othert - wa) * (t - wa) >= 0.)
	  haveOutOfTimePair = true;
      } else {
	// if we find a matching in-time neighbour, we keep the pair
	return true;
      }
    }
    return !haveOutOfTimePair;
  }

  inline void Module::addHit(unsigned short data, const bool vetoOOTPairs)
  {
    if (!isHitInWindow(data, vetoOOTPairs)) return;
    const OTDAQ::RawHit hit(data);
    const unsigned straw = m_channelmap->straw( hit.channel() );
    const unsigned tdctime = hit.time();
    const double t0 = m_detelement->strawT0( straw );
    const double t = tdctime * m_tdcconversion - t0;
    m_parent->m_ottimes.emplace_back( LHCb::OTChannelID{m_station,m_layer,
                                                        m_quarter,m_module,
                                                        straw,tdctime},
                                      t );
    ++m_idx.second;
  }

  inline size_t Module::countHitsInWindowDC06()  const
  {
    if (m_isdecoded) return m_idx.second - m_idx.first;
    if (m_window.second<m_window.first) return size();
    // now, DC06 has a padding problem: The padded hits appears before the last
    // hit. So, we need to fix that.
    const bool haspaddinghit = m_data[m_size-2] == 0;
    const bool vetoOOTPairs = m_parent->m_parent.m_vetoOutOfTimeHitPairs;
    const unsigned short* begin = m_data;
    const unsigned short* end   = begin + (haspaddinghit ? m_size -2 : m_size);
    auto n = std::count_if( begin, end, [&](const unsigned short d) {
                            return isHitInWindow(d, vetoOOTPairs);
    });
    if(haspaddinghit && isHitInWindow(m_data[m_size-1], vetoOOTPairs)) ++n;
    return n;
  }

  inline size_t Module::decodeDC06()
  {
    if(!m_isdecoded) {
      const bool vetoOOTPairs = m_parent->m_parent.m_vetoOutOfTimeHitPairs;
      m_idx.first = m_idx.second = m_parent->m_ottimes.size();
      if( m_size != 0 ) {
        // now, DC06 has a padding problem: The padded hits appears before the last
        // hit. So, we need to fix that.
        bool haspaddinghit = m_data[m_size-2] == 0;
        const unsigned short* begin = m_data;
        const unsigned short* end   = begin + (haspaddinghit ? m_size -2 : m_size);
        for( const unsigned short* ihit = begin ; ihit != end ; ++ihit)
          addHit(*ihit, vetoOOTPairs);
        if(haspaddinghit) addHit(m_data[m_size-1], vetoOOTPairs);
      }
      m_isdecoded = true;
    }
    return m_idx.second - m_idx.first;
  }

  inline size_t Module::countHitsInWindowV3() const
  {
    if (m_isdecoded) return m_idx.second - m_idx.first;
    if (m_window.second<m_window.first) return size();
    const bool vetoOOTPairs = m_parent->m_parent.m_vetoOutOfTimeHitPairs;
    return std::count_if( m_data, m_data+m_size, [&](const unsigned short d) {
            return isHitInWindow(d, vetoOOTPairs);
    });
  }

  inline size_t Module::decodeV3()
  {
    if(!m_isdecoded) {
      m_idx.first = m_idx.second = m_parent->m_ottimes.size();
      const bool vetoOOTPairs = m_parent->m_parent.m_vetoOutOfTimeHitPairs;
      if( m_size != 0 ) {
        const unsigned short* begin = m_data;
        const unsigned short* end   = begin + m_size;
        for (const unsigned short* ihit = begin ; ihit != end ; ++ihit) addHit(*ihit, vetoOOTPairs);
      }
      m_isdecoded = true;
    }
    return m_idx.second - m_idx.first;
  }

  inline size_t Module::decode()
  {
    size_t rc(0);
    switch(m_bankversion) {
      case OTBankVersion::DC06:
        rc = decodeDC06();
        break;
        // Note: SIM and v3 currently (22/07/2008) uses same decoding.
        //       If SIM changes w.r.t. to the real decoding then we'll need
        //       to change it here.
      case OTBankVersion::SIM:
      case OTBankVersion::v3:
        rc = decodeV3();
        break;
    }
    return rc;
  }

  inline size_t Module::countHitsInWindow() const
  {
    size_t rc(0);
    switch(m_bankversion) {
      case OTBankVersion::DC06:
        rc = countHitsInWindowDC06();
        break;
        // Note: SIM and v3 currently (22/07/2008) uses same decoding.
        //       If SIM changes w.r.t. to the real decoding then we'll need
        //       to change it here.
      case OTBankVersion::SIM:
      case OTBankVersion::v3:
        rc = countHitsInWindowV3();
        break;
    }
    return rc;
  }
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
    : Decoder::ToolBase ( type, name , parent ),
    m_detectordata(0),
    m_otdet(0),
    m_channelmaptool(0),
    m_timewindow(999*Gaudi::Units::ns,-999*Gaudi::Units::ns),
    m_timePerBX(25*Gaudi::Units::ns),
    m_nsPerTdcCount(-1),
    m_countsPerBX(64),
    m_numberOfBX(3),
    m_forcebankversion(OTBankVersion::UNDEFINED),
    m_vetoOutOfTimeHitPairs(true)
{
  declareInterface<IOTRawBankDecoder>(this);
  declareProperty("countsPerBX", m_countsPerBX );
  declareProperty("numberOfBX", m_numberOfBX );
  declareProperty("timePerBX", m_timePerBX );
  declareProperty("ForceBankVersion", m_forcebankversion = OTBankVersion::UNDEFINED );
  declareProperty("TimeWindow", m_timewindow );
  declareProperty("VetoOutOfTimeHitPairs", m_vetoOutOfTimeHitPairs = true);
  //new for decoders, initialize search path, and then call the base method
  m_rawEventLocations = {LHCb::RawEventLocation::Other, LHCb::RawEventLocation::Default};
  initRawEventSearch();

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

  if (msgLevel(MSG::DEBUG)) debug()<<"initializing OTRawBankDecoder"<<endmsg;

  StatusCode sc = Decoder::ToolBase::initialize();
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  // Setup incident services
  incSvc()->addListener( this, IncidentType::BeginEvent );

  // Loading OT Geometry from XML
  m_otdet = getDet<DeOTDetector>(DeOTDetectorLocation::Default );

  // access to the channel map
  m_channelmaptool = tool<IOTChannelMapTool>("OTChannelMapTool");

  // initialize the decoder data. this things basically contains the decoded event
  m_detectordata = new OTRawBankDecoderHelpers::Detector(*this, *m_otdet,m_channelmaptool->getChannelMap());

  // Read out window tool
  IOTReadOutWindow* aReadOutWindow = tool<IOTReadOutWindow>("OTReadOutWindow");
  m_startReadOutGate  = aReadOutWindow->startReadOutGate();

  m_nsPerTdcCount = m_timePerBX/ double(m_countsPerBX);

  if( m_forcebankversion != OTBankVersion::UNDEFINED ) {
    warning() << "Forcing bank version to be " << m_forcebankversion << endmsg;
  }

  info() << " countsPerBX = " << m_countsPerBX
         << " numberOfBX  = " << m_numberOfBX
         << " timePerBX = " << m_timePerBX
         << " ForceBankVersion = " << m_forcebankversion;
  if (m_timewindow.first<m_timewindow.second) {
         info() << " require window [" << m_timewindow.first <<","<<m_timewindow.second << "]";
	 if (m_vetoOutOfTimeHitPairs)
	   info() << ", vetoing out of time hit pairs";
  }
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
    delete m_detectordata;
    m_detectordata=0;
  }
  return Decoder::ToolBase::finalize();
}

//=============================================================================
// Handle for incident service
//=============================================================================

void OTRawBankDecoder::handle ( const Incident& incident )
{
  if ( IncidentType::BeginEvent == incident.type() ) m_detectordata->clearevent();
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
  const unsigned int* begin = bank.data() + 1;
  const unsigned int* end   = bank.data() + bank.size()/4;
  const unsigned int* idata ;
  unsigned int station,layer,quarter,module,numhits;
  bool decodingerror(false);
  for( idata = begin ; idata < end ; ++idata ) {
    OTDAQ::GolHeaderDC06 golHeader(*idata);
    numhits = golHeader.numberOfHits();
    // We skip empty headers without issuing a warning
    if( numhits > 0 ) {
      station = golHeader.station();
      layer = golHeader.layer();
      quarter = golHeader.quarter();
      module = golHeader.module();
      if(!m_detectordata->isvalidID(station,layer,quarter,module) ) {
        warning() << "Invalid gol header "<< golHeader << endmsg;
        decodingerror = true;
      } else {
        const unsigned short* firsthit = reinterpret_cast<const unsigned short*>(idata+1);
        m_detectordata->module(station,layer,quarter,module).setData(numhits,firsthit,bankversion,m_nsPerTdcCount,m_timewindow) ;
        if (msgLevel(MSG::DEBUG)) debug() << "Reading gol header " << golHeader << endmsg;
      }
      idata += golHeader.hitBufferSize();
    }
  }

  if(idata != end) {
    warning() << "OTRawBankDecoder: gol headers do not add up to buffer size. " << idata << " " << end << endmsg;
    decodingerror = true;
  }

  return decodingerror ? StatusCode::FAILURE : StatusCode::SUCCESS;
}

StatusCode OTRawBankDecoder::decodeGolHeadersV3(const LHCb::RawBank& bank, int bankversion) const
{
  bool decodingerror(false);
  // The first 4 bytes are the OTSpecificHeader
  OTDAQ::OTSpecificHeader otheader(*bank.data());
  if( msgLevel(MSG::DEBUG))
    debug() << "OTSpecificHeader in bank:" << otheader << endmsg;
  if( otheader.error() ) {
    std::ostringstream mess;
    mess << "OTSpecificHeader has error bit set in bank 0x0" << std::hex << bank.sourceID();
    Warning( mess.str(), StatusCode::FAILURE, 0 ).ignore();
    if ( msgLevel( MSG::DEBUG ) ) debug() << mess.str() << endmsg;
  }

  // The data starts at the next 4byte
  const unsigned int* begin = bank.begin<unsigned int>()+1;
  const unsigned int* end   = bank.end<unsigned int>();
  const unsigned int* idata;
  unsigned int station,layer,quarter,module,numhits,numgols(0);
  for( idata = begin ; idata < end; ++idata) {
    // decode the header
    OTDAQ::GolHeader golHeader(*idata);
    numhits = golHeader.numberOfHits();
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
      decodingerror = true;
    } else {
      const unsigned short* firsthit = reinterpret_cast<const unsigned short*>(idata+1);
      m_detectordata->module(station,layer,quarter,module).setData(numhits,firsthit,bankversion,m_nsPerTdcCount,m_timewindow) ;
      if (msgLevel(MSG::DEBUG)) debug() << "Reading gol header " << golHeader << endmsg;
    }
    // skip the actual hits
    idata += golHeader.hitBufferSize();
    ++numgols;
  }

  // check that everything is well aligned
  if(idata != end) {
    std::ostringstream mess;
    mess << "GOL headers do not add up to buffer size. " << idata << " " << end;
    Warning( mess.str(), StatusCode::FAILURE, 0 ).ignore();
    decodingerror = true;
  }

  // check that we have read the right number of GOLs
  if( numgols != otheader.numberOfGOLs() ) {
    std::ostringstream mess;
    mess << "Found " << otheader.numberOfGOLs() << " in bank header, but read only "
         << numgols << " from bank.";
    Warning( mess.str(), StatusCode::FAILURE, 0 ).ignore();
    decodingerror = true;
  }

  return decodingerror ? StatusCode::FAILURE : StatusCode::SUCCESS;
}

StatusCode OTRawBankDecoder::decodeGolHeaders(const LHCb::RawEvent& event) const
{
  if( &event != m_detectordata->rawEvent() ) {

    // clear the hit buffers
    m_detectordata->clearevent();
    m_detectordata->setRawEvent( &event );

    // Get the buffers associated with OT
    const std::vector<LHCb::RawBank*>& OTBanks =
      (const_cast<LHCb::RawEvent&>(event)).banks( LHCb::RawBank::OT );

    // Report the number of banks
    if (msgLevel(MSG::DEBUG))
      debug() << "Decoding GOL headers in OTRawBankDecoder. Number of OT banks is "
              << OTBanks.size() << endmsg;

    for (auto ibank = OTBanks.begin(); ibank != OTBanks.end() ; ++ibank) {

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
      StatusCode sc;
      switch( bVersion ) {
        case OTBankVersion::DC06:
          m_channelmaptool->setBankVersion( bVersion );
          sc = decodeGolHeadersDC06(**ibank,bVersion);
          break;
          // Note: SIM and v3 currently (22/07/2008) uses same decoding.
          //       If SIM changes w.r.t. to the real decoding then we'll need
          //       to change it here.
        case OTBankVersion::SIM:
          m_channelmaptool->setBankVersion( bVersion );
        case OTBankVersion::v3:
          m_channelmaptool->setBankVersion( bVersion );
          sc = decodeGolHeadersV3(**ibank,bVersion);
          break;
        default:
          Warning( "Cannot decode OT raw buffer bank version " +
                   std::to_string(bVersion) +" with this version of OTDAQ",
                   StatusCode::FAILURE, 0 ).ignore();
      };
      // ignore errors
      sc.ignore();
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode OTRawBankDecoder::decodeGolHeaders() const
{
  // Retrieve the RawEvent:
  LHCb::RawEvent* raw = findFirstRawEvent();
  if( !raw ) {
    Warning("Failed to find raw data").ignore();
  } else {
    decodeGolHeaders( *raw ).ignore() ; ///< Always returns SUCCESS. Might change in the future ;)
  }
  return StatusCode::SUCCESS;
}

size_t OTRawBankDecoder::totalNumberOfHits() const
{
  if( !m_detectordata->golHeadersLoaded() ) decodeGolHeaders().ignore();
  return m_detectordata->totalNumberOfHits();
}

size_t OTRawBankDecoder::decodeModule( OTRawBankDecoderHelpers::Module& moduledata ) const
{
  if( !m_detectordata->golHeadersLoaded() ) decodeGolHeaders().ignore();
  if( !moduledata.isDecoded() ) moduledata.decode( );
  return moduledata.ottimes().size();
}

LHCb::OTLiteTimeRange OTRawBankDecoder::decodeModule( const LHCb::OTChannelID& moduleid ) const
{
  OTRawBankDecoderHelpers::Module& moduledata =
    m_detectordata->module(moduleid.station(),moduleid.layer(),moduleid.quarter(),moduleid.module());
  if( !moduledata.isDecoded() ) OTRawBankDecoder::decodeModule(moduledata);
  return moduledata.ottimes();
}

StatusCode OTRawBankDecoder::decode( LHCb::OTLiteTimeContainer& ottimes ) const
{
  // decode all modules. keep track of total number of hits.
  size_t numhits(0);
  for( OTRawBankDecoderHelpers::Module* imod = m_detectordata->begin();
       imod != m_detectordata->end(); ++imod)
    numhits += decodeModule( *imod );
  // reserve and copy
  ottimes.clear();
  ottimes.reserve( numhits );
  for( OTRawBankDecoderHelpers::Module* imod = m_detectordata->begin();
       imod != m_detectordata->end(); ++imod)
    ottimes.insert(ottimes.end(), imod->ottimes().begin(), imod->ottimes().end() );
  // We'll need some proper error handling.
  return StatusCode::SUCCESS;
}

StatusCode OTRawBankDecoder::decode( OTDAQ::RawEvent& otrawevent ) const
{
  // this needs to be merged with the code above. this only works for
  // verison 3 banks. for now, let's keep it seperate, until we have a
  // real solution.

  // Retrieve the RawEvent:
  LHCb::RawEvent* event = findFirstRawEvent();
  if( !event ) return Error("Failed to find raw data");

  // Get the buffers associated with OT
  const std::vector<LHCb::RawBank*>& banks = event->banks(LHCb::RawBank::OT );

  // Reserve space in the otbankcontainer
  otrawevent.clear();
  OTDAQ::RawEvent::RawBankContainer& otbankcontainer = otrawevent.banks();
  otbankcontainer.reserve( banks.size() );
  // Now copy the information from all banks
  bool decodingerror(false);
  for (const auto& bank : banks) {

    if( LHCb::RawBank::MagicPattern != bank->magic() )
    {
      Error("Wrong 'magic' value: skip decoding this RawBank.", StatusCode::FAILURE, 0).ignore();
      continue;
    }

    const unsigned int* idata = bank->data();
    otbankcontainer.push_back( OTDAQ::RawBank( bank->version(), bank->size(),
					       OTDAQ::OTSpecificHeader(*idata)));
    OTDAQ::RawBank& otspecificbank = otbankcontainer.back();
    otspecificbank.gols().reserve( otspecificbank.header().numberOfGOLs());

    // The data starts at the next 4byte
    const unsigned int* begin = bank->begin<unsigned int>()+1;
    const unsigned int* end   = bank->end<unsigned int>();
    size_t numgols(0);
    for( idata = begin ; idata < end; ++idata) {
      // decode the header
      OTDAQ::GolHeader golheader(*idata);
      OTDAQ::Gol gol(golheader);
      const OTDAQ::RawHit* firsthit = reinterpret_cast<const OTDAQ::RawHit*>(idata+1);
      gol.hits().insert(gol.hits().end(),firsthit,firsthit+golheader.numberOfHits());
      otspecificbank.gols().push_back( gol );
      // increase the pointer with the gol size
      idata += golheader.hitBufferSize();
      ++numgols;
    }

    // check that everything is well aligned
    if(idata != end) {
      std::ostringstream mess;
      mess << "GOL headers do not add up to buffer size. " << idata << " " << end;
      Warning( mess.str(), StatusCode::FAILURE, 0 ).ignore();
      decodingerror = true;
    }

    // check that we have read the right number of GOLs
    if( numgols != otspecificbank.header().numberOfGOLs() ) {
      Warning( "Found " + std::to_string(otspecificbank.header().numberOfGOLs())
               + " in bank header, but read only " + std::to_string(numgols)
               + " from bank.",
              StatusCode::FAILURE, 0 ).ignore();
      decodingerror = true;
    }
  }

  return decodingerror ? StatusCode::FAILURE : StatusCode::SUCCESS;
}

LHCb::OTLiteTime
OTRawBankDecoder::time( LHCb::OTChannelID channel ) const
{
  const DeOTModule& module = m_detectordata->module(channel).detElement();
  return LHCb::OTLiteTime( channel, channel.tdcTime() * m_nsPerTdcCount - module.strawT0(channel.straw()));
}
