// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2001/07/15 20:45:12  ibelyaev
// the package restructurisation
//
// ============================================================================
#ifndef        GIGACNV_GIGAMixtureCnv_H
#define        GIGACNV_GIGAMixtureCnv_H  1 
// ============================================================================
/// from STL 
#include <set>
/// base class from GiGa
#include "GiGaCnv/GiGaCnvBase.h" 
///
template <class TYPE> 
class CnvFactory;
class Material;
class Mixture;
class Element; 
class Isotope; 

/** @class GiGaMixtureCnv GiGaMixtureCnv.h GiGa/GiGaMixtureCnv.h
    
    Converter of Mixture class to Geant4 
    
    @author  Vanya Belyaev
*/

class GiGaMixtureCnv: public GiGaCnvBase
{
  ///
  friend class CnvFactory<GiGaMixtureCnv>; 
  /// 
protected: 
  /// Standard Constructor
  GiGaMixtureCnv( ISvcLocator* );
  /// Standard (virtual) destructor 
  virtual ~GiGaMixtureCnv();
  ///
public:
  /// Create representation 
  virtual StatusCode createRep(DataObject*     Object  , 
                               IOpaqueAddress*& Address ) ;
  /// Update representation 
  virtual StatusCode updateRep(DataObject*     Object  , 
                               IOpaqueAddress*  Address ) ; 
  /// Class ID for created object == class ID for this specific converter
  static const CLID&          classID();
  /// storage Type 
  static const unsigned char storageType() ; 
  ///
protected: 
  /// miscellaneous functions which performs the conversion itself
private: 
  ///
  GiGaMixtureCnv(); /// no default constructor 
  GiGaMixtureCnv           ( const GiGaMixtureCnv& ); /// no copy
  GiGaMixtureCnv& operator=( const GiGaMixtureCnv& ); /// no assignment 
  ///
};


#endif   //     __GIGA_GEOMCONVERSION_GIGAMixtureCnv_H__



