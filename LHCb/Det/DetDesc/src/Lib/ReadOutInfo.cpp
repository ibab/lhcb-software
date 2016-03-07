//$Id: ReadOutInfo.cpp,v 1.2 2006-04-20 14:39:56 ranjard Exp $

#include "DetDesc/ReadOutInfo.h"

//---------------------------------------------------------------------------

/// Constructor
ReadOutInfo::ReadOutInfo( IDetectorElement* de,
			  const std::string& condition ) 
  : ConditionInfo( de, condition )
{
}

//---------------------------------------------------------------------------

/// Destructor
ReadOutInfo::~ReadOutInfo() 
{
}

//----------------------------------------------------------------------------
