//$Id: SlowControlInfo.cpp,v 1.1 2001-12-13 19:19:15 andreav Exp $

#include "SlowControlInfo.h"

//---------------------------------------------------------------------------

/// Constructor
SlowControlInfo::SlowControlInfo( IDetectorElement* de,
				  const std::string& condition ) 
  : ConditionInfo( de, condition )
{
}; 

//---------------------------------------------------------------------------

/// Destructor
SlowControlInfo::~SlowControlInfo() 
{
};

//----------------------------------------------------------------------------
