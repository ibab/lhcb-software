// $Id: PackedCollision.h,v 1.1.1.1 2005-06-02 08:54:21 cattanem Exp $
#ifndef EVENT_PACKEDCOLLISION_H 
#define EVENT_PACKEDCOLLISION_H 1

// Include files
#include "GaudiKernel/DataObject.h"
#include <vector>

/** @class PackedCollisions PackedCollision.h Event/PackedCollision.h
 *  DataObject containing a vector of packed Collisions
 *
 *  @author Olivier Callot
 *  @date   2005-03-18
 */

struct PackedCollision {
  int key;
  bool isSignal;
  int processType;
  int x;
  int y;
  int z;
};


static const CLID& CLID_PackedCollisions = 1512;

// Namespace for locations in TDS
namespace PackedCollisionLocation {
  static const std::string& Default = "pSim/Collisions";
}

class PackedCollisions : public DataObject {
public: 
  /// Standard constructor
  PackedCollisions( ) {}; 

  virtual ~PackedCollisions( ) {}; ///< Destructor
  virtual const CLID& clID() const { return PackedCollisions::classID(); }
  static  const CLID& classID()    { return CLID_PackedCollisions;       }

  void addEntry(  PackedCollision& obj ) { m_vect.push_back( obj ); }
  std::vector<PackedCollision>::const_iterator begin() const { return m_vect.begin(); }
  std::vector<PackedCollision>::const_iterator end()   const { return m_vect.end(); }

protected:

private:
  std::vector<PackedCollision> m_vect;
};
#endif // EVENT_PACKEDCOLLISION_H
