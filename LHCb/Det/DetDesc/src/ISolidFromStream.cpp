

#include "DetDesc/ISolidFromStream.h"
#include "DetDesc/Solids.h" 
#include "DetDesc/SolidException.h" 
 
#include "GaudiKernel/StreamBuffer.h" 


ISolid* ISolidFromStream::operator() ( StreamBuffer& s ) const 
{
  ///
  std::string solidType; 
  ///
  s >> solidType ; 
  ///
  ISolid* solid = 0 ; 
  ///
  if      ( "SolidBox"          == solidType  ) { solid = new SolidBox          () ; s >> *solid ; return solid ; } 
  else if ( "SolidCons"         == solidType  ) { solid = new SolidCons         () ; s >> *solid ; return solid ; }
  else if ( "SolidSphere"       == solidType  ) { solid = new SolidSphere       () ; s >> *solid ; return solid ; } 
  else if ( "SolidTrap"         == solidType  ) { solid = new SolidTrap         () ; s >> *solid ; return solid ; } 
  else if ( "SolidTrd"          == solidType  ) { solid = new SolidTrd          () ; s >> *solid ; return solid ; }
  else if ( "SolidTubs"         == solidType  ) { solid = new SolidTubs         () ; s >> *solid ; return solid ; }
  ///
  else if ( "SolidIntersection" == solidType  ) { solid = new SolidIntersection () ; s >> *solid ; return solid ; }
  else if ( "SolidSubtraction"  == solidType  ) { solid = new SolidSubtraction  () ; s >> *solid ; return solid ; }
  else if ( "SolidUnion"        == solidType  ) { solid = new SolidUnion        () ; s >> *solid ; return solid ; }
  ///
  throw SolidException("ISolidFromStream: could not create solid of type="+solidType );
  ///
  return solid ; 
  ///
};
