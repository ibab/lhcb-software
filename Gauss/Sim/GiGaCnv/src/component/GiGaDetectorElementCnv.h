// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Sim/GiGaCnv/src/component/GiGaDetectorElementCnv.h,v 1.1.1.1 2001-04-23 08:34:15 ibelyaev Exp $ 
#ifndef    GIGA_GIGADETECTORELEMENT_H
#define    GIGA_GIGADETECTORELEMENT_H 1 
/// GiGa 
#include "GiGaCnv/GiGaCnvBase.h"
///
template <class T> 
class CnvFactory;

/** @class GiGaDetectorElementCnv     GiGaDetectorElementCnv.h GiGaDetectorElementCnv.h

    converter of DetectorElements into Geant4 Geometry tree  

    @author Vanya Belyaev
*/

class GiGaDetectorElementCnv: public GiGaCnvBase
{
  ///
  friend class CnvFactory<GiGaDetectorElementCnv>;
  ///
public:
  ///
  GiGaDetectorElementCnv( ISvcLocator* );
  /// Standard (virtual) destructor 
  virtual ~GiGaDetectorElementCnv();
  ///
  /// create representation
  virtual StatusCode createRep( DataObject*     Object  , IOpaqueAddress*& Address ) ;
  /// Update representation 
  virtual StatusCode updateRep( DataObject*     Object  , IOpaqueAddress*  Address ) ; 
  ///
  /// class ID for converted objects
  static const CLID&         classID();
  /// storage Type 
  static const unsigned char storageType() ; 
  ///
};


#endif  // GIGA_GIGADETECTORELEMENT_H
