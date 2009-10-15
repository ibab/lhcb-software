// $Id: GiGaIsotopeCnv.h,v 1.5 2009-10-15 10:01:10 silviam Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2002/01/22 18:24:43  ibelyaev
//  Vanya: update for newer versions of Geant4 and Gaudi
//
// Revision 1.3  2001/08/12 17:24:53  ibelyaev
// improvements with Doxygen comments
//
// Revision 1.2  2001/07/15 20:45:10  ibelyaev
// the package restructurisation
// 
// ============================================================================
#ifndef      GIGACNV_GIGAIsotopeCnv_H
#define      GIGACNV_GIGAIsotopeCnv_H  1 
// ============================================================================
/// from STL 
#include <set>
/// base class from GiGaCnv
#include "GiGaCnv/GiGaCnvBase.h" 
#include "GiGaCnv/GiGaLeaf.h" 
///
template <class TYPE> 
class CnvFactory;
class Material;
class Mixture;
class Element; 
class Isotope; 

/** @class GiGaIsotopeCnv GiGaIsotopeCnv.h GiGa/GiGaIsotopeCnv.h
    
    Converter of Isotope class to Geant4 
    
    @author  Vanya Belyaev
*/

class GiGaIsotopeCnv: public GiGaCnvBase
{
  ///
  friend class CnvFactory<GiGaIsotopeCnv>; 
  ///
 protected: 
  /// Standard Constructor
  GiGaIsotopeCnv( ISvcLocator* );
  /// Standard (virtual) destructor 
  virtual ~GiGaIsotopeCnv();
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
  static unsigned char storageType() ; 
  ///
protected: 
  /// miscellaneous functions which performs the conversion itself
private: 
  ///
  GiGaIsotopeCnv(); /// no default constructor 
  GiGaIsotopeCnv( const GiGaIsotopeCnv& ); /// no copy
  GiGaIsotopeCnv& operator=( const GiGaIsotopeCnv& ); /// no assignment 
  ///
private:
  
  GiGaLeaf m_leaf;
  
};

// ============================================================================
// End 
// ============================================================================
#endif   //     __GIGA_GEOMCONVERSION_GIGAIsotopeCnv_H__
// ============================================================================



