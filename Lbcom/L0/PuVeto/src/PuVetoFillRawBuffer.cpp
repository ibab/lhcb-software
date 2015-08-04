// Include files 

// from DAQEvent
#include "Event/RawEvent.h"
// from VeloEvent
#include "Event/MCVeloFE.h"

// local
#include "PuVetoFillRawBuffer.h"
#include "bit_order.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PuVetoFillRawBuffer
// 
// 2009-07-14 : Serena Oggero, rewrite for the latest PU bank format
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PuVetoFillRawBuffer )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PuVetoFillRawBuffer::PuVetoFillRawBuffer( const std::string& name,
                                          ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
   , m_inputContainer      ( LHCb::MCVeloFELocation::PuVeto  )
   , m_threshold           ( 7000.     ) //electrons
   , m_firstPuSensor       ( 0 )
   , m_velo                ( NULL      )
{
  declareProperty( "InputContainer"     , m_inputContainer  );
  declareProperty( "SignalThreshold"    , m_threshold       );

}
//=============================================================================
// Destructor
//=============================================================================
PuVetoFillRawBuffer::~PuVetoFillRawBuffer() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode PuVetoFillRawBuffer::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if (msgLevel(MSG::DEBUG)) debug() << "==> Initialize" << endmsg;

  m_velo = getDet<DeVelo>( DeVeloLocation::Default );

  m_firstPuSensor = (*(m_velo->pileUpRSensorsBegin()))->sensorNumber();
  if (msgLevel(MSG::DEBUG)) debug() << "First PU sensor is " << m_firstPuSensor << endmsg;
  
  inizializePUcontainer( m_PUcontainerBee ) ;
  
  return StatusCode::SUCCESS;
}

//============================================================================================
// Main execution
//============================================================================================
StatusCode PuVetoFillRawBuffer::execute() {

  if (msgLevel(MSG::DEBUG)) debug() << "==> Execute " << endmsg;
  inizializePUcontainer( m_PUcontainerBee ) ;
  
  //*** Get the input data
  LHCb::MCVeloFEs *fes =get<LHCb::MCVeloFEs>( eventSvc() , m_inputContainer );
 
  // Apply a threshold on each strip, OR them by 4 and construct the hit bit pattern
  std::vector <unsigned int> rawDataVec(68);
  for ( int rawDataVecIt = 0; rawDataVecIt < 68; rawDataVecIt++ ){
    unsigned int emptyWord = 0;
    rawDataVec[rawDataVecIt] = emptyWord;
  }
  
  if (msgLevel(MSG::DEBUG)) debug() << "fes size is " << fes->size() << endmsg;
  
  int debugit = 0;
  for ( LHCb::MCVeloFEs::const_iterator itFe = fes->begin(); itFe != fes->end() ; itFe++  ) {  
    //debug() << "debugit " << debugit << ": It charge is " << (*itFe)->charge() << endmsg;
    debugit++;
    
    if ( m_threshold < (*itFe)->charge() ) {      
      if ( ! (*itFe)->channelID().isPileUp() ){
        if (msgLevel(MSG::DEBUG)) debug() << "Unexpected sensor " 
                                          << (*itFe)->channelID().sensor()
                                          << " in " << m_inputContainer << endmsg;
        continue; 
      }
      
      int sensor = (*itFe)->sensor() - m_firstPuSensor; // should be between 0 and 3
      int beetle = (*itFe)->strip()/128; // from 0 to 15
      unsigned int puChannel = ( ((*itFe)->strip())%128 )/4; // from 0 to 31 
      
      unsigned int bit = 1;
      for (unsigned int bitplace = 0; bitplace < (puChannel); bitplace++ ){
        bit = bit << 1;
      }
      if (msgLevel(MSG::DEBUG)) debug() 	<< "Sensor: " << sensor
                                          << ", beetle: " << beetle
                                          << ", puChannel: " << puChannel
                                          << " in binary " << binary( bit )
                                          << endmsg;
      
      (m_PUcontainerBee[sensor][beetle]).w += bit; 
    } //  if above threshold    
  } // loop on McFes
  
  adjustPUcontainer( m_PUcontainerBee ) ;
  writeDataVec( m_PUcontainerBee, rawDataVec);
  int srcId = 2; 
  
  LHCb::RawEvent* raw = get<LHCb::RawEvent>( LHCb::RawEventLocation::Default );
  raw->addBank( srcId, LHCb::RawBank::L0PU, 2, rawDataVec ); // new version is 2

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode PuVetoFillRawBuffer::finalize() {

  if (msgLevel(MSG::DEBUG)) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=========================================================================
//  
//=========================================================================
void PuVetoFillRawBuffer::inizializePUcontainer( Word PUcontainerBee[4][16] ) 
{
  for (int sens = 0; sens<4; sens++ ){
    for ( int bee = 0; bee<16; bee++ ){
      unsigned int word = 0;
      PUcontainerBee[sens][bee].w = word;
      
      switch(bee){
        case 3:
	case 5:
	case 6:
	case 7:
	case 11:
	case 13:
	case 14:
	case 15:
	  PUcontainerBee[sens][bee].flag = false;
	  break;
	default:
	  PUcontainerBee[sens][bee].flag = true;
      }
    }
  }     
}

//=============================================================================
//  
//=========================================================================
void PuVetoFillRawBuffer::adjustPUcontainer( Word PUcontainerBee[4][16] ) 
{
  for (int sens = 0; sens<4; sens++ ){
    for ( int bee = 0; bee<16; bee++ ){
      if ( (PUcontainerBee[sens][bee]).flag == false ){
        if (msgLevel(MSG::DEBUG)) 
          debug() << "word for s=" << sens <<",bee=" << bee << " was " << binary( PUcontainerBee[sens][bee].w );
        reverse_bit( PUcontainerBee[sens][bee].w );
        if (msgLevel(MSG::DEBUG)) debug() << " ----> " << binary( PUcontainerBee[sens][bee].w ) << endmsg;
      }
    }
  }     
}

//=============================================================================
//
//=========================================================================
void PuVetoFillRawBuffer::writeDataVec( Word PUcontainerBee[4][16], std::vector <unsigned int> & rawDataVec ) 
{
  // ugly way..
  
  // data section 1
  rawDataVec[2] = (PUcontainerBee[0][2]).w;
  rawDataVec[3] = (PUcontainerBee[0][0]).w;
  rawDataVec[4] = (PUcontainerBee[1][13]).w;
  rawDataVec[5] = (PUcontainerBee[1][15]).w;
  rawDataVec[6] = (PUcontainerBee[2][13]).w;
  rawDataVec[7] = (PUcontainerBee[2][15]).w;
  rawDataVec[8] = (PUcontainerBee[3][2]).w;
  rawDataVec[9] = (PUcontainerBee[3][0]).w;
  rawDataVec[10] = (PUcontainerBee[0][5]).w;
  rawDataVec[11] = (PUcontainerBee[0][7]).w;
  rawDataVec[12] = (PUcontainerBee[1][10]).w;
  rawDataVec[13] = (PUcontainerBee[1][8]).w;
  rawDataVec[14] = (PUcontainerBee[2][10]).w;
  rawDataVec[15] = (PUcontainerBee[2][8]).w;
  rawDataVec[16] = (PUcontainerBee[3][5]).w;
  rawDataVec[17] = (PUcontainerBee[3][7]).w;
  rawDataVec[18] = (PUcontainerBee[0][10]).w;
  rawDataVec[19] = (PUcontainerBee[0][8]).w;
  rawDataVec[20] = (PUcontainerBee[1][5]).w;
  rawDataVec[21] = (PUcontainerBee[1][7]).w;
  rawDataVec[22] = (PUcontainerBee[2][5]).w;
  rawDataVec[23] = (PUcontainerBee[2][7]).w;
  rawDataVec[24] = (PUcontainerBee[3][10]).w;
  rawDataVec[25] = (PUcontainerBee[3][8]).w;
  rawDataVec[26] = (PUcontainerBee[0][13]).w;
  rawDataVec[27] = (PUcontainerBee[0][15]).w;
  rawDataVec[28] = (PUcontainerBee[1][2]).w;
  rawDataVec[29] = (PUcontainerBee[1][0]).w;
  rawDataVec[30] = (PUcontainerBee[2][2]).w;
  rawDataVec[31] = (PUcontainerBee[2][0]).w;
  rawDataVec[32] = (PUcontainerBee[3][13]).w;
  rawDataVec[33] = (PUcontainerBee[3][15]).w;    
      
   // data section 2
  rawDataVec[36] = (PUcontainerBee[0][1]).w;
  rawDataVec[37] = (PUcontainerBee[0][3]).w;
  rawDataVec[38] = (PUcontainerBee[1][14]).w;
  rawDataVec[39] = (PUcontainerBee[1][12]).w;
  rawDataVec[40] = (PUcontainerBee[2][14]).w;
  rawDataVec[41] = (PUcontainerBee[2][12]).w;
  rawDataVec[42] = (PUcontainerBee[3][1]).w;
  rawDataVec[43] = (PUcontainerBee[3][3]).w;
  rawDataVec[44] = (PUcontainerBee[0][6]).w;
  rawDataVec[45] = (PUcontainerBee[0][4]).w;
  rawDataVec[46] = (PUcontainerBee[1][9]).w;
  rawDataVec[47] = (PUcontainerBee[1][11]).w;
  rawDataVec[48] = (PUcontainerBee[2][9]).w;
  rawDataVec[49] = (PUcontainerBee[2][11]).w;
  rawDataVec[50] = (PUcontainerBee[3][6]).w;
  rawDataVec[51] = (PUcontainerBee[3][4]).w;
  rawDataVec[52] = (PUcontainerBee[0][9]).w;
  rawDataVec[53] = (PUcontainerBee[0][11]).w;
  rawDataVec[54] = (PUcontainerBee[1][6]).w;
  rawDataVec[55] = (PUcontainerBee[1][4]).w;
  rawDataVec[56] = (PUcontainerBee[2][6]).w;
  rawDataVec[57] = (PUcontainerBee[2][4]).w;
  rawDataVec[58] = (PUcontainerBee[3][9]).w;
  rawDataVec[59] = (PUcontainerBee[3][11]).w;
  rawDataVec[60] = (PUcontainerBee[0][14]).w;
  rawDataVec[61] = (PUcontainerBee[0][12]).w;
  rawDataVec[62] = (PUcontainerBee[1][1]).w;
  rawDataVec[63] = (PUcontainerBee[1][3]).w;
  rawDataVec[64] = (PUcontainerBee[2][1]).w;
  rawDataVec[65] = (PUcontainerBee[2][3]).w;
  rawDataVec[66] = (PUcontainerBee[3][14]).w;    
  rawDataVec[67] = (PUcontainerBee[3][12]).w;             
}

//============================================================================
// binary function with strings

std::string PuVetoFillRawBuffer::binary( unsigned int number )
{
  std::string result;
  unsigned int numberTmp = number;
  while(numberTmp)
  {
    result.insert(result.begin(), numberTmp % 2 + '0');
    numberTmp /= 2;
  }
  if(number==0) result='0';
  return result;
}
