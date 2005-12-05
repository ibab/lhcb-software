// $Id: XmlLVolumeCnv.h,v 1.5 2005-12-05 18:06:48 jpalac Exp $

#ifndef DETDESCCNV_XMLCNVSVC_XMLLVOLUMECNV_H
#define DETDESCCNV_XMLCNVSVC_XMLLVOLUMECNV_H

// Include files
#include <string>
#include <deque>
#include <vector>

#include "Kernel/Transform3DTypes.h"

#include "DetDescCnv/XmlGenericCnv.h"
#include "DetDesc/CLIDLVolume.h"
#include "DetDesc/Solids.h"

// Forward and extern declarations
class ISolid;
class LVolume;
template <class TYPE> class CnvFactory;


/** @class XmlLVolumeCnv
 *
 * XML converter for Volumes
 *
 * @author Sebastien Ponce
 * @author Radovan Chytracek
 * @author Pere Mato
 */
class XmlLVolumeCnv : public XmlGenericCnv {
  
  /// Friend needed for instantiation
  friend class CnvFactory<XmlLVolumeCnv>;

 public:
  
  /**
   * accessor to the type of elements that this converter converts
   * @return the classID for this type
   */
  static const CLID& classID () { return CLID_LVolume; }
  
  
 protected:
  
  /**
   * Constructor for this converter
   * @param svcs a ISvcLocator interface to find services
   */
  XmlLVolumeCnv (ISvcLocator* svcs);
  
  /**
   * Default destructor
   */
  virtual ~XmlLVolumeCnv();
  
  /** This creates the transient representation of an object from the
   * DOMElement representing it, then fills it and process it.
   * Overrides the default method in XmlGenericCnv
   * @param element the DOMElement to be used to builds the object
   * @param refpObject the object to be built
   * @param address the address for this object
   * @return status depending on the completion of the call
   */
  virtual StatusCode internalCreateObj (xercesc::DOMElement* element,
                                        DataObject*& refpObject,
                                        IOpaqueAddress* address);


 private:

  /**
   * This encapsulates the parameters needed to build a PVolume
   */
  typedef struct _PVolumeItem {
    /// the name of the physical volume
    std::string physvolName;
    /**
     * a number to be added to the name if indexed is true
     * this is to differentiate two volumes with the same name inside
     * a parametrized physical volume
     */
    unsigned int tag;
    /// tells whether tag should be added to the name or not
    bool indexed;
    /// the name of the logical volume
    std::string logvolName;
    /// the transformation to place the volume
    Gaudi::Transform3D* transformation;
  } PVolumeItem;

  /**
   * This type defines a vector of PVolumeItem*. This is used from the
   * processing of parametrised volumes.
   */
  typedef std::deque<PVolumeItem*> PVolumes;

  /**
   * This is the encapsulation of a solid and a transformation to be
   * applied ot it
   */
  typedef struct _placedSolid {
    /// the solid
    ISolid* solid;
    /// the transformation
    Gaudi::Transform3D* transformation;
  } PlacedSolid;

  /**
   * this defines a vector of PlacedSolids
   */
  typedef std::deque<PlacedSolid> PlacedSolidList;

  /**
   * this builds the name of a PVolumeItem.
   * The name is either the physvolName field of the struct if indexed is false
   * or the physvolName field + ':' + the tag field if indexed is true.
   * @param pv the PVolumeItem
   * @return the corresponding name
   */
  std::string createPvName (PVolumeItem* pv);

  /**
   * says whether the given tag is denoting a simple solid
   * @param tag the tag to be tested
   * @result true if this denotes a simple solid
   */
  bool isSimpleSolid (const XMLCh* tag);

  /**
   * says whether the given tag is denoting a boolean solid
   * @param tag the tag to be tested
   * @result true if this denotes a boolean solid
   */
  bool isBooleanSolid (const XMLCh* tag);

  /**
   * says whether the given tag is denoting a solid
   * @param tag the tag to be tested
   * @result true if this denotes a solid
   */
  bool isSolid (const XMLCh* tag);

  /**
   * says whether the given tag is denoting a transformation
   * @param tag the tag to be tested
   * @result true if this denotes a transformation
   */
  bool isTransformation (const XMLCh* tag);

  /**
   * says whether the given tag is denoting a parametrized physical volume
   * @param tag the tag to be tested
   * @result true if this denotes a parametrized physical volume
   */
  bool isParamphysvol (const XMLCh* tag);

  /**
   * builds a string locating the element. When possible, this strings
   * gives the first named parent of element and the way to reach the
   * element from there.
   * @param element the element to locate
   * @result a locating string
   */
  std::string locateElement (xercesc::DOMElement* element);

  /**
   * deals with the xml tag /<physvol/>. Creates the corresponding C++ Object.
   * Take care that memory is allocated in case the return value is not null
   * and that the caller is responsible for deallocating it
   * @param element the DOM element representing the tag
   * @return the C++ Object
   */
  PVolumeItem* dealWithPhysvol (xercesc::DOMElement* element);

  /**
   * deals with the xml tags /<paramphysvol/>, /<paramphysvol2D/>, /<paramphysvol3D/>.
   * Instead of creating a C++ object corresponding to these parametrised
   * volumes, this method expands them in a vector of simple physical volumes.
   * The return value is a pointer on this vector or 0 if there is an
   * error in the tag definition. Take care that memory is allocated for
   * each physical volume inside the vector in case the return value is not
   * empty and that the caller is responsible for deallocating it
   * @param element the DOM element representing the tag
   * @return a vector of physical volumes
   */
  PVolumes* dealWithParamphysvol (xercesc::DOMElement* element);

  /**
   * deals with parametrized physical volumes of any dimension. The dimension
   * is given by nD.
   * Instead of creating a C++ object corresponding to these parametrised
   * volumes, this method expands them in a vector of simple physical volumes.
   * The return value is a pointer on this vector or 0 if there is an
   * error in the tag definition. Take care that memory is allocated for
   * each physical volume inside the vector in case the return value is not
   * empty and that the caller is responsible for deallocating it
   * @param element the DOM element representing the tag
   * @param nD      dimension of volume
   * @return a vector of physical volumes
   */
  PVolumes* dealWithParamphysvol (xercesc::DOMElement* element,
                                  unsigned int nD);

  /**
   * expands a given list of physical volumes. The expansion is done in 
   * n dimensions (n being the size of numbers and transformations). For a
   * given element, a n-dimensionnal array of elements is created whose
   * dimensions are given by numbers. Each element of this array is placed
   * using the translations given by transformations and is rotated using
   * the rotations given by transformations. Note that the rotations are
   * applied in a specific order : first dimension first then second one and
   * so on.
   * Note that a transformation may be null. It is supposed then to be the
   * identity. Note that null is returned if numbers and transformations
   * don't have the same size.
   * Take care that the first argument pointer may be deallocate and that
   * memory will be allocated for the return value. The deallocation of this
   * one is the responsability of the caller.
   * @param volumes the volumes to be expanded
   * @param numbers the number of copy of each volume in each dimension
   * @param transformations the transformations used to generate the volumes
   * in each dimension
   */
  PVolumes* expandParamPhysVol (PVolumes* volumes,
	                            std::vector<int> numbers,
								std::vector<Gaudi::Transform3D*> transformations);

  /**
   * deals with the xml tag /<surf/>. Returns a string giving the address of
   * the surface or 0 if the element does not correspond to the /<surf/>
   * tag.
   * @param element the DOM element representing the tag
   * @return the address of the surface
   */
  std::string dealWithSurf (xercesc::DOMElement* element);

  /**
   * deals with the xml tags /<box/>, /<trd/>, /<trap/>, /<cons/>, /<tubs/>,
   * /<sphere/>, /<union/>, /<subtraction/> and /<intersection/>.
   * Creates the corresponding
   * C++ Object or 0 if the element does not correspond to one of these
   * tags. Take care that memory is allocated in case the return value is
   * not null and that the caller is responsible for deallocating it
   * @param element the DOM element representing the tag
   * @return the C++ object
   */
  ISolid* dealWithSolid (xercesc::DOMElement* element);

  /**
   * deals with the xml tags /<union/>, /<subtraction/> and /<intersection/>.
   * Creates the corresponding C++ Object or 0 if the element does not
   * correspond to one of these tags. Take care that memory is allocated
   * in case the return value is not null and that the caller is responsible
   * for deallocating it
   * @param element the DOM element representing the tag
   * @return the C++ object
   */
  SolidBoolean* dealWithBoolean (xercesc::DOMElement* element);

  /**
   * deals with the children of a boolean operation. Builds a PlacedSolidList
   * out of them. Note that memory is allocated and that the caller is
   * responsible for its deallocation.
   * @param element the boolean solid node
   * @return the list of children, as a PlacedSolidList*
   */
  PlacedSolidList* dealWithBooleanChildren (xercesc::DOMElement* element);

  /**
   * deals with the xml tags /<box/>, /<cons/>, /<sphere/>, /<tubs/>, /<trd/> 
   * and /<trap/>.
   * Creates the corresponding C++ Object or 0 if the element does not
   * correspond to one of these tags. Take care that memory is allocated
   * in case the return value is not null and that the caller is responsible
   * for deallocating it
   * @param element the DOM element representing the tag
   * @return the C++ object
   */
  ISolid* dealWithSimpleSolid (xercesc::DOMElement* element);

  /**
   * deals with the xml tag /<box/>. Creates the corresponding C++ Object.
   * Take care that memory is allocated and that the caller is responsible
   * for deallocating it
   * @param element the DOM element representing the tag
   * @return the C++ object
   */
  SolidBox* dealWithBox (xercesc::DOMElement* element);

  /**
   * deals with the xml tag /<trd/>. Creates the corresponding C++ Object.
   * Take care that memory is allocated and that the caller is responsible
   * for deallocating it
   * @param element the DOM element representing the tag
   * @return the C++ object
   */
  SolidTrd* dealWithTrd (xercesc::DOMElement* element);

  /**
   * deals with the xml tag /<trap/>. Creates the corresponding C++ Object.
   * Take care that memory is allocated and that the caller is responsible
   * for deallocating it
   * @param element the DOM element representing the tag
   * @return the C++ object
   */
  SolidTrap* dealWithTrap (xercesc::DOMElement* element);

  /**
   * deals with the xml tag /<cons/>. Creates the corresponding C++ Object.
   * Take care that memory is allocated and that the caller is responsible
   * for deallocating it
   * @param element the DOM element representing the tag
   * @return the C++ object or 0 if an error occured
   * @return status depending on the completion of the call
   */
  SolidCons* dealWithCons (xercesc::DOMElement* element);

  /**
   * deals with the xml tag /<polycone/>. Creates the corresponding C++ Object.
   * Take care that memory is allocated and that the caller is responsible
   * for deallocating it
   * @param element the DOM element representing the tag
   * @return the C++ object or 0 if an error occured
   * @return status depending on the completion of the call
   */
  SolidPolycone* dealWithPolycone (xercesc::DOMElement* element);

  /**
   * deals with the xml tag /<tubs/>. Creates the corresponding C++ Object.
   * Take care that memory is allocated and that the caller is responsible
   * for deallocating it
   * @param element the DOM element representing the tag
   * @return the C++ object
   */
  SolidTubs* dealWithTubs (xercesc::DOMElement* element);

  /**
   * deals with the xml tag /<sphere/>. Creates the corresponding C++ Object.
   * Take care that memory is allocated and that the caller is responsible
   * for deallocating it
   * @param element the DOM element representing the tag
   * @return the C++ object
   */
  SolidSphere* dealWithSphere (xercesc::DOMElement* element);

  /**
   * deals with a transformation beginning at child number *index of the element
   * element. After processing, the index value is changed and points to
   * the next child of element.
   * The return value is either Gaudi::Transform3D::Identity in case there is no
   * transformation or the Gaudi::Transform3D corresponding to the DOM tree.
   * This method allocates memory. The deallocation is the responsability
   * of the caller. This method should never raise an exception.
   * @param element the parent element of the transformation
   * @param index of the first element of the transformation in the parent
   * @return the Hep transformation or 0 if an error occured
   */
  Gaudi::Transform3D* dealWithTransformation (xercesc::DOMElement* element,
                                          unsigned int* index);

  /**
   * deals with the xml tag transformation. Creates the corresponding
   * Gaudi::TranslationXYZ and allocates the corresponding memory. The deallocation
   * is the responsability of the caller
   * @param element the parent element of the transformation
   * @return the Hep transformation or 0 if an error occured
   */
  Gaudi::Transform3D* dealWithTransformation (xercesc::DOMElement* element);

  /**
   * deals with the xml tag /<posXYZ/>. Creates the corresponding Gaudi::TranslationXYZ
   * and allocates the corresponding memory. The deallocation is the
   * responsability of the caller
   * @param element the DOM element representing the tag
   * @return the corresponding Hep transformation or 0 if an error occured
   */
  Gaudi::Transform3D* dealWithPosXYZ (xercesc::DOMElement* element);

  /**
   * deals with the xml tag /<posRPhiZ/>. Creates the corresponding Gaudi::TranslationXYZ
   * and allocates the corresponding memory. The deallocation is the
   * responsability of the caller.
   * An XmlCnvException will be launched if the tag attribute r has a negative
   * value
   * @param element the DOM element representing the tag
   * @return the corresponding Hep transformation or 0 if an error occured
   */
  Gaudi::Transform3D* dealWithPosRPhiZ (xercesc::DOMElement* element);

  /**
   * deals with the xml tag /<posRThPhi/>. Creates the corresponding
   * Gaudi::TranslationXYZ and allocates the corresponding memory. The deallocation
   * is the responsability of the caller
   * An XmlCnvException will be launched if the tag attribute r has a negative
   * value
   * @param element the DOM element representing the tag
   * @return the corresponding Hep transformation or 0 if an error occured
   */
  Gaudi::Transform3D* dealWithPosRThPhi (xercesc::DOMElement* element);

  /**
   * deals with the xml tag /<rotXYZ/>. Creates the corresponding Gaudi::Rotation
   * and allocates the corresponding memory. The deallocation is the
   * responsability of the caller
   * @param element the DOM element representing the tag
   * @return the corresponding Hep transformation or 0 if an error occured
   */
  Gaudi::Transform3D* dealWithRotXYZ (xercesc::DOMElement* element);

  /**
   * deals with the xml tag /<rotAxis/>. Creates the corresponding Gaudi::Rotation
   * and allocates the corresponding memory. The deallocation is the
   * responsability of the caller
   * An XmlCnvException will be launched if the tag attribute theta has a
   * value outside the range 0 - 180 * degree or if the tag attribute phi
   * has a value outside the range 0 - 360 * degree
   * @param element the DOM element representing the tag
   * @return the corresponding Hep transformation or 0 if an error occured
   */
  Gaudi::Transform3D* dealWithRotAxis (xercesc::DOMElement* element);

private:

  // Constant strings for element and parameter names
  const XMLCh* unionString;
  const XMLCh* subtractionString;
  const XMLCh* intersectionString;
  const XMLCh* boxString;
  const XMLCh* trdString;
  const XMLCh* trapString;
  const XMLCh* consString;
  const XMLCh* polyconeString;
  const XMLCh* tubsString;
  const XMLCh* sphereString;
  const XMLCh* transformationString;
  const XMLCh* posXYZString;
  const XMLCh* posRPhiZString;
  const XMLCh* posRThPhiString;
  const XMLCh* rotXYZString;
  const XMLCh* rotAxisString;
  const XMLCh* paramphysvolString;
  const XMLCh* paramphysvol2DString;
  const XMLCh* paramphysvol3DString;
  const XMLCh* materialString;
  const XMLCh* magfieldString;
  const XMLCh* sensdetString;
  const XMLCh* nameString;
  const XMLCh* physvolString;
  const XMLCh* surfString;
  const XMLCh* logvolString;
  const XMLCh* numberString;
  const XMLCh* addressString;
  const XMLCh* outerRadiusMZString;
  const XMLCh* outerRadiusPZString;
  const XMLCh* innerRadiusMZString;
  const XMLCh* innerRadiusPZString;
  const XMLCh* outerRadiusString;
  const XMLCh* innerRadiusString;
  const XMLCh* sizeXString;
  const XMLCh* sizeX1String;
  const XMLCh* sizeX2String;
  const XMLCh* sizeX3String;
  const XMLCh* sizeX4String;
  const XMLCh* sizeYString;
  const XMLCh* sizeY1String;
  const XMLCh* sizeY2String;
  const XMLCh* sizeZString;
  const XMLCh* thetaString;
  const XMLCh* phiString;
  const XMLCh* alp1String;
  const XMLCh* alp2String;
  const XMLCh* xString;
  const XMLCh* yString;
  const XMLCh* zString;
  const XMLCh* rString;
  const XMLCh* axThetaString;
  const XMLCh* axPhiString;
  const XMLCh* angleString;
  const XMLCh* zplaneString;
  const XMLCh* startThetaAngleString;
  const XMLCh* deltaThetaAngleString;
  const XMLCh* startPhiAngleString;
  const XMLCh* deltaPhiAngleString;
  const XMLCh* rotXString;
  const XMLCh* rotYString;
  const XMLCh* rotZString;

};

#endif // DETDESCCNV_XMLCNVSVC_XMLLVOLUMECNV_H
