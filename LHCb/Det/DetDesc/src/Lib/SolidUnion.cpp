// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/Lib/SolidUnion.cpp,v 1.3 2001-03-13 11:58:08 mato Exp $

#include "DetDesc/SolidUnion.h"
#include "DetDesc/SolidException.h"

#include <iostream> 
#include <string> 

//
//
// constructor
SolidUnion::SolidUnion( const std::string& name  , 
                                      ISolid*            First )
  : SolidBoolean( name , First )
{
  if( 0 == First ) { throw SolidException(" SolidUnion constructor with ISolid=NULL! "); }
};

SolidUnion::SolidUnion()
  : SolidBoolean( "unnamed Union" , 0 )
{
};


//
//
// destructor 
SolidUnion::~SolidUnion(){}

//
//
//

