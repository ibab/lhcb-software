// $Id: PackedTrack.h,v 1.1.1.1 2005-06-02 08:54:21 cattanem Exp $
#ifndef EVENT_PACKEDTRACK_H 
#define EVENT_PACKEDTRACK_H 1

// Include files
#include "GaudiKernel/DataObject.h"
#include <vector>

/** @class PackedTracks PackedTrack.h Event/PackedTrack.h
 *  DataObject containing a vector of packed TrStoredTracks
 *
 *  @author Olivier Callot
 *  @date   2005-03-15
 */

struct PackedTrack {
  PackedTrack()  {};  // empty constructor

  PackedTrack( const PackedTrack& c ) :  // copy constructor
    key( c.key), charge( c.charge ), error( c.error ),
    history( c.history ), measurements( c.measurements ), states( c.states ) {}  

  int key;
  char charge;
  char error;
  unsigned short history;
  std::vector<int> measurements;
  std::vector<int> states;
};

static const CLID& CLID_PackedTracks = 1502;

// Namespace for locations in TDS
namespace PackedTrackLocation {
  static const std::string& Default = "pDst/Tracks";
}

class PackedTracks : public DataObject {
public: 
  PackedTracks() : DataObject()  {};
  virtual ~PackedTracks()        {};

  virtual const CLID& clID() const { return PackedTracks::classID(); }
  static  const CLID& classID()    { return CLID_PackedTracks;       }

  void addEntry(  PackedTrack& obj ) { m_vect.push_back( obj ); }
  std::vector<PackedTrack>::const_iterator begin() const { return m_vect.begin(); }
  std::vector<PackedTrack>::const_iterator end()   const { return m_vect.end(); }

private:
  std::vector<PackedTrack> m_vect;
};

#endif // EVENT_PACKEDTRACK_H
