// Include files

#include <string>
#include <vector>

#include "DetDescCnv/XmlUserConditionCnv.h"
#include "MuonDet/MuonStationCabling.h"

#include <xercesc/dom/DOMNamedNodeMap.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMNode.hpp>

#include "GaudiKernel/IOpaqueAddress.h"


// local

//-----------------------------------------------------------------------------
// Implementation file for class : XmlMuonCablingCnv
//
// 2003-12-30 : Alessia Satta
//-----------------------------------------------------------------------------


/** @class XmlMuonCablingCnv
 *
 * XML converter for MuonCabling
 *
 * @author Alessia Satta
 */
class XmlMuonCablingCnv : public XmlUserConditionCnv<MuonStationCabling> {

public:

  /**
   * Constructor for this converter
   * @param svc a ISvcLocator interface to find services
   */
  XmlMuonCablingCnv (ISvcLocator* svc);

  /**
   * Default destructor
   */
  ~XmlMuonCablingCnv();

protected:

  using XmlUserConditionCnv<MuonStationCabling>::i_fillSpecificObj;
  /** This fills the current object for specific child.
   * Overrides the default implementation in XmlUserDetElemCnv.
   * @param childElement the specific child processed here
   * @param dataObj the object to be filled
   * @param address ??
   * @return status depending on the completion of the call
   */
  virtual StatusCode i_fillSpecificObj (xercesc::DOMElement* childElement,
                                        MuonStationCabling* dataObj,
                                        IOpaqueAddress* address);

virtual StatusCode updateRep (IOpaqueAddress *pAddress, DataObject *pObject);

private:
StatusCode splitList( std::string &stringList,
                     std::vector<long>& vectorList);



private:
  const XMLCh* CablingString;
  const XMLCh* SupportString;
  const XMLCh* L1ListString;
  const XMLCh* L1ListRefString;
  const XMLCh* L1NumberString;

};

// -----------------------------------------------------------------------
// Instantiation of a static factory class used by clients to create
// instances of this service
// -----------------------------------------------------------------------
DECLARE_CONVERTER_FACTORY(XmlMuonCablingCnv)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
XmlMuonCablingCnv::XmlMuonCablingCnv(ISvcLocator* svc):
XmlUserConditionCnv<MuonStationCabling> (svc)
 {
    MsgStream msg(msgSvc(), "XmlMuonCablingCnv");
    if( UNLIKELY( msg.level() <= MSG::VERBOSE ) )
      msg<<MSG::VERBOSE<<" start muon cabling"<<endmsg;

    CablingString= xercesc::XMLString::transcode("MuonStationCabling");
    SupportString= xercesc::XMLString::transcode("SupportPath");
    L1ListString= xercesc::XMLString::transcode("conditionref");
    L1ListRefString=xercesc::XMLString::transcode("href");
    L1NumberString= xercesc::XMLString::transcode("L1Number");
 }

//=============================================================================
// Destructor
//=============================================================================
XmlMuonCablingCnv::~XmlMuonCablingCnv() {
  xercesc::XMLString::release((XMLCh**)&CablingString);
  xercesc::XMLString::release((XMLCh**)&SupportString);
  xercesc::XMLString::release((XMLCh**)&L1ListString);
  xercesc::XMLString::release((XMLCh**)&L1ListRefString);
  xercesc::XMLString::release((XMLCh**)&L1NumberString);

}

//=============================================================================


StatusCode XmlMuonCablingCnv::updateRep (IOpaqueAddress* /* pAddress */,
                                         DataObject*     /* pObject  */ ){
 MsgStream msg(msgSvc(), "XmlMuonCablingCnv");
 msg<<MSG::INFO<<" number of L1 "<<endmsg;
 return StatusCode::SUCCESS;
}


// -----------------------------------------------------------------------
// Fill an object with a new specific child element
// ------------------------------------------------------------------------
StatusCode
XmlMuonCablingCnv::i_fillSpecificObj(xercesc::DOMElement* childElement,
                                     MuonStationCabling* dataObj,
                                     IOpaqueAddress* /* address */){
  MsgStream msg(msgSvc(), "XmlMuonCablingCnv");
  const XMLCh* tagName = childElement->getNodeName();

  if (0 == xercesc::XMLString::compareString(CablingString, tagName)) {
    // const std::string  support =
    //   dom2Std (childElement->getAttribute (SupportString));
    const std::string l1NumberString =
      dom2Std (childElement->getAttribute (L1NumberString));
    long l1NumberValue=atol(l1NumberString.c_str());
    StatusCode sc=dataObj->update(l1NumberValue);
    if(sc.isFailure())return sc;
    if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
      msg<<MSG::DEBUG<<" number of L1 "<<l1NumberValue<<endmsg;
    xercesc::DOMNodeList* nodeChildren = childElement->getChildNodes();
    unsigned int i;
    for(i=0; i < nodeChildren->getLength(); ++i){
      if(dom2Std(nodeChildren->item(i)->getNodeName()) ==
         dom2Std(L1ListString)){
        xercesc::DOMNamedNodeMap* attributes =
          nodeChildren->item(i)->getAttributes();
        xercesc::DOMNode* L1Node = attributes->
          getNamedItem(L1ListRefString);
        std::string  L1Reference = dom2Std (L1Node->getNodeValue());
        unsigned int poundPosition = L1Reference.find_last_of('#');
        // std::string entryName = "/" + L1Reference.substr(poundPosition + 1);
        sc=dataObj->addL1Name(L1Reference.substr(poundPosition + 1));
        if(sc.isFailure())return sc;
        if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
          msg<<MSG::DEBUG<<"name of L1 "<<L1Reference.substr(poundPosition + 1)<<endmsg;
      }
    }



  }
  return StatusCode::SUCCESS;

}




StatusCode
XmlMuonCablingCnv::splitList( std::string &stringList,
                             std::vector<long>& vectorList){

  std::string::size_type cPos = stringList.find(',');
  std::string sCurr;
  while( cPos != std::string::npos ){ // found a comma in string
    sCurr = stringList.substr( 0, cPos); // get up to first comma
    vectorList.push_back(atol(sCurr.c_str())); // add to vector
    stringList.erase( 0, cPos+1); // erase up to and including comma
    cPos = stringList.find(','); // find next comma
  }
  vectorList.push_back(atol(stringList.c_str()));
return StatusCode::SUCCESS;
}


