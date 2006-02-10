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
  
//   std::cout << "L0Muon::L0MuonKernelFromXML XMLPlatformUtils::Initialize done\n";

  // Instantiate the DOM parser.
  static const XMLCh gLS[] = { chLatin_L, chLatin_S, chNull };
  
//   std::cout << "L0Muon::L0MuonKernelFromXML  gLS[] initialized \n";

  DOMImplementation *impl = 
    DOMImplementationRegistry::getDOMImplementation(gLS);

//   std::cout << "L0Muon::L0MuonKernelFromXML impl  initialized \n";
  
  parser = 
    ((DOMImplementationLS*)impl)->
    createDOMBuilder(DOMImplementationLS::MODE_SYNCHRONOUS, 0);


//   std::cout << "L0Muon::L0MuonKernelFromXML  parser initialized \n";

  parser->setFeature(XMLUni::fgDOMNamespaces, doNamespaces);
  parser->setFeature(XMLUni::fgXercesSchema, doSchema);
  parser->setFeature(XMLUni::fgXercesSchemaFullChecking, schemaFullChecking);
  parser->setFeature(XMLUni::fgDOMDatatypeNormalization, dtNormalization);
  
//   std::cout << "L0Muon::L0MuonKernelFromXML  parser features initialized\n";

  //parse the document and get the root element
  doc  = parser->parseURI(xmlFileName.c_str());
//   std::cout << "<L0Muon::L0MuonKernelFromXML>  doc initialized\n" ;

//   std::cout << "<L0Muon::L0MuonKernelFromXML>  doc: " 
//   	    << "  has children ? "<< doc->hasChildNodes()
//   	    << "; has attributes ? "<< doc->hasAttributes()
//   	    << "; name is "<< XMLString::transcode(doc->getNodeName())
//   	    <<std::endl;
  root = doc->getDocumentElement();
//   std::cout << "<L0Muon::L0MuonKernelFromXML>  root: " 
//   	    << "  tag name is "<< XMLString::transcode(root->getTagName())
//   	    << "  has children ? "<< root->hasChildNodes() 
//   	    << "; has attributes ? "<< root->hasAttributes() 
//   	    << "; name is "<<  XMLString::transcode(root->getNodeName())
//   	    << std::endl;

//   DOMNamedNodeMap* diroot = root->getAttributes();
//   DOMNodeList * liroot = root->getChildNodes ();
//   std::cout << "<L0Muon::L0MuonKernelFromXML>  root: " 
//   	    << " size= "<<getAttributeInt(diroot,"size")
//   	    << " # of children " << liroot->getLength()
//   	    << std::endl;
  
//   for (int inode=0; inode < liroot->getLength() ; inode++) {
//     std::cout << "<L0Muon::L0MuonKernelFromXML>    * "
//     	      << " " <<inode
//     	      << " " <<XMLString::transcode(liroot->item(inode)->getNodeName())
//     	      <<" " <<liroot->item(inode)->getNodeType()
//     	      << std::endl;
//   }
 
  //Get the RegisterFactory node
  xmlStr = XMLString::transcode("RegisterFactory");
//   std::cout << "<L0Muon::L0MuonKernelFromXML>  xmlStr= "<<XMLString::transcode(xmlStr)<< std::endl;
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
//   std::cout << "<L0Muon::L0MuonKernelFromXML>  DOMNodeList done" << std::endl;
  
  // Decode the node
//   std::cout << "<L0Muon::L0MuonKernelFromXML>  child... " << std::endl;
  child = li->item(0);
//   std::cout << "<L0Muon::L0MuonKernelFromXML>  child = "
//   	    <<" " <<XMLString::transcode(child->getNodeName()) 
//   	    <<" " <<child->getNodeType()
//   	    << std::endl;
  L0Muon::RegisterFactory* rfactory = L0Muon::RegisterFactory::instance();
//   std::cout << "<L0Muon::L0MuonKernelFromXML>  register factory ... " << std::endl;
  rfactory->fromXML(child);
//   std::cout << "<L0Muon::L0MuonKernelFromXML>  register factory done " << std::endl;

  // Get the UnitFactory node
  xmlStr = XMLString::transcode("UnitFactory");
//   std::cout << "<L0Muon::L0MuonKernelFromXML>  xmlStr= "<<XMLString::transcode(xmlStr)<< std::endl;
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
//   std::cout << "<L0Muon::L0MuonKernelFromXML>  DOMNodeList done" << std::endl;
  
  // Decode the node
  Unit* top;
  //  std::cout << "<L0Muon::L0MuonKernelFromXML>  child... " << std::endl;
  child = li->item(0);
//   std::cout << "<L0Muon::L0MuonKernelFromXML>  child = "
//   	    <<" " <<XMLString::transcode(child->getNodeName()) 
//   	    <<" " <<child->getNodeType()
//   	    << std::endl;
  child = li->item(0)->getFirstChild ();
  while(child){
//     std::cout << "<L0Muon::L0MuonKernelFromXML>  grand child = "
//     	      <<" " <<XMLString::transcode(child->getNodeName()) 
//     	      <<" " <<child->getNodeType() 
//     	      << std::endl;
    if (child->getNodeType()==3) {
      child = child->getNextSibling();
      continue;
    }
    L0Muon::UnitFactory* ufactory = L0Muon::UnitFactory::instance();
//     std::cout << "<L0Muon::L0MuonKernelFromXML>  top unit..." << std::endl;
    DOMNamedNodeMap* di = child->getAttributes();
    std::string type = getAttributeStr(di, "type");
//     std::cout << "<L0Muon::L0MuonKernelFromXML>  top unit type is " << type <<std::endl;
    top = ufactory->createUnit(child,type);
//     std::cout << "<L0Muon::L0MuonKernelFromXML>  top unit done " << std::endl;
    break;
  }
//   std::cout << "<L0Muon::L0MuonKernelFromXML>  OUT " << std::endl;

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

// L0Muon::L0MUONKERNELFROMXML::L0MUONKERNELFROMXML() {
// }

// L0Muon::L0MUONKERNELFROMXML::L0MUONKERNELFROMXML(std::string XMLFileName) {
// }

// L0Muon::L0MUONKERNELFROMXML::~L0MUONKERNELFROMXML() {}
