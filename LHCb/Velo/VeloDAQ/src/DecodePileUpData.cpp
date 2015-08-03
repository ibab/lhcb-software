// Include files 
// data model
#include "Event/RawBank.h"
#include "Event/RawEvent.h" 
#include "Event/VeloCluster.h" 
#include "Event/VeloLiteCluster.h"
#include "Kernel/VeloChannelID.h"

// local
#include "DecodePileUpData.h"

using namespace LHCb;
using namespace VeloTELL1;

//----------------------------------------------------------------------------------
// Implementation file for class : DecodePileUpData
//
// 2009-06-19 : Serena Oggero
//----------------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( DecodePileUpData )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DecodePileUpData::DecodePileUpData( const std::string& name,
    ISvcLocator* pSvcLocator)
: Decoder::AlgBase ( name , pSvcLocator ), 
  m_rawEvent(0)
{
  declareProperty("NonZeroSupp", m_isNonZeroSupp=true);
  declareProperty("PUClusterLocation", m_PUClusterLocation="Raw/Velo/PUClusters");
  declareProperty("PUClusterNZSLocation", m_PUClusterNZSLocation="Raw/Velo/PUClustersNZS");
  m_rawEventLocations = {LHCb::RawEventLocation::Trigger, LHCb::RawEventLocation::Default};
  initRawEventSearch();
  
}

//=============================================================================
// Destructor
//=============================================================================
DecodePileUpData::~DecodePileUpData() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode DecodePileUpData::initialize() {

  StatusCode sc = Decoder::AlgBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  if (m_rawEventLocations.empty()) 
  {
    return Error("You need to give me a valid list of locaitons to decode!",StatusCode::FAILURE);
    
  }
  
  if (m_rawEventLocations[0]!=LHCb::RawEventLocation::Trigger && m_rawEventLocations[0]!=LHCb::RawEventLocation::Default) 
  {
    info() << "Using '" << m_rawEventLocations << "' as search path for the RawEvent object" << endmsg;
  }

  inizializePUcontainer( m_PUcontainerBee ) ;
  inizializePUcontainer( m_PUcontainerBee_NZS ) ;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode DecodePileUpData::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  StatusCode sc = getRawEvent();

  if ( sc ){
    // get PU banks (NZS and ZS)
    // fill in a LUT 
    decode(); 

    LHCb::VeloClusters * clusters = new LHCb::VeloClusters();
    LHCb::VeloClusters * clustersNZS = new LHCb::VeloClusters();

    // find hits from the LUT and put them in clusters
    for ( int sensorIt = 0; sensorIt < PuTell1::NUM_SENSORS; sensorIt++ ){    
      for ( int beetleIt = 0; beetleIt < PuTell1::NUM_BEETLES; beetleIt++ ){
        sc = findPileUpHitsBee(m_PUcontainerBee[sensorIt][beetleIt], sensorIt, beetleIt, clusters); 
        if ( m_isNonZeroSupp ){ 
          sc = findPileUpHitsBeeNZS(m_PUcontainerBee_NZS[sensorIt][beetleIt], sensorIt, beetleIt, clustersNZS);
        }
      }
    }

    // put the cluster in the TES
    writePUBanks( clusters, clustersNZS );
  }
  else { return ( StatusCode::SUCCESS ); }  

   if ( msgLevel(MSG::DEBUG) ) debug() << "==> DecodePileUp done" << endmsg;
  return StatusCode::SUCCESS;
}

//=============================================================================
StatusCode DecodePileUpData::getRawEvent() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> getRawEvent()" << endmsg;

  // Retrieve the RawEvent:
  m_rawEvent = findFirstRawEvent();
  
  if( m_rawEvent == NULL ) {
    error() << "Raw Event not found in " << m_rawEventLocations << endmsg;
    VeloClusters* clusters = new LHCb::VeloClusters();
    put(clusters,m_PUClusterLocation);    
    return ( StatusCode::FAILURE );
  }

  return ( StatusCode::SUCCESS ); 
}

//==============================================================================
StatusCode DecodePileUpData::decode() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> getRawBanks()" << endmsg;

  // NZS PU decode
  LHCb::RawBank::BankType type = LHCb::RawBank::BankType(PuTell1::LOPU_NZS);
  const std::vector<LHCb::RawBank*> banksNZS = m_rawEvent->banks(type); //bank name not defined yet!

  if ( banksNZS.size() != 0 )
  {
    StatusCode nzsBinDecoder=decodePileUpBinaryNZS(banksNZS);
    nzsBinDecoder.ignore();
    if ( msgLevel(MSG::DEBUG) ) debug() << " decode() : NZS bank decoded " << endmsg;
  }
  else Info(" NZS bank empty ");

  // ZS PU decode
  const std::vector<LHCb::RawBank*> banks = m_rawEvent->banks(LHCb::RawBank::L0PU); 
  if ( banks.size() != 0 )
  {
    StatusCode binDecoder=decodePileUpBinary(banks); 
    binDecoder.ignore();
     if ( msgLevel(MSG::DEBUG) ) debug() << " decode() : ZS bank decoded " << endmsg;
  }
  else Info(" ZS bank empty ");

  return ( StatusCode::SUCCESS ); 
}

//==============================================================================
StatusCode DecodePileUpData::decodePileUpBinary( const std::vector<LHCb::RawBank*> & banks ) 
{
  // loop on PU banks, should be only 1
  std::vector<LHCb::RawBank*>::const_iterator bi;
  int counter = 0;
  for ( bi = banks.begin(); bi != banks.end(); bi++) 
  {
    
     if ( msgLevel(MSG::DEBUG) ) debug() << "************************ decodePileUpBinary ***********" << counter << "********" << endmsg;
    LHCb::RawBank* aBank = *bi;

    // --> protect against corrupted banks
    if(LHCb::RawBank::MagicPattern!=aBank->magic()) continue;

    unsigned int* data = aBank->data(); //point at the beginning of data body, header already skipped
    unsigned int* dataPtr = data;

    // fill in the data container
    unsigned int head = PuTell1::HEADERS_PER_SECTION; // skip the first 2 words (header) of the section
    unsigned int wordTot = (aBank->size() / ( 2 * sizeof(unsigned int) ));
     if ( msgLevel(MSG::DEBUG) ) debug() << "now some prints...tot numb word is " << wordTot << endmsg;
    Fill( head, wordTot, dataPtr, PuTell1::WORDS_PER_SECTION, m_PUcontainerBee );
    counter++;
  } // loop on PU banks

  return StatusCode::SUCCESS; 
} // end function

//============================================================================
StatusCode DecodePileUpData::decodePileUpBinaryNZS( const std::vector<LHCb::RawBank*> & banksNZS ) 
{
  // loop on PU non zero-supp banks, there should be only 1
  for (std::vector<LHCb::RawBank*>::const_iterator bnzs = banksNZS.begin(); bnzs!= banksNZS.end(); bnzs++) 
  {
     if ( msgLevel(MSG::DEBUG) ) debug() << "************************ decodePileUpBinaryNZS *********************" << endmsg;
    LHCb::RawBank* aBank = *bnzs;

    // --> protect against corrupted banks
    if(LHCb::RawBank::MagicPattern!=aBank->magic()) continue;

    unsigned int* data = (aBank->data()); //point at the beginning of data body skipping the header
    //loop on PP FPGA's
    for ( int PP = 0; PP < PuTell1::NUM_PP ; PP++ ){
      unsigned int* PPbegin = data + PP*(PuTell1::WORDS_PER_PP); // point at the beginning of each PP body; 

      // initialize links
      bool linkIsGood[PuTell1::NUM_PPLINKS];
      for ( int linkIt = 0; linkIt < PuTell1::NUM_PPLINKS; linkIt++) { 
        linkIsGood[linkIt] = false;
      }

      // loop on data words
      for ( int word = 0; word < PuTell1::WORDS_PER_LINK; word++ )
      {
        // loop on links; step of 2 to avoid repetitions
        for ( int link = 0; link < PuTell1::NUM_PPLINKS; link+=2 ){

          // get link format
          if ( word== PuTell1::WORD_WITH_FORMATINFO ){
            unsigned int* word1Ptr = PPbegin + (PuTell1::NUM_PPLINKS)*word + link;
            int format = ( (*word1Ptr) >> PuTell1::FORMAT_BIT);
            unsigned int* word1Ptr2 = PPbegin + (PuTell1::NUM_PPLINKS)*word + link+1;
            int format2 = ( (*word1Ptr2) >> PuTell1::FORMAT_BIT);
            if ( format && format2 ){
              linkIsGood[link] = true;
              linkIsGood[link+1] = true;
            }
          }
          // get data but only for *good* links
          else if ( (word > PuTell1::BEGIN_OF_DATAWORDS) && (linkIsGood[link]) && (linkIsGood[link+1]) ){
            // use the Fill function avoiding the loop
            unsigned int* datawordPtr = PPbegin + (PuTell1::NUM_PPLINKS)*word + link;

            // word can go from 4 to 36, but the Fill function uses a "scheme" with words from 2 to 33
            // moreover it's done to loop automatically on all the words from a ZS bank
            // thus I use a step of 1, to enter only once the while-condition, and I give it (word-2)
            Fill( word-2, word-1, datawordPtr-word+2, 1, m_PUcontainerBee_NZS );
          }
        } // link loop

      } // word loop
    } // PPi loop

  } // PU banks loop
  return StatusCode::SUCCESS;
}

//============================================================================

void DecodePileUpData::inizializePUcontainer( PuTell1::DataTable PUcontainerBee ) 
{
  for (int sens = 0; sens < PuTell1::NUM_SENSORS; sens++ ){
    for ( int bee = 0; bee < PuTell1::NUM_BEETLES; bee++ ){
      unsigned int word = 0;
      PUcontainerBee[sens][bee].dataWord = word;

      switch(bee){
        case 3:
        case 5:
        case 6:
        case 7:
        case 11:
        case 13:
        case 14:
        case 15:
          PUcontainerBee[sens][bee].isOrdered = false;
          break;
        default:
          PUcontainerBee[sens][bee].isOrdered = true;
      }
    }
  }     
}

//=========================================================================
void DecodePileUpData::Fill( unsigned int wordIt, unsigned int word_Tot, 
                             unsigned int* data_Ptr, int step, 
                             PuTell1::DataTable PUcontainerBee ){
   if ( msgLevel(MSG::DEBUG) ) debug() << "******************** Fill() *********************************" << endmsg;
  while ( wordIt < word_Tot )
  {
    unsigned int* wordPtr = data_Ptr + wordIt;
     if ( msgLevel(MSG::DEBUG) ) debug() << "*HARD* DEBUG - FILL  : wordPtr " << wordPtr << " " << binary(*wordPtr) << endmsg;

    //to take words from the 2nd section
    // step is 34 for the 0 supp bank, 35 for the non-0 supp
    unsigned int* wordPtr2 = wordPtr + step;  
     if ( msgLevel(MSG::DEBUG) ) debug() << "*HARD* DEBUG - FILL  : wordPtr2 " << wordPtr2 << " " << binary(*wordPtr2) << endmsg;     
    switch(wordIt){
      case 2:
        (PUcontainerBee[0][2]).dataWord = *wordPtr;
        (PUcontainerBee[0][1]).dataWord =  *wordPtr2;
        wordIt++;
        break;
      case 3:
        (PUcontainerBee[0][0]).dataWord =  *wordPtr;
        (PUcontainerBee[0][3]).dataWord =  *wordPtr2;
        wordIt++;
        break;
      case 4:
        (PUcontainerBee[1][13]).dataWord =  *wordPtr;
        (PUcontainerBee[1][14]).dataWord =  *wordPtr2;
        wordIt++;
        break;
      case 5:
        (PUcontainerBee[1][15]).dataWord =  *wordPtr;
        (PUcontainerBee[1][12]).dataWord =  *wordPtr2;
        wordIt++;
        break;
      case 6:
        (PUcontainerBee[2][13]).dataWord =  *wordPtr;
        (PUcontainerBee[2][14]).dataWord =  *wordPtr2;
        wordIt++;
        break;
      case 7:
        (PUcontainerBee[2][15]).dataWord =  *wordPtr;
        (PUcontainerBee[2][12]).dataWord =  *wordPtr2;
        wordIt++;      
        break;              
      case 8:
        (PUcontainerBee[3][2]).dataWord =  *wordPtr;
        (PUcontainerBee[3][1]).dataWord =  *wordPtr2;
        wordIt++;      
        break;     
      case 9:
        (PUcontainerBee[3][0]).dataWord =  *wordPtr;
        (PUcontainerBee[3][3]).dataWord =  *wordPtr2;  
        wordIt++;      
        break;     
      case 10:
        (PUcontainerBee[0][5]).dataWord =  *wordPtr;
        (PUcontainerBee[0][6]).dataWord =  *wordPtr2;
        wordIt++;      
        break;     
      case 11:
        (PUcontainerBee[0][7]).dataWord =  *wordPtr;
        (PUcontainerBee[0][4]).dataWord =  *wordPtr2;
        wordIt++;      
        break;     
      case 12:
        (PUcontainerBee[1][10]).dataWord =  *wordPtr;
        (PUcontainerBee[1][9]).dataWord =  *wordPtr2;
        wordIt++;      
        break;     
      case 13:
        (PUcontainerBee[1][8]).dataWord =  *wordPtr;
        (PUcontainerBee[1][11]).dataWord =  *wordPtr2;
        wordIt++;      
        break;             
      case 14:
        (PUcontainerBee[2][10]).dataWord =  *wordPtr;
        (PUcontainerBee[2][9]).dataWord =  *wordPtr2;
        wordIt++;      
        break;     
      case 15:
        (PUcontainerBee[2][8]).dataWord =  *wordPtr;
        (PUcontainerBee[2][11]).dataWord =  *wordPtr2;
        wordIt++;      
        break;     
      case 16:
        (PUcontainerBee[3][5]).dataWord =  *wordPtr;
        (PUcontainerBee[3][6]).dataWord =  *wordPtr2;
        wordIt++;      
        break;     
      case 17:
        (PUcontainerBee[3][7]).dataWord =  *wordPtr;
        (PUcontainerBee[3][4]).dataWord =  *wordPtr2;
        wordIt++;      
        break;              
      case 18:
        (PUcontainerBee[0][10]).dataWord =  *wordPtr ;
        (PUcontainerBee[0][9]).dataWord =  *wordPtr2;
        wordIt++;      
        break;     
      case 19:
        (PUcontainerBee[0][8]).dataWord =  *wordPtr;
        (PUcontainerBee[0][11]).dataWord =  *wordPtr2;
        wordIt++;      
        break;     
      case 20:
        (PUcontainerBee[1][5]).dataWord =  *wordPtr;
        (PUcontainerBee[1][6]).dataWord =  *wordPtr2;
        wordIt++;      
        break;     
      case 21:
        (PUcontainerBee[1][7]).dataWord =  *wordPtr;
        (PUcontainerBee[1][4]).dataWord =  *wordPtr2;
        wordIt++;      
        break;             
      case 22:
        (PUcontainerBee[2][5]).dataWord =  *wordPtr;
        (PUcontainerBee[2][6]).dataWord =  *wordPtr2;
        wordIt++;      
        break;     
      case 23:
        (PUcontainerBee[2][7]).dataWord =  *wordPtr;
        (PUcontainerBee[2][4]).dataWord =  *wordPtr2;
        wordIt++;      
        break;     
      case 24:
        (PUcontainerBee[3][10]).dataWord =  *wordPtr;
        (PUcontainerBee[3][9]).dataWord =  *wordPtr2;
        wordIt++;      
        break;     
      case 25:
        (PUcontainerBee[3][8]).dataWord =  *wordPtr;
        (PUcontainerBee[3][11]).dataWord =  *wordPtr2;
        wordIt++;      
        break;              
      case 26:
        (PUcontainerBee[0][13]).dataWord =  *wordPtr;
        (PUcontainerBee[0][14]).dataWord =  *wordPtr2;
        wordIt++;      
        break;     
      case 27:
        (PUcontainerBee[0][15]).dataWord =  *wordPtr;
        (PUcontainerBee[0][12]).dataWord =  *wordPtr2;
        wordIt++;      
        break;     
      case 28:
        (PUcontainerBee[1][2]).dataWord =  *wordPtr;
        (PUcontainerBee[1][1]).dataWord =  *wordPtr2;
        wordIt++;      
        break;     
      case 29:
        (PUcontainerBee[1][0]).dataWord =  *wordPtr;
        (PUcontainerBee[1][3]).dataWord =  *wordPtr2;
        wordIt++;      
        break;             
      case 30:
        (PUcontainerBee[2][2]).dataWord =  *wordPtr;
        (PUcontainerBee[2][1]).dataWord =  *wordPtr2;
        wordIt++;      
        break;     
      case 31:
        (PUcontainerBee[2][0]).dataWord =  *wordPtr;
        (PUcontainerBee[2][3]).dataWord =  *wordPtr2;
        wordIt++;      
        break;     
      case 32:
        (PUcontainerBee[3][13]).dataWord =  *wordPtr;
        (PUcontainerBee[3][14]).dataWord =  *wordPtr2;
        wordIt++;      
        break;     
      case 33:
        (PUcontainerBee[3][15]).dataWord =  *wordPtr;
        (PUcontainerBee[3][12]).dataWord =  *wordPtr2;
        wordIt++;      
        break;  

    } // switch case
  } // while "beetle map"  
}

//============================================================================

StatusCode DecodePileUpData::findPileUpHitsBee( PuTell1::dataObject OneBeetleData, int sens, int bee,  VeloClusters * clusters ) 
{
  unsigned int maskBee = 1;
  unsigned int inBee = OneBeetleData.dataWord;

  // VeloChannelID parameters
  unsigned int sensorNum;
  unsigned int stripNum;
  unsigned int fracStrip = 0;
  unsigned int pseudoSize = 0;
  bool hasHighThre = 1;

  for(int i=0; i < PuTell1::NUMBITS; i++)
  {
    unsigned int checkBee = maskBee & inBee; 

    if ( checkBee ){
      sensorNum = SensorId(sens);

      if ( OneBeetleData.isOrdered ){
        //not flipped word
        stripNum = ( PuTell1::STRIPS_PER_CHANNEL )*i + bee* ( PuTell1::STRIPS_PER_BEETLE ); // channel numbering starts from 0
      }
      else{
        //flipped
        stripNum = ( PuTell1::STRIPS_PER_CHANNEL )*( PuTell1::NUMBITS-1-i ) + bee*( PuTell1::STRIPS_PER_BEETLE );  
      }
      VeloChannelID vcid( sensorNum, stripNum );
      VeloLiteCluster lc( fracStrip, pseudoSize, hasHighThre, vcid );
      VeloCluster::ADCVector adcs;
      adcs.reserve(1);
      adcs.push_back( std::pair<int,unsigned int> (static_cast<int>( stripNum ), static_cast<unsigned int>( 1.)) );
      // now append new cluster
      clusters->insert( new LHCb::VeloCluster(lc,adcs), vcid );

       if ( msgLevel(MSG::DEBUG) ) 
       {
         debug()	<< "findPileUpHitsBee : VeloLiteCluster lc(" 
                  << fracStrip << ", "
                  << pseudoSize << ", " 
                  << hasHighThre << ", with channelId strip "
                  << (lc.channelID()).strip() << endmsg;
         debug() << "******************** clusters size is " 
                 << clusters->size() << ")*********************************" 
                 << endmsg;
       }
       
    } // if checkBee
    else if ( msgLevel(MSG::DEBUG) )
    {
      if ( OneBeetleData.isOrdered ){
        //not flipped word
        debug() << "No hits found in PU strip i= " << i << endmsg;
      }
      else{
        //flipped
        debug() << "No hits found in PU strip i= " << 32 - i << endmsg;
      }
    }  
    maskBee = maskBee << 1;
  } // for loop on word

  return StatusCode::SUCCESS; 
}

//==============================================================================

StatusCode DecodePileUpData::findPileUpHitsBeeNZS( PuTell1::dataObject OneBeetleData, int sens,
                                                   int bee,  VeloClusters * clustersNZS ) 
{
  unsigned int maskBee = 1;
  unsigned int inBee = OneBeetleData.dataWord;

  // VeloChannelID parameters
  unsigned int sensorNum;
  unsigned int stripNum;
  unsigned int fracStrip = 0;
  unsigned int pseudoSize = 0;
  bool hasHighThre = 1;

  for(int i=0; i < PuTell1::NUMBITS; i++)
  {
    unsigned int checkBee = maskBee & inBee;   
    if ( checkBee ){
      sensorNum = SensorId(sens);     

      if ( OneBeetleData.isOrdered ){
        //not flipped word
        stripNum = ( PuTell1::STRIPS_PER_CHANNEL )*i + bee*( PuTell1::STRIPS_PER_BEETLE );
      }
      else{
        //flipped
        stripNum = ( PuTell1::STRIPS_PER_CHANNEL )*( PuTell1::NUMBITS-1-i) + bee*( PuTell1::STRIPS_PER_BEETLE );  
      }

      VeloChannelID vcid( sensorNum, stripNum );
      VeloLiteCluster lc( fracStrip, pseudoSize, hasHighThre,
          VeloChannelID(sensorNum, stripNum) );
      VeloCluster::ADCVector adcs;
      adcs.reserve(1);
      adcs.push_back( std::pair<int,unsigned int> (static_cast<int>( stripNum ), static_cast<unsigned int>( 1.)) );
      // got all we need, now append new cluster
      clustersNZS->insert( new LHCb::VeloCluster(lc,adcs), vcid );

       if ( msgLevel(MSG::DEBUG) ) 
       {
         debug() 	<< "findPileUpHitsBeeNZS : VeloLiteCluster lc(" 
                  << fracStrip << ", " 
                  << pseudoSize << ", " 
                  << hasHighThre << ", with channelId strip "
                  << (lc.channelID()).strip() << endmsg;
         debug() 	<< "******************** clustersNZS size is " 
                  << clustersNZS->size() 
                  << ")*********************************" << endmsg;
       }
       
    } // if checkBee
    else if ( msgLevel(MSG::DEBUG) ) 
    {
      if ( OneBeetleData.isOrdered ){
        //not flipped word
        debug() << "No hits found in PU strip i= " << i << endmsg;
      }
      else{
        //flipped
        debug() << "No hits found in PU strip i= " << 32 - i << endmsg;
      }
    }
    maskBee = maskBee << 1;
  }
  return StatusCode::SUCCESS; 
}

//==============================================================================

// binary function with strings
std::string DecodePileUpData::binary( unsigned int number )
{
  std::string result;
  while(number)
  {
    result.insert(result.begin(), number % 2 + '0');
    number /= 2;
  }
  return result;
}

//============================================================================

// function to convert sensor numbering (0,1,2,3) to "official" PU sensor Id
int DecodePileUpData::SensorId (int s){
  switch(s){ 
    case (PuTell1::A1):
      return 128;
    case (PuTell1::C1): 
      return 129;
    case (PuTell1::A2):
      return 130;
    case (PuTell1::C2):
      return 131;
    default:
      return -1;
  }
}

//============================================================================

StatusCode DecodePileUpData::writePUBanks(  LHCb::VeloClusters * clusters,  LHCb::VeloClusters * clustersNZS )  {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> writePUBanks()" << endmsg;

  put(clusters, m_PUClusterLocation);
  //info() << "put clusters in " <<  m_PUClusterLocation << endmsg();
  if ( m_isNonZeroSupp ){ 
    put(clustersNZS, m_PUClusterNZSLocation);
  }
  return ( StatusCode::SUCCESS ); 
}

//==============================================================================
