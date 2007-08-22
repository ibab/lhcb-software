// $Id: OTRawBankDecoder.cpp,v 1.2 2007-08-22 08:35:52 jonrob Exp $
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
#include "OTRawBankDecoder.h"

#include "GaudiKernel/ChronoEntity.h"

namespace Tf
{

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
        for(int iStation = 0 ; iStation < 3 ; ++iStation )
          for(int iLayer = 0 ; iLayer < 4 ; ++iLayer )
            for(int iQuarter = 0 ; iQuarter < 4 ; ++iQuarter )
              for( int iModule = 0 ; iModule < 9 ; ++iModule )
                m_modules[iStation][iLayer][iQuarter][iModule].clearevent() ;
      }
      Module& module(int station, int layer, int quarter, int module) {
        return m_modules[station-1][layer][quarter][module-1] ;
      }
      bool golHeadersLoaded() const { return m_golHeadersLoaded ; }
      Module m_modules[3][4][4][9];
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
    declareInterface<IOTRawBankDecoder>(this);
    //declareProperty( "OutputLocation", m_timeLocation = OTTimeLocation::Default );
    //declareProperty("ToFCorrection", m_tofCorrection = true);
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

    m_tdcConversion = m_timePerBX/ double(m_countsPerBX);
    OTLiteTime::setNsPerTdc( m_tdcConversion ) ;

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
  //decode the gol headers in the raw banks. each header is assigned
  //to its corresponding module. the contents of the gol are only
  //decoded once the module data is asked for.
  //=============================================================================

  StatusCode OTRawBankDecoder::decodeGolHeaders() const
  {
    // Retrieve the RawEvent:
    // info() << "OTRawBankDecoder::decodeGolHeaders()" << endreq ;
    RawEvent* event = get<RawEvent>("DAQ/RawEvent" );

    // Get the buffers associated with OT
    const std::vector<RawBank*>& OTBanks = event->banks(RawBank::OT );

    // Loop over vector of banks (The Buffer)
    size_t numgolheaders(0) ;
    for (std::vector<RawBank*>::const_iterator  ibank = OTBanks.begin();
         ibank != OTBanks.end(); ++ibank) {
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
        return StatusCode::FAILURE;
      }

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

        // info() << "Found gol header "
        //         << station << " " << layer << " " << quarter << " " << module << " " << size << endreq ;

      }
      if(idata != end) std::cout << "data pointer mismatch: " << idata << " " << end << std::endl ;

    }

    (*m_detectordata).m_golHeadersLoaded = true ;
    //std::cout << "End of OTRawBankDecoder::decodeGolHeaders " << numgolheaders << std::endl ;
    return StatusCode::SUCCESS ;
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


  void OTRawBankDecoder::decodeModule( const LHCb::OTChannelID& moduleid,
                                       LHCb::OTLiteTimeRange& ottimes ) const
  {
    //std::cout << "OTRawBankDecoder::decodeModule: " << moduleid << std::endl ;

    // load the Gol headers if necessary
    if( !m_detectordata->golHeadersLoaded() ) decodeGolHeaders().ignore() ;

    OTRawBankDecoderHelpers::Module& moduledata =
      m_detectordata->module(moduleid.station(),moduleid.layer(),moduleid.quarter(),moduleid.module()) ;

    // load the module if necessary
    if( !moduledata.isDecoded() && moduledata.data()!=0) {

      // index to the first hit in this module
      //size_t beginhit = m_detectordata->m_ottimes.size() ;

      const unsigned int* data = moduledata.data();
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
    ottimes = moduledata.ottimes() ;
  }
}
