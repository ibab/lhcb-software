// $Id: OTRawBankDecoder.cpp,v 1.1 2007-09-07 13:19:21 wouter Exp $
// Include files
#include <algorithm>

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/IIncidentSvc.h"

// from Detector
#include "OTDet/DeOTDetector.h"
#include "OTDet/DeOTModule.h"

// Event
#include "Event/GolHeader.h"
#include "Event/RawBank.h"
#include "Event/RawEvent.h"
#include "Event/DataWord.h"

// GSL
#include <algorithm>

// local
#include "Event/OTBankVersion.h"

// Tool interface
#include "OTDAQ/IOTReadOutWindow.h"

// local
#include "OTDAQ/OTRawBankDecoder.h"

#include "GaudiKernel/ChronoEntity.h"

namespace OTRawBankDecoderHelpers
{
  class Module
  {
  public:
    Module() : m_isdecoded(false), m_data(0) { /* FIXME */ m_ottimes.reserve(128) ; }
    void clearevent() { m_isdecoded=false ; m_data=0 ; m_ottimes.clear() ; }
    void setGolHeader( const LHCb::GolHeader& header, const unsigned int* data) {
      m_golHeader = header; m_data = data ; }
    
    const LHCb::GolHeader& golHeader() const { return m_golHeader ; }
    bool isDecoded() const { return m_isdecoded ; }
    void setIsDecoded(bool b=true) { m_isdecoded = b; }
    const unsigned int* data() const { return m_data ; }
    LHCb::OTLiteTimeContainer& ownedottimes() { return m_ottimes ; }
    LHCb::OTLiteTimeRange ottimes() const { return LHCb::OTLiteTimeRange(m_ottimes.begin(),m_ottimes.end()) ; }
    
  private:
    bool m_isdecoded ;
    LHCb::GolHeader m_golHeader ;
    const unsigned int* m_data ;
    LHCb::OTLiteTimeContainer m_ottimes ;
  } ;
  
  class Detector
  {
  public:
    Detector() : m_golHeadersLoaded(false) {}
    void clearevent() {
      m_golHeadersLoaded = false ;
      for(iterator imod = begin(); imod!= end(); ++imod)
	imod->clearevent() ;
    }
    Module& module(int station, int layer, int quarter, int module) {
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
  : GaudiTool ( type, name , parent )
{
  declareInterface<OTRawBankDecoder>(this);
  declareProperty("countsPerBX", m_countsPerBX = 64);
  declareProperty("numberOfBX", m_numberOfBX = 3);
  declareProperty("timePerBX", m_timePerBX = 25.0*Gaudi::Units::ns);
  
}
//=============================================================================
// Destructor
//=============================================================================
OTRawBankDecoder::~OTRawBankDecoder() {}

//=============================================================================

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode OTRawBankDecoder::initialize() {
  
  debug()<<"initializing OTRawBankDecoder"<<endmsg;
  
  StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  // Setup incident services
  incSvc()->addListener( this, IncidentType::EndEvent );
  
  // Loading OT Geometry from XML
  m_tracker = getDet<DeOTDetector>(DeOTDetectorLocation::Default );
  m_detectordata = new OTRawBankDecoderHelpers::Detector() ;
  
  // Read out window tool
  IOTReadOutWindow* aReadOutWindow = tool<IOTReadOutWindow>("OTReadOutWindow");
  m_startReadOutGate  = aReadOutWindow->startReadOutGate();
  release( aReadOutWindow );
  
  m_nsPerTdcCount = m_timePerBX/ double(m_countsPerBX);
  OTLiteTime::setNsPerTdcCount( m_nsPerTdcCount ) ;
  
  return StatusCode::SUCCESS;
};

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
  size_t numgolheaders(0) ;
  bool decodingerror(false) ;
  
  for (std::vector<RawBank*>::const_iterator  ibank = OTBanks.begin();
       ibank != OTBanks.end() && !decodingerror ; ++ibank) {
    // get bank version
    int bVersion = (*ibank)->version();
    unsigned int nTell1 = 0u;
    if (bVersion == OTBankVersion::v1) {
      //set up decoding with one header word
      nTell1 = 3;
    }
    else if (bVersion == OTBankVersion::v2 ) {
      //set up decoding with one header word
      nTell1 = 1;
    } else {
      error() << "Cannot decode OT raw buffer bank version "
	      << bVersion << " with this version of OTDAQ" << endmsg;
      decodingerror = true ;
    }
    
    if( !decodingerror ) {
      
      if (msgLevel(MSG::DEBUG))
	debug() << " Bank Nr " << (*ibank)->sourceID() << " with Size "
		<< (*ibank)->size()/4 << endmsg;
      
      const unsigned int* begin = (*ibank)->data() + nTell1 ;
      const unsigned int* end   = (*ibank)->data() + (*ibank)->size()/4 ;
      const unsigned int* idata ;
      for( idata = begin ; idata < end; ++idata ) {
	LHCb::GolHeader golHeader(*idata) ;
	unsigned int size = golHeader.size();
	unsigned int station = golHeader.station();
	unsigned int layer = golHeader.layer();
	unsigned int quarter = golHeader.quarter();
	unsigned int module = golHeader.module();
	m_detectordata->module(station,layer,quarter,module).setGolHeader(golHeader,idata+1) ;
	idata += size ;
	++numgolheaders ;
	
      }
      if(idata != end) {
	error() << "OTRawBankDecoder: data pointer mismatch: " << idata << " " << end << endreq ;
	decodingerror = true ;
      }
    }
  }
  
  // make sure we don't call this until the next event
  m_detectordata->setGolHeadersLoaded(true) ; 
  
  // this needs to be replaced with something sensible in real data
  OTLiteTime::setNsPerTdcCount( m_nsPerTdcCount ) ;
  
  return decodingerror ? StatusCode::FAILURE : StatusCode::SUCCESS ;
}

//==============================================================================
inline int getStrawID(const int otisID, const int channel) {
  
  /*
   * Conversion from Straw Number 1 to 128 in Otis (0 to 3) and Channel Number
   * form 0 to 31. The second numberig scheme is the Eletronic Numberig Scheme.
   */
  
  int straw = 0;
  if ((otisID == 0) || (otisID == 1)) {
    straw = (channel + 1) + otisID * 32;
  }
  else if ((otisID == 3) || (otisID == 2)) {
    int tempstraw = (31 - channel) ;
    int tempOtis = 0;
    if (otisID == 2) {
      tempOtis =  3 * 32;
    }
    else {
      tempOtis =  2 * 32;
    }
    straw = tempstraw + tempOtis + 1;
  }
  return (straw);
}

//=============================================================================
//Decode the data of a single module.
//=============================================================================
size_t OTRawBankDecoder::decodeModule( OTRawBankDecoderHelpers::Module& moduledata ) const 
{
  // load the module data if necessary
  if( !moduledata.isDecoded() ) {
    
    // load the Gol headers if necessary
    if( !m_detectordata->golHeadersLoaded() ) decodeGolHeaders().ignore() ;
    
    const unsigned int* data = moduledata.data();
    if( data != 0 ) {
      
      const GolHeader& golHeader = moduledata.golHeader() ;
      
      size_t size = golHeader.size();
      // Given Gol Header we Get Station, Layer, Quarter, Module Nr.
      unsigned int station = golHeader.station();
      unsigned int layer = golHeader.layer();
      unsigned int quarter = golHeader.quarter();
      unsigned int module = golHeader.module();
      
      //std::cout << "decoding module with num hits = " << size << std::endl ;
      //std::cout << "num hits in global list before: " << m_detectordata->m_ottimes.size() << std::endl ;
      
      for(unsigned int i=0; i<size; ++i) {
	DataWord dataWord = data[i];
	
	//getting data word information using the mask
	int nextTime = dataWord.nextTime();
	int nextChannelID = dataWord.nextChannel();
	int nextOtisID = dataWord.nextOtis();
	int firstTime = dataWord.firstTime();
	int firstChannelID = dataWord.firstChannel();
	int firstOtisID = dataWord.firstOtis();
	
	//Get Straw numbers
	int Fstraw  = getStrawID(firstOtisID, firstChannelID);
	int Nstraw = ((nextTime==0 && nextOtisID==0 && nextChannelID==0) ? 0 : getStrawID(nextOtisID, nextChannelID)) ;
	
	// format statement is always evaluated so check msg level
	if (msgLevel(MSG::DEBUG)) {
	  debug() << " OTTIME " << format("firstOtisID %d, firstStrawID %d, firstTime %d, "
					  "nextOtisID %d, nextStrawID %d, nextTime %d",
					  firstOtisID, firstChannelID, firstTime,
					  nextOtisID, nextChannelID, nextTime) << endmsg;
	}
	
	//Get First ChannelID
	OTChannelID fchannelID(station, layer, quarter, module, Fstraw, firstTime);
	moduledata.ownedottimes().push_back( OTLiteTime( fchannelID ) ) ;
	
	//Next Hit
	if (Nstraw != 0) { //To Check that this is not a No Channel Case
	  
	  //Get Next ChannelID
	  OTChannelID nchannelID(station, layer, quarter, module, Nstraw, nextTime);
	    moduledata.ownedottimes().push_back(OTLiteTime( nchannelID ) ) ;
	}
      }
    }
    moduledata.setIsDecoded(true) ;
  }
  return moduledata.ottimes().size() ;
}


LHCb::OTLiteTimeRange OTRawBankDecoder::decodeModule( const LHCb::OTChannelID& moduleid ) const
{
  OTRawBankDecoderHelpers::Module& moduledata =
    m_detectordata->module(moduleid.station(),moduleid.layer(),moduleid.quarter(),moduleid.module()) ;
  if( !moduledata.isDecoded() ) decodeModule( moduledata ) ;
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
