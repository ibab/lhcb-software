//$Id: CalibrationInfo.h,v 1.1 2001-12-13 19:19:15 andreav Exp $
#ifndef DETDESC_CALIBRATIONINFO_H
#define DETDESC_CALIBRATIONINFO_H 1

#include <string>

// Base classes
#include "ConditionInfo.h"
#include "DetDesc/ICalibration.h"

///---------------------------------------------------------------------------
/** @class CalibrationInfo CalibrationInfo.h Det/DetDesc/CalibrationInfo.h

    The simplest implementation of the ICalibration abstract interface,
    derived from the generic class ConditionInfo.

    @author Andrea Valassi 
    @date December 2001
*///--------------------------------------------------------------------------

class CalibrationInfo: virtual public ConditionInfo,
                       virtual public ICalibration
{

 public: 
  
  /// Constructor
  CalibrationInfo( IDetectorElement* de,
		   const std::string& condition );

  /// Destructor 
  virtual ~CalibrationInfo();
  
};

#endif // DETDESC_CALIBRATIONINFO_H
