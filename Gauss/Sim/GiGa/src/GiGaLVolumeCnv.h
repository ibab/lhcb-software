#ifndef     GIGA_GIGALVOLUMECNV_H
#define     GIGA_GIGALVOLUMECNV_H 1 


#include "GiGa/GiGaCnv.h"


template <class T> class CnvFactory;

/** @class GiGaLVolumeCnv     GiGaLVolumeCnv.h GiGa/GiGaLVolumeCnv.h

    converter of LVolumes into Geant4 G4LogicalVolume 

    @author Vanya Belyaev
*/


class GiGaLVolumeCnv: public GiGaCnv
{
  ///
  friend class CnvFactory<GiGaLVolumeCnv>;
  ///
 public:
  ///
  GiGaLVolumeCnv( ISvcLocator* Locator );
  /// Standard (virtual) destructor 
  virtual ~GiGaLVolumeCnv();
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




#endif      GIGA_GIGALVOLUMECNV_H
