#ifndef DETDESC_ICALIBRATION_H
#define DETDESC_ICALIBRATION_H 1

// Base class
#include "DetDesc/IConditionInfo.h"

///---------------------------------------------------------------------------
/** @class ICalibration ICalibration.h Det/DetDesc/ICalibration.h

    An abstract interface to get the calibration information of
    a detector element.

    @author Andrea Valassi 
    @date December 2001
*///--------------------------------------------------------------------------

class ICalibration : virtual public IConditionInfo
{

};

#endif // DETDESC_ICALIBRATION_H




