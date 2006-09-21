// $Id: PuVetoFillRawBuffer.cpp,v 1.2 2006-09-21 12:23:32 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// from DAQEvent
#include "Event/RawEvent.h"
// from VeloEvent
#include "Event/MCVeloFE.h"

// local
#include "PuVetoFillRawBuffer.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PuVetoFillRawBuffer
//
// 2006-09-19 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PuVetoFillRawBuffer );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PuVetoFillRawBuffer::PuVetoFillRawBuffer( const std::string& name,
                                          ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
   , m_inputContainer      ( LHCb::MCVeloFELocation::PuVeto  )
   , m_threshold           ( 7000.     ) //electrons
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

  debug() << "==> Initialize" << endmsg;

  DeVelo* m_velo = getDet<DeVelo>( DeVeloLocation::Default );

  m_nbPuSensor = m_velo->numberPileUpSensors();
  m_firstPuSensor = (*(m_velo->pileUpRSensorsBegin()))->sensorNumber();

  debug() << "Strip charge threshold: " << m_threshold << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PuVetoFillRawBuffer::execute() {

  debug() << "==> Execute" << endmsg;

  //*** Get the input data

  LHCb::MCVeloFEs *fes =get<LHCb::MCVeloFEs>( eventSvc() , m_inputContainer );
 
  // Apply a threshold on each strip, OR them by 4 and construct the hit bit pattern
  
  std::vector<unsigned short int> rawPuData;
  std::vector<unsigned int> rawDataVec;
  unsigned short int prevRawHit = 0xFFFF;
  
  for ( LHCb::MCVeloFEs::const_iterator itFe = fes->begin();
        fes->end() != itFe ; itFe++  ) {
    if ( m_threshold < (*itFe)->charge() ) {      
      if ( ! (*itFe)->channelID().isPileUp() ){
        info() << "Unexpected sensor " 
               << (*itFe)->channelID().sensor()
               << " in " << m_inputContainer << endreq;
        continue; 
      }
      
      int sensor = (*itFe)->sensor() - m_firstPuSensor;

      //== Coded value is the central strip of each group of 4 strips.

      unsigned int sfired = 4 * ( (*itFe)->strip()/4 ) + 2;
      unsigned short int rawHit = rawEncode( sensor, sfired );
      if ( MSG::VERBOSE <= msgLevel() ) {
        verbose() << format( "Sensor %2d real strip%5d   1/4 strip %4d raw %4x ",
                             sensor, (*itFe)->strip(), sfired, rawHit );
      }
      if ( rawHit != prevRawHit ) {
        rawPuData.push_back( rawHit );
        prevRawHit = rawHit;
        verbose() << " stored" << endreq;
      } else {
        verbose() << " duplicate." << endreq;
      }
    }    
  }

  // Store all hits in the RawEvent buffer

  unsigned int header = 0;
  rawVec( &rawPuData, &rawDataVec );
  LHCb::RawEvent* raw = get<LHCb::RawEvent>( LHCb::RawEventLocation::Default );
  raw->addBank(header,LHCb::RawBank::L0PU,1,rawDataVec);

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode PuVetoFillRawBuffer::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=========================================================================
//  
//=========================================================================
void PuVetoFillRawBuffer::rawVec ( std::vector<unsigned short int> *vecin,
                                   std::vector<unsigned int> *vecout) {
  std::vector<unsigned short int>::iterator itraw = vecin->begin();
  unsigned short int temp1,temp2;
  unsigned int tempraw;
  
  while (itraw != vecin->end()){
    temp1 = (*itraw);
    itraw++;
    temp2 = 0;
    if ( itraw != vecin->end() ) {
      temp2 = (*itraw);
      itraw++;
    }
    tempraw = temp2;
    tempraw = tempraw << 16;
    tempraw += temp1;
    vecout->push_back(tempraw);
  }
}
//=============================================================================
