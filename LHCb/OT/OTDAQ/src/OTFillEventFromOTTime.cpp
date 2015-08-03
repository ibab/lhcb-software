// Include files

// From event model
#include "Event/RawEvent.h"
#include "Event/RawBank.h"

// Kernel
#include "Kernel/OTChannelID.h"

// Event
#include "Event/RawEvent.h"
#include "Event/OTTime.h"

// from Detector
#include "OTDet/DeOTDetector.h"

/// GSL
#include "gsl/gsl_math.h"

// local
#include "OTFillEventFromOTTime.h"
#include "Event/OTBankVersion.h"

//-----------------------------------------------------------------------------
// Implementation file for class : OTFillEventFromOTTime
//
// 2004-01-09 : Jacopo Nardulli & Bart Hommels
//-----------------------------------------------------------------------------

using namespace LHCb;

// Declaration of the algorithm Factory
DECLARE_ALGORITHM_FACTORY( OTFillEventFromOTTime )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
OTFillEventFromOTTime::OTFillEventFromOTTime( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_time(NULL)
  , dataContainer(NULL)
  , goldatacontainer(NULL)
  , finalBuf(NULL)
  //Fixed Number of Banks
  , numberOfBanks(48)
{
  // Constructor

}

//=============================================================================
// Destructor
//=============================================================================
OTFillEventFromOTTime::~OTFillEventFromOTTime(){}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode OTFillEventFromOTTime::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  // create a container for the vectors of OTimes
  dataContainer = new mBank(); // bank type
  goldatacontainer = new mGol();

  // init pointer to global container
  finalBuf = new dataBuffer();
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode OTFillEventFromOTTime::execute() 
{
  warning() << "Please do not use. Must be adapted to new decoding first." << endmsg ;
  // Retrieve the RawEvent
  LHCb::RawEvent* rawEvent = get<LHCb::RawEvent>( LHCb::RawEventLocation::Default );
 
  // Retrieve OTTime
  m_time = get<OTTimes>( OTTimeLocation::Default );

  // Sorting Times into Banks
  sortTimesIntoBanks();

  // Loop the map bank structure
  for (mBank::iterator iBank = dataContainer->begin();
       iBank != dataContainer->end(); ++iBank) {
    vOTime* aTime = iBank->second;

    dataBank* aBank = new dataBank();
    // Empty Bank -- Still sending all the Headers
    (aTime->empty())?convertToRAWEmptyBank(aBank):convertToRAWDataBank(aTime, aBank);
       
    // put converted vOTime into final buffer container
    finalBuf->push_back(aBank);

    /* 
     *  Generate bank ID and push bank into raw buffer
     *  Helder Lopez header (see note 2003-152)
     *  The bank ID corresponds to the source ID
     */

    int bankID = iBank->first;
    dataBank& bBank = (*aBank);
    rawEvent->addBank(bankID , LHCb::RawBank::OT, 2, bBank);
    aBank->clear();
  }
  
  // clear all containers used in the process:
  dataContainer->clear();
  goldatacontainer->clear();
  finalBuf->clear();
  
  put( rawEvent, "DAQ/RawEvent"  );

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode OTFillEventFromOTTime::finalize() 
{      
  delete (dataContainer);
  delete (goldatacontainer);
  delete (finalBuf);
  return GaudiAlgorithm::finalize();  // must be called after all other actions 
}

//-----------------------------------------------------------------------------
StatusCode OTFillEventFromOTTime::sortTimesIntoBanks()
{
  // create the map of OTTime vectors, numberOfBanks entries
  for ( int i = 1; i <= numberOfBanks; i++) {
    vOTime* currentOTTime = new vOTime();
    mBank::value_type numBank(i, currentOTTime);
    dataContainer->insert(dataContainer->end(), numBank);
  }
  // Now the bank-containers are created. Sort the OTTimes into these banks
  for (vOTime::iterator iTime = m_time->begin();
       iTime != m_time->end(); ++iTime) {
    mBank::iterator iBank = dataContainer->find(chID2int((*iTime)->channel()));
    if (iBank != dataContainer->end()) iBank->second->push_back((*iTime));
  }
 
  return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------
StatusCode OTFillEventFromOTTime::sortTimesIntoGol(vOTime* BankOTime, 
                                                    dataBank* /* aBank */)

{
  // There are 9 Gols per Bank, each one containing the data of a short module 
  for (vOTime::iterator iTime = BankOTime->begin();
       iTime != BankOTime->end(); ++iTime) {
   
    mGol::iterator iGol = goldatacontainer->find(((*iTime)->channel()).module());
    iGol->second->push_back((*iTime));
  }
  return StatusCode::SUCCESS;
}  

//-----------------------------------------------------------------
StatusCode OTFillEventFromOTTime::convertToRAWEmptyBank(dataBank* aBank)
{
  // Creating the Tell1 Headers - 1 word of 32 bits - I do not fill them...
  // 1 Tell header and 9 Gol headers
  aBank->insert(aBank->end(), 1+9, 0u);
   
  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------
StatusCode OTFillEventFromOTTime::convertToRAWDataBank(vOTime* vToConvert, 
                                                        dataBank* aBank)
{
  // Creating the Tell1 Headers - 3 words of 32 bits - I do not fill them...
  aBank->push_back(0);

  //Useful definitions 
  unsigned int station = 0u; 
  unsigned int layer = 0u; 
  unsigned int quarter = 0u;
  unsigned int module = 0u; 
  unsigned int size = 0u;

  // For each Bank Data we sort the OTTimes into Gol
  sortTimesIntoGol(vToConvert, aBank);
  
  // loop the map GOL structure, retrieve the vOTime vectors 
  for (mGol::iterator iGol = goldatacontainer->begin();
       iGol != goldatacontainer->end(); ++iGol) {

    //The New Gol Time Vector : 
    vOTime* aGolTime = iGol->second;  
    if (!(aGolTime->empty())) {
      // One gol header per module
      // Get location of module; stored in header
      OTTime* firstTime = aGolTime->front();
      station = (firstTime->channel()).station();
      layer = (firstTime->channel()).layer();
      quarter = (firstTime->channel()).quarter();
      module = (firstTime->channel()).module();

      /*
       * Creating the Gol Headers, given Station, Layer, Quarter, Module, Size
       * Numbers; Using Magic Shift operators
       */

      //The Size
      //If aGolMCTime->size is odd add 1, for padding!
      size = (GSL_IS_EVEN(aGolTime->size())?int(aGolTime->size()*0.5)
	      :int((aGolTime->size()+1)*0.5));
    
      // GolHeader
      GolHeader golHeader(0, station, layer, quarter, module, 0 , size);
      /// insert golheader
      aBank->push_back(golHeader.returnInt(golHeader));
    
      // The Hits -- Some useful definitions
      vOTime* pCurrent = aGolTime;
      vOTime::iterator iHitGolBegin = pCurrent->begin();
      vOTime::iterator iHitGolEnd = pCurrent->end();
      vOTime::iterator iTimeCurrent = iHitGolBegin;
      
      /* 
       * Now the hits
       * convert the channel + time inf. in RAW format: 8bit channel 
       * 8 bit time 
       * 8bit channel: 1 bit letf + 2 bit OTIS ID + 5 bit channel ID 
       * 8 bit time: 2 bit BX + 6 bit time itself
       */
      
      while ( iTimeCurrent != pCurrent->end() ) { //While loop over the OTTime
	OTTime* firstTime = (*iTimeCurrent);
	//First Time - we get Otis and Straw Number
	unsigned int firstOtisID = chID2Otis(  firstTime->channel() );
	unsigned int firstStrawID = ( (firstTime->channel()).straw());
	int firstTdcTime = ( (firstTime->channel()).tdcTime());
	
	// Straw Number Conversion
	firstStrawID = ( firstStrawID - 1 ) % 32;
	if ((firstOtisID == 2) || (firstOtisID == 3)) firstStrawID = 31 - firstStrawID;
	
	//Next 
	vOTime::iterator iTimeNext = ++iTimeCurrent;
	
	//First we get Otis and Straw Number
	unsigned int nextOtisID = 0;
	unsigned int nextStrawID = 0;
	int nextTdcTime = 0;
	
	if (iTimeNext != iHitGolEnd) {
	  OTTime* nextTime = (*iTimeNext);
	  nextOtisID = chID2Otis(  nextTime->channel() );
	  nextStrawID = ((nextTime->channel()).straw());
	  nextTdcTime = ((nextTime->channel()).tdcTime());
	  
	  /* Straw number conversion.
	   * Converting straw number from 1 to 128 in a number from 0 to 31
	   * The strange conversion depends on the fact that here we have a 
	   * straw numbering  different from the one in OTChannelID, and more 
	   * near to the eletronic scheme. 
	   */
	  
	  nextStrawID = ( nextStrawID - 1 ) % 32;
	  if((nextOtisID == 2) || (nextOtisID == 3)) nextStrawID = 31 - nextStrawID;
	}
           
	DataWord dataWord(1, firstOtisID, firstStrawID, firstTdcTime, 
			  0, nextOtisID, nextStrawID, nextTdcTime);
	/// insert date
	aBank->push_back(dataWord.returnInt(dataWord));
	
	if (iTimeNext != iHitGolEnd) iTimeCurrent++;     
      } //while loop over OTTimes
    
      // Erase Gol Vector of Time
      aGolTime->clear();
    
    } else {
      // Module contains zero hits
      module = iGol->first;
    }
  } // GOL Loop

  return StatusCode::SUCCESS;
}

int OTFillEventFromOTTime::chID2Otis(OTChannelID otChannel)
{
  unsigned int nStraw = otChannel.straw();
  int OtisID = nStraw/33;
  //We get the OtisID from the Straw Number
  if (( 97 <= nStraw ) && ( nStraw <= 128)) {OtisID = 2;}
  if (( 65 <= nStraw ) && ( nStraw <= 96)) {OtisID = 3;}
  
  /* 
   * It seems wrong, but the point is that here we want a different straw 
   * numbering scheme from OTChannelID. In OTchannelID the Straw number 
   * goes from 1 to 64 for OTIS 0 & 1 and then from 65 to 128 for OTIS 2 and 3,
   * but for OTIS 2 and 3 we want it the other way round.
   * So first there is OTIS 3 (for straws from 65 to 96) and then OTIS 2.
   */

  return(OtisID);
}

int OTFillEventFromOTTime::chID2int(OTChannelID otChannel)
{
  /// Tell1 numbering starts from 1 to 48
  return ((otChannel.station() - 1)*16 + otChannel.layer()*4 + (otChannel.quarter()+1));
}

