// $Id: OTTimeCreator.cpp,v 1.6 2004-12-10 08:10:25 jnardull Exp $
// Include files

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

// Declaration of the Algorithm Factory
static const  AlgFactory<OTTimeCreator>          s_factory ;
const        IAlgFactory& OTTimeCreatorFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
OTTimeCreator::OTTimeCreator( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  this->declareProperty( "OutputLocation",
                         m_timeLocation = OTTimeLocation::Default );
  declareProperty("ToFCorrection", m_tofCorrection = true);
  declareProperty("countsPerBX", m_countsPerBX = 64);
  declareProperty("numberOfBX", m_numberOfBX = 2);
  declareProperty("timePerBX", m_timePerBX = 25*ns);

   
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
  DeOTDetector* tracker = getDet<DeOTDetector>(DeOTDetectorLocation::Default );
  m_tracker = tracker;
  
  // Read out window tool
  IOTReadOutWindow* aReadOutWindow = 0;
  aReadOutWindow = tool<IOTReadOutWindow>("OTReadOutWindow");
  m_startReadOutGate  = aReadOutWindow->startReadOutGate();
  release( aReadOutWindow );

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode OTTimeCreator::execute() {

  // Retrieve the RawEvent:
  RawEvent* event;
  
  if( exist<RawEvent>(  RawEventLocation::Default )) {
    event = get<RawEvent>( RawEventLocation::Default );
  } else {
    debug() << "Raw Event does not exist at " 
            << RawEventLocation::Default << endmsg;
    debug() << "Accessing RawBuffer for decoding" << endmsg;
    RawBuffer* rawBuffer = get<RawBuffer>( RawBufferLocation::Default );
    event = new RawEvent(*rawBuffer);
    StatusCode sc = put( event, RawEventLocation::Default );
    if( sc.isFailure() ) 
      return Error( "Unable to register RawEvent to TES", sc );
  } 

  // Get the buffers associated with OT
  const std::vector<RawBank>& OTBanks = event->banks( RawBuffer::OT );
  
  // make OTTime container 
  OTTimes* outputTimes = new OTTimes();
  // register output buffer to TES
  put(outputTimes, m_timeLocation);

  // Loop over vector of banks (The Buffer)
  std::vector<RawBank>::const_iterator ibank;
  for ( ibank = OTBanks.begin(); ibank != OTBanks.end(); ++ibank) {
    // Check the bank version
    if( (*ibank).version() != OTBankVersion::v1 ) {
      error() << "Cannot decode OT raw buffer bank version "
              << (*ibank).version() << " with this version of OTDAQ" << endmsg;
      return StatusCode::FAILURE;
    }
    //Getting the values of the number of bank and of the Bank size 
    long bankSize = (*ibank).dataSize();

    // Some Useful Initilisation
    GolHeader golHeader;
    DataWord dataWord;
    int check = 1;
    unsigned nStation = 0;
    unsigned nLayer = 0;
    unsigned nQuarter = 0;
    unsigned nModule = 0;  
  
    
    // The bank are vec. of raw_int: Loop over the data words inside the bank 
    const raw_int * data = (*ibank).data();  
    for ( long i = 0; i < bankSize; ++i ) {
      raw_int aDataWord = data[i];
      raw_int isData = 0;
      
      /* Let's introduce the Golheader and the Data Word doing 
       * a logical & between aDataWord and dataMask  
       */
      
      if(i < 3 ){
      } else if(i > 2) {
        isData = dataMask & aDataWord;
       
        // We get an undefinedobjects: a golHeader or a data Word
        if(isData != 0 ){
          dataWord = aDataWord;
          check = 0;
        }
        else if( isData == 0 ){
          golHeader = aDataWord;
          // Given Gol Header we Get Station, Layer, Quarter, Module Nr.
          nStation = golHeader.station();
          nLayer = golHeader.layer();
          nQuarter = golHeader.quarter();
          nModule = golHeader.module();
        } 
        
        // Given Station, Layer, Quarter, Module Nr. and  Data Word, 
        // get the OT Time and put it in the output container
        
        if(check == 0){ //To avoid filling OTTimes with Empty stuff
          raw2OTTime(nStation, nLayer, nQuarter, nModule, dataWord, 
                     *outputTimes);
          check = 1;
        } // To avoid useless Calling of Raw to OTTime function 
        
      } // No Tell 1 Header
    }// Loop over the data words inside the bank    
  }  // Loop over vector of banks (The Buffer)
  
  return StatusCode::SUCCESS;
};

//=============================================================================
StatusCode OTTimeCreator::raw2OTTime(int station, int layer, int quarter,
                                     int module, DataWord dataWord,
                                     OTTimes& times)
{
  //getting data word information using the mask
  int nextTime = dataWord.nextTime();
  int nextChannelID = dataWord.nextChannel();
  int nextOtisID = dataWord.nextOtis();
  int firstTime = dataWord.firstTime();
  int firstChannelID = dataWord.firstChannel();
  int firstOtisID = dataWord.firstOtis();

  //Get Straw numbers
  int Fstraw  = getStrawID (firstOtisID, firstChannelID);  
  int Nstraw = 0;
  
  //Next Channel = 0
  if((nextTime == 0) && (nextOtisID == 0) && (nextChannelID == 0)){
    Nstraw = 0;
  } else {
    Nstraw  = getStrawID (nextOtisID, nextChannelID);
  }
  
  //Get First ChannelID  
  OTChannelID fchannelID(station, layer, quarter, module, Fstraw, firstTime);
  
  // make times
  this->createTimes(fchannelID, times);
  
  //Next Hit 
  if( Nstraw != 0) { //To Check that this is not a No Channel Case
    
    //Get Next ChannelID  
    OTChannelID nchannelID(station, layer, quarter, module, Nstraw, nextTime);
        
    // make times
    this->createTimes(nchannelID, times);
  }
  return StatusCode::SUCCESS;
}
//==========================================================================
StatusCode OTTimeCreator::createTimes(const OTChannelID aChan, OTTimes& times)

{
  int tdctime = aChan.tdcTime();
  double unCorrectedTime = (double)(tdctime);
  double correctTime; 
  if (m_tofCorrection) { 
    correctTime = correctedTime(aChan,unCorrectedTime);
  } else {
    correctTime = unCorrectedTime;
  }
  OTTime* newTim = new OTTime(aChan, correctTime);
  times.insert(newTim);
  return StatusCode::SUCCESS;
}
//==========================================================================
double OTTimeCreator::correctedTime(const OTChannelID aChan, 
                                    double unCorrectedTime) const
{
  // Conversion to TDC counts
  unCorrectedTime /= double(m_countsPerBX) / m_timePerBX ;

  //Apply Time of Flight
  //Currently take time of flight to 0,0,0 xz proj,get straw's xyz
  DeOTModule* aModule = m_tracker->module(aChan);
  double timeOfFlight = -99999.;
  if (aModule != 0) {
    HepPoint3D aPoint = aModule->centerOfStraw(aChan.straw());
    timeOfFlight = sqrt(pow(aPoint.x(),2.)+pow(aPoint.z(),2.))/c_light;
  } else {
    warning () << "Failed to find DeOTModule" << endreq;
  }
  // Apply Time of Flight Correction and Read Out Gate Correction
  unsigned stationNum = (aChan).station();
  return (unCorrectedTime - timeOfFlight + m_startReadOutGate[stationNum-1]); 
}

//==============================================================================
int OTTimeCreator::getStrawID(int otisID , int channel)

{ 

  /*
   * Conversion from Straw Number 1 to 128 in Otis (0 to 3) and Channel Number
   * form 0 to 31. The second numberig scheme is the Eletronic Numberig Scheme.
   */
  
  int straw = 0;
  int tempOtis = 0;
  if((otisID == 0) || (otisID == 1)){
    straw = (channel + 1) + otisID * 32;
  } else if((otisID == 3) || (otisID == 2)){
    int tempstraw = (31 - channel) ;
    if(otisID == 2){tempOtis =  3 * 32;}
    else if(otisID == 3){tempOtis =  2 * 32;}
    straw = tempstraw + tempOtis + 1;
  }
  return(straw);
}



