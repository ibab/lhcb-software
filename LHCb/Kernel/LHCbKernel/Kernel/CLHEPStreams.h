/**------------------------------------------------------------------------------
 *
 * Description: MsgStream operators of CLHEP classes used in LHCb Event
 *
 * CLHEPStreams.h defines additional operators to used classes from
 * <A HREF="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/index.html">CLHEP</A>
 *              
 * @author     Pavel Binko
 * @author     Markus Frank
 * @author     Vanya Belyaev
 *
 *----------------------------------------------------------------------------*/
// $Id: CLHEPStreams.h,v 1.2 2004-12-10 15:06:39 cattanem Exp $
#ifndef EVENT_CLHEPSTREAMS_H
#define EVENT_CLHEPSTREAMS_H 1


// Include files
#include "GaudiKernel/MsgStream.h"

// Forward declarations
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


/** HepPoint3D printout method to Gaudi MsgStream object
 *  @see MsgStream
 *  <A HREF="http://cern.ch/wwwasd/lhc++/clhep/doxygen/html/classHepPoint3D.html">class HepPoint3D</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi message stream
 *  @param value Reference to HepPoint3D structure
 *  @return reference to Gaudi message stream
 */
MsgStream& printHepPoint3D(MsgStream& s, const HepPoint3D& value );

/** HepPoint3D printout method to Gaudi MsgStream object
 *  @see MsgStream
 *  <A HREF="http://cern.ch/wwwasd/lhc++/clhep/doxygen/html/classHepPoint3D.html">class HepPoint3D</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi message stream
 *  @param value Reference to HepPoint3D structure
 *  @return reference to Gaudi message stream
 */
inline MsgStream& operator<<(MsgStream& s, const HepPoint3D& value )    {
  return (s.isActive()) ? printHepPoint3D(s,value) : s;
}

/** HepVector3D printout method to Gaudi MsgStream object
 *  @see MsgStream
 *  <A HREF="http://cern.ch/wwwasd/lhc++/clhep/doxygen/html/classHepVector3D.html">class HepVector3D</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi message stream
 *  @param value Reference to HepVector3D structure
 *  @return reference to Gaudi message stream
 */
MsgStream& printHepVector3D(MsgStream& s, const HepVector3D& value );

/** HepVector3D printout method to Gaudi MsgStream object
 *  @see MsgStream
 *  <A HREF="http://cern.ch/wwwasd/lhc++/clhep/doxygen/html/classHepVector3D.html">class HepVector3D</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi message stream
 *  @param value Reference to HepVector3D structure
 *  @return reference to Gaudi message stream
 */
inline MsgStream& operator<<(MsgStream& s, const HepVector3D& value )    {
  return (s.isActive()) ? printHepVector3D(s,value) : s;
}

/** HepLorentzVector printout method to Gaudi MsgStream object
 *  @see MsgStream
 *  <A HREF="http://cern.ch/wwwasd/lhc++/clhep/doxygen/html/classHepLorentzVector.html">class HepLorentzVector</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi message stream
 *  @param value Reference to HepLorentzVector structure
 *  @return reference to Gaudi message stream
 */
MsgStream& printHepLorentzVector(MsgStream& s, const HepLorentzVector& value );

/** HepLorentzVector printout method to Gaudi MsgStream object
 *  @see MsgStream
 *  <A HREF="http://cern.ch/wwwasd/lhc++/clhep/doxygen/html/classHepLorentzVector.html">class HepLorentzVector</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi message stream
 *  @param value Reference to HepLorentzVector structure
 *  @return reference to Gaudi message stream
 */
inline MsgStream& operator<<(MsgStream& s, const HepLorentzVector & value )  {
  return (s.isActive()) ? printHepLorentzVector(s,value) : s;
}

/** HepVector printout method to Gaudi MsgStream object
 *  @see MsgStream
 *  <A HREF="http://cern.ch/wwwasd/lhc++/clhep/doxygen/html/classHepVector.html">class HepVector</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi message stream
 *  @param value Reference to HepVector structure
 *  @return reference to Gaudi message stream
 */
MsgStream& printHepVector(MsgStream& s, const HepVector & value);

/** HepVector printout method to Gaudi MsgStream object
 *  @see MsgStream
 *  <A HREF="http://cern.ch/wwwasd/lhc++/clhep/doxygen/html/classHepVector.html">class HepVector</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi message stream
 *  @param value Reference to HepVector structure
 *  @return reference to Gaudi message stream
 */
inline MsgStream& operator<<(MsgStream& s, const HepVector & value )         {
  return (s.isActive()) ? printHepVector(s,value) : s;
}

/** HepSymMatrix printout method to Gaudi MsgStream object
 *  @see MsgStream
 *  <A HREF="http://cern.ch/wwwasd/lhc++/clhep/doxygen/html/classHepSymMatrix.html">class HepSymMatrix</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi message stream
 *  @param value Reference to HepSymMatrix structure
 *  @return reference to Gaudi message stream
 */
MsgStream& printHepSymMatrix(MsgStream& s, const HepSymMatrix & value);

/** HepSymMatrix printout method to Gaudi MsgStream object
 *  @see MsgStream
 *  <A HREF="http://cern.ch/wwwasd/lhc++/clhep/doxygen/html/classHepSymMatrix.html">class HepSymMatrix</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi message stream
 *  @param value Reference to HepSymMatrix structure
 *  @return reference to Gaudi message stream
 */
inline MsgStream& operator<<(MsgStream& s, const HepSymMatrix & value )         {
  return (s.isActive()) ? printHepSymMatrix(s,value) : s;
}

/** HepDiagMatrix printout method to Gaudi MsgStream object
 *  @see MsgStream
 *  <A HREF="http://cern.ch/wwwasd/lhc++/clhep/doxygen/html/classHepDiagMatrix.html">class HepDiagMatrix</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi message stream
 *  @param value Reference to HepDiagMatrix structure
 *  @return reference to Gaudi message stream
 */
MsgStream& printHepDiagMatrix(MsgStream& s, const HepDiagMatrix & value);

/** HepDiagMatrix printout method to Gaudi MsgStream object
 *  @see MsgStream
 *  <A HREF="http://cern.ch/wwwasd/lhc++/clhep/doxygen/html/classHepDiagMatrix.html">class HepDiagMatrix</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi message stream
 *  @param value Reference to HepDiagMatrix structure
 *  @return reference to Gaudi message stream
 */
inline MsgStream& operator<<(MsgStream& s, const HepDiagMatrix & value )         {
  return (s.isActive()) ? printHepDiagMatrix(s,value) : s;
}

/** HepMatrix printout method to Gaudi MsgStream object
 *  @see MsgStream
 *  <A HREF="http://cern.ch/wwwasd/lhc++/clhep/doxygen/html/classHepMatrix.html">class HepMatrix</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi message stream
 *  @param value Reference to HepMatrix structure
 *  @return reference to Gaudi message stream
 */
MsgStream& printHepMatrix(MsgStream& s, const HepMatrix & value);

/** HepMatrix printout method to Gaudi MsgStream object
 *  @see MsgStream
 *  <A HREF="http://cern.ch/wwwasd/lhc++/clhep/doxygen/html/classHepMatrix.html">class HepMatrix</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi message stream
 *  @param value Reference to HepMatrix structure
 *  @return reference to Gaudi message stream
 */
inline MsgStream& operator<<(MsgStream& s, const HepMatrix & value )         {
  return (s.isActive()) ? printHepMatrix(s,value) : s;
}


/** HepRotation printout method to Gaudi MsgStream object
 *  @see MsgStream
 *  <A HREF="http://cern.ch/wwwasd/lhc++/clhep/doxygen/html/classHepRotation.html">class HepRotation</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi message stream
 *  @param value Reference to HepRotation structure
 *  @return reference to Gaudi message stream
 */
MsgStream& printHepRotation(MsgStream& s, const HepRotation & value);

/** HepRotation printout method to Gaudi MsgStream object
 *  @see MsgStream
 *  <A HREF="http://cern.ch/wwwasd/lhc++/clhep/doxygen/html/classHepRotation.html">class HepRotation</A>
 *  @author Markus Frank Markus.Frank@cern.ch
 *
 *  @param s     Gaudi message stream
 *  @param value Reference to HepRotation structure
 *  @return reference to Gaudi message stream
 */
inline MsgStream& operator<<(MsgStream& s, const HepRotation & value )         {
  return (s.isActive()) ? printHepRotation(s,value) : s;
}

/** printout operator for class HepAxisAngle to Gaudi Message stream
 *  @see MsgStream
 *  <a href="http://cern.ch/wwwasd/lhc++/clhep/doxygen/html/classHepAxisAngle.html>class HepAxisAngle</a>
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @param ms Gaudi Message Stream
 *  @param aa reference to HepAxisAngle structure
 *  @return reference to Gaudi Message Stream
 */
MsgStream& printHepAxisAngle( MsgStream& ms, const HepAxisAngle& aa );

/** printout operator for class HepAxisAngle to Gaudi Message stream
 *  @see MsgStream
 *  <a href="http://cern.ch/wwwasd/lhc++/clhep/doxygen/html/classHepAxisAngle.html>class HepAxisAngle</a>
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @param ms Gaudi Message Stream
 *  @param aa reference to HepAxisAngle structure
 *  @return reference to Gaudi Message Stream
 */
inline MsgStream& operator<<( MsgStream& ms, const HepAxisAngle& aa) {
  return (ms.isActive()) ? printHepAxisAngle(ms,aa) : ms;
}


/** printout operator for class HepTransform3D to Gaudi Message stream
 *  @see MsgStream
 *  <a href="http://cern.ch/wwwasd/lhc++/clhep/doxygen/html/classHepTransform3D.html>class HepTransform3D</a>
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @param ms Gaudi Message Stream
 *  @param aa reference to HepAxisAngle structure
 *  @return reference to Gaudi Message Stream
 */
MsgStream& printHepTransform3D( MsgStream& ms , const HepTransform3D& aa );

/** printout operator for class HepTransform3D to Gaudi Message stream
 *  @see MsgStream
 *  <a href="http://cern.ch/wwwasd/lhc++/clhep/doxygen/html/classHepTransform3D.html>class HepTransform3D</a>
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @param ms Gaudi Message Stream
 *  @param aa reference to HepRotation structure
 *  @return reference to Gaudi Message Stream
 */
inline MsgStream&    operator<<( MsgStream& ms , const HepTransform3D& aa ) {
  return (ms.isActive()) ? printHepTransform3D(ms,aa) : ms;
}


#endif    // EVENT_CLHEPSTREAMS_H
