// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.4  2002/01/22 18:24:43  ibelyaev
///  Vanya: update for newer versions of Geant4 and Gaudi
///
/// Revision 1.3  2001/08/12 17:24:52  ibelyaev
/// improvements with Doxygen comments
///
/// Revision 1.2  2001/07/15 20:45:10  ibelyaev
/// the package restructurisation
///
// ============================================================================
#ifndef         GIGACNV_GIGAElementCnv_H
#define         GIGACNV_GIGAElementCnv_H  1 
// ============================================================================
/// STL 
#include <set>
/// GiGaCnv
#include "GiGaCnv/GiGaCnvBase.h" 
#include "GiGaCnv/GiGaLeaf.h" 
///
template <class TYPE> 
class CnvFactory;
class Material;
class Mixture;
class Element; 
class Isotope; 

/** @class GiGaElementCnv GiGaElementCnv.h 
 *
 *  Converter of Element class to Geant4 
 *
 *  @author  Vanya Belyaev
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
  virtual StatusCode createRep
  ( DataObject*      Object  , 
    IOpaqueAddress*& Address ) ;
  
  /// Update representation 
  virtual StatusCode updateRep
  ( DataObject*      Object  , 
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
  GiGaElementCnv           (); /// no default constructor 
  GiGaElementCnv           ( const GiGaElementCnv& ); /// no copy
  GiGaElementCnv& operator=( const GiGaElementCnv& ); /// no assignment  
 ///
private:

  GiGaLeaf m_leaf;  

};

// ============================================================================
/// End 
// ============================================================================
#endif   ///< GIGACNV_GIGAElementCnv_H
// ============================================================================



