// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/component/XmlLVolumeCnv.h,v 1.3 2001-05-14 15:13:42 sponce Exp $

#ifndef DETDESC_XMLCNVSVC_XMLLVOLUMECNV_H
#define DETDESC_XMLCNVSVC_XMLLVOLUMECNV_H

// Include files
#include <string>
#include <vector>

#include "CLHEP/Geometry/Transform3D.h"

#include "DetDesc/XmlGenericCnv.h"
#include "DetDesc/CLIDLVolume.h"

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
   * This structure keeps temporary information for each phys. vol.
   * we have processed
   */
  typedef struct _PVitem {
    std::string m_pvName;
    std::string m_lvName;
    HepPoint3D  m_translation;
    HepRotation m_rotation;
  } PVitem;
  
  /**
   * This structure keeps temporary information for each solid
   * we have processed inside boolean solids
   */
  typedef struct _SolidItem {
    ISolid*         m_solid;
    HepPoint3D      m_translation;
    HepRotation     m_rotation;
  } SolidItem;

  /// Parameterized physical volume type
  typedef struct _ParamPV {
    unsigned long m_number;
    PVitem        m_initialPos;
    HepVector3D   m_stepTranslation;
    HepRotation   m_stepRotation;
  } ParamPV;
  
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
  virtual ~XmlLVolumeCnv() {}
  
  /** Creates the transient representation of an object from a DOM_Element.
   * Overrides the default method in XmlGenericCnv
   * @param element the DOM_Element to be used to builds the object
   * @param refpObject the object to be built
   * @return status depending on the completion of the call
   */
  virtual StatusCode i_createObj (DOM_Element element,
                                  DataObject*& refpObject);

  /** Fills the current object for its child element childElement.
   * Overrides the default method in XmlGenericCnv
   * @param element the child processed here
   * @param refpObject the object to be filled
   * @return status depending on the completion of the call
   */
  virtual StatusCode i_fillObj (DOM_Element childElement,
                                DataObject* refpObject);

  /** This processes the current object.
   * Overrides the default method in XmlGenericCnv
   * @param refpObject the object to be processed
   * @return status depending on the completion of the call
   */
  virtual StatusCode i_processObj (DataObject* refpObject);

  
 private:

  /**
   * deals with the xml tag <physvol>
   * @param childElement the DOM element representing the tag
   * @return status depending on the completion of the call
   */
  StatusCode dealWithPhysvol (DOM_Element childElement);

  /**
   * deals with the xml tag <paramphysvol>
   * @param childElement the DOM element representing the tag
   * @return status depending on the completion of the call
   */
  StatusCode dealWithParamphysvol (DOM_Element childElement);

  /**
   * deals with the xml tag <box>
   * @param childElement the DOM element representing the tag
   * @return status depending on the completion of the call
   */
  StatusCode dealWithBox (DOM_Element childElement);

  /**
   * deals with the xml tag <trd>
   * @param childElement the DOM element representing the tag
   * @return status depending on the completion of the call
   */
  StatusCode dealWithTrd (DOM_Element childElement);

  /**
   * deals with the xml tag <cons>
   * @param childElement the DOM element representing the tag
   * @return status depending on the completion of the call
   */
  StatusCode dealWithCons (DOM_Element childElement);

  /**
   * deals with the xml tag <tubs>
   * @param childElement the DOM element representing the tag
   * @return status depending on the completion of the call
   */
  StatusCode dealWithTubs (DOM_Element childElement);

  /**
   * deals with the xml tag <sphere>
   * @param childElement the DOM element representing the tag
   * @return status depending on the completion of the call
   */
  StatusCode dealWithSphere (DOM_Element childElement);

  /**
   * deals with the xml tags <union>, <subtraction> and <intersection>
   * @param childElement the DOM element representing the tag
   * @return status depending on the completion of the call
   */
  StatusCode dealWithBoolean (DOM_Element childElement);

  /**
   * deals with the xml tag <posXYZ>
   * @param childElement the DOM element representing the tag
   * @return status depending on the completion of the call
   */
  StatusCode dealWithPosXYZ (DOM_Element childElement);

  /**
   * deals with the xml tag <posRPhiZ>
   * @param childElement the DOM element representing the tag
   * @return status depending on the completion of the call
   */
  StatusCode dealWithPosRPhiZ (DOM_Element childElement);

  /**
   * deals with the xml tag <posRThPhi>
   * @param childElement the DOM element representing the tag
   * @return status depending on the completion of the call
   */
  StatusCode dealWithPosRThPhi (DOM_Element childElement);

  /**
   * deals with the xml tag <rotXYZ>
   * @param childElement the DOM element representing the tag
   * @return status depending on the completion of the call
   */
  StatusCode dealWithRotXYZ (DOM_Element childElement);

  /**
   * deals with the xml tag <rotAxis>
   * @param childElement the DOM element representing the tag
   * @return status depending on the completion of the call
   */
  StatusCode dealWithRotAxis (DOM_Element childElement);

  /**
   * deals with the xml tags <posXYZ>, <posRPhiZ>, <posRThPhi>, <rotXYZ>
   * and <rotAxis>
   * @param childElement the DOM element representing the tag
   * @return status depending on the completion of the call
   */
  void dealWithPosRotChildren      (DOM_Element element);

  /**
   * deals with the xml tags <box>, <cons>, <sphere>, <tubs>, <trd>
   * @param childElement the DOM element representing the tag
   * @return status depending on the completion of the call
   */
  void dealWithSimpleSolidChildren (DOM_Element element);

  /**
   * creates a 3D translation in cartesian view
   * @param x translation along the x axis
   * @param y translation along the y axis
   * @param z translation along the z axis
   */
  HepPoint3D doTranslation (std::string x,
                            std::string y,
                            std::string z);

  /**
   * creates a 3D translation in cylindrical view
   * @param r radial translation
   * @param phi translation angle
   * @param z translation along the z axis
   */
  HepPoint3D doRPhiZTranslation (std::string r,
                                 std::string phi,
                                 std::string z);

  /**
   * creates a 3D translation
   * @param r radial translation
   * @param theta translation angle along theta
   * @param phi translation angle along phi
   */
  HepPoint3D doRThPhiTranslation (std::string r,
                                  std::string theta,
                                  std::string phi);

  /**
   * creates a 3D rotation
   * @param rx rotation on the x axis
   * @param ry rotation on the y axis
   * @param rz rotation on the z axis
   */
  HepRotation doRotation (std::string rx,
                          std::string ry,
                          std::string rz);

  /**
   * creates a 3D rotation
   */
  HepRotation doAxisRotation (std::string axtheta,
                              std::string axphi,
                              std::string angle);
  
  /// Accessor to the transformation context
  inline std::string& transContext() { return m_transContext; }

  /// Accessor to the transformation context
  inline void setTransContext (std::string& context) {
    m_transContext = context;
  }

  /// Accessor to the transformation context
  inline void setTransContext (const std::string& context) {
    m_transContext = context;
  }
  
  /// Accessor to the transformation context
  inline void setTransContext (const char* context) {
    m_transContext = context;
  }

  /// Accessor to the transformation context
  inline void setTransContext (char* context) {
    m_transContext = context;
  }

  /// Remember context for figuring out what transformation is being found
  std::string m_transContext;

  /// Vector of physical volumes found
  std::vector<PVitem> m_pvstore;

  /// Simple solid found
  ISolid* m_solid;

  /// Needed for boolean volumes to keep all daughter solids found
  std::vector<SolidItem>   m_bstore;
  std::string              m_bsName;
  bool                     m_insideBoolean;

  /// Needed for a parameterized volume
  bool                     m_insideParameterized;
  std::vector<ParamPV>     m_ppvstore;

  /// connected surfaces
  std::vector<std::string> m_surfaces; 

};

#endif // DETDESC_XMLCNVSVC_XMLLVOLUMECNV_H
