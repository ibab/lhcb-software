#ifndef         __GIGA_GEOMCONVERSION_GIGAElementCnv_H__
#define         __GIGA_GEOMCONVERSION_GIGAElementCnv_H__  1 


///
/// from STL 
#include <set>

///
/// base class from GiGa
#include "GiGa/GiGaCnv.h" 
template <class TYPE> class CnvFactory;

///
/// from Gaudi
class Material;
class Mixture;
class Element; 
class Isotope; 

///
/// 


class GiGaElementCnv: public GiGaCnv
{
  
  ///
  ///
  ///

  friend class CnvFactory<GiGaElementCnv>; 

  ///
  ///
  ///
 
 protected: 
 
  ///
  /// Standard Constructor
  ///
  
  GiGaElementCnv( ISvcLocator* Locator );

  ///
  /// Standard (virtual) destructor 
  ///

  virtual ~GiGaElementCnv();

  ///
  ///
  ///
  
 public:

  ///
  /// Create representation 
  ///
  
  virtual StatusCode createRep(DataObject*     Object  , IOpaqueAddress*& Address ) ;
  
  ///
  /// Update representation 
  ///
  
  virtual StatusCode updateRep(DataObject*     Object  , IOpaqueAddress*  Address ) ; 
  
  ///
  /// Class ID for created object == class ID for this specific converter
  ///

  static const CLID&          classID();


  ///
  /// storage Type 
  ///

  static const unsigned char storageType() ; 

  ///
  ///
  ///

 protected: 

  ///
  /// miscellaneous functions which performs the conversion itself
  ///  

 private: 

  ///
};


#endif   //     __GIGA_GEOMCONVERSION_GIGAElementCnv_H__



