// $Id: XmlMuonRegionCnv.cpp,v 1.7 2003-06-16 13:50:29 sponce Exp $

// Include files

#include "DetDescCnv/XmlUserDetElemCnv.h"
#include "DeMuonRegion.h"

/// Utility classes
#include "GaudiKernel/MsgStream.h"

#include <xercesc/dom/DOMNamedNodeMap.hpp>
#include <xercesc/dom/DOMNodeList.hpp>

/** @class XmlMuonRegionCnv
 *
 * simplified XML converter for MuonRegion
 * See whole implementation by David in package Det/MuonDet
 *
 * @author David Hutchcroft
 * @author Sebastien Ponce
 */
class XmlMuonRegionCnv : public XmlUserDetElemCnv<DeMuonRegion> {

public:

  /**
   * Constructor for this converter
   * @param svc a ISvcLocator interface to find services
   */
  XmlMuonRegionCnv (ISvcLocator* svc);
  
  /**
   * Default destructor
   */
  ~XmlMuonRegionCnv();

  virtual StatusCode initialize();
  
protected:

  /** This fills the current object for specific child.
   * Overrides the default implementation in XmlUserDetElemCnv.
   * @param childElement the specific child processed here
   * @param refpObject the object to be filled
   * @return status depending on the completion of the call
   */
  virtual StatusCode i_fillSpecificObj (xercesc::DOMElement* childElement,
                                        DeMuonRegion* dataObj,
                                        IOpaqueAddress* address);

private:

  // Some constant strings used for parsing XML
  const XMLCh* ChambersString;
  const XMLCh* GasGapString;
  const XMLCh* NumberString;
  const XMLCh* ReadoutMapString;
  const XMLCh* ReadoutTypeString;
  const XMLCh* NFEChamberXString;
  const XMLCh* NFEChamberYString;
  const XMLCh* AnodeString;
  const XMLCh* CathodeString;
};


// -----------------------------------------------------------------------
// Instantiation of a static factory class used by clients to create
// instances of this service
// -----------------------------------------------------------------------
static CnvFactory<XmlMuonRegionCnv> muonreg_factory;
const ICnvFactory& XmlMuonRegionCnvFactory = muonreg_factory;


// -----------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
XmlMuonRegionCnv::XmlMuonRegionCnv(ISvcLocator* svc) :
  XmlUserDetElemCnv<DeMuonRegion> (svc) {
  ChambersString = xercesc::XMLString::transcode("Chambers");
  GasGapString = xercesc::XMLString::transcode("GasGap");
  NumberString = xercesc::XMLString::transcode("Number");
  ReadoutMapString = xercesc::XMLString::transcode("ReadoutMap");
  ReadoutTypeString = xercesc::XMLString::transcode("ReadoutType");
  NFEChamberXString = xercesc::XMLString::transcode("NFEChamberX");
  NFEChamberYString = xercesc::XMLString::transcode("NFEChamberY");
  AnodeString = xercesc::XMLString::transcode("Anode");
  CathodeString = xercesc::XMLString::transcode("Cathode");
}


// -----------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
XmlMuonRegionCnv::~XmlMuonRegionCnv() {
  xercesc::XMLString::release((XMLCh**)&ChambersString);
  xercesc::XMLString::release((XMLCh**)&GasGapString);
  xercesc::XMLString::release((XMLCh**)&NumberString);
  xercesc::XMLString::release((XMLCh**)&ReadoutMapString);
  xercesc::XMLString::release((XMLCh**)&ReadoutTypeString);
  xercesc::XMLString::release((XMLCh**)&NFEChamberXString);
  xercesc::XMLString::release((XMLCh**)&NFEChamberYString);
  xercesc::XMLString::release((XMLCh**)&AnodeString);
  xercesc::XMLString::release((XMLCh**)&CathodeString);
}


// -----------------------------------------------------------------------
// Initialize
// ------------------------------------------------------------------------
StatusCode XmlMuonRegionCnv::initialize(){
  StatusCode sc = XmlBaseDetElemCnv::initialize();
  if(sc.isSuccess()){
    MsgStream log (msgSvc(), "XmlDetElemCnv");
    log << MSG::VERBOSE << "Initializing" << endmsg;    
  }
  return sc;
}

// -----------------------------------------------------------------------
// Fill an object with a new specific child element
// ------------------------------------------------------------------------
StatusCode
XmlMuonRegionCnv::i_fillSpecificObj (xercesc::DOMElement* childElement,
                                     DeMuonRegion* dataObj,
                                     IOpaqueAddress* /*address*/) {
  MsgStream log (msgSvc(), "XmlMuonRegionCnv");
  
  // gets the element's name
  const XMLCh* tagName = childElement->getNodeName();

  char* tagNameString = xercesc::XMLString::transcode(tagName);
  log << MSG::DEBUG << "Processing element " << tagNameString << endmsg;
  xercesc::XMLString::release(&tagNameString);
  
  /*
    <!-- Number of Chambers in region -->
    <!-- logical pad sizes in mm (be explicit) -->
    <!ELEMENT Chambers (GasGap,readoutinfo)>
    <!ATTLIST Chambers 
              Number CDATA #REQUIRED
              logvol CDATA #REQUIRED
              support CDATA #REQUIRED
              padx CDATA #REQUIRED
              pady CDATA #REQUIRED>
    
    <!-- Information on the logical location of the gas gap -->
    <!ELEMENT GasGap EMPTY>
    <!ATTLIST GasGap 
              Number CDATA #REQUIRED
              offset CDATA #REQUIRED
              logvol CDATA #REQUIRED>
    <!-- Information on the mapping of FE channels to logical channels -->
    <!-- NFEChamber(X/Y) are the number of channels in x and y
         across a chamber -->
    <!-- MergeLogical are the information on which FE channels to merge 
         into either one or two logical strip (or pads if not crosses) 
         sets (-1 is no mapping)-->
    <!ELEMENT ReadoutMap EMPTY>
    <!ATTLIST ReadoutMap 
              ReadoutType ( Anode | Cathode ) #REQUIRED
              NFEChamberX  CDATA #REQUIRED
              NFEChamberY  CDATA #REQUIRED
              MergeLogicalX1 CDATA #REQUIRED
              MergeLogicalY1 CDATA #REQUIRED
              MergeLogicalX2 CDATA "-1"
              MergeLogicalY2 CDATA "-1">                  
  */


  if (0 == xercesc::XMLString::compareString(ChambersString, tagName)) {

    // checks there are children
    if(!childElement->hasChildNodes()){
      log << MSG::DEBUG << "No readout/gasgap associated to this region" 
          << endmsg;
      return StatusCode::FAILURE;
    }

    // look for children of type GasGap and retrieve the number of gasGap
    int gasGapNumber=0;
    xercesc::DOMNodeList* nodeChildren = childElement->getChildNodes();
    unsigned int i;
    for (i=0; i < nodeChildren->getLength(); ++i) {
      // get child and type it
      if (nodeChildren->item(i)->getNodeType() !=
          xercesc::DOMNode::ELEMENT_NODE) {
        log << MSG::WARNING 
            << "Non-element in the chambers tag" << endmsg;
        return StatusCode::FAILURE;
      }

      if (0 == xercesc::XMLString::compareString
          (nodeChildren->item(i)->getNodeName(),
           GasGapString)) {
        xercesc::DOMNamedNodeMap* attributes =
          nodeChildren->item(i)->getAttributes();
        xercesc::DOMNode* ggNumberNode = attributes->getNamedItem(NumberString);
        char* gasGapNumberStr =
          xercesc::XMLString::transcode(ggNumberNode->getNodeValue());
        gasGapNumber = (int) xmlSvc()->eval(gasGapNumberStr, false);
        xercesc::XMLString::release(&gasGapNumberStr);
      }
    }

    // for now with MWPCs and RPCs this is a good formula
    dataObj->setGapsPerFE(gasGapNumber / 2);

    // get a value of the 'Number' attribute
    const XMLCh* number = childElement->getAttribute (NumberString);
    if (0 == number) {
      log <<MSG::WARNING << "Chamber object did not have number attribute" 
          << endmsg;
      return StatusCode::FAILURE;
    }
    char* numberString = xercesc::XMLString::transcode(number);
    dataObj->setchamberNum ((int) xmlSvc()->eval(numberString, false));
    xercesc::XMLString::release(&numberString);

  } else if(0 == xercesc::XMLString::compareString(ReadoutMapString, tagName)){

    const XMLCh* ReadoutType = childElement->getAttribute (ReadoutTypeString);
    const XMLCh* NFEChamberX = childElement->getAttribute (NFEChamberXString);
    const XMLCh* NFEChamberY = childElement->getAttribute (NFEChamberYString);

    if (!(0 == xercesc::XMLString::compareString(AnodeString, ReadoutType) ||
          0 == xercesc::XMLString::compareString(CathodeString, ReadoutType))) {
      log << MSG::WARNING << "Was given a readout type :" 
          << ReadoutType << endmsg;
      return StatusCode::FAILURE;
    }
    char* NFEChamberXString = xercesc::XMLString::transcode(NFEChamberX);
    char* NFEChamberYString = xercesc::XMLString::transcode(NFEChamberY);
    if (0 == xercesc::XMLString::compareString(AnodeString, ReadoutType)) {
      dataObj->setFEAnodeX
        ((int) xmlSvc()->eval(NFEChamberXString, false));
      dataObj->setFEAnodeY
        ((int) xmlSvc()->eval(NFEChamberYString, false));
    } else {
      dataObj->setFECathodeX
        ((int) xmlSvc()->eval(NFEChamberXString, false));
      dataObj->setFECathodeY
        ((int) xmlSvc()->eval(NFEChamberYString, false));
    }
    xercesc::XMLString::release (&NFEChamberXString);
    xercesc::XMLString::release (&NFEChamberYString);    

  }else {
    // Unknown tag, a warning message could be issued here
    log << MSG::WARNING << "Can not interpret specific type :" 
        << tagName << endmsg;
  }
  return StatusCode::SUCCESS;
}
