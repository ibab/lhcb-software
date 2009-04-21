#ifndef OTCALIBRATION_IOTCALIBRATIONIOTOOL_H
#define OTCALIBRATION_IOTCALIBRATIONIOTOOL_H

// Include files
// -------------
#include "GaudiKernel/IAlgTool.h"
class DeOTDetector ;

static const InterfaceID IID_IMCHitCollector( "IOTCalibrationIOTool", 1, 0 );

class IOTCalibrationIOTool  : virtual public IAlgTool 
{
public:  
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IMCHitCollector ; }
  
  virtual StatusCode writeToFile(const std::string& filename, bool combineChannelsInOTIS=false) const = 0 ;
  virtual StatusCode readFromFile(const std::string& filename) const = 0 ;
} ;

#endif // TRACKMCINTERFACES_IMCHITCOLLECTOR_H
