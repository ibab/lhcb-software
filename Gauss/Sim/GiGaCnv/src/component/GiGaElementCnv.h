// $header: $ 
#ifndef         GIGA_GIGAElementCnv_H
#define         GIGA_GIGAElementCnv_H  1 
/// STL 
#include <set>
/// GiGa
#include "GiGaCnv/GiGaCnvBase.h" 
///
template <class TYPE> 
class CnvFactory;
class Material;
class Mixture;
class Element; 
class Isotope; 

/** @class GiGaElementCnv GiGaElementCnv.h GiGa/GiGaElementCnv.h

    Converter of Element class to Geant4 

    @author  Vanya Belyaev
*/

class GiGaElementCnv: public GiGaCnvBase
{
  ///
  friend class CnvFactory<GiGaElementCnv>; 
  /// 
 protected: 
  /// Standard Constructor
  GiGaElementCnv( ISvcLocator* );
  /// Standard (virtual) destructor 
  virtual ~GiGaElementCnv();
  ///
 public:
  /// Create representation 
  virtual StatusCode createRep(DataObject*     Object  , IOpaqueAddress*& Address ) ;
  /// Update representation 
  virtual StatusCode updateRep(DataObject*     Object  , IOpaqueAddress*  Address ) ; 
  /// Class ID for created object == class ID for this specific converter
  static const CLID&          classID();
  /// storage Type 
  static const unsigned char storageType() ; 
  ///
 protected: 
  /// miscellaneous functions which performs the conversion itself
private: 
  ///  
  GiGaElementCnv           ()                       ; /// no default constructor 
  GiGaElementCnv           ( const GiGaElementCnv& ); /// no copy
  GiGaElementCnv& operator=( const GiGaElementCnv& ); /// no assignment  
 ///
};


#endif   //     __GIGA_GEOMCONVERSION_GIGAElementCnv_H__



