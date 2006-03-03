#include "L0MuonKernel/L0MuonKernelFromXML.h"
#include "ProcessorKernel/RegisterFactory.h"
#include "L0MuonKernel/UnitFactory.h"


static const std::string XMLL0MuonTrigger =  "L0MuonTrigger";

L0Muon::Unit *L0Muon::L0MuonKernelFromXML(std::string xmlFileName){

  

  DOMDocument*  doc;      //The DOM document
  DOMElement*   root;     //The root element
  DOMBuilder*   parser;   //The XML parser
  XMLCh*        xmlStr;   //The XML String ...
  DOMNodeList*  li;       //The DOM list of nodes 
  DOMNode*     child;


//   std::cout << "<L0Muon::L0MuonKernelFromXML>  IN : "<<xmlFileName << std::endl;

  // Necessary incantations to initialize ?

  bool doNamespaces       = false;
  bool doSchema           = false;
  bool schemaFullChecking = false;
  bool dtNormalization    = false;

  // Initialize the XML4C system
  XMLPlatformUtils::Initialize();

  // Instantiate the DOM parser.
  static const XMLCh gLS[] = { chLatin_L, chLatin_S, chNull };
  DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(gLS);  
  parser = ((DOMImplementationLS*)impl)->
    createDOMBuilder(DOMImplementationLS::MODE_SYNCHRONOUS, 0);

  parser->setFeature(XMLUni::fgDOMNamespaces, doNamespaces);
  parser->setFeature(XMLUni::fgXercesSchema, doSchema);
  parser->setFeature(XMLUni::fgXercesSchemaFullChecking, schemaFullChecking);
  parser->setFeature(XMLUni::fgDOMDatatypeNormalization, dtNormalization);

  // Parse the document and get the root element
  doc  = parser->parseURI(xmlFileName.c_str());
  root = doc->getDocumentElement();
 
  //Get the RegisterFactory node
  xmlStr = XMLString::transcode("RegisterFactory");
  li = root->getElementsByTagName(xmlStr);
  XMLString::release(&xmlStr);
  //Consistency check
  if( li->getLength() == 0 ){
    std::cout << "<L0Muon::L0MuonKernelFromXML>  li->getLength() = " << li->getLength() << std::endl;
    std::cout << "<L0Muon::L0MuonKernelFromXML>  No node found ... exiting" << std::endl;
    exit(-1);
  }
  
  if( li->getLength() > 1 ){
    std::cout << "<L0Muon::L0MuonKernelFromXML>  li->getLength() = " << li->getLength() << std::endl;
    std::cout << "<L0Muon::L0MuonKernelFromXML>  Too many nodes found ... exiting" << std::endl;
    exit(-1);    
  }
  
  // Decode the node
  child = li->item(0);
  L0Muon::RegisterFactory* rfactory = L0Muon::RegisterFactory::instance();
  rfactory->fromXML(child);

  // Get the UnitFactory node
  xmlStr = XMLString::transcode("UnitFactory");
  li = root->getElementsByTagName(xmlStr);
  XMLString::release(&xmlStr);
  //Consistency check
  if( li->getLength() == 0 ){
    std::cout << "<L0Muon::L0MuonKernelFromXML>  li->getLength() = " << li->getLength() << std::endl;
    std::cout << "<L0Muon::L0MuonKernelFromXML>  No node found ... exiting" << std::endl;
    exit(-1);
  }
  
  if( li->getLength() > 1 ){
    std::cout << "<L0Muon::L0MuonKernelFromXML>  li->getLength() = " << li->getLength() << std::endl;
    std::cout << "<L0Muon::L0MuonKernelFromXML>  Too many nodes found ... exiting" << std::endl;
    exit(-1);    
  }
  
  // Decode the node
  Unit* top=0;
  child = li->item(0);
  child = li->item(0)->getFirstChild ();
  while(child){
    if (child->getNodeType()==3) {
      child = child->getNextSibling();
      continue;
    }
    L0Muon::UnitFactory* ufactory = L0Muon::UnitFactory::instance();
    DOMNamedNodeMap* di = child->getAttributes();
    std::string type = getAttributeStr(di, "type");
    top = ufactory->createUnit(child,type);
    break;
  }

  parser->release();
  XMLPlatformUtils::Terminate();

  return top;
}
////////////////////////////////////////////////////////////////////////////////
//
//  getAttribute
//
////////////////////////////////////////////////////////////////////////////////

int L0Muon::getAttributeInt(DOMNamedNodeMap* di, const char* key){
  
  const XMLCh* valX;
  int          valI;
  
  XMLCh* xmlKey = XMLString::transcode(key);
  
  valX = di->getNamedItem(xmlKey)->getNodeValue();
  valI = XMLString::parseInt(valX);
  
  XMLString::release(&xmlKey); //Release memory used by the transcode method
  return valI;

}

std::string L0Muon::getAttributeStr(DOMNamedNodeMap* di, const char* key){

  const XMLCh* valX;
  char*  valC;
  std::string  valS;
  
  XMLCh* xmlKey = XMLString::transcode(key);
  
  valX = di->getNamedItem(xmlKey)->getNodeValue();
  valC = XMLString::transcode(valX);
  valS = valC;
  
  XMLString::release(&xmlKey); //Release memory used by the transcode method
  XMLString::release(&valC);   //Release memory used by the transcode method
  return valS;
  
}
