// $Id: CLHEPStreams.cpp,v 1.1.1.1 2002-03-18 09:02:18 cattanem Exp $
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
// GaudiKernel
#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/MsgStream.h"
// Local
#include "Kernel/CLHEPStreams.h"

static const int NUMBERS_PER_LINE = 6;
//------------------------------------------------------------------------------

/** Input operator for class HepPoint3D to Gaudi StreamBuffer object
 *  @see StreamBuffer
 *  <A HREF="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Geometry/HepPoint3D.html">class HepPoint3D</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi stream
 *  @param value Reference to HepPoint3D structure
 *  @return reference to Gaudi stream
 */
StreamBuffer& operator>> ( StreamBuffer& s, HepPoint3D& value )     {
  float   x, y, z;
  s >> x >> y >> z;
  value.setX(x);
  value.setY(y);
  value.setZ(z);
  return s;
}

/** Output operator for class HepPoint3D to Gaudi StreamBuffer object
 *  @see StreamBuffer
 *  <A HREF="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Geometry/HepPoint3D.html">class HepPoint3D</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi stream
 *  @param value const reference to HepPoint3D structure
 *  @return reference to Gaudi stream
 */
StreamBuffer& operator<< ( StreamBuffer& s, const HepPoint3D& value)   {
  return s  << float(value.x()) << float(value.y()) << float(value.z());
}


/** HepPoint3D printout method to Gaudi MsgStream object
 *  @see MsgStream
 *  <A HREF="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Geometry/HepPoint3D.html">class HepPoint3D</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi message stream
 *  @param value Reference to HepPoint3D structure
 *  @return reference to Gaudi message stream
 */
MsgStream& printHepPoint3D(MsgStream& s, const HepPoint3D& value )         {
  s << "X=";   s.width(12);  s << value.x();
  s << " Y=";  s.width(12);  s << value.y();
  s << " Z=";  s.width(12);  s << value.z();
  return s;
}

//------------------------------------------------------------------------------

/** Input operator for class HepVector3D to Gaudi StreamBuffer object
 *  @see StreamBuffer
 *  <A HREF="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Geometry/HepVector3D.html">class HepVector3D</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi stream
 *  @param value Reference to HepVector3D structure
 *  @return reference to Gaudi stream
 */
StreamBuffer& operator>> ( StreamBuffer& s, HepVector3D& value )     {
  float   x, y, z;
  s  >> x >> y >> z;
  value.setX(x);
  value.setY(y);
  value.setZ(z);
  return s;
}

/** Output operator for class HepVector3D to Gaudi StreamBuffer object
 *  @see StreamBuffer
 *  <A HREF="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Geometry/HepVector3D.html">class HepVector3D</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi stream
 *  @param value const reference to HepVector3D structure
 *  @return reference to Gaudi stream
 */
StreamBuffer& operator<< ( StreamBuffer& s, const HepVector3D& value)   {
  return s  << float(value.x()) << float(value.y()) << float(value.z());
}

/** HepVector3D printout method to Gaudi MsgStream object
 *  @see MsgStream
 *  <A HREF="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Geometry/HepVector3D.html">class HepVector3D</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi message stream
 *  @param value Reference to HepVector3D structure
 *  @return reference to Gaudi message stream
 */
MsgStream& printHepVector3D(MsgStream& s, const HepVector3D& value )    {
  s << "X=";   s.width(12);  s << value.x();
  s << " Y=";  s.width(12);  s << value.y();
  s << " Z=";  s.width(12);  s << value.z();
  return s;
}

//------------------------------------------------------------------------------

/** Input operator for class HepLorentzVector to Gaudi StreamBuffer object
 *  @see StreamBuffer
 *  <A HREF="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Vector/HepLorentzVector.html">class HepLorentzVector</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi stream
 *  @param value Reference to HepLorentzVector structure
 *  @return reference to Gaudi stream
 */
StreamBuffer& operator>> ( StreamBuffer& s, HepLorentzVector& value )   {
  float px, py, pz, E;
  s  >> px >> py >> pz >> E;
  value.setPx(px);
  value.setPy(py);
  value.setPz(pz);
  value.setE(E);
  return s;
}

/** Output operator for class HepLorentzVector to Gaudi StreamBuffer object
 *  @see StreamBuffer
 *  <A HREF="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Vector/HepLorentzVector.html">class HepLorentzVector</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi stream
 *  @param value const reference to HepLorentzVector structure
 *  @return reference to Gaudi stream
 */
StreamBuffer& operator<< ( StreamBuffer& s, const HepLorentzVector& value) {
  return s  << float(value.px()) 
            << float(value.py())
            << float(value.pz())
            << float(value.e());
}

/** HepLorentzVector printout method to Gaudi MsgStream object
 *  @see MsgStream
 *  <A HREF="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Vector/HepLorentzVector.html">class HepLorentzVector</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi message stream
 *  @param value Reference to HepLorentzVector structure
 *  @return reference to Gaudi message stream
 */
MsgStream& printHepLorentzVector(MsgStream& s, const HepLorentzVector& value )   {
  s << "px=";   s.width(12);  s << value.px();
  s << " py=";  s.width(12);  s << value.py();
  s << " pz=";  s.width(12);  s << value.pz();
  s << " E=";   s.width(12);  s << value.e();
  return s;
}

//------------------------------------------------------------------------------

/** Input operator for class HepVector to Gaudi StreamBuffer object
 *  @see StreamBuffer
 *  <A HREF="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Matrix/HepVector.html">class HepVector</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi stream
 *  @param value Reference to HepVector structure
 *  @return reference to Gaudi stream
 */
StreamBuffer& operator>> ( StreamBuffer& s, HepVector & value )     {
  int   nrow;
  float _val;
  s >> nrow;
  if ( nrow != value.num_row() )  {
    HepVector v(nrow);
    value = v;
  }
  for ( int i = 1; i <= nrow; ++i)  {
    s >> _val;
    value(i) = _val;
  }
  return s;  
}

/** Output operator for class HepVector to Gaudi StreamBuffer object
 *  @see StreamBuffer
 *  <A HREF="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Matrix/HepVector.html">class HepVector</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi stream
 *  @param value const reference to HepVector structure
 *  @return reference to Gaudi stream
 */
StreamBuffer& operator<< ( StreamBuffer& s, const HepVector& value)    {
  int nrow = value.num_row();
  s << nrow;
  for ( int i = 1; i <= nrow; ++i)  {
    s << float(value(i));
  }
  return s;  
}

/** HepVector printout method to Gaudi MsgStream object
 *  @see MsgStream
 *  <A HREF="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Matrix/HepVector.html">class HepVector</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi message stream
 *  @param value Reference to HepVector structure
 *  @return reference to Gaudi message stream
 */
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

/** Input operator for class HepSymMatrix to Gaudi StreamBuffer object
 *  @see StreamBuffer
 *  <A HREF="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Matrix/HepSymMatrix.html">class HepSymMatrix</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi stream
 *  @param value Reference to HepSymMatrix structure
 *  @return reference to Gaudi stream
 */
StreamBuffer& operator>> ( StreamBuffer& s, HepSymMatrix & value )  {
  int nrow;
  float _val;
  s >> nrow;
  if ( nrow != value.num_row() )  {
    HepSymMatrix m(nrow);
    value = m;
  }
  for( int i=1; i<=nrow; i++ ) {
    for( int j=1; j<=i; j++ ) {
      s >> _val;
      value(i,j) = _val;
    }
  }
  return s;
}

/** Output operator for class HepSymMatrix to Gaudi StreamBuffer object
 *  @see StreamBuffer
 *  <A HREF="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Matrix/HepSymMatrix.html">class HepSymMatrix</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi stream
 *  @param value const reference to HepSymMatrix structure
 *  @return reference to Gaudi stream
 */
StreamBuffer& operator<< ( StreamBuffer& s, const HepSymMatrix& value) {
  int  nrow = value.num_row();
  s << nrow;
  for( int i=1; i<=nrow; i++ ) {
    for( int j=1; j<=i; j++ ) {
      s << float(value(i,j));
    }
  }
  return s;
}

/** HepSymMatrix printout method to Gaudi MsgStream object
 *  @see MsgStream
 *  <A HREF="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Geometry/HepSymMatrix.html">class HepSymMatrix</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi message stream
 *  @param value Reference to HepSymMatrix structure
 *  @return reference to Gaudi message stream
 */
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

/** Input operator for class HepDiagMatrix to Gaudi StreamBuffer object
 *  @see StreamBuffer
 *  <A HREF="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Matrix/HepDiagMatrix.html">class HepDiagMatrix</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi stream
 *  @param value Reference to HepDiagMatrix structure
 *  @return reference to Gaudi stream
 */
StreamBuffer& operator>> ( StreamBuffer& s, HepDiagMatrix & value )  {
  int nrow;
  float _val;
  s >> nrow;
  if ( nrow != value.num_row() )  {
    HepDiagMatrix m(nrow);
    value = m;
  }
  for( int i=1; i<=nrow; i++ ) {
    s >> _val;
    value(i,i) = _val;
  }
  return s;
}

/** Output operator for class HepDiagMatrix to Gaudi StreamBuffer object
 *  @see StreamBuffer
 *  <A HREF="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Matrix/HepDiagMatrix.html">class HepDiagMatrix</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi stream
 *  @param value const reference to HepDiagMatrix structure
 *  @return reference to Gaudi stream
 */
StreamBuffer& operator<< ( StreamBuffer& s, const HepDiagMatrix& value) {
  int  nrow = value.num_row();
  s << nrow;
  for( int i=1; i<=nrow; i++ ) {
    s << float(value(i,i));
  }
  return s;
}

/** HepDiagMatrix printout method to Gaudi MsgStream object
 *  @see MsgStream
 *  <A HREF="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Geometry/HepDiagMatrix.html">class HepDiagMatrix</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi message stream
 *  @param value Reference to HepDiagMatrix structure
 *  @return reference to Gaudi message stream
 */
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

/** Input operator for class HepMatrix to Gaudi StreamBuffer object
 *  @see StreamBuffer
 *  <A HREF="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Matrix/HepMatrix.html">class HepMatrix</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi stream
 *  @param value Reference to HepMatrix structure
 *  @return reference to Gaudi stream
 */
StreamBuffer& operator>> ( StreamBuffer& s, HepMatrix & value )   {
  int nrow, ncol;
  float _val;
  s >> nrow >> ncol;
  if ( nrow != value.num_row() ||
       ncol != value.num_col() )  {
    HepMatrix m(nrow,ncol);
    value = m;
  }
  for( int i=1; i<=nrow; i++ ) {
    for( int j=1; j<=ncol; j++ ) {
      s >> _val;
      value(i,j) = _val;
    }
  }
  return s;
}

/** Output operator for class HepMatrix to Gaudi StreamBuffer object
 *  @see StreamBuffer
 *  <A HREF="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Matrix/HepMatrix.html">class HepMatrix</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi stream
 *  @param value const reference to HepMatrix structure
 *  @return reference to Gaudi stream
 */
StreamBuffer& operator<< ( StreamBuffer& s, const HepMatrix& value) {
  int   nrow = value.num_row();
  int   ncol = value.num_col();
  s << nrow << ncol;
  for( int i=1; i<=nrow; i++ ) {
    for( int j=1; j<=ncol; j++ ) {
      s << float(value(i,j));
    }
  }
  return s;
}

/** HepMatrix printout method to Gaudi MsgStream object
 *  @see MsgStream
 *  <A HREF="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Geometry/HepMatrix.html">class HepMatrix</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi message stream
 *  @param value Reference to HepMatrix structure
 *  @return reference to Gaudi message stream
 */
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

/** output operator for class HepRotation to Gaudi StreamBuffer object
 *  @see StreamBuffer
 *  <a href="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Vector/include/HepRotation.html>class Rotation</a>
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @param sb Gaudi stream
 *  @param aa const reference to HepRotation structure
 *  @return reference to Gaudi stream
 */
StreamBuffer& operator<<( StreamBuffer& sb , const HepRotation& aa )  { 
  return sb << aa.axisAngle() ;
}

/** input operator for class HepRotation from Gaudi StreamBuffer object
 *  @see StreamBuffer
 *  <a href="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Vector/include/HepRotation.html>class Rotation</a>
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @param sb Gaudi stream
 *  @param aa reference to HepRotation structure
 *  @return reference to Gaudi stream
 */
StreamBuffer& operator>>( StreamBuffer& sb ,       HepRotation& aa )  {
  HepAxisAngle aux;
  sb >> aux ; aa.set( aux );
  return sb ; 
}

/** HepRotation printout method to Gaudi MsgStream object
 *  @see MsgStream
 *  <A HREF="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Vector/HepRotation.html">class HepRotation</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param ms    Gaudi message stream
 *  @param aa    Reference to HepRotation structure
 *  @return reference to Gaudi message stream
 */
MsgStream& printHepRotation(MsgStream& ms, const HepRotation & aa)  {
  if( aa.isIdentity() ) { ms << "Identity rotation"; }
  else                  { ms << "Rotation: " << aa.axisAngle() ; }
  ///
  return ms ;
}

//------------------------------------------------------------------------------

/** output operator for class HepAxisAngle to Gaudi StreamBuffer object
 *  @see StreamBuffer
 *  <a href="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Vector/include/AxisAngle_h.html>class HepAxisAngle</a>
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @param sb Gaudi stream
 *  @param aa const reference to HepAxisAngle structure
 *  @return reference to Gaudi stream
 */
StreamBuffer& operator<<( StreamBuffer& sb , const HepAxisAngle& aa ) {
  return sb << HepVector3D( aa.axis() ) << (float) aa.delta() ;
}

/** input operator for class HepAxisAngle from Gaudi StreamBuffer object
 *  @see StreamBuffer
 *  <a href="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Vector/include/AxisAngle_h.html>class HepAxisAngle</a>
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @param sb Gaudi stream
 *  @param aa reference to HepAxisAngle structure
 *  @return reference to Gaudi stream
 */
StreamBuffer& operator>>( StreamBuffer& sb ,       HepAxisAngle& aa )  {
  // de-serialize the axis
  HepVector3D axis  ; sb >> axis  ; aa.setAxis  ( axis  );
  // de-serialize the angle 
  float       delta ; sb >> delta ; aa.setDelta ( delta );
  return sb ;
}

/** printout operator for class HepAxisAngle to Gaudi Message stream
 *  @see StreamBuffer
 *  <a href="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Vector/include/AxisAngle_h.html>class HepAxisAngle</a>
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @param ms Gaudi Message Stream
 *  @param aa reference to HepAxisAngle structure
 *  @return reference to Gaudi Message Stream
 */
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
  ///
  return ms;
}

//------------------------------------------------------------------------------

/** output operator for class HepTransform3D to Gaudi StreamBuffer object
 *  @see StreamBuffer
 *  <a href="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Geometry/include/HepTransform3D.html>class HepTransform3D</a>
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @param sb Gaudi stream
 *  @param aa const reference to HepRotation structure
 *  @return reference to Gaudi stream
 */
StreamBuffer& operator<<( StreamBuffer& sb , const HepTransform3D& aa ) {
  return sb << aa.getRotation() << HepVector3D( aa.getTranslation() );
}

/** input operator for class HepTransform3D from Gaudi StreamBuffer object
 *  @see StreamBuffer
 *  <a href="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Geometry/include/HepTransform3D.html>class HepTransform3D</a>
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @param sb Gaudi stream
 *  @param aa reference to HepRotation structure
 *  @return reference to Gaudi stream
 */
StreamBuffer& operator>>( StreamBuffer& sb ,       HepTransform3D& aa )  {
  HepRotation rotation    ;
  sb >> rotation    ;
  HepVector3D translation ;
  sb >> translation ;
  const HepTransform3D aux( rotation , translation );
  aa = aux ;
  return sb ;
}

/** printout operator for class HepTransform3D to Gaudi Message stream
 *  @see StreamBuffer
 *  <a href="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Geometry/include/HepTransform3D.html>class HepTransform3D</a>
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @param ms Gaudi Message Stream
 *  @param aa reference to HepAxisAngle structure
 *  @return reference to Gaudi Message Stream
 */
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
  ///
  return ms ;
}
