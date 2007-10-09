#include "ProcessorKernel/RegisterFactory.h"

static L0Muon::RegisterFactory* s_instance_0=0;
static L0Muon::RegisterFactory* s_instance_1=0;
static int s_sel=0;

L0Muon::RegisterFactory::RegisterFactory() {

  m_found =0;

}

L0Muon::RegisterFactory::~RegisterFactory() {}

void L0Muon::RegisterFactory::selectInstance(int i){ 
  s_sel= i<2 ? i : 1; 
}

L0Muon::RegisterFactory* L0Muon::RegisterFactory::instance() {
  
  if (s_sel==0) {
    if ( s_instance_0 == 0 ) {
      s_instance_0 = new L0Muon::RegisterFactory;
    } 
    return s_instance_0;
  }

  if (s_sel==1) {
    if ( s_instance_1 == 0 ) {
      s_instance_1 = new L0Muon::RegisterFactory;
    } 
    return s_instance_1;
  }

  return s_instance_1;
  
}

L0Muon::Register* L0Muon::RegisterFactory::createRegister
(std::string name, int size) {
  
  std::map<std::string,L0Muon::Register*>::iterator ind;
  ind = m_registers.find(name);
  if (ind != m_registers.end() ) {
       
    return (*ind).second;
  } else {
    
    Register* pr= new Register(size);
    pr->setName(name);
    m_registers[name] = pr;
    return pr;
  }  
}

L0Muon::Register* L0Muon::RegisterFactory::searchRegister
(std::string name) {
  
  std::map<std::string,L0Muon::Register*>::iterator ind;
  ind = m_registers.find(name);
  if (ind != m_registers.end() ) {

    m_found = true;
       
    return (*ind).second;
  } else {

    m_found = false;
    return 0;
    
  }  
}

L0Muon::TileRegister* L0Muon::RegisterFactory::createTileRegister
(std::string name, int size) {
  
  std::map<std::string,L0Muon::Register*>::iterator ind;
  ind = m_registers.find(name);
  if (ind != m_registers.end() ) {
    return dynamic_cast<L0Muon::TileRegister*>((*ind).second);
  } else {
    TileRegister* pr= new TileRegister(size);
    pr->setName(name);
    m_registers[name] = pr;
   
    return pr;
  }  
}

L0Muon::TileRegister* L0Muon::RegisterFactory::searchTileRegister
(std::string name) {
  
  std::map<std::string,L0Muon::Register*>::iterator ind;
  ind = m_registers.find(name);
  if (ind != m_registers.end() ) {
   
    m_found = true;
    
    return dynamic_cast<L0Muon::TileRegister*>((*ind).second);
  } else {
    m_found = false;
    return 0;
    
  }  
}

void L0Muon::RegisterFactory::deleteRegister(std::string name) {
  
  Register* pr = m_registers[name];
  delete pr;
  m_registers.erase(name);
  
}


int L0Muon::RegisterFactory::
createAlias(std::string name, std::string alias) {
  std::map<std::string,L0Muon::Register*>::iterator ind;
  ind = m_registers.find(name);
  if (ind != m_registers.end() ) {
    m_registers[alias] = (*ind).second;
    return 0;
  } else {
    return -1;
  }
}

void L0Muon::RegisterFactory::dump(bool full) {
  std::cout << "RegisterFactory: " << m_registers.size() << " registers" << std::endl;
  if (full) {
    std::map<std::string,Register*>::iterator itreg;
    for (itreg = m_registers.begin(); itreg!=m_registers.end(); itreg++) {
      std::cout << "RegisterFactory:  "
                << " " << itreg->first 
                << " " << (itreg->second)->name()
                << " " << (itreg->second)->type()
                << " " << (itreg->second)->size()
                << "\n";
    }
  }
  
}

void  L0Muon::RegisterFactory::reset(){
  m_registers.clear();
  s_instance_0=0;
  s_instance_1=0;
}



std::string L0Muon::RegisterFactory::toXML(std::string tab){

  std::string xmlString=tab;

  char buf[8];
  std::string str;


  xmlString += "<"+XMLRegisterFactory+" ";
  sprintf(buf,"%d",m_registers.size());
  str = buf;
  xmlString += " size = \""+str+"\" ";
  xmlString += " >\n";


  // Write registers (skip aliases)
  std::map<std::string,Register*>::iterator i_registers;  
  for (i_registers=m_registers.begin();i_registers!=m_registers.end();i_registers++){
    if ((*i_registers).second->name()==(*i_registers).first)
      xmlString += (*i_registers).second->toXML(tab+"    ");
  }
  
  // Write aliases
  for (i_registers=m_registers.begin();i_registers!=m_registers.end();i_registers++){
    if ((*i_registers).second->name()!=(*i_registers).first) {
      xmlString += tab+"    ";
      xmlString += "<"+XMLRegisterAlias+" ";
      xmlString += " alias = \""+(*i_registers).first+"\" ";
      xmlString += " name = \""+(*i_registers).second->name()+"\" ";
      xmlString += " />\n";
    } 
  }
  
  xmlString += tab;
  xmlString +="</"+XMLRegisterFactory+">\n";

  return xmlString;
  
}

void L0Muon::RegisterFactory::fromXML(DOMNode* pNode){
  
  //Get the attributes
  DOMNamedNodeMap* di = pNode->getAttributes();
  unsigned int size   = getAttributeInt(di, "size");

  //Fill the internal dictionary with puNode, 
  //objects pointed to should be released after usage²
  XMLCh* REGISTER      = XMLString::transcode(XMLRegister.c_str());
  XMLCh* TILEREGISTER  = XMLString::transcode(XMLTileRegister.c_str());
  XMLCh* REGISTERALIAS = XMLString::transcode(XMLRegisterAlias.c_str());
  DOMNode *child = pNode->getFirstChild();
  while(child) {
    
    if(XMLString::equals(child->getNodeName(),REGISTER)) {
      registerFromNode(child);
    } else if (XMLString::equals(child->getNodeName(),TILEREGISTER)) {
      tileRegisterFromNode(child);
    }else if (XMLString::equals(child->getNodeName(),REGISTERALIAS)) {
      registerAliasFromNode(child);
    }
    child = child->getNextSibling();
  }
  
  if (m_registers.size()!=size) {
    std::cout <<"L0Muon::RegisterFactory::fillFactory "<<m_registers.size()<<" != "<<size <<std::endl;
    std::cout << "Please raise a proper exception"<<std::endl;
    XMLString::release(&REGISTER     );
    XMLString::release(&TILEREGISTER );
    XMLString::release(&REGISTERALIAS);
    exit(-1);
  }
  XMLString::release(&REGISTER     );
  XMLString::release(&TILEREGISTER );
  XMLString::release(&REGISTERALIAS);

} 

void L0Muon::RegisterFactory::registerFromNode(DOMNode* pNode){
  DOMNamedNodeMap* di = pNode->getAttributes();
  std::string name = getAttributeStr(di, "name");
  std::string type = getAttributeStr(di, "type");
  int size         = getAttributeInt(di, "size");
  Register* preg = createRegister(name,size);

  preg->setType(type);
}

void L0Muon::RegisterFactory::tileRegisterFromNode(DOMNode* pNode){

  XMLCh* TILE = XMLString::transcode(XMLTile.c_str());

  DOMNamedNodeMap* di = pNode->getAttributes();
  std::string name = getAttributeStr(di, "name");
  std::string type = getAttributeStr(di, "type");
  int size         = getAttributeInt(di, "size");
  bool duplicatedTiles = getAttributeInt(di, "duplicatedTiles");

  TileRegister* preg = createTileRegister(name,size);
  preg->setType(type);
  if (duplicatedTiles) preg->setDuplicatedTiles(true);

  std::vector<LHCb::MuonTileID> ids;  
  boost::dynamic_bitset<> tilestag ;
  boost::dynamic_bitset<> stripstag;

  DOMNode *child = pNode->getFirstChild();
  while(child) {
    if(XMLString::equals(child->getNodeName(),TILE)) {
      tileFromNode(child,&ids,&tilestag,&stripstag);
    }
    child = child->getNextSibling();
  }
  preg->setTileVector(ids);
  preg->setTilesTagVector(tilestag);
  preg->setStripsTagVector(stripstag);

  XMLString::release(&TILE);

}

void L0Muon::RegisterFactory::registerAliasFromNode(DOMNode* pNode){
  DOMNamedNodeMap* di = pNode->getAttributes();
  std::string alias = getAttributeStr(di, "alias");
  std::string name  = getAttributeStr(di, "name");
  createAlias(name,alias);
}

void L0Muon::RegisterFactory::tileFromNode(DOMNode* pNode, 
                                           std::vector<LHCb::MuonTileID> * mids, 
                                           boost::dynamic_bitset<> * tilestag, 
                                           boost::dynamic_bitset<> * stripstag){

  DOMNamedNodeMap* di = pNode->getAttributes();
  
  std::string id  = getAttributeStr(di, "id");
  int itilestag   = getAttributeInt(di, "tilestag");
  int istripstag  = getAttributeInt(di, "stripstag");

  mids->push_back(LHCb::MuonTileID(id));
  tilestag->push_back(itilestag);
  stripstag->push_back(istripstag);

}

////////////////////////////////////////////////////////////////////////////////
//
//  getAttribute
//
////////////////////////////////////////////////////////////////////////////////

int L0Muon::RegisterFactory::getAttributeInt(DOMNamedNodeMap* di, const char* key){
  
  const XMLCh* valX;
  int          valI;
  
  XMLCh* xmlKey = XMLString::transcode(key);
  
  valX = di->getNamedItem(xmlKey)->getNodeValue();
  valI = XMLString::parseInt(valX);
  
  XMLString::release(&xmlKey); //Release memory used by the transcode method
  return valI;

}

std::string L0Muon::RegisterFactory::getAttributeStr(DOMNamedNodeMap* di, const char* key){

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

