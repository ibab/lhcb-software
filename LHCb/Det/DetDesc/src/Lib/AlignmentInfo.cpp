//$Id: AlignmentInfo.cpp,v 1.1 2001-12-13 19:19:14 andreav Exp $

#include "AlignmentInfo.h"

//---------------------------------------------------------------------------

/// Constructor
AlignmentInfo::AlignmentInfo( IDetectorElement* de,
			      const std::string& condition ) 
  : ConditionInfo( de, condition )
{
}; 

//---------------------------------------------------------------------------

/// Destructor
AlignmentInfo::~AlignmentInfo() 
{
};

//----------------------------------------------------------------------------
