/**------------------------------------------------------------------------------
 *
 * Description: Streams operators of CLHEP classes used in LHCb Event
 *              (used in serialize() methods)
 *
 * CLHEPStreams.h defines additional oprators to used classes from
 * <A HREF="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/index.html">CLHEP</A>
 *              
 * @author     Pavel Binko
 * @author     Markus Frank
 * @author     Vanya Belyaev
 *
 *----------------------------------------------------------------------------*/
// $Id: CLHEPStreams.h,v 1.1.1.1 2002-03-18 09:02:18 cattanem Exp $
#ifndef EVENT_CLHEPSTREAMS_H
#define EVENT_CLHEPSTREAMS_H 1


// Include files
#include "GaudiKernel/MsgStream.h"

// Forward declarations
class StreamBuffer;
class HepPoint3D;
class HepVector3D;
class HepLorentzVector;
class HepVector;
class HepMatrix;
class HepSymMatrix;
class HepDiagMatrix;
class HepAxisAngle;
class HepRotation;
class HepTransform3D;

/** Input operator for class HepPoint3D to Gaudi StreamBuffer object
 *  @see StreamBuffer
 *  <A HREF="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Geometry/HepPoint3D.html">class HepPoint3D</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi stream
 *  @param value Reference to HepPoint3D structure
 *  @return reference to Gaudi stream
 */
StreamBuffer& operator>> ( StreamBuffer& s, HepPoint3D& value );

/** Output operator for class HepPoint3D to Gaudi StreamBuffer object
 *  @see StreamBuffer
 *  <A HREF="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Geometry/HepPoint3D.html">class HepPoint3D</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi stream
 *  @param value const reference to HepPoint3D structure
 *  @return reference to Gaudi stream
 */
StreamBuffer& operator<< ( StreamBuffer& s, const HepPoint3D& value);

/** HepPoint3D printout method to Gaudi MsgStream object
 *  @see MsgStream
 *  <A HREF="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Geometry/HepPoint3D.html">class HepPoint3D</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi message stream
 *  @param value Reference to HepPoint3D structure
 *  @return reference to Gaudi message stream
 */
MsgStream& printHepPoint3D(MsgStream& s, const HepPoint3D& value );

/** HepPoint3D printout method to Gaudi MsgStream object
 *  @see MsgStream
 *  <A HREF="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Geometry/HepPoint3D.html">class HepPoint3D</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi message stream
 *  @param value Reference to HepPoint3D structure
 *  @return reference to Gaudi message stream
 */
inline MsgStream& operator<<(MsgStream& s, const HepPoint3D& value )    {
  return (s.isActive()) ? printHepPoint3D(s,value) : s;
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
StreamBuffer& operator>> ( StreamBuffer& s, HepVector3D& value );

/** Output operator for class HepVector3D to Gaudi StreamBuffer object
 *  @see StreamBuffer
 *  <A HREF="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Geometry/HepVector3D.html">class HepVector3D</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi stream
 *  @param value const reference to HepVector3D structure
 *  @return reference to Gaudi stream
 */
StreamBuffer& operator<< ( StreamBuffer& s, const HepVector3D& value);

/** HepVector3D printout method to Gaudi MsgStream object
 *  @see MsgStream
 *  <A HREF="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Geometry/HepVector3D.html">class HepVector3D</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi message stream
 *  @param value Reference to HepVector3D structure
 *  @return reference to Gaudi message stream
 */
MsgStream& printHepVector3D(MsgStream& s, const HepVector3D& value );

/** HepVector3D printout method to Gaudi MsgStream object
 *  @see MsgStream
 *  <A HREF="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Geometry/HepVector3D.html">class HepVector3D</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi message stream
 *  @param value Reference to HepVector3D structure
 *  @return reference to Gaudi message stream
 */
inline MsgStream& operator<<(MsgStream& s, const HepVector3D& value )    {
  return (s.isActive()) ? printHepVector3D(s,value) : s;
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
StreamBuffer& operator>> ( StreamBuffer& s, HepLorentzVector& value );

/** Output operator for class HepLorentzVector to Gaudi StreamBuffer object
 *  @see StreamBuffer
 *  <A HREF="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Vector/HepLorentzVector.html">class HepLorentzVector</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi stream
 *  @param value const reference to HepLorentzVector structure
 *  @return reference to Gaudi stream
 */
StreamBuffer& operator<< ( StreamBuffer& s, const HepLorentzVector& value);

/** HepLorentzVector printout method to Gaudi MsgStream object
 *  @see MsgStream
 *  <A HREF="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Vector/HepLorentzVector.html">class HepLorentzVector</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi message stream
 *  @param value Reference to HepLorentzVector structure
 *  @return reference to Gaudi message stream
 */
MsgStream& printHepLorentzVector(MsgStream& s, const HepLorentzVector& value );

/** HepLorentzVector printout method to Gaudi MsgStream object
 *  @see MsgStream
 *  <A HREF="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Vector/HepLorentzVector.html">class HepLorentzVector</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi message stream
 *  @param value Reference to HepLorentzVector structure
 *  @return reference to Gaudi message stream
 */
inline MsgStream& operator<<(MsgStream& s, const HepLorentzVector & value )  {
  return (s.isActive()) ? printHepLorentzVector(s,value) : s;
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
StreamBuffer& operator>> ( StreamBuffer& s, HepVector & value );

/** Output operator for class HepVector to Gaudi StreamBuffer object
 *  @see StreamBuffer
 *  <A HREF="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Matrix/HepVector.html">class HepVector</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi stream
 *  @param value const reference to HepVector structure
 *  @return reference to Gaudi stream
 */
StreamBuffer& operator<< ( StreamBuffer& s, const HepVector& value);

/** HepVector printout method to Gaudi MsgStream object
 *  @see MsgStream
 *  <A HREF="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Geometry/HepVector.html">class HepVector</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi message stream
 *  @param value Reference to HepVector structure
 *  @return reference to Gaudi message stream
 */
MsgStream& printHepVector(MsgStream& s, const HepVector & value);

/** HepVector printout method to Gaudi MsgStream object
 *  @see MsgStream
 *  <A HREF="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Geometry/HepVector.html">class HepVector</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi message stream
 *  @param value Reference to HepVector structure
 *  @return reference to Gaudi message stream
 */
inline MsgStream& operator<<(MsgStream& s, const HepVector & value )         {
  return (s.isActive()) ? printHepVector(s,value) : s;
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
StreamBuffer& operator>> ( StreamBuffer& s, HepSymMatrix & value );

/** Output operator for class HepSymMatrix to Gaudi StreamBuffer object
 *  @see StreamBuffer
 *  <A HREF="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Matrix/HepSymMatrix.html">class HepSymMatrix</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi stream
 *  @param value const reference to HepSymMatrix structure
 *  @return reference to Gaudi stream
 */
StreamBuffer& operator<< ( StreamBuffer& s, const HepSymMatrix& value);

/** HepSymMatrix printout method to Gaudi MsgStream object
 *  @see MsgStream
 *  <A HREF="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Geometry/HepSymMatrix.html">class HepSymMatrix</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi message stream
 *  @param value Reference to HepSymMatrix structure
 *  @return reference to Gaudi message stream
 */
MsgStream& printHepSymMatrix(MsgStream& s, const HepSymMatrix & value);

/** HepSymMatrix printout method to Gaudi MsgStream object
 *  @see MsgStream
 *  <A HREF="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Geometry/HepSymMatrix.html">class HepSymMatrix</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi message stream
 *  @param value Reference to HepSymMatrix structure
 *  @return reference to Gaudi message stream
 */
inline MsgStream& operator<<(MsgStream& s, const HepSymMatrix & value )         {
  return (s.isActive()) ? printHepSymMatrix(s,value) : s;
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
StreamBuffer& operator>> ( StreamBuffer& s, HepDiagMatrix & value );

/** Output operator for class HepDiagMatrix to Gaudi StreamBuffer object
 *  @see StreamBuffer
 *  <A HREF="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Matrix/HepDiagMatrix.html">class HepDiagMatrix</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi stream
 *  @param value const reference to HepDiagMatrix structure
 *  @return reference to Gaudi stream
 */
StreamBuffer& operator<< ( StreamBuffer& s, const HepDiagMatrix& value);

/** HepDiagMatrix printout method to Gaudi MsgStream object
 *  @see MsgStream
 *  <A HREF="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Geometry/HepDiagMatrix.html">class HepDiagMatrix</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi message stream
 *  @param value Reference to HepDiagMatrix structure
 *  @return reference to Gaudi message stream
 */
MsgStream& printHepDiagMatrix(MsgStream& s, const HepDiagMatrix & value);

/** HepDiagMatrix printout method to Gaudi MsgStream object
 *  @see MsgStream
 *  <A HREF="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Geometry/HepDiagMatrix.html">class HepDiagMatrix</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi message stream
 *  @param value Reference to HepDiagMatrix structure
 *  @return reference to Gaudi message stream
 */
inline MsgStream& operator<<(MsgStream& s, const HepDiagMatrix & value )         {
  return (s.isActive()) ? printHepDiagMatrix(s,value) : s;
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
StreamBuffer& operator>> ( StreamBuffer& s, HepMatrix & value );

/** Output operator for class HepMatrix to Gaudi StreamBuffer object
 *  @see StreamBuffer
 *  <A HREF="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Matrix/HepMatrix.html">class HepMatrix</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi stream
 *  @param value const reference to HepMatrix structure
 *  @return reference to Gaudi stream
 */
StreamBuffer& operator<< ( StreamBuffer& s, const HepMatrix& value);

/** HepMatrix printout method to Gaudi MsgStream object
 *  @see MsgStream
 *  <A HREF="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Geometry/HepMatrix.html">class HepMatrix</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi message stream
 *  @param value Reference to HepMatrix structure
 *  @return reference to Gaudi message stream
 */
MsgStream& printHepMatrix(MsgStream& s, const HepMatrix & value);

/** HepMatrix printout method to Gaudi MsgStream object
 *  @see MsgStream
 *  <A HREF="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Geometry/HepMatrix.html">class HepMatrix</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi message stream
 *  @param value Reference to HepMatrix structure
 *  @return reference to Gaudi message stream
 */
inline MsgStream& operator<<(MsgStream& s, const HepMatrix & value )         {
  return (s.isActive()) ? printHepMatrix(s,value) : s;
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
StreamBuffer& operator<<( StreamBuffer& sb , const HepRotation& aa );

/** input operator for class HepRotation from Gaudi StreamBuffer object
 *  @see StreamBuffer
 *  <a href="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Vector/include/HepRotation.html>class Rotation</a>
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @param sb Gaudi stream
 *  @param aa reference to HepRotation structure
 *  @return reference to Gaudi stream
 */
StreamBuffer& operator>>( StreamBuffer& sb ,       HepRotation& aa );

/** HepRotation printout method to Gaudi MsgStream object
 *  @see MsgStream
 *  <A HREF="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Vector/HepRotation.html">class HepRotation</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi message stream
 *  @param value Reference to HepRotation structure
 *  @return reference to Gaudi message stream
 */
MsgStream& printHepRotation(MsgStream& s, const HepRotation & value);

/** HepRotation printout method to Gaudi MsgStream object
 *  @see MsgStream
 *  <A HREF="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Vector/HepRotation.html">class HepRotation</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi message stream
 *  @param value Reference to HepRotation structure
 *  @return reference to Gaudi message stream
 */
inline MsgStream& operator<<(MsgStream& s, const HepRotation & value )         {
  return (s.isActive()) ? printHepRotation(s,value) : s;
}

//------------------------------------------------------------------------------

/** output operator for class HepAxisAngle to Gaudi StreamBuffer object
 *  @see StreamBuffer
 *  <a href="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Geometry/include/HepAxisAngle.html>class HepAxisAngle</a>
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @param sb Gaudi stream
 *  @param aa const reference to HepAxisAngle structure
 *  @return reference to Gaudi stream
 */
StreamBuffer& operator<<( StreamBuffer& sb, const HepAxisAngle& aa );

/** input operator for class HepAxisAngle from Gaudi StreamBuffer object
 *  @see StreamBuffer
 *  <a href="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Geometry/include/HepAxisAngle.html>class HepAxisAngle</a>
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @param sb Gaudi stream
 *  @param aa reference to HepAxisAngle structure
 *  @return reference to Gaudi stream
 */
StreamBuffer& operator>>( StreamBuffer& sb, HepAxisAngle& aa );

/** printout operator for class HepAxisAngle to Gaudi Message stream
 *  @see StreamBuffer
 *  <a href="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Geometry/include/HepAxisAngle.html>class HepAxisAngle</a>
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @param ms Gaudi Message Stream
 *  @param aa reference to HepAxisAngle structure
 *  @return reference to Gaudi Message Stream
 */
MsgStream& printHepAxisAngle( MsgStream& ms, const HepAxisAngle& aa );

/** printout operator for class HepAxisAngle to Gaudi Message stream
 *  @see StreamBuffer
 *  <a href="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Geometry/include/HepAxisAngle.html>class HepAxisAngle</a>
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @param ms Gaudi Message Stream
 *  @param aa reference to HepAxisAngle structure
 *  @return reference to Gaudi Message Stream
 */
inline MsgStream& operator<<( MsgStream& ms, const HepAxisAngle& aa) {
  return (ms.isActive()) ? printHepAxisAngle(ms,aa) : ms;
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
StreamBuffer& operator<<( StreamBuffer& sb , const HepTransform3D& aa );

/** input operator for class HepTransform3D from Gaudi StreamBuffer object
 *  @see StreamBuffer
 *  <a href="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Geometry/include/HepTransform3D.html>class HepTransform3D</a>
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @param sb Gaudi stream
 *  @param aa reference to HepRotation structure
 *  @return reference to Gaudi stream
 */
StreamBuffer& operator>>( StreamBuffer& sb ,       HepTransform3D& aa );

/** printout operator for class HepTransform3D to Gaudi Message stream
 *  @see StreamBuffer
 *  <a href="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Geometry/include/HepTransform3D.html>class HepTransform3D</a>
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @param ms Gaudi Message Stream
 *  @param aa reference to HepAxisAngle structure
 *  @return reference to Gaudi Message Stream
 */
MsgStream& printHepTransform3D( MsgStream& ms , const HepTransform3D& aa );

/** printout operator for class HepTransform3D to Gaudi Message stream
 *  @see MsgStream
 *  <a href="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/Geometry/include/HepTransform3D.html>class HepTransform3D</a>
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @param ms Gaudi Message Stream
 *  @param aa reference to HepRotation structure
 *  @return reference to Gaudi Message Stream
 */
inline MsgStream&    operator<<( MsgStream& ms , const HepTransform3D& aa ) {
  return (ms.isActive()) ? printHepTransform3D(ms,aa) : ms;
}


#endif    // EVENT_CLHEPSTREAMS_H
