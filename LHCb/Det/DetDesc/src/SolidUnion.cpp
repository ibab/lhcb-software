#include <iostream> 
#include <string> 


#include "DetDesc/SolidUnion.h"
#include "DetDesc/SolidException.h"


//
//
// constructor
SolidUnion::SolidUnion( const std::string& name  , 
				      ISolid*            First )
  : SolidBoolean( name , First )
{
  if( 0 == First ) { throw SolidException(" SolidUnion constructor with ISolid=NULL! "); }
};



//
//
// destructor 
SolidUnion::~SolidUnion(){}

//
//
//

