#include <iostream> 
#include <string> 

#include "DetDesc/SolidIntersection.h"
#include "DetDesc/SolidException.h"

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














