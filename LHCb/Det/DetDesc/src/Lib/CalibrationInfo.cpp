//$Id: CalibrationInfo.cpp,v 1.1 2001-12-13 19:19:15 andreav Exp $

#include "CalibrationInfo.h"

//---------------------------------------------------------------------------

/// Constructor
CalibrationInfo::CalibrationInfo( IDetectorElement* de,
				  const std::string& condition ) 
  : ConditionInfo( de, condition )
{
}; 

//---------------------------------------------------------------------------

/// Destructor
CalibrationInfo::~CalibrationInfo() 
{
};

//----------------------------------------------------------------------------
