//$Id: IFastControl.h,v 1.1 2001-12-13 19:20:29 andreav Exp $
#ifndef DETDESC_IFASTCONTROL_H
#define DETDESC_IFASTCONTROL_H 1

// Base class
#include "DetDesc/IConditionInfo.h"

///---------------------------------------------------------------------------
/** @class IFastControl IFastControl.h Det/DetDesc/IFastControl.h

    An abstract interface to get the slow control information of
    a detector element.

    @author Andrea Valassi 
    @date December 2001
*///--------------------------------------------------------------------------

class IFastControl : virtual public IConditionInfo
{

};

#endif // DETDESC_IFASTCONTROL_H




