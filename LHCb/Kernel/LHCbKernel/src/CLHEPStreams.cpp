// $Id: CLHEPStreams.cpp,v 1.3 2004-12-10 15:06:39 cattanem Exp $
// Include files

// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"
// CLHEP Geometry 
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Geometry/Point3D.h"
// CLHEP Vector 
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/AxisAngle.h"
#include "CLHEP/Vector/Rotation.h"
// CLHEP Matrix
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/DiagMatrix.h"

// Local
#include "Kernel/CLHEPStreams.h"

static const int NUMBERS_PER_LINE = 6;

//------------------------------------------------------------------------------
MsgStream& printHepPoint3D(MsgStream& s, const HepPoint3D& value )         {
  s << "X=";   s.width(12);  s << value.x();
  s << " Y=";  s.width(12);  s << value.y();
  s << " Z=";  s.width(12);  s << value.z();
  return s;
}

//------------------------------------------------------------------------------
MsgStream& printHepVector3D(MsgStream& s, const HepVector3D& value )    {
  s << "X=";   s.width(12);  s << value.x();
  s << " Y=";  s.width(12);  s << value.y();
  s << " Z=";  s.width(12);  s << value.z();
  return s;
}

//------------------------------------------------------------------------------
MsgStream& printHepLorentzVector(MsgStream& s, const HepLorentzVector& value )   {
  s << "px=";   s.width(12);  s << value.px();
  s << " py=";  s.width(12);  s << value.py();
  s << " pz=";  s.width(12);  s << value.pz();
  s << " E=";   s.width(12);  s << value.e();
  return s;
}

//------------------------------------------------------------------------------
MsgStream& printHepVector(MsgStream& s, const HepVector& value)   {
  int cnt = 0, nrow = value.num_row();
  for( int i=1; i<=nrow; i++, cnt=0 ) {
    s << "[" << (i-1) << "]="; 
    s.width(12); 
    s << value(i) << " ";
    if ( (++cnt)%NUMBERS_PER_LINE == 0 )  s << endmsg;
  }
  return s;
}

//------------------------------------------------------------------------------
MsgStream& printHepSymMatrix(MsgStream& s, const HepSymMatrix & value)   {
  int cnt = 0, ncol = value.num_col();
  for( int i=1; i<=ncol; i++ ) {
    for( int j=1; j<=i; j++ ) {
      s << "[" << (i-1) << "," << (j-1) << "]="; 
      s.width(12); 
      s << value(i,j) << " ";
      if ( (++cnt)%NUMBERS_PER_LINE == 0 )  s << endmsg;
    }
    if ( (cnt%NUMBERS_PER_LINE) != 0 ) s << endmsg;
    cnt = 0;
  }
  return s;
}

//------------------------------------------------------------------------------
MsgStream& printHepDiagMatrix(MsgStream& s, const HepDiagMatrix& value)   {
  int   ncol = value.num_col();
  int   cnt = 0;
  for( int i=1; i<=ncol; i++ ) {
    s << "[" << (i-1) << "," << (i-1) << "]="; 
    s.width(12);
    s << value(i,i) << " ";
    if ( (++cnt)%NUMBERS_PER_LINE == 0 )  s << endmsg;
  }
  return s;
}

//------------------------------------------------------------------------------
MsgStream& printHepMatrix(MsgStream& s, const HepMatrix& value)    {
  int nrow = value.num_row();
  int ncol = value.num_col();
  int cnt = 0;
  for( int i=1; i<=nrow; i++ ) {
    for( int j=1; j<=ncol; j++ ) {
      s << "[" << (i-1) << "," << (j-1) << "]="; 
      s.width(12);
      s << value(i,j) << " ";
      if ( (++cnt)%NUMBERS_PER_LINE == 0 )  s << endmsg;
    }
    if ( (cnt%NUMBERS_PER_LINE) != 0 ) s << endmsg;
    cnt = 0;
  }
  return s;
}

//------------------------------------------------------------------------------
MsgStream& printHepRotation(MsgStream& ms, const HepRotation & aa)  {
  if( aa.isIdentity() ) { ms << "Identity rotation"; }
  else                  { ms << "Rotation: " << aa.axisAngle() ; }

  return ms ;
}

//------------------------------------------------------------------------------
MsgStream& printHepAxisAngle( MsgStream&    ms , const HepAxisAngle& aa ) {
  const HepVector3D axis( aa.axis() );
  if( 0 != aa.delta() )     {
      ms << "Axis/Angle: (" << axis.x   () 
         << ","             << axis.y   () 
         << ","             << axis.z   () 
         << ")/"            << aa.delta () / degree ;
  }
  else     { 
    ms << "identity rotation" ; 
  }
  return ms;
}

//------------------------------------------------------------------------------
MsgStream& printHepTransform3D( MsgStream&    ms , const HepTransform3D& aa )
{
  const HepRotation rotation    ( aa.getRotation    () );
  const HepVector3D translation ( aa.getTranslation () );
  //
  if      ( rotation.isIdentity() && translation == HepVector3D() )  {
    ms << "Identity transformation" << endreq ; 
  }
  else if (                          translation == HepVector3D() )  {
    ms << "Transformation: "        << rotation 
       << " Translation: identity " << endreq ;    
  }
  else  {
    ms << "Transformation: "        << rotation 
       << " Translation: " 
       << translation.x() << "," 
       << translation.y() << "," 
       << translation.z() << ") " ;    
  }
  return ms ;
}
