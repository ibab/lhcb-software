//$Id: ReadOutInfo.cpp,v 1.1 2001-12-13 19:19:15 andreav Exp $

#include "ReadOutInfo.h"

//---------------------------------------------------------------------------

/// Constructor
ReadOutInfo::ReadOutInfo( IDetectorElement* de,
			  const std::string& condition ) 
  : ConditionInfo( de, condition )
{
}; 

//---------------------------------------------------------------------------

/// Destructor
ReadOutInfo::~ReadOutInfo() 
{
};

//----------------------------------------------------------------------------
