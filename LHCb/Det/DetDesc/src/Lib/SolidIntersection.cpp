// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/Lib/SolidIntersection.cpp,v 1.3 2001-03-13 11:58:08 mato Exp $

#include "DetDesc/SolidIntersection.h"
#include "DetDesc/SolidException.h"

#include <iostream> 
#include <string> 

//
// constructor
SolidIntersection::SolidIntersection( const std::string& name  , 
                                      ISolid*            First )
  : SolidBoolean( name , First )
{
  if( 0 == First ) { throw SolidException(" SolidIntersection constructor with ISolid=NULL! "); }
};


//
//
// destructor 
SolidIntersection::~SolidIntersection(){}

//
//
//


///
/// fictive default constructor 
///

SolidIntersection::SolidIntersection()
  : SolidBoolean( "unnamed Intersection" , 0 )
{};














