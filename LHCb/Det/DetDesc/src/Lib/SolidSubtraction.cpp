// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/Lib/SolidSubtraction.cpp,v 1.3 2001-03-13 11:58:08 mato Exp $


#include "DetDesc/SolidSubtraction.h"
#include "DetDesc/SolidException.h"

#include <iostream> 
#include <string> 

//
//
// constructor
SolidSubtraction::SolidSubtraction( const std::string& name  , 
                                      ISolid*            First )
  : SolidBoolean( name , First )
{
  if( 0 == First ) { throw SolidException(" SolidSubtraction constructor with ISolid=NULL! "); }
};

SolidSubtraction::SolidSubtraction()
  : SolidBoolean( "unnamed Subtraction" , 0 )
{
};

//
//
// destructor 
SolidSubtraction::~SolidSubtraction(){}

//
//
//

