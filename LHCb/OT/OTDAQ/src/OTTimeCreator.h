// $Id: OTTimeCreator.h,v 1.8 2006-02-06 14:53:29 janos Exp $
#ifndef OTDAQ_OTTIMECREATOR_H 
#define OTDAQ_OTTIMECREATOR_H 1

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// Kernel
#include "Kernel/OTChannelID.h"

// Event
#include "Event/RawBank.h"
#include "Event/RawEvent.h"

//local
#include "Event/GolHeader.h"
#include "Event/DataWord.h"

// forward declarations
class DeOTDetector;
namespace LHCb 
{
  class OTTime;
}

/** @class OTTimeCreator OTTimeCreator.h OTDAQ/OTTimeCreator.h
 *  
 *
 *  @author Jacopo Nardulli
 *  @date   2004-06-06
 */

class OTTimeCreator : public GaudiAlgorithm {
public: 

  /// Standard constructor
  OTTimeCreator( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~OTTimeCreator( ); ///< Destructor
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
 
protected:

private:
  
  // From Raw to OTTime
  StatusCode raw2OTTime(int station, int layer, int quarter, int module, 
                        LHCb::DataWord dataWord, LHCb::OTTimes& times);

  /// Make the OTTimes
  StatusCode createTimes(const LHCb::OTChannelID aChan, LHCb::OTTimes& time);

  /// t0 calibration
  double correctedTime(const LHCb::OTChannelID aChan, 
                       const double unCorrectedTime) const;
  
  // Get address information 
  int getStrawID(int otisID, int channel);

  // job options
  std::string m_timeLocation;
  bool m_tofCorrection;                     ///< Time of Flight Correction
  std::vector<double> m_startReadOutGate;   ///< Start of readout gate
  int m_countsPerBX;                        ///< Counts per BX
  int  m_numberOfBX;                        ///< Number of BX
  double m_timePerBX;                       ///< Time Per BX
  DeOTDetector* m_tracker;                  ///< Pointer to XML geometry
};

#endif // OTDAQ_OTTIMECREATOR_H
