// Include files:
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
// Kernel
#include "Kernel/VPChannelID.h"
// VPelDet
#include "VPDet/DeVP.h"
// local
#include "VPDigitWord.h"
#include "VPDigitRawBankEncoder.h"

/*
 * Algorithm to encode VPDigits to VPDigit rawbank  (VPChannelID + ToT)
 * Based (entirely) on VP Raw bank code
 * Tim Evans 25-09-2013
 */

using namespace LHCb;

//=============================================================================
// Declaration of VP(Digit)RawBankEncoder
//=============================================================================
DECLARE_ALGORITHM_FACTORY(VPDigitRawBankEncoder)

VPDigitRawBankEncoder::VPDigitRawBankEncoder(const std::string& name,
                                             ISvcLocator* pSvcLocator)
  : GaudiAlgorithm(name, pSvcLocator)
  , m_bankVersion(1)
  , m_bankSizeInBytes(0)
  , m_isDebug(false)
  , m_isVerbose(false)
  , m_vPelDet(NULL)
{
  declareProperty("DigitLocation", m_digitLocation = LHCb::VPDigitLocation::VPDigitLocation );
  declareProperty("RawEventLocation", m_rawEventLocation = LHCb::RawEventLocation::Default);
}

VPDigitRawBankEncoder::~VPDigitRawBankEncoder(){}

//=============================================================================
// Initialisation
//=============================================================================
StatusCode VPDigitRawBankEncoder::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if(sc.isFailure()) return sc;
  m_isDebug = msgLevel(MSG::DEBUG);
  m_isVerbose = msgLevel(MSG::VERBOSE);
  if(m_isDebug) debug() << "==> Initialise" << endmsg;

  m_vPelDet = getDet<DeVP>(DeVPLocation::Default);
  std::vector<DeVPSensor*>::const_iterator sIter = m_vPelDet->sensorsBegin();
  std::vector<DeVPSensor*>::const_iterator sEnd = m_vPelDet->sensorsEnd();
  for(; sIter != sEnd; ++sIter) {
    m_sensorNumbers.push_back((*sIter)->sensorNumber());
  }
  std::sort(m_sensorNumbers.begin(),m_sensorNumbers.end());
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Execution
//=============================================================================
StatusCode VPDigitRawBankEncoder::execute() {
  if(m_isDebug) debug() << "==> Execute" << endmsg;
  const VPDigits* digits = getIfExists<VPDigits>(m_digitLocation);
  if( NULL == digits ) {
    return Error( " ==> There are no VPDigits in TES! " );
  }

  // Check if RawEvent exits
  RawEvent* rawEvent = getIfExists<RawEvent>(m_rawEventLocation);
  if( NULL == rawEvent ) {
    // Create RawEvent
    rawEvent = new LHCb::RawEvent();
    put(rawEvent,m_rawEventLocation);
  }
  m_sortedDigits.clear();
  m_sortedDigits.resize(digits->size());
  std::copy(digits->begin(),digits->end(),m_sortedDigits.begin());
  // Sort by sensor number
  std::stable_sort(m_sortedDigits.begin(),m_sortedDigits.end(), sortLessBySensor);

  std::vector<const VPDigit*>::const_iterator firstOnSensor, lastOnSensor;
  lastOnSensor = firstOnSensor = m_sortedDigits.begin();

  unsigned int currentSensorNumber;
  int sensorIndex = -1;
  while(firstOnSensor != m_sortedDigits.end()) {
    currentSensorNumber = (*firstOnSensor)->channelID().module();
    // Move to next on list of expected sensors
    sensorIndex++;
    // Check there was not a missing sensor
    while(currentSensorNumber != m_sensorNumbers[sensorIndex]) {
      // Store an empty bank
      storeBank(m_sensorNumbers[sensorIndex],lastOnSensor,lastOnSensor,
                rawEvent);
      if(m_isDebug) debug() << "Added empty bank for sensor "
                            << m_sensorNumbers[sensorIndex] << endreq;
      // Move to next on list, try again
      sensorIndex++;
    }
    while(lastOnSensor != m_sortedDigits.end() &&
         (*lastOnSensor)->channelID().module() == currentSensorNumber) {
      ++lastOnSensor;
    }
    // Make and store the bank
    storeBank(currentSensorNumber,firstOnSensor,lastOnSensor,rawEvent);
    firstOnSensor = lastOnSensor;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Store RawBank
//=============================================================================
void VPDigitRawBankEncoder::storeBank(int sensor,
                      std::vector<const VPDigit*>::const_iterator begin,
                      std::vector<const VPDigit*>::const_iterator end,
                      RawEvent* rawEvent)
{
  // Create new raw buffer in raw data cache, old one is cleared
  makeBank(begin,end);
  if(m_vPelDet->sensor(sensor)) {
    if(m_isDebug) debug() << "Sensor = " << sensor << endmsg;
    LHCb::RawBank* newBank = 
      rawEvent->createBank(static_cast<SiDAQ::buffer_word>(sensor),
                               LHCb::RawBank::VPDigit,
                               m_bankVersion,
                               m_bankSizeInBytes,
                               &(m_rawData[0]));
    // Add new bank and pass memory ownership to raw event
    rawEvent->adoptBank(newBank,true);
  }
}


//=============================================================================
// Make RawBank 
//=============================================================================
void VPDigitRawBankEncoder::makeBank(
                std::vector<const LHCb::VPDigit*>::const_iterator begin,
                std::vector<const LHCb::VPDigit*>::const_iterator end)
{
  m_rawData.clear();
  int numDigi = end - begin;
  SiHeaderWord hw(numDigi,0,0);
  SiDAQ::buffer_word pcnAndNumDigi = hw.value();
  m_rawData.push_back(pcnAndNumDigi); // Add header
  std::vector<const LHCb::VPDigit*>::const_iterator iD = begin;
  for(; iD != end; ++iD) {
		VPDigitWord vphw((*iD)->channelID().channelID(), (*iD)->ToTValue() ); //packs the digit
		m_rawData.push_back(static_cast<SiDAQ::buffer_word>(vphw.value())); //pushes back into the RawBank
	}

  m_bankSizeInBytes = sizeof(SiDAQ::buffer_word) * m_rawData.size();

}


