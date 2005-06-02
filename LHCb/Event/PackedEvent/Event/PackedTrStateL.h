// $Id: PackedTrStateL.h,v 1.1.1.1 2005-06-02 08:54:21 cattanem Exp $
#ifndef EVENT_PACKEDTRSTATEL_H 
#define EVENT_PACKEDTRSTATEL_H 1

// Include files
#include "GaudiKernel/DataObject.h"
#include <vector>

/** @class PackedTrStateLs PackedTrStateL.h Event/PackedTrStateL.h
 *  DataObject containing a vector of packed TrStateLs
 *
 *  @author Olivier Callot
 *  @date   2005-03-15
 */
struct PackedTrStateL {
  /// Standard constructor
  PackedTrStateL( ) {}; 
  int key;
  int x;
  int y;
  int z;
  int tx;
  int ty;

  int cov_00;
  int cov_11;
  int cov_22;
  int cov_33;
  short int cov_10;
  short int cov_20;
  short int cov_21;
  short int cov_30;
  short int cov_31;
  short int cov_32;
};

static const CLID& CLID_PackedTrStateLs = 1500;

// Namespace for locations in TDS
namespace PackedTrStateLLocation {
  static const std::string& Default = "pDst/TrStatesL";
}

class PackedTrStateLs : public DataObject {
public:
  PackedTrStateLs() : DataObject()  {};
  virtual ~PackedTrStateLs()        {};

  virtual const CLID& clID() const { return PackedTrStateLs::classID(); }
  static  const CLID& classID()    { return CLID_PackedTrStateLs;       }

  void addEntry(  PackedTrStateL& obj ) { m_vect.push_back( obj ); }

  std::vector<PackedTrStateL>::const_iterator begin() const { return m_vect.begin(); }
  std::vector<PackedTrStateL>::const_iterator end()   const { return m_vect.end(); }

private:
  std::vector<PackedTrStateL> m_vect;
};

#endif // EVENT_PACKEDTRSTATEL_H
