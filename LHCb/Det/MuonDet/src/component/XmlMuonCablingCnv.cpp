// $Id: XmlMuonCablingCnv.cpp,v 1.4 2006-12-14 13:27:07 ranjard Exp $
// Include files 

#include <vector>

#include "DetDescCnv/XmlUserConditionCnv.h"
#include "MuonDet/MuonReadoutCond.h"
#include "MuonDet/MuonL1Board.h"
#include "MuonDet/MuonStationCabling.h"
#include "GaudiKernel/RegistryEntry.h"

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

private:
StatusCode splitList( std::string &stringList,
                     std::vector<long>& vectorList);
  
StatusCode L1BoardReadAndCreate( xercesc::DOMNode* L1Board,
                                 IOpaqueAddress* address);
  
private:
  const XMLCh* CablingString;
  const XMLCh* SupportString;
  const XMLCh* L1String;
  const XMLCh* L1NameString;
  const XMLCh* L1NumberString;
  const XMLCh* L1StationString;
  const XMLCh* ODENumberString;
  const XMLCh* ODEListString;
  const XMLCh* ODEReferenceString;
  const XMLCh* ODEConditionString;
  const XMLCh* TSLayoutString;
  const XMLCh* TSLayoutRegionString;
  const XMLCh* TSLayoutXString;
  const XMLCh* TSLayoutYString;
  
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
    msg<<MSG::VERBOSE<<" start muon cabling"<<endreq;
    
    CablingString= xercesc::XMLString::transcode("MuonStationCabling");
    SupportString= xercesc::XMLString::transcode("SupportPath");
    L1String= xercesc::XMLString::transcode("L1Board");
    L1NameString= xercesc::XMLString::transcode("L1Name");
    L1NumberString= xercesc::XMLString::transcode("L1SerialNumber");
    L1StationString= xercesc::XMLString::transcode("L1Station");
    ODENumberString= xercesc::XMLString::transcode("ODENumber");
    ODEListString= xercesc::XMLString::transcode("ODEList");
    ODEReferenceString= xercesc::XMLString::transcode("conditionref");
    ODEConditionString= xercesc::XMLString::transcode("href");
    TSLayoutString= xercesc::XMLString::transcode("TSLayout");
    TSLayoutRegionString= xercesc::XMLString::transcode("Region");    
    TSLayoutXString= xercesc::XMLString::transcode("TSLayoutX");    
    TSLayoutYString= xercesc::XMLString::transcode("TSLayoutY");
 }

//=============================================================================
// Destructor
//=============================================================================
XmlMuonCablingCnv::~XmlMuonCablingCnv() {
  xercesc::XMLString::release((XMLCh**)&CablingString);
  xercesc::XMLString::release((XMLCh**)&SupportString);
  xercesc::XMLString::release((XMLCh**)&L1String);
  xercesc::XMLString::release((XMLCh**)&L1NameString);
  xercesc::XMLString::release((XMLCh**)&L1NumberString);
  xercesc::XMLString::release((XMLCh**)&L1StationString);
  xercesc::XMLString::release((XMLCh**)&ODENumberString);
  xercesc::XMLString::release((XMLCh**)&ODEListString);
  xercesc::XMLString::release((XMLCh**)&ODEReferenceString);
  xercesc::XMLString::release((XMLCh**)&ODEConditionString);
  xercesc::XMLString::release((XMLCh**)&TSLayoutString);
  xercesc::XMLString::release((XMLCh**)&TSLayoutRegionString);
  xercesc::XMLString::release((XMLCh**)&TSLayoutXString);
  xercesc::XMLString::release((XMLCh**)&TSLayoutYString);  
}; 

//=============================================================================


// -----------------------------------------------------------------------
// Fill an object with a new specific child element
// ------------------------------------------------------------------------
StatusCode 
XmlMuonCablingCnv::i_fillSpecificObj(xercesc::DOMElement* childElement,
                                          MuonStationCabling* dataObj,
                                          IOpaqueAddress* address){
  MsgStream msg(msgSvc(), "XmlMuonCablingCnv");
  const XMLCh* tagName = childElement->getNodeName();

  if (0 == xercesc::XMLString::compareString(CablingString, tagName)) {
    const std::string  support =
      dom2Std (childElement->getAttribute (SupportString));
    //search for L1 board definition inside the Cabling element
    xercesc::DOMNodeList* nodeChildren = childElement->getChildNodes();
    unsigned int i;
    for(i=0; i < nodeChildren->getLength(); ++i){
      msg << MSG::VERBOSE << "Processing child "<<
        dom2Std(nodeChildren->item(i)->getNodeName())<<endreq;
      if(dom2Std(nodeChildren->item(i)->getNodeName()) ==   
         dom2Std(L1String)){
        L1BoardReadAndCreate(nodeChildren->item(i),address);
        
        const std::string  L1Name =
          dom2Std(nodeChildren->item(i)
                  ->getAttributes()->getNamedItem(L1NameString)
                  ->getNodeValue());
        const std::string  L1Number = dom2Std(nodeChildren->item(i)
                  ->getAttributes()->getNamedItem(L1NumberString)
                                              ->getNodeValue());
        //       const std::string  L1Station = dom2Std(nodeChildren->item(i)
        //          ->getAttributes()->getNamedItem(L1StationString)
        //                                      ->getNodeValue());
        
        //int L1NumberValue= atof(L1Number.c_str());
        //int L1StationValue= atof(L1Station.c_str());        
        msg << MSG::DEBUG<< atof(L1Number.c_str())<<endreq;
        msg << MSG::VERBOSE<< L1Name<<endreq;  
        dataObj->update(L1Name); 
      
        
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


StatusCode 
XmlMuonCablingCnv::L1BoardReadAndCreate( xercesc::DOMNode* L1Board,
                                         IOpaqueAddress* address)
{
  MsgStream msg(msgSvc(), "XmlMuonCablingCnv");
  StatusCode sc;
  MuonL1Board* createdBoard=new MuonL1Board();
  
  const std::string  L1Name =
    dom2Std(L1Board->getAttributes()->getNamedItem(L1NameString)
            ->getNodeValue());
  const std::string  L1Number = dom2Std(L1Board
                                        ->getAttributes()->
                                        getNamedItem(L1NumberString)
                                        ->getNodeValue());
  long L1NumberValue=atol(L1Number.c_str());
  const std::string  L1Station = dom2Std(L1Board
                                        ->getAttributes()->
                                        getNamedItem(L1StationString)
                                        ->getNodeValue());
  long L1StationValue=atol(L1Station.c_str());
  
  const std::string  ODENumber = dom2Std(L1Board
                                        ->getAttributes()->
                                        getNamedItem(ODENumberString)
                                        ->getNodeValue());
  long ODENumberValue=atol(ODENumber.c_str());

  std::string  ODEList = dom2Std(L1Board
                                        ->getAttributes()->
                                        getNamedItem(ODEListString)
                                        ->getNodeValue());
  std::vector<long> ODEListValue;
  splitList(ODEList,ODEListValue);
  
  xercesc::DOMNodeList* nodeChildren = L1Board->getChildNodes();
  unsigned int i;
  unsigned int iODE=0;
  
  for(i=0; i < nodeChildren->getLength(); ++i){   
    msg << MSG::VERBOSE << "Processing child "<<
      dom2Std(nodeChildren->item(i)->getNodeName())<<endreq;
    if(dom2Std(nodeChildren->item(i)->getNodeName()) == 
       dom2Std(ODEReferenceString)){
      
      msg << MSG::VERBOSE << "Processing element "
          << dom2Std(ODEReferenceString) <<" "<<iODE<< endreq;
      xercesc::DOMNamedNodeMap* attributes =      
        nodeChildren->item(i)->getAttributes();
      xercesc::DOMNode* odeNode = attributes->
        getNamedItem(ODEConditionString);
      std::string  OdeReference = dom2Std (odeNode->getNodeValue());  
      unsigned int poundPosition = OdeReference.find_last_of('#');
      // std::string entryName = "/" + OdeReference.substr(poundPosition + 1);
      //      std::string location = address->par()[0];
      //      std::string location1 = address->par()[1];
      //      IOpaqueAddress* xmlAddr = 0;      
      //      xmlAddr = createAddressForHref (OdeReference, 5, address);
      //      msg << MSG::VERBOSE << " location : " << xmlAddr->par()[0]
      //          << " entryName : " << xmlAddr->par()[1]
      //          << " isString : " << xmlAddr->ipar()[0] << endreq;      
      //      msg << MSG::DEBUG << "Processing element "
      //   << OdeReference << " "<<entryName<< " "<<location 
      //   <<" "<<location1<<" "<<dataObj->name()<<endreq;
      createdBoard->addODE(ODEListValue[iODE],
                           OdeReference.substr(poundPosition + 1));
      msg << MSG::VERBOSE <<ODEListValue[iODE]<<" "<<
        OdeReference.substr(poundPosition + 1)<<endreq;
      iODE++;
      
    }
    
    else if(dom2Std(nodeChildren->item(i)->getNodeName()) == 
            dom2Std(TSLayoutString)){
      msg << MSG::VERBOSE << "Processing element "<<
        dom2Std(TSLayoutString)<<endreq;
      
      xercesc::DOMNamedNodeMap* attributes =      
        nodeChildren->item(i)->getAttributes();
      xercesc::DOMNode* TSRegionNode = attributes->
        getNamedItem(TSLayoutRegionString);
      long  TSRegionValue = atol(dom2Std(TSRegionNode
                                         ->getNodeValue()).c_str());  

      xercesc::DOMNode* TSLayoutXNode = attributes->
        getNamedItem(TSLayoutXString);
      long TSXValue = atol(dom2Std (TSLayoutXNode
                                    ->getNodeValue()).c_str());

   
      xercesc::DOMNode* TSLayoutYNode = attributes->
        getNamedItem(TSLayoutYString);
      long TSYValue = atol(dom2Std (TSLayoutYNode
                                     ->getNodeValue()).c_str()); ;
	msg<<MSG::VERBOSE<<"layout "<<TSXValue<<" "<<TSYValue<<
        " "<<TSRegionValue<<endreq;	
      createdBoard->addLayout(TSRegionValue,TSXValue,TSYValue);
      
    }
    
  }
  
  
  createdBoard->setL1Number(L1NumberValue);
  createdBoard->setL1Station(L1StationValue);
  createdBoard->setNumberOfODE(ODENumberValue);
  
  DataSvcHelpers::RegistryEntry* cablingEntry = 
    dynamic_cast<DataSvcHelpers::RegistryEntry*> 
    (address->registry());
    DataSvcHelpers::RegistryEntry* TESEntry = 
    dynamic_cast<DataSvcHelpers::RegistryEntry*> 
    (cablingEntry->parent());
  sc = TESEntry->add(L1Name,createdBoard);    
 if(StatusCode::SUCCESS != sc) {
      msg << MSG::WARNING << "The store rejected L1Board "
          << L1Name << endreq;
      return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}


