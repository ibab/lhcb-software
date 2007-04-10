// $Id: OTTimeCreator.cpp,v 1.18 2007-04-10 15:21:51 cattanem Exp $
// Include files
// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/PhysicalConstants.h"

// from Detector
#include "OTDet/DeOTDetector.h"
#include "OTDet/DeOTModule.h"

// Event
#include "Event/OTTime.h"

/// GSL
#include "gsl/gsl_math.h"

// local
#include "OTTimeCreator.h"
#include "Event/OTBankVersion.h"

// Tool interface
#include "OTDAQ/IOTReadOutWindow.h"

//-----------------------------------------------------------------------------
// Implementation file for class : OTTimeCreator
//
// 2004-06-06 : Jacopo Nardulli
//-----------------------------------------------------------------------------

using namespace LHCb;
 
// Declaration of the tool Factory
DECLARE_ALGORITHM_FACTORY( OTTimeCreator );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
OTTimeCreator::OTTimeCreator( const std::string& name,
			      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "OutputLocation", m_timeLocation = OTTimeLocation::Default );
  declareProperty("ToFCorrection", m_tofCorrection = true);
  declareProperty("countsPerBX", m_countsPerBX = 64);
  declareProperty("numberOfBX", m_numberOfBX = 3);
  declareProperty("timePerBX", m_timePerBX = 25.0*Gaudi::Units::ns);
}

//=============================================================================
// Destructor
//=============================================================================
OTTimeCreator::~OTTimeCreator() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode OTTimeCreator::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  // Loading OT Geometry from XML
  m_tracker = getDet<DeOTDetector>(DeOTDetectorLocation::Default );
    
  // Read out window tool
  IOTReadOutWindow* aReadOutWindow = tool<IOTReadOutWindow>("OTReadOutWindow");
  m_startReadOutGate  = aReadOutWindow->startReadOutGate();
  release( aReadOutWindow );
  
  m_tdcConversion = m_timePerBX/ double(m_countsPerBX);

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode OTTimeCreator::execute() {

  // Retrieve the RawEvent:
  RawEvent* event = get<RawEvent>("DAQ/RawEvent" );
  
  // Get the buffers associated with OT
  const std::vector<RawBank*>& OTBanks = event->banks(RawBank::OT );
  
  // make OTTime container 
  OTTimes* outputTimes = new OTTimes();
  outputTimes->reserve(10000);
  // register output buffer to TES
  put(outputTimes, m_timeLocation);

  // Loop over vector of banks (The Buffer)
  unsigned int nTell1 = 0u;
  std::vector<RawBank*>::const_iterator ibank;
  for ( ibank = OTBanks.begin(); ibank != OTBanks.end(); ++ibank) {
    // get bank version
    int bVersion = (*ibank)->version();
    // Check bank version
    if (bVersion == OTBankVersion::v1) {
      //set up decoding with three header words
      nTell1 = 3;
    } else if (bVersion == OTBankVersion::v2 ) {
      //set up decoding with one header word
      nTell1 = 1;
    } else {
      error() << "Cannot decode OT raw buffer bank version "
              << bVersion << " with this version of OTDAQ" << endmsg;
      return StatusCode::FAILURE;
    }
            
    // get number of banks 
    unsigned int bankSize = (*ibank)->size();

    debug() << " Bank Nr " << (*ibank)->sourceID() << " with Size " 
            << bankSize/4 << endmsg;
 
    // Some Useful Initilisation
    GolHeader golHeader;
    DataWord dataWord;
    unsigned int station = 0u;
    unsigned int layer = 0u;
    unsigned int quarter = 0u;
    unsigned int module = 0u;  
    unsigned int size = 0u;
    unsigned int k = 0u;
        
    // A bank is vec of unsigned ints: Loop over the data words inside the bank
    unsigned int* data = (*ibank)->data();  
    for (unsigned int i = 0; i < bankSize/4; ++i ) {
        
      //loop after the Tell 1 Header
      if (i > nTell1-1) {
        //Check if is a GolHeader
        if ((i == nTell1) || (i == 1 + size + k)) {
          golHeader = data[i];
          size = golHeader.size();      
          // Given Gol Header we Get Station, Layer, Quarter, Module Nr.
          station = golHeader.station();
          layer = golHeader.layer();
          quarter = golHeader.quarter();
          module = golHeader.module();
          // DEBUG
          // format statement is always evaluated so check msg level
          if (msgLevel(MSG::DEBUG)) {
            debug() << " OTTIME " << format("Station %d, Layer %d, Quarter %d, Module %d, Size %d", 
                                            station, layer, quarter, module, size) << endmsg;
          }
          k = i;
        } else {
          // it is a dataword if it is not a golHeader.
          dataWord =  data[i];
          
          // Given Station, Layer, Quarter, Module Nr. and  Data Word, 
          // get the OT Time and put it in the output container
        
          raw2OTTime(station, layer, quarter, module, dataWord, *outputTimes);
        }
      } // No Tell 1 Header
    } // Loop over the data words inside the bank    
  } // Loop over vector of banks (The Buffer)
  
  return StatusCode::SUCCESS;
};

//=============================================================================
void OTTimeCreator::raw2OTTime(const int station, const int layer, const int quarter,
                               const int module, const DataWord dataWord,
                               OTTimes& times) const
{
  //getting data word information using the mask
  int nextTime = dataWord.nextTime();
  int nextChannelID = dataWord.nextChannel();
  int nextOtisID = dataWord.nextOtis();
  int firstTime = dataWord.firstTime();
  int firstChannelID = dataWord.firstChannel();
  int firstOtisID = dataWord.firstOtis();

  //Get Straw numbers
  int Fstraw  = getStrawID(firstOtisID, firstChannelID);  
  int Nstraw = ((nextTime==0 && nextOtisID==0 && nextChannelID==0)?0
                :getStrawID(nextOtisID, nextChannelID));
  
  // format statement is always evaluated so check msg level
  if (msgLevel(MSG::DEBUG)) {
    debug() << " OTTIME " << format("firstOtisID %d, firstStrawID %d, firstTime %d, " 
                                    "nextOtisID %d, nextStrawID %d, nextTime %d",
                                    firstOtisID, firstChannelID, firstTime,
                                    nextOtisID, nextChannelID, nextTime) << endmsg;  
  }
  
  //Get First ChannelID  
  OTChannelID fchannelID(station, layer, quarter, module, Fstraw, firstTime);
  
  // make times
  createTimes(fchannelID, times);
  
  //Next Hit 
  if (Nstraw != 0) { //To Check that this is not a No Channel Case
    
    //Get Next ChannelID  
    OTChannelID nchannelID(station, layer, quarter, module, Nstraw, nextTime);
        
    // make times
    createTimes(nchannelID, times);
  }
}

//==========================================================================
void OTTimeCreator::createTimes(const OTChannelID aChan, OTTimes& times) const
{
  /// conversion from tdc counts to seconds
  double unCorrectedTime = aChan.tdcTime()*m_tdcConversion;
  double correctTime = (m_tofCorrection?correctedTime(aChan,unCorrectedTime):unCorrectedTime); 
  times.insert(new OTTime(aChan, correctTime));
}

//==========================================================================
double OTTimeCreator::correctedTime(const OTChannelID aChan, 
                                    const double unCorrectedTime) const
{
  //Apply Time of Flight
  //Currently take time of flight to 0,0,0 xz proj,get straw's xyz
  DeOTModule* aModule = m_tracker->findModule(aChan);
  double timeOfFlight = -99999.;
  if (aModule) {
    Gaudi::XYZPoint aPoint = aModule->centerOfStraw(aChan.straw());
    timeOfFlight = gsl_hypot(aPoint.x(), aPoint.z())/Gaudi::Units::c_light;
  } else {
    warning () << "Failed to find DeOTModule" << endreq;
  }
  // Apply Time of Flight Correction and Read Out Gate Correction
  return (unCorrectedTime - timeOfFlight + m_startReadOutGate[aChan.station()-1]); 
}

//==============================================================================
int OTTimeCreator::getStrawID(const int otisID, const int channel) const{ 

  /*
   * Conversion from Straw Number 1 to 128 in Otis (0 to 3) and Channel Number
   * form 0 to 31. The second numberig scheme is the Eletronic Numberig Scheme.
   */
  
  /// If otisID == 0 or otisID == 1
  int straw = (channel + 1) + 32 * otisID;  
  /// If otisID == 2 or otisID == 3
  if (2 == otisID || 3 == otisID) straw = (32 - channel) + 32 * (5-otisID);

  return (straw);
}
