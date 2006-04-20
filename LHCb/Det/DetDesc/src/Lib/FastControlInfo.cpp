//$Id: FastControlInfo.cpp,v 1.2 2006-04-20 14:39:56 ranjard Exp $

#include "DetDesc/FastControlInfo.h"

//---------------------------------------------------------------------------

/// Constructor
FastControlInfo::FastControlInfo( IDetectorElement* de,
				  const std::string& condition ) 
  : ConditionInfo( de, condition )
{
}; 

//---------------------------------------------------------------------------

/// Destructor
FastControlInfo::~FastControlInfo() 
{
};

//----------------------------------------------------------------------------
