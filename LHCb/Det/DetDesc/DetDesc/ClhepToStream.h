#ifndef     DETDESC_CLHEP_TO_STREAM_H
#define     DETDESC_CLHEP_TO_STREAM_H 1 


#include "GaudiKernel/StreamBuffer.h"

///
/// small collection of output operators of some CLHEP objects to StreamBuffer 
///


///
/// Hep3Vector
///

#include "CLHEP/Vector/ThreeVector.h"                
inline StreamBuffer& operator<<( StreamBuffer& sb , const Hep3Vector& Vect ) 
{ return sb << Vect.x() << Vect.y() << Vect.z() ; }
inline StreamBuffer& operator>>( StreamBuffer& sb ,       Hep3Vector& Vect ) 
{ 
  double  x , y, z ;
  sb >> x >> y >> z ; 
  ///
  Vect.setX( x ) ; 
  Vect.setY( y ) ; 
  Vect.setZ( z ) ; 
  ///
  return sb; 
}

///
/// HepRotation
///

#include "CLHEP/Vector/Rotation.h"                
inline StreamBuffer& operator<<( StreamBuffer& sb , const HepRotation& Rot ) 
{
  Hep3Vector Axis  ; double     Angle ;
  Rot.getAngleAxis( Angle, Axis );
  return sb << Angle << Axis ; 
}
inline StreamBuffer& operator>>( StreamBuffer& sb ,       HepRotation& Rot ) 
{
  Hep3Vector Axis  ; double     Angle ;
  sb >> Angle >> Axis ; 
  Rot = HepRotation().rotate( Angle , Axis );
  return sb; 
}

///
/// HepTransform3D
///

#include "CLHEP/Geometry/Transform3D.h" 
inline StreamBuffer& operator<<( StreamBuffer& sb , const HepTransform3D& Tr ) 
{ return sb << Tr.getTranslation() << Tr.getRotation() ; }
inline StreamBuffer& operator>>( StreamBuffer& sb ,       HepTransform3D& Tr ) 
{ 
  HepRotation rot;
  Hep3Vector  pos;
  sb >> pos >> rot ; 
  Tr.setIdentity() ; 
  Tr = HepTransform3D( rot , pos ); 
  return sb; 
}



#endif  //  DETDESC_CLHEP_TO_STREAM_H





