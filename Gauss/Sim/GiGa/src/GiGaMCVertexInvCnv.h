#ifndef         __GIGA_KINECONVERSION_GIGAMCVertexInvCNV_H__
#define         __GIGA_KINECONVERSION_GIGAMCVertexInvCNV_H__  1 

/// from STL 
#include <set>
#include <functional>

///
/// base class from GiGa
#include "GiGa/GiGaCnv.h" 
template <class TYPE> class CnvFactory;

#include "LHCbEvent/MCVertex.h"

class GiGaTrajectory;

/** @class GiGaMCVertexInvCnv GiGaMCVertexInvCnv.h GiGa/GiGaMCVertexInvCnv.h

    Converter of Geant4 trajectories into Gaudi MCVertexs

    @author  Vanya Belyaev
    @date    22/02/2001
*/


class GiGaMCVertexInvCnv: public GiGaCnv
{
  ///
  friend class CnvFactory<GiGaMCVertexInvCnv>; 
  /// 
protected: 
  /// Standard Constructor
  GiGaMCVertexInvCnv( ISvcLocator* Locator );
  /// Standard (virtual) destructor 
  virtual ~GiGaMCVertexInvCnv();
  ///
public:
  ///
  /// create Object 
  virtual StatusCode createObj     ( IOpaqueAddress* /* Address */, DataObject*&     /* Object  */) ;
  /// update Object 
  virtual StatusCode updateObj     ( IOpaqueAddress* /* Address */, DataObject*      /* Object  */) ; 
  /// fill the references 
  virtual StatusCode fillObjRefs   ( IOpaqueAddress* /* Address */, DataObject*      /* Object  */) ;
  /// update the references 
  virtual StatusCode updateObjRefs ( IOpaqueAddress* /* Address */, DataObject*      /* Object  */) ;
  /// Class ID for created object == class ID for this specific converter
  static const CLID&          classID();
  /// storage Type 
  static const unsigned char storageType() ; 
  ///
private: 
  ///
};


#endif   //     __GIGA_KINECONVERSION_GIGAMCVertexInvCNV_H__














