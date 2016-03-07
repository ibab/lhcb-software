//$Id: CalibrationInfo.cpp,v 1.2 2006-04-20 14:39:56 ranjard Exp $

#include "DetDesc/CalibrationInfo.h"

//---------------------------------------------------------------------------

/// Constructor
CalibrationInfo::CalibrationInfo( IDetectorElement* de,
				  const std::string& condition ) 
  : ConditionInfo( de, condition )
{
}

//---------------------------------------------------------------------------

/// Destructor
CalibrationInfo::~CalibrationInfo() 
{
}

//----------------------------------------------------------------------------
