//$Id: ReadOutInfo.h,v 1.1 2006-04-20 14:39:56 ranjard Exp $
#ifndef DETDESC_READOUTINFO_H
#define DETDESC_READOUTINFO_H 1

#include <string>

// Base classes
#include "ConditionInfo.h"
#include "DetDesc/IReadOut.h"

///---------------------------------------------------------------------------
/** @class ReadOutInfo ReadOutInfo.h Det/DetDesc/ReadOutInfo.h

    The simplest implementation of the IReadOut abstract interface,
    derived from the generic class ConditionInfo.

    @author Andrea Valassi 
    @date December 2001
*///--------------------------------------------------------------------------

class ReadOutInfo: virtual public ConditionInfo,
                   virtual public IReadOut
{

 public: 
  
  /// Constructor
  ReadOutInfo( IDetectorElement* de,
	       const std::string& condition );

  /// Destructor 
  virtual ~ReadOutInfo();
  
};

#endif // DETDESC_READOUTINFO_H
