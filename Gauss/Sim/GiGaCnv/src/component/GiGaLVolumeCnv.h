// $Id: GiGaLVolumeCnv.h,v 1.6 2003-04-06 18:55:32 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef     GIGA_GIGALVOLUMECNV_H
#define     GIGA_GIGALVOLUMECNV_H 1 
// ============================================================================
/// GiGa
#include "GiGaCnv/GiGaCnvBase.h"
#include "GiGaCnv/GiGaLeaf.h"
///
template <class T> 
class CnvFactory        ; ///< GaudiKernel
class G4LogicalVolume   ; ///< Geant4 
 
/** @class GiGaLVolumeCnv GiGaLVolumeCnv.h
 *
 *  converter of LVolumes into Geant4 G4LogicalVolume 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 */

class GiGaLVolumeCnv: public GiGaCnvBase
{
  ///
  friend class CnvFactory<GiGaLVolumeCnv>;
  ///
protected:

  /** standard constructor 
   *  @param Locator pointer to service locator 
   */
  GiGaLVolumeCnv( ISvcLocator* Locator );

  /// Standard (virtual) destructor 
  virtual ~GiGaLVolumeCnv();

  ///
public:
  
  /** create the representation]
   *  @param Object pointer to object 
   *  @param Address address 
   *  @return status code 
   */
  virtual StatusCode createRep
  ( DataObject*     Object  , 
    IOpaqueAddress*& Address ) ;

  /** Update representation 
   *  @param Object pointer to object 
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
  
private:
  ///
  GiGaLVolumeCnv(); /// no default constructor 
  GiGaLVolumeCnv           ( const GiGaLVolumeCnv& ); /// no copy
  GiGaLVolumeCnv& operator=( const GiGaLVolumeCnv& ); /// no assignment  
  ///
private:
  
  GiGaLeaf m_leaf ;

};

// ============================================================================
// The END 
// ============================================================================
#endif  /// GIGA_GIGALVOLUMECNV_H
// ============================================================================
