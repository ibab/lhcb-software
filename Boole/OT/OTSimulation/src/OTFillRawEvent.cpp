// Include files

// From Gaudi
#include "GaudiKernel/AlgFactory.h"

// EVENT/DAQ/OTDAQ
#include "Event/RawEvent.h"
#include "Event/RawBank.h"
#include "Event/GolHeader.h"
#include "Event/DataWord.h"
#include "Event/OTBankVersion.h"

/// GSL
#include "gsl/gsl_math.h"

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
  declareProperty( "NumberOfBanks", m_numberOfBanks = 48 );
  declareProperty( "NumberOfGols",  m_numberOfGols  = 9 );
  declareProperty( "MCOTTimeLocation", m_MCOTTimeLoc = MCOTTimeLocation::Default );
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
  for (int i = 1; i <= m_numberOfGols; ++i) {
    VecMCOTTimes* currentMCOTTimes = new VecMCOTTimes();
    OTGol::value_type numGol(i, currentMCOTTimes);
    m_goldatacontainer.insert( m_goldatacontainer.end(), numGol );
  }

  // create the map of OTMcTime vectors, m_numberOfBanks entries
  for (int i = 1; i <= m_numberOfBanks; ++i) {
    VecMCOTTimes* currentMCOTTimes = new VecMCOTTimes();
    OTBank::value_type numBank(i, currentMCOTTimes);
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
  sortMcTimesIntoBanks();
  
  typedef OTBank::iterator Iter;
  // Loop the map bank structure
  for (Iter iBank = m_dataContainer.begin(); iBank != m_dataContainer.end(); ++iBank) {
    //(*ibank).first gives you the BankID
    debug() << "Bank Nr " <<  iBank->first << endmsg; 
    
    VecMCOTTimes* times = iBank->second;
        
    DataBank aBank;
    // Empty Bank -- Still sending all the Headers :feed vMCOTime vectors to converter routine
    (times->empty())?convertToRAWEmptyBank(&aBank):convertToRAWDataBank(times, &aBank);
    
    /* 
     *  Generate bank ID and push bank into raw buffer
     *  Helder Lopez header (see note 2003-152)
     *  The bank ID corresponds to the source ID
     */

    DataBank& bBank = aBank;
    rawEvent->addBank(iBank->first, RawBank::OT, 2, bBank);
    aBank.clear();
  }
  
  // Finally, clear the data container and the vectors it contains 
  for (Iter iBank = m_dataContainer.begin(); iBank != m_dataContainer.end(); ++iBank) {
    VecMCOTTimes* times = iBank->second;
    times->clear();
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode OTFillRawEvent::finalize() 
{      
  // clear all containers used in the process and delete all new objects:
  typedef OTBank::iterator Iter;
  for (Iter iBank = m_dataContainer.begin(); iBank != m_dataContainer.end(); ++iBank) {
    delete iBank->second;
  }
  m_dataContainer.clear();
  
  for (Iter iBank = m_goldatacontainer.begin(); iBank != m_goldatacontainer.end(); ++iBank) {
    delete iBank->second;
  }
  m_goldatacontainer.clear();
  
  return GaudiAlgorithm::finalize();  // must be called after all other actions 
}

//-----------------------------------------------------------------------------
void OTFillRawEvent::sortMcTimesIntoBanks()
{
  // Retrieve MCOTTime and sort them into banks
  MCOTTimes* times = get<MCOTTimes>( MCOTTimeLocation::Default ); 
  
  if (times) {
    VecMCOTTimes::iterator iTime  = times->begin();
    for ( ; iTime != times->end(); ++iTime) {
      OTBank::iterator iBank = m_dataContainer.find(chID2int((*iTime)->channel()));
      if (iBank != m_dataContainer.end()) iBank->second->push_back((*iTime));
    }
  }
}

//-------------------------------------------------------------------------
void OTFillRawEvent::sortMcTimesIntoGol(VecMCOTTimes* mcOTTimes)
{
  VecMCOTTimes::iterator iTime = mcOTTimes->begin();
  for ( ; iTime != mcOTTimes->end(); ++iTime) {
    OTGol::iterator iGol = m_goldatacontainer.find(((*iTime)->channel()).module());
    if (iGol != m_goldatacontainer.end()) iGol->second->push_back((*iTime));
  }
}  

//-----------------------------------------------------------------
void OTFillRawEvent::convertToRAWEmptyBank(DataBank* aBank)
{
  // Creating the Tell1 Headers - 1 word of 32 bits - I do not fill them...
  // 1 Tell header and 9 Gol headers
  aBank->insert(aBank->end(), 1+9, 0u);
}

//-----------------------------------------------------------------
void OTFillRawEvent::convertToRAWDataBank(VecMCOTTimes* mcOTTimes, DataBank* aBank)
{
  // Creating the Tell1 Headers - 1 word of 32 bits - I do not fill them...
  aBank->push_back(0u);
  
  //Useful definitions 
  unsigned int station = 0u; 
  unsigned int layer = 0u; 
  unsigned int quarter = 0u;
  unsigned int module = 0u; 
  unsigned int size = 0u;
      
  // For each Bank Data we sort the MCOTTimes into Gol
  sortMcTimesIntoGol(mcOTTimes);

  // loop the map GOL structure, retrieve the vmcOTime vectors 
  OTGol::iterator iGol = m_goldatacontainer.begin();
  for ( ; iGol != m_goldatacontainer.end(); ++iGol) {
    
    VecMCOTTimes* times = iGol->second;
    // Valid gol
    if (!(times->empty())) {
      // We need to get the location
      // One gol header per module
      // Get location of module; stored in header
      MCOTTime* firstMCTime = times->front();
      station = (firstMCTime->channel()).station();
      layer = (firstMCTime->channel()).layer();
      quarter = (firstMCTime->channel()).quarter();
      module = (firstMCTime->channel()).module();

      /*
       * Creating the Gol Headers, given Station, Layer, Quarter, Module, Size
       * Numbers; Using Magic Shift operators
       */
      
      // Set gol size
      // If aGolMCTime->size is odd add 1, for padding!
      size = (GSL_IS_EVEN(times->size())?(times->size()/2):((times->size()+1)/2));
			 
      // format statement is always evaluated so check msg level
      if (msgLevel(MSG::DEBUG)) {
        debug() << " We Get " << format(" Station %d, Layer %d, Quarter %d, Module %d, Size %d",
                                        station, layer, quarter, module, size ) << endmsg;
      }
      
      // GolHeader
      GolHeader golHeader(0, station, layer, quarter, module, 0 , size);
      // insert golheader
      aBank->push_back(golHeader.returnInt(golHeader));
      
      // The Hits -- Some useful definitions
      VecMCOTTimes* pCurrent = times;
      VecMCOTTimes::iterator iHitGolBegin = pCurrent->begin();
      VecMCOTTimes::iterator iHitGolEnd = pCurrent->end();
      VecMCOTTimes::iterator iTimeCurrent = iHitGolBegin;
      
      /* 
       * Now the hits
       * convert the channel + time inf. in RAW format: 8bit channel 
       * 8 bit time 
       * 8bit channel: 1 bit letf + 2 bit OTIS ID + 5 bit channel ID 
       * 8 bit time: 2 bit BX + 6 bit time itself
       */
      
      while (iTimeCurrent != pCurrent->end()) { //While loop over the MCOTTime
        MCOTTime* firstTime = (*iTimeCurrent);
        //First Time - we get Otis and Straw Number
        unsigned int firstOtisID = chID2Otis(firstTime->channel());
        unsigned int firstStrawID = ((firstTime->channel()).straw());
        int firstTdcTime = ((firstTime->channel()).tdcTime());
        
        // Straw Number Conversion
        // if first Otis id == 0 or 1
        firstStrawID = ( firstStrawID - 1 ) % 32;
        // else
        if ((firstOtisID == 2) || (firstOtisID == 3)) firstStrawID = 31 - firstStrawID;
	
        //Next 
        VecMCOTTimes::iterator iTimeNext = ++iTimeCurrent;
        
        //First we get Otis and Straw Number
        unsigned int nextOtisID = 0u;
        unsigned int nextStrawID = 0u;
        int nextTdcTime = 0;
        	
        if (iTimeNext != iHitGolEnd) {
          MCOTTime* nextTime = (*iTimeNext);
          nextOtisID = chID2Otis(nextTime->channel());
          nextStrawID = ((nextTime->channel()).straw());
          nextTdcTime = ((nextTime->channel()).tdcTime());
          
          /* Straw number conversion.
           * Converting straw number from 1 to 128 in a number from 0 to 31
           * The strange conversion depends on the fact that here we have a 
           * straw numbering  different from the one in OTChannelID, and more 
           * near to the eletronic scheme. 
           */
	  
          // If otis id == 0 or 1
          nextStrawID = ( nextStrawID - 1 ) % 32;
          // else 
          if ((nextOtisID == 2) || (nextOtisID == 3)) nextStrawID = 31 - nextStrawID;
        }// nextotisid

        // format statement is always evaluated so check msg level
        if (msgLevel(MSG::DEBUG)) {
          debug() << " We Get " << format("firstOtisID %d, firstStrawID %d, firstTime %d," 
                                          " nextOtisID %d, nextStrawID %d, nextTime %d",
                                          firstOtisID, firstStrawID, firstTdcTime,
                                          nextOtisID, nextStrawID, nextTdcTime) << endmsg;
        }
        
        DataWord dataWord(1, firstOtisID, firstStrawID, firstTdcTime, 0, nextOtisID, nextStrawID, nextTdcTime);
        
        /// insert dataword
        aBank->push_back(dataWord.returnInt(dataWord));
        
        if (iTimeNext != iHitGolEnd) ++iTimeCurrent;
        
      } //while loop over MCOTTimes
      
      times->clear();
			   
    } else {
      // zero hits in module
      if (msgLevel(MSG::DEBUG)) {
        module = iGol->first;
        debug() << " NO HIT IN MODULE " << module << endmsg;
      }
    }
  } // GOL Loop
}

int OTFillRawEvent::chID2Otis(const OTChannelID& aChannel)
{    
  unsigned int nStraw = aChannel.straw();
  
  //We get the OtisID from the Straw Number
  // left to right
  // 1 - 32 => otisid = 0 or 33 - 64 => otisid = 1
  int OtisID = nStraw/33;
  // right to left
  if (( 97 <= nStraw ) && ( nStraw <= 128)) {OtisID = 2;}
  if (( 65 <= nStraw ) && ( nStraw <= 96)) {OtisID = 3;}
  
   /* 
   * It seems wrong, but the point is that here we want a different straw 
   * numbering scheme from OTChannelID. In OTchannelID the Straw number 
   * goes from 1 to 64 for OTIS 0 & 1 and then from 65 to 128 for OTIS 2 and 3,
   * but for OTIS 2 and 3 we want it the other way round.
   * So first there is OTIS 3 (for straws from 65 to 96) and then OTIS 2.
   */

  return OtisID;
}

int OTFillRawEvent::chID2int(const OTChannelID& aChannel) {    
  /// Tell1 numbering starts from 1 to 48
  return ((aChannel.station() - 1)*16 + aChannel.layer()*4 + (aChannel.quarter()+1));
}

