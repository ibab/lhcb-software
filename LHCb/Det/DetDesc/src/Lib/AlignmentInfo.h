//$Id: AlignmentInfo.h,v 1.2 2002-04-24 12:41:26 mato Exp $
#ifndef DETDESC_ALIGNMENTINFO_H
#define DETDESC_ALIGNMENTINFO_H 1

#include <string>

// Base classes
#include "ConditionInfo.h"
#include "DetDesc/IAlignment.h"

///---------------------------------------------------------------------------
/** @class AlignmentInfo AlignmentInfo.h Det/DetDesc/AlignmentInfo.h

    The simplest implementation of the IAlignment abstract interface,
    derived from the generic class ConditionInfo.

    @author Andrea Valassi 
    @date December 2001
*///--------------------------------------------------------------------------

class AlignmentInfo : public ConditionInfo,
                      virtual public IAlignment
{

 public: 
  
  /// Constructor
  AlignmentInfo( IDetectorElement* de,
		 const std::string& condition );

  /// Destructor 
  virtual ~AlignmentInfo();
  
};

#endif // DETDESC_ALIGNMENTINFO_H
