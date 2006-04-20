//$Id: SlowControlInfo.h,v 1.1 2006-04-20 14:39:56 ranjard Exp $
#ifndef DETDESC_SLOWCONTROLINFO_H
#define DETDESC_SLOWCONTROLINFO_H 1

#include <string>

// Base classes
#include "ConditionInfo.h"
#include "DetDesc/ISlowControl.h"

///---------------------------------------------------------------------------
/** @class SlowControlInfo SlowControlInfo.h Det/DetDesc/SlowControlInfo.h

    The simplest implementation of the ISlowControl abstract interface,
    derived from the generic class ConditionInfo.

    @author Andrea Valassi 
    @date December 2001
*///--------------------------------------------------------------------------

class SlowControlInfo: virtual public ConditionInfo,
                       virtual public ISlowControl
{

 public: 
  
  /// Constructor
  SlowControlInfo( IDetectorElement* de,
		   const std::string& condition );

  /// Destructor 
  virtual ~SlowControlInfo();
  
};

#endif // DETDESC_SLOWCONTROLINFO_H
