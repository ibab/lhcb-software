#include "L0MuonKernel/L0MuonKernelFromXML.h"
#include "ProcessorKernel/RegisterFactory.h"
#include "L0MuonKernel/UnitFactory.h"


static const std::string XMLL0MuonTrigger =  "L0MuonTrigger";

// L0Muon::Unit* L0Muon::L0MuonKernelFromXML(std::string xmlFileName){  
void L0Muon::L0MuonKernelFromXML(std::string xmlFileName, bool emulator){  

  DOMDocument*  doc;      //The DOM document
  DOMElement*   root;     //The root element
#ifdef XERCESC_GE_31
  DOMLSParser*   parser;  //The XML parser
#else
  DOMBuilder*   parser;   //The XML parser
#endif
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
#ifdef XERCESC_GE_31
  parser = ((DOMImplementationLS*)impl)->
    createLSParser(DOMImplementationLS::MODE_SYNCHRONOUS, 0);
  if (parser->getDomConfig()->canSetParameter(XMLUni::fgDOMNamespaces, doNamespaces))
    parser->getDomConfig()->setParameter(XMLUni::fgDOMNamespaces, doNamespaces);
  if (parser->getDomConfig()->canSetParameter(XMLUni::fgXercesSchema, doSchema))
    parser->getDomConfig()->setParameter(XMLUni::fgXercesSchema, doSchema);
  if (parser->getDomConfig()->canSetParameter(XMLUni::fgXercesSchemaFullChecking, schemaFullChecking))
    parser->getDomConfig()->setParameter(XMLUni::fgXercesSchemaFullChecking, schemaFullChecking);
  if (parser->getDomConfig()->canSetParameter(XMLUni::fgDOMDatatypeNormalization, dtNormalization))
    parser->getDomConfig()->setParameter(XMLUni::fgDOMDatatypeNormalization, dtNormalization);
#else
  parser = ((DOMImplementationLS*)impl)->
    createDOMBuilder(DOMImplementationLS::MODE_SYNCHRONOUS, 0);
  parser->setFeature(XMLUni::fgDOMNamespaces, doNamespaces);
  parser->setFeature(XMLUni::fgXercesSchema, doSchema);
  parser->setFeature(XMLUni::fgXercesSchemaFullChecking, schemaFullChecking);
  parser->setFeature(XMLUni::fgDOMDatatypeNormalization, dtNormalization);
#endif

  // Parse the document and get the root element
  doc  = parser->parseURI(xmlFileName.c_str());
  root = doc->getDocumentElement();
 
  // Get the RegisterFactory node
  //-----------------------------
  xmlStr = XMLString::transcode("RegisterFactory");
  li = root->getElementsByTagName(xmlStr);
  XMLString::release(&xmlStr);
  //Consistency check
  if( li->getLength() == 0 ){
    std::cout << "<L0Muon::L0MuonKernelFromXML>  li->getLength() = " << li->getLength() << std::endl;
    std::cout << "<L0Muon::L0MuonKernelFromXML>  RegisterFactory : no node found ... exiting" << std::endl;
    exit(-1);
  }
  if( li->getLength() > 1 ){
    std::cout << "<L0Muon::L0MuonKernelFromXML>  li->getLength() = " << li->getLength() << std::endl;
    std::cout << "<L0Muon::L0MuonKernelFromXML>  RegisterFactory : too many nodes found ... exiting" << std::endl;
    exit(-1);    
  }  // Decode the node
  child = li->item(0);
  L0Muon::RegisterFactory* rfactory = L0Muon::RegisterFactory::instance();
  rfactory->fromXML(child);

  if (emulator) {

    // Get the UnitFactory node
    //-------------------------
    xmlStr = XMLString::transcode("UnitFactory");
    li = root->getElementsByTagName(xmlStr);
    XMLString::release(&xmlStr);
    //Consistency check
    if( li->getLength() == 0 ){
      std::cout << "<L0Muon::L0MuonKernelFromXML>  li->getLength() = " << li->getLength() << std::endl;
      std::cout << "<L0Muon::L0MuonKernelFromXML>  UnitFactory : no node found ... exiting" << std::endl;
      exit(-1);
    }  
    if( li->getLength() > 1 ){
      std::cout << "<L0Muon::L0MuonKernelFromXML>  li->getLength() = " << li->getLength() << std::endl;
      std::cout << "<L0Muon::L0MuonKernelFromXML>  UnitFactory : too many nodes found ... exiting" << std::endl;
      exit(-1);    
    }
    // Decode the node
    child = li->item(0);
    L0Muon::UnitFactory* ufactory = L0Muon::UnitFactory::instance();
    //   L0Muon::Unit * top = ufactory->fromXML(child);
    ufactory->fromXML(child);

  }

  parser->release();
  XMLPlatformUtils::Terminate();

//   return top;

}
