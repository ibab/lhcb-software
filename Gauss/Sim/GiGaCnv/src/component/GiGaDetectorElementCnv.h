/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.2  2001/07/15 20:45:09  ibelyaev
/// the package restructurisation
///
/// ===========================================================================
#ifndef    GIGA_GIGADETECTORELEMENT_H
#define    GIGA_GIGADETECTORELEMENT_H 1 
/// ===========================================================================
/// GiGaCnv 
#include "GiGaCnv/GiGaCnvBase.h"
///
template <class T> 
class CnvFactory;

/** @class GiGaDetectorElementCnv GiGaDetectorElementCnv.h 

    converter of DetectorElements into Geant4 Geometry tree  

    @author Vanya Belyaev
*/

class GiGaDetectorElementCnv: public GiGaCnvBase
{
  ///
  friend class CnvFactory<GiGaDetectorElementCnv>;
  ///
public:
  
  /** standard constructor 
   *  @param svc pointer to Service Locator 
   */
  GiGaDetectorElementCnv( ISvcLocator* svc );
  /// virtual destructor 
  virtual ~GiGaDetectorElementCnv();

  /** create the representation
   *  @param Object  pointer to data object 
   *  @param Address address 
   *  @return status code 
   */
  virtual StatusCode createRep( DataObject*     Object  , 
                                IOpaqueAddress*& Address ) ;
  /** update the  representation
   *  @param Object  pointer to data object 
   *  @param Address address 
   *  @return status code 
   */
  virtual StatusCode updateRep( DataObject*     Object  , 
                                IOpaqueAddress*  Address ) ; 

  /// class ID for converted objects
  static const CLID&         classID();
  /// storage Type 
  static const unsigned char storageType() ; 
  ///
};


#endif  // GIGA_GIGADETECTORELEMENT_H
