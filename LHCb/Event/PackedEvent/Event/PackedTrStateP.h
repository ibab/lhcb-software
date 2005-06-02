// $Id: PackedTrStateP.h,v 1.1.1.1 2005-06-02 08:54:21 cattanem Exp $
#ifndef EVENT_PACKEDTRSTATEP_H 
#define EVENT_PACKEDTRSTATEP_H 1

// Include files
#include "GaudiKernel/DataObject.h"
#include <vector>

/** @class PackedTrStatePs PackedTrStateP.h Event/PackedTrStateP.h
 *  DataObject containing a vector of packed TrStatePs
 *
 *  @author Olivier Callot
 *  @date   2005-03-15
 */
struct PackedTrStateP {
  /// Standard constructor
  PackedTrStateP( ) {}; 
  int key;
  int x;
  int y;
  int z;
  int tx;
  int ty;
  int p;

  int cov_00;
  int cov_11;
  int cov_22;
  int cov_33;
  int cov_44;
  short int cov_10;
  short int cov_20;
  short int cov_21;
  short int cov_30;
  short int cov_31;
  short int cov_32;
  short int cov_40;
  short int cov_41;
  short int cov_42;
  short int cov_43;
};

static const CLID& CLID_PackedTrStatePs = 1501;

// Namespace for locations in TDS
namespace PackedTrStatePLocation {
  static const std::string& Default = "pDst/TrStatesP";
}

class PackedTrStatePs : public DataObject {
public:
  PackedTrStatePs() : DataObject()  {};
  virtual ~PackedTrStatePs()        {};

  virtual const CLID& clID() const { return PackedTrStatePs::classID(); }
  static  const CLID& classID()    { return CLID_PackedTrStatePs;       }

  void addEntry(  PackedTrStateP& obj ) { m_vect.push_back( obj ); }
  std::vector<PackedTrStateP>::const_iterator begin() const { return m_vect.begin(); }
  std::vector<PackedTrStateP>::const_iterator end()   const { return m_vect.end(); }

private:
  std::vector<PackedTrStateP> m_vect;
};
#endif // EVENT_PACKEDTRSTATEPS_H
