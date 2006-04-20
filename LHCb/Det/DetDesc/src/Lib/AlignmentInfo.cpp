//$Id: AlignmentInfo.cpp,v 1.2 2006-04-20 14:39:56 ranjard Exp $

#include "DetDesc/AlignmentInfo.h"

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
