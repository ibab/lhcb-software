#ifndef    GIGA_GIGADETECTORELEMENT_H
#define    GIGA_GIGADETECTORELEMENT_H 1 


#include "GiGa/GiGaCnv.h"


template <class T> class CnvFactory;

/** @class GiGaDetectorElementCnv     GiGaDetectorElementCnv.h GiGaDetectorElementCnv.h

    converter of DetectorElements into Geant4 Geometry tree  

    @author Vanya Belyaev
*/


class GiGaDetectorElementCnv: public GiGaCnv
{
  ///
  friend class CnvFactory<GiGaDetectorElementCnv>;
  ///
 public:
  ///
  GiGaDetectorElementCnv( ISvcLocator* Locator );
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
