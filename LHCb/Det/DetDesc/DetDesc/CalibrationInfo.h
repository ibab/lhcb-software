//$Id: CalibrationInfo.h,v 1.1 2006-04-20 14:39:55 ranjard Exp $
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

class CalibrationInfo: public ConditionInfo,
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
