// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Ex/DetDescExample/src/DeVertexDetector.h,v 1.1.1.1 2001-03-13 15:11:14 cattaneb Exp $
#ifndef DDEXAMPLE_DEVERTEXDETECTOR_H
#define DDEXAMPLE_DEVERTEXDETECTOR_H 1


// Include files
#include "DetDesc/DetectorElement.h"


// External declarations
extern const CLID& CLID_DEVertex;


//------------------------------------------------------------------------------
//
// ClassName:   DeVertexDetector 
//  
// Description: Example of an especific detertor element class.
//
//------------------------------------------------------------------------------


class DeVertexDetector : public DetectorElement
{
public:
  // Constructors
  DeVertexDetector();
  DeVertexDetector( unsigned int stations );

  // Destructor
  ~DeVertexDetector();

  const CLID& clID() const {
    return classID();
  }

  // Retrieve reference to class identifier
  static const CLID& classID();


  // Get/Set number of stations
  unsigned int stations();
  void         setStations( unsigned int stations );

private:

  unsigned int m_stations;           /// Number of stations in Vertex detector
};

#endif    // DDEXAMPLE_DEVERTEXDETECTOR_H
