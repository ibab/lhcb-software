//$Id: FastControlInfo.cpp,v 1.1 2001-12-13 19:19:15 andreav Exp $

#include "FastControlInfo.h"

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
