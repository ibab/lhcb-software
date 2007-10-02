// $Id: GiGaDetectorElementCnv.h,v 1.6 2007-10-02 13:14:51 gcorti Exp $ 
#ifndef    GIGA_GIGADETECTORELEMENT_H
#define    GIGA_GIGADETECTORELEMENT_H 1 

// Include files
// from GiGaCnv 
#include "GiGaCnv/GiGaCnvBase.h"
#include "GiGaCnv/GiGaLeaf.h"

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


#endif  // GIGA_GIGADETECTORELEMENT_H
// ============================================================================
