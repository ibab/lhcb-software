// $Id: OTRetrieveBuffer.cpp,v 1.4 2004-03-26 18:37:50 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"

// from Detector
#include "OTDet/DeOTDetector.h"

// Event
#include "Event/DAQTypes.h"
#include "Event/RawEvent.h"

// local
#include "OTDAQ/OTRetrieveBuffer.h"

//-----------------------------------------------------------------------------
// Implementation file for class : OTRetrieveBuffer
//
// 2004-01-26 : Jacopo Nardulli & Bart Hommels
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<OTRetrieveBuffer>          s_factory ;
const        IAlgFactory& OTRetrieveBufferFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
OTRetrieveBuffer::OTRetrieveBuffer( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : Algorithm ( name , pSvcLocator )
{
  this->declareProperty( "RawEventLocation",
                         m_RawEventLoc = RawEventLocation::Default );
  this->declareProperty( "OutputLocation",
                         m_digitLocation = OTDigitLocation::Default );
  
}
//=============================================================================
// Destructor
//=============================================================================
OTRetrieveBuffer::~OTRetrieveBuffer() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode OTRetrieveBuffer::initialize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Initialise" << endreq;
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode OTRetrieveBuffer::execute() {

  MsgStream  msg( msgSvc(), name() );
  msg << MSG::DEBUG << "==> Execute" << endreq;

  // Retrieve the RawEvent:
  SmartDataPtr<RawEvent> rawEvent( eventSvc(), m_RawEventLoc );
  
  if( ! rawEvent ){
    MsgStream  msg( msgSvc(), name() );
    msg << MSG::ERROR << "Unable to retrieve Raw Event at: " 
        <<  m_RawEventLoc << endreq;
    return StatusCode::FAILURE;
  }
  //Get the buffers associated with OT
  const std::vector<RawBank>& OTBanks = rawEvent->banks( RawBuffer::OT );
  // define the vector where the output goes
  OTDigits* outputDigits = new OTDigits();
  // register output buffer to TES
  eventSvc()->registerObject( m_digitLocation, outputDigits );

  // Loop over vector of banks (The Buffer)
  buffer = 1; // usefule buffer 
  std::vector<RawBank>::const_iterator ibank;
  for ( ibank = OTBanks.begin(); ibank != OTBanks.end(); ++ibank) {
    //Getting the values of the number of bank and of the Bank size 
    long bankSize = (*ibank).dataSize();
    
    // Some Useful Initilisation
    raw_int dataWord = 999999999; //To avoid filling Digits with empty stuff
    raw_int golHeader = 0;
    int nStation = 0;
    int nLayer = 0;
    int nQuarter = 0;
    int nModule = 0;  

    // The bank are vectors of raw int.
    //We get the information that are stored in each raw int. 
    const raw_int * data = (*ibank).data();
    
    // Loop over the data words inside the bank 
    for ( long i = 0; i < bankSize; ++i ) {
      raw_int aDataWord = data[i];
    
      /*Let's introduce the Golheader and the Data Word doing 
       * a logical & between aDataWord and dataMask  
       * We need two of them to skip the 4 Otis Headers 
       * present after each Gol Header (They do not contan useful inf.)
       */
      raw_int isData1 = dataMask1 & aDataWord;
      raw_int isData2 = dataMask2 & aDataWord;
     
      // We get two undefinedobjects that tell us if we 
      // got a golHeader or a data Word

      if((isData1 != 0 ) && (isData2 != 0 )){
        dataWord = aDataWord;
      }

      else if(( isData1 == 0 ) && (isData2 == 0)){
        golHeader = aDataWord;
        // Given Gol Header we Get Station, Layer, Quarter, Module Nr.
        nStation = getStation (golHeader);
        nLayer = getLayer (golHeader);
        nQuarter = getQuarter (golHeader);
        nModule = getModule (golHeader);
        continue;
      }
      // Given Station, Layer, Quarter, Module Nr. and  Data Word, 
      // get the OT digit and put it in the output container
      
      if(dataWord != 999999999){ //To avoid filling digit with Empty stuff
        raw2OTDigit(nStation, nLayer, nQuarter, nModule, dataWord, 
                    *outputDigits);
        dataWord = 999999999;
      } // To avoid useless Calling of Raw to OTDigit function 
      
    }// Loop over the data words inside the bank    
  }  // Loop over vector of banks (The Buffer)

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode OTRetrieveBuffer::finalize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Finalize" << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
StatusCode OTRetrieveBuffer::raw2OTDigit(int Station, int Layer, int Quarter,
                                         int Module, raw_int dataWord,
                                         OTDigits& vOTDs)
{
  // Vector of Integers Times
  std::vector<int> firstTimes;
  std::vector<int> nextTimes;

  //getting data word information using the mask
  int nextTime = dataWord & NextTimeMask;
  nextTime = nextTime >> 0;
  int nextChannelID = dataWord & NextChannelMask;
  nextChannelID = nextChannelID >> 8;
  int nextOtisID = dataWord & NextOtisMask;
  nextOtisID = nextOtisID >> 13;
  int firstTime = dataWord & FirstTimeMask;
  firstTime = firstTime >> 16;
  int firstChannelID = dataWord & FirstChannelMask;
  firstChannelID = firstChannelID >> 24;
  int firstOtisID = dataWord & FirstOtisMask;
  firstOtisID = firstOtisID >> 29;

  int Fstraw = 0;
  int Nstraw = 0;
  int onlyOnceBuffer = 0;

   //Using Buffer Mask
  raw_int dataWordBis = dataWord;
  multiBuffer = dataWordBis & multiBuffMask;
  multiBuffer = multiBuffer >> 0;
  nextBuffBis = dataWordBis & nextBuffMaskBis;
  nextBuffBis = nextBuffBis >> 8;
  nextBuff = dataWordBis & nextBuffMask;
  nextBuff = nextBuff >> 24;
  //Again
  raw_int dataWordBuffer = dataWord;
  firstBuff = firstBuffMask & dataWordBuffer;
  firstBuff = firstBuff  >> 16;
  firstTimeBuffer = timeBuffMask & dataWordBuffer;
  firstTimeBuffer = firstTimeBuffer >> 23;

  //Multiple Hit 
  //Checking the Multiple Hit
  if((firstOtisID == nextOtisID) && (firstChannelID == nextChannelID) 
     //Basilar requirment to have a multiple hit!
     && (nextTime != 0) && (multiBuffer != 101) && (multiBuffer != 105)
     && (buffer == 1)){
    //It is Necessary to Check the NextTime Value, and that there is no 
    //Multiple hit inside the first multiple hit
    if((nextBuff != 100) || ((nextBuff == 100) && (nextBuff == nextBuffBis) 
                             && (firstTime != 0) && (nextTime != 0))){
      //If there is a M. Hit exactly on the Magic Value of the Next Buffer:100
      if((firstBuff != 1) || 
         ((firstBuff == 1) && (firstBuff == nextOtisID) 
          && (firstOtisIDCopy == 1000))){
        //If there is a M. Hit exactly on the Magic Value of the First Buffer: 
        //Next Otis ID = 1.
        firstChannelIDCopy = firstChannelID;
        firstOtisIDCopy = firstOtisID;
        firstTimeCopy = firstTime; //Copy the First and Next Time and then exit
        nextTimeCopy = nextTime;
        buffer = 0;
        return StatusCode::SUCCESS;
      } //First Digit Multiple Hit Check
    } //Next Digit Multiple Hit Check
  } // Multiple Hit Checking 

  //Multiple Hit Filling the tim: First Digit 
  if((firstBuff == 1) && ( firstOtisIDCopy != 1000)){
    //Multiple Hit Get Straw numbers
    Nstraw  = getStrawID (nextOtisID, nextChannelID);
    Fstraw  = getStrawID (firstOtisIDCopy, firstChannelIDCopy);
    //Pushing times
    if(multiBuffer != 105){nextTimes.push_back( nextTime );}
    firstTimes.push_back( firstTimeCopy );
    firstTimes.push_back( nextTimeCopy );
    firstTimes.push_back( firstTimeBuffer );
    onlyOnceBuffer = 1;  
  }
  
 //Multiple Hit Filling the time Next Digit
  if((nextBuff == 100) && ( firstOtisIDCopy != 1000) && (onlyOnceBuffer != 1)){
    //Multiple Hit Get Straw numbers
    Fstraw  = getStrawID (nextOtisID, nextChannelID); //Looks strange, 
    Nstraw  = getStrawID (firstOtisIDCopy, firstChannelIDCopy); //but is correct
    if(multiBuffer != 101){firstTimes.push_back(nextTime);}
    //Pushing times
    nextTimes.push_back( firstTimeCopy );
    nextTimes.push_back( nextTimeCopy );
    nextTimes.push_back( firstTime );
    onlyOnceBuffer = 1;  
  }     
  // No Multiple Hit -- checking if statements
    if(((firstBuff != 1) || (nextTime == 0) || ( firstOtisIDCopy == 1000)) 
       && (onlyOnceBuffer != 1 )){
      //Get Straw numbers
      Fstraw  = getStrawID (firstOtisID, firstChannelID);
      Nstraw  = getStrawID (nextOtisID, nextChannelID);
      //Filling time --- in case of First Multiple Hit, it has already be filled
      int null = 0;
      firstTimes.push_back( firstTime ); //First Time
      firstTimes.push_back( null );
      firstTimes.push_back( null );
      nextTimes.push_back ( nextTime ); //Next Time
      nextTimes.push_back( null );
      nextTimes.push_back( null );
    }
  //Get First ChannelID
  if(multiBuffer != 101){
    OTChannelID fchannelID = this->getChannelID(Station, Layer, 
                                                Quarter, Module, Fstraw);
    // New First OTDigit
    OTDigit* firstDigit = new OTDigit ( fchannelID, firstTimes);
    vOTDs.add(firstDigit);
  }
  //Next Hit 
  if(multiBuffer != 105){
    if ( nextTime != 0 ) { //To Check that this is not a No Channel Case
      //Get Next ChannelID
      OTChannelID nchannelID = this->getChannelID(Station, Layer, 
                                                  Quarter, Module, Nstraw);
      // Filling the next vector of type OTDigit called nextDigit 
      // with nextChannel and nextTime
      OTDigit* nextDigit = new OTDigit ( nchannelID, nextTimes);
      vOTDs.add(nextDigit);
    }
  }
  
  //To restart the First Multiple Hit Loop
  firstTimeCopy = 1000000;
  nextTimeCopy = 1000000;
  firstOtisIDCopy = 1000;
  firstChannelIDCopy = 1000000;
  nextBuffBis = 55;
  nextBuff = 50;
  multiBuffer = 51;
  firstBuff = 0;
  onlyOnceBuffer = 0;
  buffer = 1; 
  return StatusCode::SUCCESS;
}

//=============================================================================
// Member functions
//=============================================================================

int OTRetrieveBuffer::getStation(raw_int golHeader)

{    
  //getting gol header information using the mask: Station
  int nStation = golHeader & StationMask;
  nStation = nStation >> 29;
  return(nStation);
}
//=============================================================================
// Member functions
//=============================================================================
int OTRetrieveBuffer::getLayer(raw_int golHeader)

{    
  //getting gol header information using the mask: Layer
  int nLayer = golHeader & LayerMask;
  nLayer = nLayer >> 27;
  return(nLayer);
}
//=============================================================================
// Member functions
//=============================================================================
int OTRetrieveBuffer::getQuarter(raw_int golHeader)

{    
  //getting gol header information using the mask: Quarter
  int nQuarter = golHeader & QuarterMask;
  nQuarter = nQuarter >> 25;
  return(nQuarter);
}
//=============================================================================
// Member functions
//=============================================================================
int OTRetrieveBuffer::getModule( raw_int golHeader)

{    
  //getting gol header information using the mask: Module
  int nModule = golHeader & ModuleMask;
  nModule = nModule >> 21;
  return(nModule);  
}
//=============================================================================
// Member functions
//=============================================================================
int OTRetrieveBuffer::getStrawID(int otisID , int channel)

{    
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
//=============================================================================
// Member functions
//=============================================================================
OTChannelID OTRetrieveBuffer::getChannelID (int station, int layer, 
                                            int quarter, int module, int straw)
{
  OTChannelID channel(station, layer, quarter, module, straw);
  return channel;
}





