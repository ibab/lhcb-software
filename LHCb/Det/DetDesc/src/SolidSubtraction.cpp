#include <iostream> 
#include <string> 


#include "DetDesc/SolidSubtraction.h"
#include "DetDesc/SolidException.h"


//
//
// constructor
SolidSubtraction::SolidSubtraction( const std::string& name  , 
				      ISolid*            First )
  : SolidBoolean( name , First )
{
  if( 0 == First ) { throw SolidException(" SolidSubtraction constructor with ISolid=NULL! "); }
};



//
//
// destructor 
SolidSubtraction::~SolidSubtraction(){}

//
//
//

