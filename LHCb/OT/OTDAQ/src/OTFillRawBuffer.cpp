// $Id: OTFillRawBuffer.cpp,v 1.4 2004-03-26 18:37:50 cattanem Exp $
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

  msg << MSG::DEBUG
      << "Loading OTR geometry from XML... " << endreq;
  SmartDataPtr<DeOTDetector> Otracker( detSvc(), m_otTrackerPath);
  if( !Otracker){
    msg << MSG::ERROR
        << "Unable to retrieve Outer Tracker detector element from XML"
        << endreq;
    return StatusCode::FAILURE;
  }
  m_otTracker = Otracker;


  //Fixed Numbers of Banks and of Gols
  numberOfBanks = 24;
  numberOfGols = 18;

  // create a container for the vectors of OTdigits
  dataContainer = new mBank(); // bank type
  goldatacontainer = new mGol();
  // later: fix size to 32 entries for speed gain

  // init pointer to global container
  finalBuf = new dataBuffer();
  
   return StatusCode::SUCCESS;
}


//=============================================================================
// Main execution
//=============================================================================
StatusCode OTFillRawBuffer::execute() 
{
  MsgStream  msg( msgSvc(), name() );
  msg << MSG::DEBUG << "==> Execute" << endreq;

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

  // Sorting digits into Banks
  this->sortDigitsIntoBanks();

  // Loop the map bank structure
  for(mBank::iterator iBank = dataContainer->begin();
      iBank != dataContainer->end();
      iBank++){
    dataBank* aBank = new dataBank();
    vDigi* aDigi = (*iBank).second;

    if(aDigi->size() == 0){
      break;
    }


    // feed vdigi vectors to converter routine
    this->convertToRAWDataBank(aDigi, aBank);
   
    // put converted vDigi into final buffer container
    finalBuf->push_back(aBank);

    /* 
     *  Generate bank ID and push bank into raw buffer
     *  Helder Lopez header (see note 2003-152)
     *  The bank ID corresponds to the source ID
     */

    int bankID = (*iBank).first;
    dataBank& bBank = (*aBank);
    m_rawBuffer->addBank( bankID , RawBuffer::OT, bBank );  
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
  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Finalize" << endreq;
  delete(dataContainer);
  delete(goldatacontainer);
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
//-------------------------------------------------------------------------
StatusCode OTFillRawBuffer::sortDigitsIntoGol(vDigi* BankDigi, 
                                              dataBank* /* aBank */)

{
  // create the map of OTDigi vectors, numberOfGols entries
  for( int k = 1; k <= numberOfGols; k++){
    vDigi* currentGolDigi = new vDigi(); 
    mGol::value_type numGol(k, currentGolDigi);
    goldatacontainer->insert( goldatacontainer->end(), numGol );
  }
  // Now the gol-containers are created. Sort the otdigits into these Gols
  mGol::iterator iGol = goldatacontainer->begin();
  vDigi* currentGolDigi;
  // There are 18 Gol per Bank, each one containing the data of a short module 
  for(vDigi::iterator iDigit = BankDigi->begin();
      iDigit != BankDigi->end();
      iDigit++){

    long nQuarter = ((*iDigit)->channel()).quarter();
    long nModule = ((*iDigit)->channel()).module();
    int GOL = 0;
    //There are 2 quarters in each Gol (It is necessary to specify)
    if((nQuarter == 1) || (nQuarter == 3)){GOL = nModule + 9;} 
    else if((nQuarter == 0 ) || (nQuarter == 2)){GOL = nModule;}
    int nGolID = GOL;
    iGol = goldatacontainer->find(nGolID);
    currentGolDigi = (*iGol).second;
    currentGolDigi->push_back( (*iDigit) );
  }
  return StatusCode::SUCCESS;
}  

//-----------------------------------------------------------------
StatusCode OTFillRawBuffer::convertToRAWDataBank(vDigi* vToConvert, 
                                                 dataBank* aBank)
{
  //Gol Vector of Digi and other useful definitions 
  vDigi* vGol = new vDigi();
  long nStation = 0; 
  long nLayer = 0; 
  long nQuarter = 0;
  long nQuarto = 0;
  long nModule = 0; 
  long nStraw = 0;
  long channel = 0;
  int quar = 0;

  // For each Bank Data we sort the digits into Gol
  this->sortDigitsIntoGol(vToConvert, aBank);
  vDigi::iterator IBank = vToConvert->begin();
  // Get Station, Layer, Quarter Numbers
  nStation = ((*IBank)->channel()).station();
  nLayer = ((*IBank)->channel()).layer();  
  nQuarto = ((*IBank)->channel()).quarter();
  //Per each Gol we have 2 quarters data. 
  //We need a useful Buffer to specify.
  if((nQuarto == 0) || (nQuarto == 1)){ quar = 1;}
  else if((nQuarto == 2) || (nQuarto == 3)){ quar = 2;}

  // loop the map GOL structure, retrieve the vDigi vectors 
  for(mGol::iterator iGol = goldatacontainer->begin();
      iGol != goldatacontainer->end();
      iGol++){

    //The New Gol Digi Vector : 
    vDigi* aGolDigi = (*iGol).second;  

    //Defining iterator
    for(vDigi::iterator iDigi = aGolDigi->begin();
        iDigi != aGolDigi->end();
        iDigi++){
    
      vGol->push_back( (*iDigi) );

      // Get Station, Layer, Quarter, Module Numbers for each OTDigit
      nStation = ((*iDigi)->channel()).station();
      nLayer = ((*iDigi)->channel()).layer();  
      nQuarter = ((*iDigi)->channel()).quarter();
      nModule = ((*iDigi)->channel()).module();
      nStraw = ((*iDigi)->channel()).straw();
      channel = ((*iDigi)->channel());
    }
     
    // GOL Headers
    // First the Size
    long size = aGolDigi->size() / 2;
    if(((aGolDigi->size() / 2) * 2) != aGolDigi->size()){ 
      size = (aGolDigi->size() + 1) /2;
    }  
    /*
     * Creating the Gol Headers, given Station, Layer, Quarter, Module
     * Numbers; Using Magic Shift operators
     */
    
    // If the Size is 0, we need the correct values for module and quarter Nr.
    if( aGolDigi->size() == 0){
      int IGOL = (*iGol).first;
      if(IGOL < 10 ){
        nModule = IGOL;
        //Using the Quarter Buffers introduced above to get wich quarter is it
        if(quar == 1 ){nQuarter = 0;} 
        else if(quar == 2){nQuarter = 2;}
      } else if(IGOL > 9){
        nModule = IGOL - 9;
        if(quar == 1 ){nQuarter = 1;}
        else if(quar == 2){nQuarter = 3;}
      }
      size = 0;
    } 
    raw_int golHeader = createGolHeader(nStation, nLayer, nQuarter, 
                                        nModule, size);
    
    // pushing the Gol Header
    raw_int header = golHeader;
    aBank->push_back(header);  
    //Decoding Gol Header
    rawInt2Decode(golHeader);
    
    // Generate Otis Header
    for(int j = 0; j < 4 ; j ++ ){
      //We have four OTIS Header per Gol
      raw_int otisHeader = createOtisHeader(nStation, nLayer, nQuarter, 
                                            nModule, j);
      // pushing the Otis Header
      aBank->push_back(otisHeader);
    }    
    
    // The Hits.
    // Some useful definitions
    vDigi* pCurrent = 0;    
    pCurrent = aGolDigi;
    vDigi::iterator iHitGolBegin = pCurrent->begin();
    vDigi::iterator iHitGolEnd = pCurrent->end();
    vDigi::iterator iDigiCurrent = iHitGolBegin;
    std::vector<int> vTimes;
    std::vector<int> vnextTime;

    // Now the hits. What if there's none?
    if( pCurrent->size() == 0){
      continue;
    }
    /* 
     * Now the hits
     * convert the channel + time inf. in RAW format: 8bit channel 
     * 8 bit time 
     * 8bit channel: 1 bit letf + 2 bit OTIS ID + 5 bit channel ID 
     * 8 bit time: 2 bit BX + 6 bit time itself
     */
    while( iDigiCurrent != pCurrent->end() ){ //While loop over the Digits
      OTDigit* firstDigit = (*iDigiCurrent);
      //First Digit      
      //First we get Otis and Straw Number
      long firstchannel = firstDigit->channel();
      long firstOtisID = chID2Otis( firstchannel );
      long firstStrawID = ( (firstDigit->channel()).straw());      

      //Time stuff
      vInt& vTimes = firstDigit->tdcTimes();
      int firstTimeSize = (vTimes.size());
      int firstTime = (*vTimes.begin());  

      /* 
       * Converting straw number from 1 to 128 in a number from 0 to 31
       * The strange conversion depends on the fact that here we have a 
       * straw numbering  different from the one in OTChannelID, and more 
       * near to the eletronic scheme. 
       */
      if((firstOtisID == 0) || (firstOtisID == 1)){
        firstStrawID = ( firstStrawID - 1 ) % 32;
      } else if((firstOtisID == 2) || (firstOtisID == 3)){
        firstStrawID = 31 - ((firstStrawID - 1) % 32);
      }

      //Next Digit
      vDigi::iterator iDigiNext = ++iDigiCurrent;
      //First we get Otis and Straw Number
      long nextOtisID = 0;
      long nextStrawID = 0;
      int nextTimeSize = 0;
      int nextTime = 0;

      if(iDigiNext != iHitGolEnd){
        OTDigit* nextDigit = (*iDigiNext);
        long nextchannel = nextDigit->channel();
        nextOtisID = chID2Otis( nextchannel );
        nextStrawID = ((nextDigit->channel()).straw());

        //Time stuff
        vInt& vnextTime = nextDigit->tdcTimes();
        nextTime = (*vnextTime.begin());
        nextTimeSize = (vnextTime.size());

        //Straw number conversion
        if((nextOtisID == 0) || (nextOtisID == 1)){
          nextStrawID = ( nextStrawID - 1 ) % 32;
        } else if((nextOtisID == 2) || (nextOtisID == 3)){
          nextStrawID = 31 - ((nextStrawID - 1) % 32);
        }

        //Multiple Hit Implementation --- Next Digit
        int nextTimeTwo = 0;
        int nextTimeThree = 0;
        long nextOtisIDBis = nextOtisID;
        long nextStrawIDBis = nextStrawID;
        if(nextTimeSize > 1 ){
          if(nextTimeSize > 3){nextTimeSize = 3;} // Maximum Three Hits
          if(nextTimeSize == 2){
            nextTimeTwo = vnextTime[1]; ///vTimes[1] == vTime(2)
            nextTimeThree = 0;
          } else if(nextTimeSize == 3){
            nextTimeThree = vnextTime[2];
            nextTimeTwo = vnextTime[1];
          }
          // use the magic bitshift operators!
          int usefulbit = 1;
          int usefulbit2 = 1;
          usefulbit = usefulbit << 31;
          nextOtisID = nextOtisID << 29;  // we know OTIS ID is 2 bits
          nextStrawID = nextStrawID << 24;   // Straw ID is only 5 bits
          nextTime = nextTime << 16;
          usefulbit2 = usefulbit2 << 15;
          nextOtisIDBis = nextOtisIDBis << 13;
          nextStrawIDBis = nextStrawIDBis << 8;
          nextTimeTwo = nextTimeTwo << 0;
          
          //Next Data Word: Next Digi, First Time and Second Time
          raw_int dataWordNextBis = 
            usefulbit | nextOtisID | nextStrawID | nextTime | usefulbit2 |
            nextOtisIDBis | nextStrawIDBis | nextTimeTwo;
          
          //Pushing the Gol Data Word
          aBank->push_back(dataWordNextBis);
          //Decoding the data Word
          rawInt2Decode(dataWordNextBis);
          
          if(firstTimeSize == 1){
            int usefulbitf = 1;
            int usefulbitf2 = 1;
            int multiFirstOtisID = firstOtisID;
            int multiFirstStrawID = firstStrawID;
            int multiFirstTime = firstTime;
            //Useful Buffer to understand that this is a Next Digit Multiple Hit
            int nextDigiMultipleHitBuffer = 100; 

            usefulbitf = usefulbitf << 31;
            nextDigiMultipleHitBuffer = nextDigiMultipleHitBuffer << 24;
            nextTimeThree = nextTimeThree << 16;
            usefulbitf2 = usefulbitf2 << 15;
            multiFirstOtisID = multiFirstOtisID << 13; 
            multiFirstStrawID = multiFirstStrawID << 8;
            multiFirstTime = multiFirstTime << 0;
            
            //Second Data Word: Next Digi - Third Time
            //and First Digit (in case of no first digit multiple hit)
            raw_int dataWordNextTris = 
              usefulbitf | nextDigiMultipleHitBuffer | nextTimeThree | 
              usefulbitf2 | multiFirstOtisID | multiFirstStrawID | 
              multiFirstTime;            
            
            //Pushing the Gol Data Word
            aBank->push_back(dataWordNextTris);
            //Decoding the data Word
            rawInt2Decode(dataWordNextTris);
          }         
          // And if the First Digit is also a Multiple Hit ?
          else if(firstTimeSize > 1){
            int usefulbitff = 1;
            int nothing = 0;
            int usefulbitff2 = 1;
            int nextDigiMultipleHitBuffer = 100;
            //Buffer to understand that there is a first Digi Multiple Hit 
            // together with the Next Digi Multiple Hit
            int firstMultipleHitBuff = 101;
            int nextMultiTimeThree = nextTimeThree;

            usefulbitff = usefulbitff << 31;
            nextDigiMultipleHitBuffer = nextDigiMultipleHitBuffer << 24;
            nextMultiTimeThree = nextMultiTimeThree << 16;
            usefulbitff2 = usefulbitff2 << 15;
            nothing = nothing << 7;
            firstMultipleHitBuff = firstMultipleHitBuff << 0;
            
            //Second Data Word: Next Digi - Third Time
            //and First Digit (multiple hit)
            raw_int dataWordMultiNextTris = 
              usefulbitff | nextDigiMultipleHitBuffer | nextMultiTimeThree | 
              usefulbitff2 | nothing | firstMultipleHitBuff;
            
            //Pushing the Gol Data Word
            aBank->push_back(dataWordMultiNextTris);
            //Decoding the data Word
            rawInt2Decode(dataWordMultiNextTris);
          }
        } //Multiple Hit -- Next Digit
      } //Next Digit Loop

      //Multiple Hit Implementation --- First Digit
      int firstTimeTwo = 0;
      int firstTimeThree = 0;
      long firstOtisIDBis = firstOtisID;
      long firstStrawIDBis = firstStrawID;
      if(firstTimeSize > 1 ){
        if(firstTimeSize > 3){firstTimeSize = 3;} // Maximum Three Hits
        if(firstTimeSize == 2){
          firstTimeTwo = vTimes[1]; ///vTimes[1] == vTime(2)
          firstTimeThree = 0;
        } else if(firstTimeSize == 3){
          firstTimeThree = vTimes[2];
          firstTimeTwo = vTimes[1];
        }
        // use the magic bitshift operators!
        int usefulbitF = 1;
        int usefulbitF2 = 1;
        usefulbitF = usefulbitF << 31;
        firstOtisID = firstOtisID << 29;  // we know OTIS ID is 2 bits
        firstStrawID = firstStrawID << 24;   // Straw ID is only 5 bits
        firstTime = firstTime << 16;
        usefulbitF2 = usefulbitF2 << 15;
        firstOtisIDBis = firstOtisIDBis << 13;
        firstStrawIDBis = firstStrawIDBis << 8;
        firstTimeTwo = firstTimeTwo << 0;
        
        //First Data Word: First Digi, First Time, First Digi Second Time
        raw_int dataWordBis = 
          usefulbitF | firstOtisID | firstStrawID | firstTime | usefulbitF2 |
          firstOtisIDBis | firstStrawIDBis | firstTimeTwo;
      
        //Pushing the Gol Data Word
        aBank->push_back(dataWordBis);
        //Decoding the data Word
        rawInt2Decode(dataWordBis);

  
        if(nextTimeSize < 2){  //if Next Digit is not a Multiple Hit
          int usefulbitFN = 1;
          int usefulbitFN2 = 1;
          //Useful Buffer to specify that this is a First Digit Multiple Hit
          int firstDigiMultiBuff = 1;
          usefulbitFN = usefulbitFN << 31;
          firstTimeThree = firstTimeThree << 23; 
          //This strange inversion between the Time and the buffer is useful 
          // in retrieving the program 
          firstDigiMultiBuff = firstDigiMultiBuff << 16;
          usefulbitFN2 = usefulbitFN2 << 15;
          nextOtisID = nextOtisID << 13;
          nextStrawID = nextStrawID << 8;
          nextTime = nextTime << 0;

          //Second Data Word: First Digi - Third Time and the Next Digit
          raw_int dataWordTris = 
            usefulbitFN | firstTimeThree | firstDigiMultiBuff |
            usefulbitFN2 | nextOtisID | nextStrawID | nextTime;
          
          //Pushing the Gol Data Word
          aBank->push_back(dataWordTris);
          //Decoding the data Word
          rawInt2Decode(dataWordTris);
        } else if(nextTimeSize > 1){
          int usefulbitFNN = 1;
          int usefulbitFNN2 = 1;
          int nothing = 0;
          int firstDigiMultiBuff = 1;
          int firstMultiTimeThree = firstTimeThree;
          int nextDigiMultipleHitBuff = 105;
          
          usefulbitFNN = usefulbitFNN << 31;
          firstMultiTimeThree = firstMultiTimeThree << 23;
          firstDigiMultiBuff = firstDigiMultiBuff << 16;
          usefulbitFNN2 = usefulbitFNN2 << 15;
          nothing = nothing << 7;
          nextDigiMultipleHitBuff = nextDigiMultipleHitBuff << 0;

          //Second Data Word: First Digi - Third Time and the Next Digit
          raw_int dataWordMultiFirstTirs = 
            usefulbitFNN | firstMultiTimeThree | firstDigiMultiBuff |
            usefulbitFNN2 | nothing | nextDigiMultipleHitBuff;
      
          //Pushing the Gol Data Word
          aBank->push_back(dataWordMultiFirstTirs);
        }
      } //Multiple Hit -- First Digit

      // No Multiple Hit
      // use the magic bitshift operators!
      int usefulbit = 1;
      int usefulbitbis = 1;
      usefulbit = usefulbit << 31;
      firstOtisID = firstOtisID << 29;  // we know OTIS ID is 2 bits
      firstStrawID = firstStrawID << 24;   // Straw ID is only 5 bits
      firstTime = firstTime << 16;
      usefulbitbis = usefulbitbis << 15;
      nextOtisID = nextOtisID << 13;
      nextStrawID = nextStrawID << 8;
      nextTime = nextTime << 0;
      
      raw_int dataWord = 
        usefulbit | firstOtisID | firstStrawID | firstTime | usefulbitbis |
        nextOtisID | nextStrawID | nextTime;
      
      if((firstTimeSize == 1 ) && (nextTimeSize < 2)) {  // No Multiple Hit
        aBank->push_back(dataWord);
        //Decoding the data Word
        rawInt2Decode(dataWord);
      }  
      if(iDigiNext != iHitGolEnd) iDigiCurrent++;     
    } //while loop over Digits
    
    // Erase Gol Vector of Digi
    vTimes.erase( vTimes.begin(), vTimes.end() );
    vnextTime.erase( vnextTime.begin(), vnextTime.end() );
    aGolDigi->erase( aGolDigi->begin(),aGolDigi->end() );
    vGol->erase( vGol->begin(),vGol->end() );
  
  } // GOL Loop

  return StatusCode::SUCCESS;
}
//-----------------------------------------------------------------------
StatusCode OTFillRawBuffer::rawInt2Decode(raw_int dataWord)

{
  raw_int firstBitMask = 0x80000000;   
  int firstBit = (firstBitMask & dataWord) >> 31;

  //Decoding the Gol Header
  if(!firstBit){
    int StationIDwithmask = dataWord & StationIDMask;
    int LayerIDwithmask = dataWord & LayerIDMask;
    int QuarterIDwithmask = dataWord & QuarterIDMask;
    int ModuleIDwithmask = dataWord & ModuleIDMask;
    int GolSizewithmask = dataWord & golSizeMask;
    StationIDwithmask =  StationIDwithmask >> 29;
    LayerIDwithmask =  LayerIDwithmask >> 27;
    QuarterIDwithmask =  QuarterIDwithmask >> 25; 
    ModuleIDwithmask =  ModuleIDwithmask >> 21;
    GolSizewithmask = GolSizewithmask >> 0;
  }
  //Decoding the Data word
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

//-------------------------------------------------------------------------
raw_int OTFillRawBuffer::createGolHeader(long nStation, long nLayer, 
                                         long nQuarter, long nModule, 
                                         int size)
{
  raw_int golHeader = 0;
  int usefulbit = 0;
  nStation = nStation << 29;
  nLayer = nLayer << 27;
  nQuarter = nQuarter << 25;
  nModule = nModule << 21;
  usefulbit = usefulbit << 15;
  size = size << 0;
  golHeader = golHeader | nStation | nLayer | nQuarter | nModule | 
    usefulbit | size;
  return(golHeader);
  
}
//---------------------------------------------------------------
raw_int OTFillRawBuffer::createOtisHeader(long nStation, long nLayer, 
                                          long nQuarter, long nModule, 
                                          int otisID)
{
  raw_int otisHeader = 0;
  int firstbit = 0;
  int usefulbit = 1;
  int Status = 0;
  int R0 = 0;
  int BX = 0;
  firstbit = firstbit << 31;
  nStation = nStation << 29;
  nLayer = nLayer << 27;
  nQuarter = nQuarter << 25;
  nModule = nModule << 21;
  otisID = otisID << 19;
  Status = Status << 16;
  usefulbit = usefulbit << 15;
  R0 = R0 << 11;
  BX = BX << 0;
  otisHeader = otisHeader | firstbit | nStation | nLayer | nQuarter | nModule | 
    otisID | usefulbit | Status | R0 | BX;
  return(otisHeader);
}
//=============================================================================
// Member functions
//=============================================================================

int OTFillRawBuffer::chID2Otis(OTChannelID otChannel)

{    
  int OtisID = 0;
  unsigned long nStraw = otChannel.straw();

  //Loop to Obtain the OtisID from the Straw Number

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

