// $Id: OTFillRawBuffer.cpp,v 1.1.1.1 2004-02-03 09:49:17 jnardull Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 

// from Detector
#include "OTDet/DeOTDetector.h"
// from Event
#include "Event/DAQTypes.h"
#include "Event/RawEvent.h"
#include "Event/RawBank.h"

// local
#include "OTDAQ/OTFillRawBuffer.h"

//-----------------------------------------------------------------------------
// Implementation file for class : OTFillRawBuffer
//
// 2004-01-09 : Jacopo Nardulli & Bart Hommels
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<OTFillRawBuffer>          s_factory ;
const        IAlgFactory& OTFillRawBufferFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
OTFillRawBuffer::OTFillRawBuffer( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : Algorithm ( name , pSvcLocator )
{
  this->declareProperty( "NumberOfBanks", numberOfBanks );
  this->declareProperty( "RawBufferLocation",   
                         m_RawBuffLoc = RawBufferLocation::Default );
  this->declareProperty( "OTDigitLocation", 
                         m_OTDigitLoc = OTDigitLocation::Default );
  this->declareProperty( "OTGeometryName", 
                         m_otTrackerPath = "/dd/Structure/LHCb/OT"); 
}

//=============================================================================
// Destructor
//=============================================================================
OTFillRawBuffer::~OTFillRawBuffer(){}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode OTFillRawBuffer::initialize() {
  MsgStream  msg( msgSvc(), name() );
  msg << MSG::DEBUG << "==> Initialise" << endreq;

  SmartDataPtr<DeOTDetector> Otracker( detSvc(), m_otTrackerPath);
  if( !Otracker){
    msg << MSG::ERROR
        << "Unable to retrieve Outer Tracker detector element from XML"
        << endreq;
    return StatusCode::FAILURE;
  }
  m_otTracker = Otracker;

  numberOfBanks = 128;
  
  // create a container for the vectors of OTdigits
  dataContainer = new mBank();
  // later: fix size to 128 entries for speed gain

  // init pointer to global container
  finalBuf = new dataBuffer();
  

  return StatusCode::SUCCESS;
}


//=============================================================================
// Main execution
//=============================================================================
StatusCode OTFillRawBuffer::execute() {

  // Retrieve the RawBuffer
  SmartDataPtr<RawBuffer> rawBuffer( eventSvc(), m_RawBuffLoc );
  if ( !rawBuffer ) {
    MsgStream  msg( msgSvc(), name() );
    msg << MSG::ERROR
        << "Unable to retrieve Raw buffer " <<  m_RawBuffLoc << endreq;
    return StatusCode::FAILURE;
  }
  m_rawBuffer = rawBuffer;
  

  // Retrieve OTDigits
  SmartDataPtr<OTDigits> digit( eventSvc(), m_OTDigitLoc );
  if ( !digit ) {
    MsgStream  msg( msgSvc(), name() );
    msg << MSG::ERROR
        << "Unable to retrieve OTDigit " << m_OTDigitLoc << endreq;
    return StatusCode::FAILURE;
  }
  m_otdigit = digit;


  this->sortDigitsIntoBanks();

  // loop the mbank structure, retrieve the vDigi vectors 
  for(mBank::iterator iBank = dataContainer->begin();
      iBank != dataContainer->end();
      iBank++){
    dataBank* aBank = new dataBank();
    
    vDigi* aDigi = (*iBank).second;
    
    // feed vdigi vectors to converter routine
    this->convertToRAWDataBank(aDigi, aBank);

    
    // put converted vDigi into final buffer container
    finalBuf->push_back(aBank);

    // generate bank ID and push bank into raw buffer
    //
    //Helder Lopez header (see note 2003-152)
    //The bank ID corresponds to the source ID
    //
    int bankID = (*iBank).first;
    dataBank& bBank = (*aBank);
    m_rawBuffer->addBank( bankID , RawBuffer::OT, bBank );
    aBank->erase( aBank->begin(),aBank->end() );
    
  }
  dataContainer->erase( dataContainer->begin(), dataContainer->end() );
  finalBuf->erase( finalBuf->begin(), finalBuf->end() );

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode OTFillRawBuffer::finalize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Finalize" << endreq;
  delete(dataContainer);
  delete(finalBuf);

  return StatusCode::SUCCESS;
}
//-----------------------------------------------------------------------------

StatusCode OTFillRawBuffer::sortDigitsIntoBanks()
{

  // create the map of OTDigi vectors, numberOfBanks entries
  for( int i = 1; i <= numberOfBanks; i++){
    vDigi* currentDigi = new vDigi();
    mBank::value_type numBank(i, currentDigi);
    dataContainer->insert( dataContainer->end(), numBank );
  }

  // Now the bank-containers are created. Sort the otdigits into these banks
  // sorting we don't care about for the time being
  mBank::iterator iBank = dataContainer->begin();
  vDigi* currentDigi;
    
  for(OTDigits::iterator iDigit = m_otdigit->begin();
      iDigit != m_otdigit->end();
      iDigit++){
    
    int nBankID = chID2int( (*iDigit)->channel() );
    
    iBank = dataContainer->find(nBankID);
    currentDigi = (*iBank).second;
    currentDigi->push_back( (*iDigit) );
    
  }
  return StatusCode::SUCCESS;
}
//-----------------------------------------------------------------
StatusCode OTFillRawBuffer::convertToRAWDataBank(vDigi* vToConvert, 
                                                  dataBank* aBank)
{
  vDigi* vGol0 = new vDigi();
  vDigi* vGol1 = new vDigi();
  vDigi* vGol2 = new vDigi();
  vDigi* vGol3 = new vDigi();
  
  for(vDigi::iterator iDigi = vToConvert->begin();
      iDigi != vToConvert->end();
      iDigi++){
    
    int nGol = chID2Gol( (*iDigi)->channel() );

    switch(nGol){
    case 0:
      vGol0->push_back( *iDigi );
      break;
    case 1:
      vGol1->push_back( *iDigi );
      break;
    case 2:
      vGol2->push_back( *iDigi );
      break;
    case 3:
      vGol3->push_back( *iDigi );
      break;
    }// switch nGol
    
  }// loop OTDigits inside vector

  // generate GOL headers
  
  //gol 0
  long size = vGol0->size()/2;
  if(((vGol0->size() / 2) * 2) != vGol0->size()){
    size = (vGol0->size() + 1) /2;
  } 
  raw_int gol0Header = createGolHeader(0, size);
  rawInt2Decode(gol0Header);

  //gol 1
  size = vGol1->size() / 2;
  if(((vGol1->size() / 2) * 2) != vGol1->size()){
    size = (vGol1->size() + 1) /2;
  }  
  raw_int gol1Header = createGolHeader(1, size);
  rawInt2Decode(gol1Header);

  //gol 2 
  size = vGol2->size() / 2;
  if(((vGol2->size() / 2) * 2) != vGol2->size()){
    size = (vGol2->size() + 1) /2;
  }
  raw_int gol2Header = createGolHeader(2, size);
  rawInt2Decode(gol2Header);

  //gol 3
  size = vGol3->size() / 2;
  if(((vGol3->size() / 2) * 2) != vGol3->size()){
    size = (vGol3->size() + 1) /2;
  }
  raw_int gol3Header = createGolHeader(3, size);
  rawInt2Decode(gol3Header);

  //loop the GOL sorted OTDigit vectors
  // convert the channel + time inf. in RAW format: 8bit channel + 8bit time
  // 8bit channel: 1 bit letf + 2 bit OTIS ID + 5 bit channel ID 
  // 8 bit time: 2 bit BX + 6 bit time itself

  // make a loop which you do 4 times, e.g.
  for (int i=0; i<4; i++){
  
    vDigi* pCurrent = 0;
    raw_int header;
    
    switch(i){
    case 0: 
      pCurrent = vGol0;
      header = gol0Header;
      break;      
    case 1: 
      pCurrent = vGol1;
      header = gol1Header;
      break;
    case 2: 
      pCurrent = vGol2;
      header = gol2Header;
      break;
    case 3: 
      pCurrent = vGol3;
      header = gol3Header;
      break;
      // loop over measurements
      //for iHits
    } //swith case
    vDigi::iterator iHitGolBegin = pCurrent->begin();
    vDigi::iterator iHitGolEnd = pCurrent->end();
    
    aBank->push_back(header);
    
    // now do the hits. What if there's none?
    if( pCurrent->size() == 0){
      continue;
    }
    
    vDigi::iterator iDigiCurrent = iHitGolBegin;
    
    while( iDigiCurrent != pCurrent->end() ){
      OTDigit* firstDigit = (*iDigiCurrent);
      long firstchannel = firstDigit->channel();
      long firstOtisID = chID2Otis( firstchannel );
      long firstStrawID = ( (firstDigit->channel()).straw());
      vInt& vTimes = firstDigit->tdcTimes();
      long firstTime = (*vTimes.begin());
      
      firstStrawID = ( firstStrawID - 1 ) % 32;
      
      vDigi::iterator iDigiNext = ++iDigiCurrent;
      long nextOtisID = 0;
      long nextStrawID = 0;
      long nextTime = 0;
      
      if(iDigiNext != iHitGolEnd){
        OTDigit* nextDigit = (*iDigiNext);
        long nextchannel = nextDigit->channel();
        nextOtisID = chID2Otis( nextchannel );
        nextStrawID = ((nextDigit->channel()).straw());
        vInt& vnextTime = nextDigit->tdcTimes();
        nextTime = (*vnextTime.begin());
        nextStrawID = ( nextStrawID - 1 ) % 32;
      }      
      
      // use the magic bitshift operators!
      int firstbit = 1;
      firstbit = firstbit << 31;
      firstOtisID = firstOtisID << 29;        // we know OTIS ID is only 2 bits
      firstStrawID = firstStrawID << 24;     // Straw ID is only 5 bits
      firstTime = firstTime << 16;
      nextOtisID = nextOtisID << 13;
      nextStrawID = nextStrawID << 8;
      nextTime = nextTime << 0;

      raw_int dataWord = 
        firstbit | firstOtisID | firstStrawID | firstTime | 
        nextOtisID | nextStrawID | nextTime;
      
      aBank->push_back(dataWord);
      
      rawInt2Decode(dataWord);

      if(iDigiNext != iHitGolEnd) iDigiCurrent++;
      

    }//while
  } // Loop over the Gols (i=0, i,4)

  return StatusCode::SUCCESS;
}
//-----------------------------------------------------------------------
StatusCode OTFillRawBuffer::rawInt2Decode(raw_int dataWord)
{

  raw_int firstBitMask = 0x80000000;
  int firstBit = (firstBitMask & dataWord) >> 31;
  

  if(!firstBit){
    int GolIDwithmask = dataWord & golIDMask;
    int GolSizewithmask = dataWord & golSizeMask;
    GolSizewithmask = GolSizewithmask >> 0;
    GolIDwithmask =  GolIDwithmask >> 29;
  }
  else{
    int NextTimewithmask = dataWord & NextTimeMask;
    NextTimewithmask = NextTimewithmask >> 0;
    int NextChannelwithmask = dataWord & NextChannelMask;
    NextChannelwithmask = NextChannelwithmask >> 8;
    int NextOtiswithmask = dataWord & NextOtisMask;
    NextOtiswithmask = NextOtiswithmask >> 13;
    int FirstTimewithmask = dataWord & FirstTimeMask;
    FirstTimewithmask = FirstTimewithmask >> 16;
    int FirstChannelwithmask = dataWord & FirstChannelMask;
    FirstChannelwithmask = FirstChannelwithmask >> 24;
    int FirstOtiswithmask = dataWord & FirstOtisMask;
    FirstOtiswithmask = FirstOtiswithmask >> 29;
  }
  
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------
raw_int OTFillRawBuffer::createGolHeader(int golID, int size)
{
  raw_int golHeader = 0;
  golID = golID << 29;
  golHeader = golHeader | golID | size;
  
  return(golHeader);
  
}


////==========================================================================
// Member functions
//=============================================================================

int OTFillRawBuffer::chID2Gol(OTChannelID otChannel)

{    
  int GolID = 0;
  int tempGolID = 0;
  unsigned long nQuarter = otChannel.quarter();
  unsigned long nModule = otChannel.module();
  
  //Loop to Obtain the GolID from the Module & Quarter  Number
  
  if(nQuarter == 0){
    if( (nModule == 1) || (nModule == 5) || (nModule == 9) ) {
      tempGolID = 0;
    }
    else if( (nModule == 2) || (nModule == 6) ){
      tempGolID = 1;
    }
    else if( (nModule == 3) || (nModule == 7) ){
      tempGolID = 2;
    }
    else if( (nModule == 4) || (nModule == 8) ){
      tempGolID = 3;
    }
  }
  else if(nQuarter == 1){
    if( (nModule == 1) || (nModule == 5) ) {
      tempGolID = 0;
    }
    else if( (nModule == 2) || (nModule == 6) || (nModule == 9) ){
      tempGolID = 1;
    }
    else if( (nModule == 3) || (nModule == 7) ){
      tempGolID = 2;
    }
    else if( (nModule == 4) || (nModule == 8) ){
      tempGolID = 3;
    }
  }
  else if(nQuarter == 2){
    if( (nModule == 1) || (nModule == 5) ) {
      tempGolID = 0;
    }
    else if( (nModule == 2) || (nModule == 6) ){
      tempGolID = 1;
    }
    else if( (nModule == 3) || (nModule == 7) || (nModule == 9) ){
      tempGolID = 2;
    }
    else if( (nModule == 4) || (nModule == 8) ){
      tempGolID = 3;
    }
  }
  else if(nQuarter == 3){
    if( (nModule == 1) || (nModule == 5) ) {
      tempGolID = 0;
    }
    else if( (nModule == 2) || (nModule == 6) ){
      tempGolID = 1;
    }
    else if( (nModule == 3) || (nModule == 7) ){
      tempGolID = 2;
    }
    else if( (nModule == 4) || (nModule == 8) || (nModule == 9) ){
      tempGolID = 3;
    }
  }
  
  GolID = tempGolID;
  return(GolID);
}




//=============================================================================
// Member functions
//=============================================================================

int OTFillRawBuffer::chID2Otis(OTChannelID otChannel)

{    
  int OtisID = 0;
  unsigned long nStraw = otChannel.straw();

  //Loop to Obtain the OtisID from the Straw Number

  if(( 1 <= nStraw ) && ( nStraw <= 32)){
    OtisID = 0;
  }
  else if(( 33 <= nStraw ) && ( nStraw <= 64)){
    OtisID = 1;
  }
  else if(( 65 <= nStraw ) && ( nStraw <= 96)){
    OtisID = 2;
  }
  else if(( 97 <= nStraw ) && ( nStraw <= 128)){
    OtisID = 3;
  }
  return(OtisID);
}





//=============================================================================
// Member functions
//=============================================================================

int OTFillRawBuffer::chID2int(OTChannelID otChannel)

{    
  int nArray = 0;
  int tempnArray = 0;
  unsigned long nStation = otChannel.station();
  unsigned long nLayer = otChannel.layer();  
  unsigned long nQuarter = otChannel.quarter();
  unsigned long nModule = otChannel.module();
   
  // Loop to get the bank number from the OTChannel ID Information 
  
  if(nStation <= 3 ){                    //General loop for every hit!
    //Looping over the quarters and modules filling tempnArray
    if( (1 <= nModule) && (nModule <= 4 ) ){
      if( nQuarter == 0 ) {tempnArray = 1;}
      else if( nQuarter == 1 ) {tempnArray = 3;}
      else if( nQuarter == 2 ) {tempnArray = 5;}  
      else if( nQuarter == 3 ) {tempnArray = 7;}
    }
    else if( (5 <= nModule) && (nModule <= 8 ) ){
      if( nQuarter == 0 ) {tempnArray = 2;}
      else if( nQuarter == 1 ) {tempnArray = 4;}
      else if( nQuarter == 2 ) {tempnArray = 6;}  
      else if( nQuarter == 3 ) {tempnArray = 8;}
    }
    else if (nModule == 9){tempnArray = 9;}
    if(nStation == 1){          //Looping over the Station
      if(nLayer == 0){          // Loop Layer
        nArray = tempnArray;
      }
      else if(nLayer == 1){
        nArray = tempnArray + 9;
      }
      else if(nLayer == 2){
        nArray = tempnArray + 18;
      }
      else if(nLayer == 3){
        nArray = tempnArray + 27;
      }                             //Layer Loop 
    }                               //Station Loop      
    if(nStation == 2){          //Looping over the Station
      if(nLayer == 0){          // Loop Layer
        nArray = tempnArray + 36;
      }
      else if(nLayer == 1){
        nArray = tempnArray + 9 + 36;
      }
      else if(nLayer == 2){
        nArray = tempnArray + 18 + 36;
      }
      else if(nLayer == 3){
        nArray = tempnArray + 27 + 36;
      }                             //Layer Loop 
    }                               //Station Loop      
    if(nStation == 3){          //Looping over the Station
      if(nLayer == 0){          // Loop Layer
        nArray = tempnArray + 72;
      }
      else if(nLayer == 1){
        nArray = tempnArray + 9 + 72;
      }
      else if(nLayer == 2){
        nArray = tempnArray + 18 + 72;
      }
      else if(nLayer == 3){
        nArray = tempnArray + 27 + 72;
      }                             //Layer Loop 
    }                               //Station Loop      
  }
  return(nArray);
}

