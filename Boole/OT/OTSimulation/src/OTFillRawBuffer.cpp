// $Id: OTFillRawBuffer.cpp,v 1.4 2004-11-10 13:05:14 jnardull Exp $
// Include files

// local
#include "OTFillRawBuffer.h"
#include "Event/OTBankVersion.h"

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
  : GaudiAlgorithm ( name , pSvcLocator )
{
  this->declareProperty( "NumberOfBanks", numberOfBanks );
  this->declareProperty( "RawBufferLocation",   
                         m_RawBuffLoc = RawBufferLocation::Default );
  this->declareProperty( "MCOTTimeLocation", 
                         m_MCOTTimeLoc = MCOTTimeLocation::Default );
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
 
   SmartDataPtr<DeOTDetector> Otracker( detSvc(), m_otTrackerPath);
   if( !Otracker){
     return Error ( "Unable to retrieve OT detector element from XML");
   }
   m_otTracker = Otracker;

  //Fixed Numbers of Banks and of Gols
  numberOfBanks = 24;
  numberOfGols = 18;

  // create a container for the vectors of MCOTimes
  dataContainer = new mBank(); // bank type
  goldatacontainer = new mGol();

  // init pointer to global container
  finalBuf = new dataBuffer();
  
   return StatusCode::SUCCESS;
}
//=============================================================================
// Main execution
//=============================================================================
StatusCode OTFillRawBuffer::execute() 
{
  // Retrieve the RawBuffer
  SmartDataPtr<RawBuffer> rawBuffer( eventSvc(), m_RawBuffLoc );
  if ( !rawBuffer ) {
    warning () << "Unable to retrieve Raw buffer at " <<m_RawBuffLoc << endreq;
    return StatusCode::FAILURE;
  }
  m_rawBuffer = rawBuffer;

  // Retrieve MCOTTime
  SmartDataPtr<MCOTTimes>
    mcTime ( eventDataService(), MCOTTimeLocation::Default);
  if ( !mcTime ) {
    warning () << "Unable to retrieve MCOTTime at " << m_MCOTTimeLoc << endreq;
    return StatusCode::FAILURE;
  }
  m_mcTime = mcTime;

  // Sorting MCTimes into Banks
  this->sortMcTimesIntoBanks();

  // Loop the map bank structure
  for(mBank::iterator iBank = dataContainer->begin();
      iBank != dataContainer->end();
      iBank++){
    dataBank* aBank = new dataBank();
    vmcOTime* amcTime = (*iBank).second;

    // Empty Bank -- Still sending all the Headers
    if(amcTime->size() == 0){
      this->convertToRAWEmptyBank(aBank);
    }
    
    // feed vMCOTime vectors to converter routine
    this->convertToRAWDataBank(amcTime, aBank);
   
    // put converted vMCOTime into final buffer container
    finalBuf->push_back(aBank);

    /* 
     *  Generate bank ID and push bank into raw buffer
     *  Helder Lopez header (see note 2003-152)
     *  The bank ID corresponds to the source ID
     */

    int bankID = (*iBank).first;
    dataBank& bBank = (*aBank);
    m_rawBuffer->addBank( bankID , RawBuffer::OT, bBank, OTBankVersion::v1 );  
    aBank->erase( aBank->begin(),aBank->end() );
  }
  
  // clear all containers used in the process:
  dataContainer->erase( dataContainer->begin(), dataContainer->end() );
  goldatacontainer->erase( goldatacontainer->begin(), goldatacontainer->end() );
  finalBuf->erase( finalBuf->begin(), finalBuf->end() );
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode OTFillRawBuffer::finalize() 
{      
  delete(dataContainer);
  delete(goldatacontainer);
  delete(finalBuf);
  return StatusCode::SUCCESS;
}
//-----------------------------------------------------------------------------
StatusCode OTFillRawBuffer::sortMcTimesIntoBanks()
{
  // create the map of OTMcTime vectors, numberOfBanks entries
  for( int i = 1; i <= numberOfBanks; i++){
    vmcOTime* currentMCOTTime = new vmcOTime();
    mBank::value_type numBank(i, currentMCOTTime);
    dataContainer->insert( dataContainer->end(), numBank );
  }
  // Now the bank-containers are created. Sort the MCOTTimes into these banks
  mBank::iterator iBank = dataContainer->begin();
  vmcOTime* currentMCOTTime;
    
  for(vmcOTime::iterator iTime = m_mcTime->begin();
      iTime != m_mcTime->end();
      iTime++){

    int nBankID = chID2int( (*iTime)->channel() );
    iBank = dataContainer->find(nBankID);
    currentMCOTTime = (*iBank).second;
    currentMCOTTime->push_back( (*iTime) );
  }
  return StatusCode::SUCCESS;
}
//-------------------------------------------------------------------------
StatusCode OTFillRawBuffer::sortMcTimesIntoGol(vmcOTime* BankmcOTime, 
                                              dataBank* /* aBank */)

{
  // create the map of OT vectors, numberOfGols entries
  for( int k = 1; k <= numberOfGols; k++){
    vmcOTime* currentMCTime = new vmcOTime();
    mGol::value_type numGol(k, currentMCTime);
    goldatacontainer->insert( goldatacontainer->end(), numGol );
  }
  // Now the gol-containers are created. Sort the mcottimes into these Gols
  mGol::iterator iGol = goldatacontainer->begin();
  vmcOTime* currentMCTime;
  // There are 18 Gol per Bank, each one containing the data of a short module 
  for(vmcOTime::iterator iTime = BankmcOTime->begin();
      iTime != BankmcOTime->end();
      iTime++){

    long nQuarter = ((*iTime)->channel()).quarter();
    long nModule = ((*iTime)->channel()).module();

    int GOL = 0;
    //There are 2 quarters in each Gol (It is necessary to specify)
    if((nQuarter == 1) || (nQuarter == 3)){GOL = nModule + 9;} 
    else if((nQuarter == 0 ) || (nQuarter == 2)){GOL = nModule;}
    int nGolID = GOL;
    iGol = goldatacontainer->find(nGolID);
    currentMCTime = (*iGol).second;
    currentMCTime->push_back( (*iTime) );
  }
  return StatusCode::SUCCESS;
}  
//-----------------------------------------------------------------
StatusCode OTFillRawBuffer::convertToRAWEmptyBank(dataBank* aBank)
{
  // Creating the Tell1 Headers - 3 words of 32 bits - I do not fill them...
  for(int i = 0; i < 3; i++){
    raw_int tell1Header = 0;
    aBank->push_back(tell1Header);
  }
  for(int j = 0; j < 19; j++){
    GolHeader golHeader(0, 0, 0, 0, 0, 0, 0);
    aBank->push_back(golHeader);
  }
 
  return StatusCode::SUCCESS;
}
//-----------------------------------------------------------------
StatusCode OTFillRawBuffer::convertToRAWDataBank(vmcOTime* vToConvert, 
                                                 dataBank* aBank)
{
  // Creating the Tell1 Headers - 3 words of 32 bits - I do not fill them...
  for(int i = 0; i < 3; i++){
    raw_int tell1Header = 0;
    aBank->push_back(tell1Header);
  }

  //Useful definitions 
  long nStation = 0; 
  long nLayer = 0; 
  long nQuarter = 0;
  long nQuarto = 0;
  long nModule = 0; 
  long nStraw = 0;
  long channel = 0;
  int quar = 0;

  // For each Bank Data we sort the MCOTTimes into Gol
  this->sortMcTimesIntoGol(vToConvert, aBank);
  vmcOTime::iterator IBank = vToConvert->begin();
  nQuarto = ((*IBank)->channel()).quarter();
  //Per each Gol we have 2 quarters data  - We need a useful Buffer to specify.
  if((nQuarto == 0) || (nQuarto == 1)){ quar = 1;}
  else if((nQuarto == 2) || (nQuarto == 3)){ quar = 2;}

  // loop the map GOL structure, retrieve the vmcOTime vectors 
  for(mGol::iterator iGol = goldatacontainer->begin();
      iGol != goldatacontainer->end();
      iGol++){

    //The New Gol McTime Vector : 
    vmcOTime* aGolMCTime = (*iGol).second;  

    //Defining iterator
    for(vmcOTime::iterator iTime = aGolMCTime->begin();
        iTime != aGolMCTime->end();
        iTime++){
    
      // Get Station, Layer, Quarter, Module Numbers for each MCOTTime
      nStation = ((*iTime)->channel()).station();
      nLayer = ((*iTime)->channel()).layer();  
      nQuarter = ((*iTime)->channel()).quarter();
      nModule = ((*iTime)->channel()).module();
      nStraw = ((*iTime)->channel()).straw();
      channel = ((*iTime)->channel());
    }

    /*
     * Creating the Gol Headers, given Station, Layer, Quarter, Module, Size
     * Numbers; Using Magic Shift operators
     */

    //The Size
    long size = aGolMCTime->size() / 2;
    if(((aGolMCTime->size() / 2) * 2) != aGolMCTime->size()){ 
      size = (aGolMCTime->size() + 1) /2;
    }
    
    // If the Size is 0, we need the correct values for module and quarter Nr.
    if( aGolMCTime->size() == 0){
      int IGOL = (*iGol).first;
      if(IGOL < 10 ){
        nModule = IGOL;
        //Using the Quarter Buffers introduced above to get which quarter is it
        if(quar == 1 ){nQuarter = 0;} 
        else if(quar == 2){nQuarter = 2;}
      } else if(IGOL > 9){
        nModule = IGOL - 9;
        if(quar == 1 ){nQuarter = 1;}
        else if(quar == 2){nQuarter = 3;}
      }
      size = 0;
    } 
    // GolHeader
    GolHeader golHeader(0, nStation, nLayer, nQuarter, nModule, 0 , size);
    // pushing the Gol Header
    aBank->push_back(golHeader);  
    
    // The Hits -- Some useful definitions
    vmcOTime* pCurrent = 0;    
    pCurrent = aGolMCTime;
    vmcOTime::iterator iHitGolBegin = pCurrent->begin();
    vmcOTime::iterator iHitGolEnd = pCurrent->end();
    vmcOTime::iterator iTimeCurrent = iHitGolBegin;
    
    /* 
     * Now the hits
     * convert the channel + time inf. in RAW format: 8bit channel 
     * 8 bit time 
     * 8bit channel: 1 bit letf + 2 bit OTIS ID + 5 bit channel ID 
     * 8 bit time: 2 bit BX + 6 bit time itself
     */
    
    while( iTimeCurrent != pCurrent->end() ){ //While loop over the MCOTTime
      MCOTTime* firstTime = (*iTimeCurrent);
      //First Time - we get Otis and Straw Number
      long firstOtisID = chID2Otis(  firstTime->channel() );
      long firstStrawID = ( (firstTime->channel()).straw());
      int firstTdcTime = ( (firstTime->channel()).tdcTime());

      //Next 
      vmcOTime::iterator iTimeNext = ++iTimeCurrent;

      //First we get Otis and Straw Number
      long nextOtisID = 0;
      long nextStrawID = 0;
      int nextTdcTime = 0;

      if(iTimeNext != iHitGolEnd){
        MCOTTime* nextTime = (*iTimeNext);
        nextOtisID = chID2Otis(  nextTime->channel() );
        nextStrawID = ((nextTime->channel()).straw());
        nextTdcTime = ((nextTime->channel()).tdcTime());

        /* Straw number conversion.
         * Converting straw number from 1 to 128 in a number from 0 to 31
         * The strange conversion depends on the fact that here we have a 
         * straw numbering  different from the one in OTChannelID, and more 
         * near to the eletronic scheme. 
         */

        if((nextOtisID == 0) || (nextOtisID == 1)){
          nextStrawID = ( nextStrawID - 1 ) % 32;
        } else if((nextOtisID == 2) || (nextOtisID == 3)){
          nextStrawID = 31 - ((nextStrawID - 1) % 32);
        }
      }
      // Straw Number Conversion
      if((firstOtisID == 0) || (firstOtisID == 1)){
        firstStrawID = ( firstStrawID - 1 ) % 32;
      } else if((firstOtisID == 2) || (firstOtisID == 3)){
        firstStrawID = 31 - ((firstStrawID - 1) % 32);
      }

      DataWord dataWord (1, firstOtisID, firstStrawID, firstTdcTime, 
                         0, nextOtisID, nextStrawID, nextTdcTime);
      aBank->push_back(dataWord);
      
      if(iTimeNext != iHitGolEnd) iTimeCurrent++;     
    } //while loop over MCOTTimes
    
    // Erase Gol Vector of McTime
    aGolMCTime->erase( aGolMCTime->begin(),aGolMCTime->end() );

  } // GOL Loop

  return StatusCode::SUCCESS;
}
//=============================================================================
// Member functions
//=============================================================================

int OTFillRawBuffer::chID2Otis(OTChannelID otChannel)

{    
  int OtisID = 0;
  unsigned int nStraw = otChannel.straw();

  //We get the OtisID from the Straw Number
  if(( 1 <= nStraw ) && ( nStraw <= 32)){ OtisID = 0;}
  else if(( 33 <= nStraw ) && ( nStraw <= 64)){OtisID = 1;}
  else if(( 65 <= nStraw ) && ( nStraw <= 96)){OtisID = 3;}
  else if(( 97 <= nStraw ) && ( nStraw <= 128)){OtisID = 2;}

  /* 
   * It seems wrong, but the point is that here we want a different straw 
   * numbering scheme from OTChannelID. In OTchannelID the Straw number 
   * goes from 1 to 64 for OTIS 0 & 1 and then from 65 to 128 for OTIS 2 and 3,
   * but for OTIS 2 and 3 we want it the other way round.
   * So first there is OTIS 3 (for straws from 65 to 96) and then OTIS 2.
   */

  return(OtisID);
}
//=============================================================================
// Member functions
//=============================================================================

int OTFillRawBuffer::chID2int(OTChannelID otChannel)

{    
  int nArray = 0;
  int tempnArray = 0;  
  int tempn2Array = 0;
  int tempn3Array = 0;
  unsigned long nStation = otChannel.station();
  unsigned long nLayer = otChannel.layer();  
  unsigned long nQuarter = otChannel.quarter();
   
  // Loop to get the bank number from the OTChannel ID Information
  if (nStation == 1){tempn3Array = 0;}
  else if (nStation == 2){tempn3Array = 8;}
  else if (nStation == 3){tempn3Array = 16;}
  if (nLayer == 0 ){ tempn2Array = 0;}
  else if (nLayer == 1 ){ tempn2Array = 2;}
  else if (nLayer == 2 ){ tempn2Array = 4;}
  else if (nLayer == 3 ){ tempn2Array = 6;}
  if((nQuarter == 0) || (nQuarter == 1)){ tempnArray = 1; }
  else if((nQuarter == 2) || (nQuarter == 3)){ tempnArray = 2; }
  nArray = tempn3Array + tempn2Array + tempnArray;
  return(nArray);
}

