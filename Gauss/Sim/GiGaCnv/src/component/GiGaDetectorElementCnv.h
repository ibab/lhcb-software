// $Id: GiGaDetectorElementCnv.h,v 1.5 2002-01-22 18:24:43 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2001/08/12 17:24:52  ibelyaev
// improvements with Doxygen comments
//
// Revision 1.3  2001/07/24 11:13:55  ibelyaev
// package restructurization(III) and update for newer GiGa
//
// Revision 1.2  2001/07/15 20:45:09  ibelyaev
// the package restructurisation
//
// ============================================================================
#ifndef    GIGA_GIGADETECTORELEMENT_H
#define    GIGA_GIGADETECTORELEMENT_H 1 
// ============================================================================
// GiGaCnv 
#include "GiGaCnv/GiGaCnvBase.h"
#include "GiGaCnv/GiGaLeaf.h"
//
template <class T> 
class CnvFactory;

/** @class GiGaDetectorElementCnv GiGaDetectorElementCnv.h 
 *
 *  converter of DetectorElements into Geant4 Geometry tree  
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 */

class GiGaDetectorElementCnv: public GiGaCnvBase
{
  ///
  friend class CnvFactory<GiGaDetectorElementCnv>;
  ///
public:
  
  /** create the representation
   *  @param Object  pointer to data object 
   *  @param Address address 
   *  @return status code 
   */
  virtual StatusCode createRep
  ( DataObject*     Object  , 
    IOpaqueAddress*& Address ) ;

  /** update the  representation
   *  @param Object  pointer to data object 
   *  @param Address address 
   *  @return status code 
   */
  virtual StatusCode updateRep
  ( DataObject*     Object  , 
    IOpaqueAddress*  Address ) ; 
  
  /// class ID for converted objects
  static const CLID&         classID();

  /// storage Type 
  static const unsigned char storageType() ; 
  ///

protected: 
  
  /** standard constructor 
   *  @param svc pointer to Service Locator 
   */
  GiGaDetectorElementCnv( ISvcLocator* svc );

  /// virtual destructor 
  virtual ~GiGaDetectorElementCnv();

private: 
  
  GiGaDetectorElementCnv();
  GiGaDetectorElementCnv           ( const GiGaDetectorElementCnv& ) ;
  GiGaDetectorElementCnv& operator=( const GiGaDetectorElementCnv& ) ;
  
private:
  
  GiGaLeaf  m_leaf;

};


// ============================================================================
// End 
// ============================================================================
#endif  // GIGA_GIGADETECTORELEMENT_H
// ============================================================================
