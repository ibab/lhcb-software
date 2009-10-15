// $Id: GiGaLVolumeCnv.h,v 1.8 2009-10-15 10:01:45 silviam Exp $ 
#ifndef     GIGA_GIGALVOLUMECNV_H
#define     GIGA_GIGALVOLUMECNV_H 1 

// Include files
// from GiGa
#include "GiGaCnv/GiGaCnvBase.h"
#include "GiGaCnv/GiGaLeaf.h"

// Forward declarations
template <class T> 
class CnvFactory;         ///< GaudiKernel
class G4LogicalVolume;    ///< Geant4 
class IDetectorElement;   ///< LHcb 
 
/** @class GiGaLVolumeCnv GiGaLVolumeCnv.h component/GiGaLVolumeCnv.h 
 *
 *  converter of LVolumes into Geant4 G4LogicalVolume 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @author Sajan Easo, Gloria Corti
 *  @date   Last modified: 2007-07-09
 */

class GiGaLVolumeCnv: public GiGaCnvBase
{
  friend class CnvFactory<GiGaLVolumeCnv>;

protected:

  /** standard constructor 
   *  @param Locator pointer to service locator 
   */
  GiGaLVolumeCnv( ISvcLocator* Locator );

  /// Standard (virtual) destructor 
  virtual ~GiGaLVolumeCnv();

public:
  
  /** create the representation]
   *  @param Object pointer to object 
   *  @param Address address 
   *  @return status code 
   */
  virtual StatusCode createRep( DataObject*     Object  , 
                                IOpaqueAddress*& Address );

  /** Update representation 
   *  @param Object pointer to object 
   *  @param Address address 
   *  @return status code 
   */
  virtual StatusCode updateRep( DataObject*     Object  , 
                                IOpaqueAddress* Address ); 
  
  /// class ID for converted objects
  static const CLID& classID();

  /// storage Type 
  static unsigned char storageType(); 

  /** Provide matrix transformation of physical volume in its mother reference
   *  system with corresponding values from associated detector element
   *  @param IPVolume pointer to physical volume
   *  @param Gaudi::Transform3D matrix transformation 
   *  @return status code
   */
  StatusCode transformWithAlignment( const IPVolume* pv, 
                                     Gaudi::Transform3D& resultMatrix );
  
  /** Method to find detector element(s) which are:
   *    - valid
   *    - has geometry
   *    - has logical volume
   *    - corresponds (by name) to a given logical volume
   *    - has AlignmentConditions
   *  It could become a separate utility function.
   *   
   *  @param some top-level detector element
   *  @param logical volume name
   *  @param output container
   *  @return int number of detector element for a given Logical Volume 
   *
   *  @author Vanya BELYAEV
   *  @date 2007-03-05
   * 
   */  
  int detElementByLVNameWithAlignment( const IDetectorElement* det, 
                                       const std::string& lvName,
                                       std::vector<const IDetectorElement*>& dets);

  /** Method to search in tree the detector element corresponding to a
   *  physical element and check how many physical volumes are associated to it
   *  @param std::string name of physical volume 
   *  @param std::vector<IDetectorElement*> pointers to detector elements
   *  @param int number of detector elements found
   *  @return int number of detector element for a given physical volume 
   */
  int findBestDetElemFromPVName( std::string pvName,
                                 std::vector<const IDetectorElement*> foundDe, 
                                 int& numDetElemFound ) ;

  /* Find the detector element path and fill the variable path.
   * If parent_path is not specified, it is retrieved recursively.
   * @param IDectorElement pointer to detector element of which to find the path
   * @param std::string path in TES of detector element
   * @param std::string path in TES of parent detetor element
   * @return StatusCode
   */
  StatusCode detector_element_support_path(const IDetectorElement *de, 
                                           std::string &path, 
                                           const std::string &parent_path= "");
  
  
private:

  GiGaLVolumeCnv(); /// no default constructor 
  GiGaLVolumeCnv( const GiGaLVolumeCnv& ); /// no copy
  GiGaLVolumeCnv& operator=( const GiGaLVolumeCnv& ); /// no assignment  

private:
  
  GiGaLeaf m_leaf ;   /// Leave in Store

};

#endif  /// GIGA_GIGALVOLUMECNV_H
