// $Id: GiGaLVolumeCnv.h,v 1.5 2002-01-22 18:24:44 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2001/08/12 17:24:53  ibelyaev
// improvements with Doxygen comments
//
// Revision 1.3  2001/07/24 11:13:56  ibelyaev
// package restructurization(III) and update for newer GiGa
//
// Revision 1.2  2001/07/15 20:45:11  ibelyaev
// the package restructurisation
// 
// ============================================================================
#ifndef     GIGA_GIGALVOLUMECNV_H
#define     GIGA_GIGALVOLUMECNV_H 1 
// ============================================================================
/// GiGa
#include "GiGaCnv/GiGaCnvBase.h"
#include "GiGaCnv/GiGaLeaf.h"
///
template <class T> 
class CnvFactory      ; ///< GaudiKernel
class G4LogicalVolume ; ///< Geant4 

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

  ///
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
// End 
// ============================================================================
#endif  /// GIGA_GIGALVOLUMECNV_H
// ============================================================================
