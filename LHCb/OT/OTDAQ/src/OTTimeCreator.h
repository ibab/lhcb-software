// $Id: OTTimeCreator.h,v 1.3 2004-12-10 08:10:25 jnardull Exp $
#ifndef OTDAQ_OTTIMECREATOR_H 
#define OTDAQ_OTTIMECREATOR_H 1

// Include files
// from STL
#include <string>
#include <vector>
#include <map>

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/AlgFactory.h"

// Event
#include "Kernel/OTChannelID.h"
#include "Event/OTTime.h"
#include "Event/RawBuffer.h"
#include "Event/RawBank.h"
#include "Event/RawEvent.h"
#include "Event/DAQTypes.h"

// from Detector
#include "OTDet/DeOTDetector.h"

// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

//local
#include "Event/GolHeader.h"
#include "Event/DataWord.h"

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
  
   enum DataMasks {dataMask = 0x80000000};


  // From Raw to OTTime
  StatusCode raw2OTTime(int station, int layer, int quarter, int module, 
                        DataWord dataWord, OTTimes& times);

  /// Make the OTTimes
  StatusCode createTimes(const OTChannelID aChan, OTTimes& time);

  /// t0 calibration
  double correctedTime(const OTChannelID aChan, 
                       const double unCorrectedTime) const;
  
  // Get address information 
  int getStrawID(int otisID, int channel);

  // job options
  std::string m_RawEventLoc;
  std::string m_timeLocation;
  bool m_tofCorrection;                     ///< Time of Flight Correction
  std::vector<double> m_startReadOutGate;   ///< Start of readout gate
  int m_countsPerBX;                        ///< Counts per BX
  int  m_numberOfBX;                        ///< Number of BX
  double m_timePerBX;                       ///< Time Per BX
  DeOTDetector* m_tracker;                  ///< Pointer to XML geometry
};

#endif // OTDAQ_OTTIMECREATOR_H
