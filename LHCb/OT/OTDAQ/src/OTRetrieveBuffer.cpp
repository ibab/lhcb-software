// $Id: OTRetrieveBuffer.cpp,v 1.1.1.1 2004-02-03 09:49:17 jnardull Exp $
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

  // fillBuffer = new OTRFillRawBuffer();
  
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode OTRetrieveBuffer::execute() {

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
  std::vector<RawBank>::const_iterator ibank;
  for ( ibank = OTBanks.begin(); ibank != OTBanks.end(); ++ibank) {
    //Getting the values of the number of banks and of the Bank size 
    int nbank = (*ibank).bankSourceID();
    long bankSize = (*ibank).dataSize();
    
    // Some Initilisation
    raw_int dataWord = 0;
    raw_int golHeader = 0;
    int GolID = 0;

    // The bank are vectors of raw int.
    //We get the information that are stored in each raw int. 
    const raw_int * data = (*ibank).data();
    
    // Loop over the data words inside the bank 
    for ( long i = 0; i < bankSize; ++i ) {
      raw_int aDataWord = data[i];
    
      //Let's introduce the Golheader and the Data Word doing 
      // a logical & between aDataWord and dataMask  
      raw_int isData = dataMask & aDataWord;
      
      // We get an undefinedobject that is either the golHeader or the data Word
      if ( isData != 0 ) {
        dataWord = aDataWord;
      }   
      else if ( isData == 0 ) {
        golHeader = aDataWord;
        
        // given the gol header we get the Gol ID
        GolID = raw2OTGolID ( golHeader );
        continue;
      }

      // We  create a vector of tipe OT Digit called otDigits
      OTDigits doubleDigits;

      // Given Gol ID, Bank ID, the Data Word and the empty vector otDigits
      // we get the OT Digit
      raw2OTDigit(nbank, GolID, dataWord, doubleDigits);
      
      // Loop over all the doubleDigits vector and put in output container
      OTDigits::const_iterator iDigit;
      for ( iDigit = doubleDigits.begin(); 
            iDigit != doubleDigits.end(); ++iDigit ) {
        outputDigits->add( *iDigit );
      }

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

StatusCode OTRetrieveBuffer::raw2OTDigit(int nbank, 
                                         int GolID, 
                                         raw_int dataWord,
                                         OTDigits& vOTDs)
{
  //getting information using the mask
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

  //First
  OTChannelID fchannelID = this->getChannelID ( nbank, GolID, firstOtisID, 
                                               firstChannelID);

  // Vector 
  std::vector<int> firstTimes;
  firstTimes.push_back ( firstTime );
  OTDigit* firstDigit = new OTDigit ( fchannelID, firstTimes);
  vOTDs.add(firstDigit);

  //Next
  if ( nextTime != 0 ) {
    OTChannelID nchannelID  = this->getChannelID ( nbank, GolID, nextOtisID, 
                                                   nextChannelID );
    
    // Filling the next vector of type OTDigit called nextDigit 
    // with nextChannel and nextTime
    std::vector<int> nextTimes;
    nextTimes.push_back ( nextTime );
    OTDigit* nextDigit = new OTDigit ( nchannelID, nextTimes);
    vOTDs.add(nextDigit);
    
  }
  return StatusCode::SUCCESS;
  
}
 
//=============================================================================

int OTRetrieveBuffer::raw2OTGolID(raw_int golHeader)
{
  // getting the information of the GolID using the mask
  int GolID = golIDMask & golHeader;
  GolID =  GolID >> 29;
  return(GolID);
}

//=============================================================================
// Member functions
//=============================================================================

OTChannelID OTRetrieveBuffer::getChannelID ( int bank, int GolID, 
                                           unsigned int OtisID, 
                                           unsigned int bstraw )
{
  unsigned int station = 1;
  unsigned int layer = 0;
  unsigned int quarter = 0;
  unsigned int module = 0;
  unsigned int straw = 0;

  straw = (bstraw + 1) + OtisID * 32;

  while( bank > 36){
    station += 1;
    bank -= 36;
  }
  while(bank > 9){
    layer += 1;
    bank -= 9;
  }
  
  if( bank == 1 ){
    quarter = 0;
    if (GolID == 0){ module = 1; }
    else if (GolID == 1){ module = 2; }
    else if (GolID == 2){ module = 3; }
    else if (GolID == 3){ module = 4; }
  }
  else if( bank == 2 ){
    quarter = 0;
    if (GolID == 0){ module = 5; }
    else if (GolID == 1){ module = 6; }
    else if (GolID == 2){ module = 7; }
    else if (GolID == 3){ module = 8; }
  }
  else if( bank == 3 ){
    quarter = 1;
    if (GolID == 0){ module = 1; }
    else if (GolID == 1){ module = 2; }
    else if (GolID == 2){ module = 3; }
    else if (GolID == 3){ module = 4; }
  }
  else if( bank == 4 ){
    quarter = 1;
    if (GolID == 0){ module = 5; }
    else if (GolID == 1){ module = 6; }
    else if (GolID == 2){ module = 7; }
    else if (GolID == 3){ module = 8; }
  }
  else if( bank == 5 ){
    quarter = 2;
    if (GolID == 0){ module = 1; }
    else if (GolID == 1){ module = 2; }
    else if (GolID == 2){ module = 3; }
    else if (GolID == 3){ module = 4; }
  }
  else if( bank == 6 ){
    quarter = 2;
    if (GolID == 0){ module = 5; }
    else if (GolID == 1){ module = 6; }
    else if (GolID == 2){ module = 7; }
    else if (GolID == 3){ module = 8; }
  }
  else if( bank == 7 ){
    quarter = 3;
    if (GolID == 0){ module = 1; }
    else if (GolID == 1){ module = 2; }
    else if (GolID == 2){ module = 3; }
    else if (GolID == 3){ module = 4; }
  }
  else if( bank == 8 ){
    quarter = 3;
    if (GolID == 0){ module = 5; }
    else if (GolID == 1){ module = 6; }
    else if (GolID == 2){ module = 7; }
    else if (GolID == 3){ module = 8; }
  }
  else if( bank == 9 ){
    if (GolID == 0){
      quarter = 0;
      module = 9; 
    } else if (GolID == 1){
      quarter = 1;
      module = 9; 
    } else if (GolID == 2){
      quarter = 2;
      module = 9; 
    } else if (GolID == 3){
      quarter = 3;
      module = 9; }   
  }
  OTChannelID channel(station, layer, quarter, module, straw);
  return channel;
}








