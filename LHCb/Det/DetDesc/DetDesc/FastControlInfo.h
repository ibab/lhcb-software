//$Id: FastControlInfo.h,v 1.1 2006-04-20 14:39:55 ranjard Exp $
#ifndef DETDESC_FASTCONTROLINFO_H
#define DETDESC_FASTCONTROLINFO_H 1

#include <string>

// Base classes
#include "ConditionInfo.h"
#include "DetDesc/IFastControl.h"

///---------------------------------------------------------------------------
/** @class FastControlInfo FastControlInfo.h Det/DetDesc/FastControlInfo.h

    The simplest implementation of the IFastControl abstract interface,
    derived from the generic class ConditionInfo.

    @author Andrea Valassi 
    @date December 2001
*///--------------------------------------------------------------------------

class FastControlInfo: virtual public ConditionInfo,
                       virtual public IFastControl
{

 public: 
  
  /// Constructor
  FastControlInfo( IDetectorElement* de,
		   const std::string& condition );

  /// Destructor 
  virtual ~FastControlInfo();
  
};

#endif // DETDESC_FASTCONTROLINFO_H
