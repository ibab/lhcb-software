//$Id: IReadOut.h,v 1.1 2001-12-13 19:20:29 andreav Exp $
#ifndef DETDESC_IREADOUT_H
#define DETDESC_IREADOUT_H 1

// Base class
#include "DetDesc/IConditionInfo.h"

///---------------------------------------------------------------------------
/** @class IReadOut IReadOut.h Det/DetDesc/IReadOut.h

    An abstract interface to get the readout information of
    a detector element.

    @author Andrea Valassi 
    @date December 2001
*///--------------------------------------------------------------------------

class IReadOut : virtual public IConditionInfo
{

};

#endif // DETDESC_IREADOUT_H




