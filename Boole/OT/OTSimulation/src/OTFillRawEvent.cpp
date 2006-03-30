// Include files

// From Gaudi
#include "GaudiKernel/AlgFactory.h"

// EVENT/DAQ/OTDAQ
#include "Event/RawEvent.h"
#include "Event/RawBank.h"
#include "Event/GolHeader.h"
#include "Event/DataWord.h"
#include "Event/OTBankVersion.h"

// local
#include "OTFillRawEvent.h"

//-----------------------------------------------------------------------------
// Implementation file for class : OTFillRawEvent
//
// 2004-01-09 : Jacopo Nardulli & Bart Hommels
//-----------------------------------------------------------------------------

using namespace LHCb;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( OTFillRawEvent );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
OTFillRawEvent::OTFillRawEvent( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  this->declareProperty( "NumberOfBanks", m_numberOfBanks = 24 );
  this->declareProperty( "NumberOfGols",  m_numberOfGols  = 18 );
  this->declareProperty( "MCOTTimeLocation", 
                         m_MCOTTimeLoc = MCOTTimeLocation::Default );
}

//=============================================================================
// Destructor
//=============================================================================
OTFillRawEvent::~OTFillRawEvent(){}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode OTFillRawEvent::initialize() {
 
  StatusCode sc = GaudiAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  // create the map of OT vectors, m_numberOfGols entries
  for (int k = 1; k <= m_numberOfGols; k++) {
    vmcOTime* currentMCTime = new vmcOTime();
    mGol::value_type numGol(k, currentMCTime);
    m_goldatacontainer.insert( m_goldatacontainer.end(), numGol );
  }

  // create the map of OTMcTime vectors, m_numberOfBanks entries
  for (int i = 1; i <= m_numberOfBanks; i++) {
    vmcOTime* currentMCOTTime = new vmcOTime();
    mBank::value_type numBank(i, currentMCOTTime);
    m_dataContainer.insert( m_dataContainer.end(), numBank );
  }

  return StatusCode::SUCCESS;
}
//=============================================================================
// Main execution
//=============================================================================
StatusCode OTFillRawEvent::execute() 
{
  // A new RawEvent
  RawEvent* rawEvent = get<LHCb::RawEvent>( LHCb::RawEventLocation::Default );

  // Sorting MCTimes into Banks
  this->sortMcTimesIntoBanks();

  // Loop the map bank structure
  for (mBank::iterator iBank = m_dataContainer.begin();
       iBank != m_dataContainer.end();
       iBank++) {

    dataBank aBank;
    vmcOTime* amcTime = (*iBank).second;

    if (amcTime->size() == 0) {
      // Empty Bank -- Still sending all the Headers
      this->convertToRAWEmptyBank(&aBank);
    } else {
      // feed vMCOTime vectors to converter routine
      this->convertToRAWDataBank(amcTime, &aBank);
    }

    /* 
     *  Generate bank ID and push bank into raw buffer
     *  Helder Lopez header (see note 2003-152)
     *  The bank ID corresponds to the source ID
     */

    int bankID = (*iBank).first;
    dataBank& bBank = aBank;
    rawEvent->addBank(bankID, RawBank::OT, 2, bBank);  
    aBank.erase( aBank.begin(),aBank.end() );
  }

  // Finally, clear the data container and the vectors it contains 
  for (mBank::iterator iBank = m_dataContainer.begin();
       iBank != m_dataContainer.end();
       iBank++) {
    vmcOTime* amcTime = (*iBank).second;
    amcTime->erase( amcTime->begin(), amcTime->end() );
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode OTFillRawEvent::finalize() 
{      
  // clear all containers used in the process and delete all new objects:
  for (mBank::iterator iBank = m_dataContainer.begin();
       iBank != m_dataContainer.end();
       iBank++) {
    delete (*iBank).second;
  }
  m_dataContainer.erase( m_dataContainer.begin(), m_dataContainer.end() );

  for (mBank::iterator iBank = m_goldatacontainer.begin();
       iBank != m_goldatacontainer.end();
       iBank++) {
    delete (*iBank).second;
  }
  m_goldatacontainer.erase(m_goldatacontainer.begin(),m_goldatacontainer.end());

  return GaudiAlgorithm::finalize();  // must be called after all other actions 
}
//-----------------------------------------------------------------------------
StatusCode OTFillRawEvent::sortMcTimesIntoBanks()
{
  // Retrieve MCOTTime and sort them into banks
  MCOTTimes* mcTime = get<MCOTTimes>( MCOTTimeLocation::Default ); 

  for (vmcOTime::iterator iTime  = mcTime->begin(); 
       iTime != mcTime->end(); iTime++ ) {

    int nBankID = chID2int( (*iTime)->channel() );
    mBank::iterator iBank = m_dataContainer.find(nBankID);
    
    vmcOTime* currentMCOTTime = (*iBank).second;
    currentMCOTTime->push_back( (*iTime) );
  }
  return StatusCode::SUCCESS;
}
//-------------------------------------------------------------------------
StatusCode OTFillRawEvent::sortMcTimesIntoGol( vmcOTime* BankmcOTime )
{
  // There are 18 Gol per Bank, each one containing the data of a short module 
  for (vmcOTime::iterator iTime = BankmcOTime->begin();
       iTime != BankmcOTime->end();
       iTime++) {

    long nQuarter = ((*iTime)->channel()).quarter();
    long nModule = ((*iTime)->channel()).module();

    int GOL = 0;
    //There are 2 quarters in each Gol (It is necessary to specify)
    if ((nQuarter == 1) || (nQuarter == 3)) {GOL = nModule + 9;} 
    else if ((nQuarter == 0 ) || (nQuarter == 2)){GOL = nModule;}
    int nGolID = GOL;
    mGol::iterator iGol = m_goldatacontainer.find(nGolID);
    
    vmcOTime* currentMCTime = (*iGol).second;
    currentMCTime->push_back( (*iTime) );
  }
  
  return StatusCode::SUCCESS;
}  
//-----------------------------------------------------------------
StatusCode OTFillRawEvent::convertToRAWEmptyBank(dataBank* aBank)
{
  // Creating the Tell1 Headers - 3 words of 32 bits - I do not fill them...
  for (int i = 0; i < 1; i++) {
    unsigned int tell1Header = 0;
    aBank->push_back(tell1Header);
  }
  for (int j = 0; j < 19; j++) {
    unsigned int golHeader = 0;
    aBank->push_back(golHeader);
  }
  
  return StatusCode::SUCCESS;
}
//-----------------------------------------------------------------
StatusCode OTFillRawEvent::convertToRAWDataBank(vmcOTime* vToConvert, 
                                                 dataBank* aBank)
{
  // Creating the Tell1 Headers - 3 words of 32 bits - I do not fill them...
  for (int i = 0; i < 1; i++) {
    unsigned int tell1Header = 0;
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
  this->sortMcTimesIntoGol(vToConvert);
  vmcOTime::iterator IBank = vToConvert->begin();
  nQuarto = ((*IBank)->channel()).quarter();
  //Per each Gol we have 2 quarters data  - We need a useful Buffer to specify.
  if ((nQuarto == 0) || (nQuarto == 1)) { quar = 1;}
  else if ((nQuarto == 2) || (nQuarto == 3)) { quar = 2;}

  // loop the map GOL structure, retrieve the vmcOTime vectors 
  for (mGol::iterator iGol = m_goldatacontainer.begin();
       iGol != m_goldatacontainer.end();
       iGol++) {

    //The New Gol McTime Vector : 
    vmcOTime* aGolMCTime = (*iGol).second;  

    //  Valid Header
    int NoValidHeader = 0;

    //Defining iterator
    for (vmcOTime::iterator iTime = aGolMCTime->begin();
	 iTime != aGolMCTime->end();
	 iTime++) {
    
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
    if (((aGolMCTime->size() / 2) * 2) != aGolMCTime->size()) { 
      size = (aGolMCTime->size() + 1) /2;
    }
    
    // If the Size is 0, we need the correct values for module and quarter Nr.
    if ( aGolMCTime->size() == 0) {
      int IGOL = (*iGol).first;
      if(IGOL < 10 ){
        nModule = IGOL;
        //Using the Quarter Buffers introduced above to get which quarter is it
        if(quar == 1 ){nQuarter = 0;} 
        else if(quar == 2){nQuarter = 2;}
      } else if(IGOL > 9) {
        nModule = IGOL - 9;
        if(quar == 1 ){nQuarter = 1;}
        else if(quar == 2){nQuarter = 3;}
      }
      size = 0;
    } 
    if (((nStation == 0) && (nQuarter== 0) && (nLayer == 0)) || (size == 0)) {
      NoValidHeader = 1;
      debug() << " NO HIT IN MODULE " << nModule << " with Size " << size 
              <<endmsg;
    }

    if (NoValidHeader != 1) {
      // DEBUG
      debug() << " We Get " 
              << format(" Station %d, Layer %d, Quarter %d, Module %d, Size %d",
                        nStation, nLayer, nQuarter, nModule, size ) 
              << endmsg;      
      
      // GolHeader
      GolHeader golHeader(0, nStation, nLayer, nQuarter, nModule, 0 , size);
      unsigned int gol = golHeader.returnInt(golHeader);
      aBank->push_back(gol);
    }
    
    if (NoValidHeader != 1) {
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
      
      while ( iTimeCurrent != pCurrent->end() ) { //While loop over the MCOTTime
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
        
        if (iTimeNext != iHitGolEnd) {
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
          
          if ((nextOtisID == 0) || (nextOtisID == 1)) {
            nextStrawID = ( nextStrawID - 1 ) % 32;
          } else if ((nextOtisID == 2) || (nextOtisID == 3)) {
            nextStrawID = 31 - ((nextStrawID - 1) % 32);
	  }
        }
        // Straw Number Conversion
        if ((firstOtisID == 0) || (firstOtisID == 1)) {
          firstStrawID = ( firstStrawID - 1 ) % 32;
        } else if ((firstOtisID == 2) || (firstOtisID == 3)) {
          firstStrawID = 31 - ((firstStrawID - 1) % 32);
        }
        debug() << " We Get " 
                << format("firstOtisID %d, firstStrawID %d, firstTime %d," 
                          "nextOtisID %d, nextStrawID %d, nextTime %d",
                          firstOtisID, firstStrawID, firstTdcTime,
                          nextOtisID, nextStrawID, nextTdcTime) 
                << endmsg;
        
        DataWord dataWord (1, firstOtisID, firstStrawID, firstTdcTime, 
                           0, nextOtisID, nextStrawID, nextTdcTime);
        unsigned int data = dataWord.returnInt(dataWord);
        aBank->push_back(data);
        
        if (iTimeNext != iHitGolEnd) iTimeCurrent++;
        
      } //while loop over MCOTTimes
    
      aGolMCTime->erase( aGolMCTime->begin(),aGolMCTime->end() );

    }// Not Valid Header
  } // GOL Loop

  return StatusCode::SUCCESS;
}
//=============================================================================
// Member functions
//=============================================================================

int OTFillRawEvent::chID2Otis(OTChannelID otChannel)

{    
  int OtisID = 0;
  unsigned int nStraw = otChannel.straw();

  //We get the OtisID from the Straw Number
  if (( 1 <= nStraw ) && ( nStraw <= 32)) { OtisID = 0;}
  else if (( 33 <= nStraw ) && ( nStraw <= 64)) {OtisID = 1;}
  else if (( 65 <= nStraw ) && ( nStraw <= 96)) {OtisID = 3;}
  else if (( 97 <= nStraw ) && ( nStraw <= 128)) {OtisID = 2;}

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

int OTFillRawEvent::chID2int(OTChannelID otChannel) {    
  int nArray = 0;
  int tempnArray = 0;  
  int tempn2Array = 0;
  int tempn3Array = 0;
  unsigned long nStation = otChannel.station();
  unsigned long nLayer = otChannel.layer();  
  unsigned long nQuarter = otChannel.quarter();
   
  // Loop to get the bank number from the OTChannel ID Information
  if (nStation == 1) {tempn3Array = 0;}
  else if (nStation == 2) {tempn3Array = 8;}
  else if (nStation == 3) {tempn3Array = 16;}
  if (nLayer == 0 ) { tempn2Array = 0;}
  else if (nLayer == 1 ) { tempn2Array = 2;}
  else if (nLayer == 2 ) { tempn2Array = 4;}
  else if (nLayer == 3 ) { tempn2Array = 6;}
  if ((nQuarter == 0) || (nQuarter == 1)) { tempnArray = 1; }
  else if ((nQuarter == 2) || (nQuarter == 3)) { tempnArray = 2; }
  nArray = tempn3Array + tempn2Array + tempnArray;
  return (nArray);
}

