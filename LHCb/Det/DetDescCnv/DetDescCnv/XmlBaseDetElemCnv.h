///  $Id: XmlBaseDetElemCnv.h,v 1.6 2009-05-04 14:57:08 ocallot Exp $

#ifndef DETDESCCNV_XMLDETELEMCNV_H
#define DETDESCCNV_XMLDETELEMCNV_H

/// Include files
#include "DetDescCnv/XmlGenericCnv.h"
#include "DetDesc/DetectorElement.h"

#include "GaudiKernel/Converter.h"

/// Forward and external declarations
class     ISvcLocator;
template <class TYPE> class CnvFactory;


/** @class XmlBaseDetElemCnv XmlBaseDetElemCnv.h DetDescCnv/XmlBaseDetElemCnv.h
 *
 * Generic XML converter for DetectorElements. It provides support for basic
 * DetectorElements and should be the ancester of every specific DetectorElement
 * converter.
 *
 * @author Sebastien Ponce
 * @author Radovan Chytracek
 * @author Pere Mato
 */
class XmlBaseDetElemCnv : public XmlGenericCnv {

public:
  
  /**
   * Initializes the converter - Overrides the default method in XmlGenericCnv
   * @return status depending on the completion of the call
   */
  virtual StatusCode initialize();
  
  /**
   * accessor to the type of elements that this converter converts
   * @return the classID for this type
   */
  static const CLID& classID();
  
protected:
  
  /**
   * Constructor for this converter
   * @param svcs a ISvcLocator interface to find services
   */
  XmlBaseDetElemCnv(ISvcLocator* svcs);

  /**
   * Constructor for this converter
   * @param svc   a ISvcLocator interface to find services
   * @param clsID the type of object the converter is able to convert
   */
  XmlBaseDetElemCnv(ISvcLocator* svc, const CLID& clsID );

  /**
   * Default destructor
   */
  virtual ~XmlBaseDetElemCnv();

  /**
   * Resolves the references of the created transient object.
   */
  virtual StatusCode fillObjRefs(IOpaqueAddress* pAddress, DataObject* pObject);

  /** Creates the transient representation of an object from a DOMElement.
   * Overrides the default method in XmlGenericCnv
   * @param element the DOMElement to be used to builds the object
   * @param refpObject the object to be built
   * @return status depending on the completion of the call
   */
  virtual StatusCode i_createObj (xercesc::DOMElement* element,
                                  DataObject*& refpObject);

  using XmlGenericCnv::i_fillObj;
  /** Fills the current object for its child element childElement.
   * Overrides the default method in XmlGenericCnv
   * @param childElement the child processed here
   * @param refpObject the object to be filled
   * @param address the address for this object
   * @return status depending on the completion of the call
   */
  virtual StatusCode i_fillObj (xercesc::DOMElement* childElement,
                                DataObject* refpObject,
                                IOpaqueAddress* address);

  /** This fills the current object for specific child.
   * Specific children are children of children \<specific\>
   * of the \<detelem\> tag. This is typically used for specializing
   * detector elements and define new properties.
   * This will be called for each specific child of the current object
   * @param childElement the specific child processed here
   * @param refpObject the object to be filled
   * @param address the address for this object
   * @return status depending on the completion of the call
   */
  virtual StatusCode i_fillSpecificObj (xercesc::DOMElement* childElement,
                                        DetectorElement* refpObject,
                                        IOpaqueAddress* address) = 0;


private:
  
  /// Whether to use the generic converter in case a specific one does not exist
  bool m_doGenericCnv;

private:
  // a silly method to init strings
  void initStrings();

  // Constant strings for element names
  const XMLCh* specificString;
  const XMLCh* detelemString;
  const XMLCh* detelemrefString;
  const XMLCh* versionString;
  const XMLCh* authorString;
  const XMLCh* geometryinfoString;
  const XMLCh* alignmentinfoString;
  const XMLCh* calibrationinfoString;
  const XMLCh* readoutinfoString;
  const XMLCh* slowcontrolinfoString;
  const XMLCh* fastcontrolinfoString;
  const XMLCh* paramString;
  const XMLCh* paramVectorString;
  const XMLCh* userParameterString;
  const XMLCh* userParameterVectorString;
  const XMLCh* conditioninfoString;

  // Constant Strings for parameter names
  const XMLCh* typeString;
  const XMLCh* nameString;
  const XMLCh* commentString;
  const XMLCh* hrefString;
  const XMLCh* lvnameString;
  const XMLCh* supportString;
  const XMLCh* rpathString;
  const XMLCh* npathString;
  const XMLCh* conditionString;
};

#endif // DETDESCCNV_XMLDETELEMCNV_H

