// $ID: $

// Include Files
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/MsgStream.h"

#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMNodeList.hpp>

#include "XmlTools/IXmlSvc.h"
#include <DetDesc/ILVolume.h>

#include "SimulationSvc.h"


// -----------------------------------------------------------------------
// Instantiation of a static factory class used by clients to create
// instances of this service
// -----------------------------------------------------------------------

// Declaration of the Service Factory
DECLARE_SERVICE_FACTORY( SimulationSvc )



// -----------------------------------------------------------------------
// build a standard string from a DOMString
// -----------------------------------------------------------------------
const std::string dom2Std (const XMLCh* domString) {
  char *cString = xercesc::XMLString::transcode(domString);
  std::string stdString;
  if (cString) {
    stdString = cString;
    xercesc::XMLString::release(&cString);
  }
  return stdString;
}


// -----------------------------------------------------------------------
// Standard Constructor
// ------------------------------------------------------------------------
SimulationSvc::SimulationSvc (const std::string& name, ISvcLocator* svc) :
  Service (name, svc) {
  declareProperty ("SimulationDbLocation", m_simDbLocation="empty" );
}

SimulationSvc::~SimulationSvc() 
{
  for (AttributeSet::iterator it=m_attributeSet.begin();
       it!= m_attributeSet.end();it++)  delete (*it).second;
  m_attributeSet.clear();

  for (Dict::iterator itt=m_logvol2Sim.begin();
       itt!= m_logvol2Sim.end();itt++)  delete (*itt).second;
  m_logvol2Sim.clear();

  m_regionsDefs.clear();
}

// -----------------------------------------------------------------------
// Initialize the service.
// -----------------------------------------------------------------------
StatusCode SimulationSvc::initialize() {
  // Before anything we have to initialize grand mother
  StatusCode status = Service::initialize();
  if (!status.isSuccess()) {
    return status;  
  }
  if( m_simDbLocation.empty() || "empty" == m_simDbLocation ) { 
    if ( 0 != getenv("GAUSSROOT") ) {
      m_simDbLocation  = getenv("GAUSSROOT");
      m_simDbLocation += "/xml/Simulation.xml";
    } else {
      m_simDbLocation = "";
    }
  }
  // loads the color set
  reload();
  // returns
  return StatusCode::SUCCESS;
}

// -----------------------------------------------------------------------
// clear
// ------------------------------------------------------------------------
void SimulationSvc::clear () {
  m_attributeSet.clear();
  m_logvol2Sim.clear();
  m_regionsDefs.clear();
}

// -----------------------------------------------------------------------
// reload
// ------------------------------------------------------------------------
void SimulationSvc::reload () {
  MsgStream msg (msgSvc(), "SimulationSvc");

  // erases the old set of attributes
  clear();

  // gets the XmlSvc
  IXmlSvc* xmlSvc;
  StatusCode status = serviceLocator()->service("XmlCnvSvc", xmlSvc, true);
  if (status.isFailure()) {
    msg << MSG::ERROR << "Unable to get XmlCnvSvc. The simulation "
        << "attributes will not be loaded." << endmsg;
    return;
  }

  msg << MSG::INFO << "Loading simulation attributes file \""
      << m_simDbLocation << "\" ..." << endmsg;

  // parses the file containing the simatt definitions
  IOVDOMDocument* iovDoc = xmlSvc->parse(m_simDbLocation.c_str());
  if (!iovDoc) {
    msg << MSG::ERROR << "Unable to parse file " << m_simDbLocation
        << ". The simulation attributes will not be loaded." << endmsg;
    return;
  }
  xercesc::DOMDocument* document = iovDoc->getDOM();
  if (0 == document) {
    msg << MSG::ERROR << "Unable to parse file " << m_simDbLocation
        << ". The simulation attributes will not be loaded." << endmsg;
    xmlSvc->releaseDoc(iovDoc);
    return;
  }

  // go through the tree of elements and fill in the attribute sets
  const XMLCh* SimStr = xercesc::XMLString::transcode("SimAtt");
  xercesc::DOMNodeList* domAttrList = document->getElementsByTagName(SimStr);
  unsigned int i;
  const XMLCh* nameStr = xercesc::XMLString::transcode("name");
  const XMLCh* mASStr  = xercesc::XMLString::transcode("maxAllowedStep");
  const XMLCh* mTRStr  = xercesc::XMLString::transcode("maxTrackLength");
  const XMLCh* mTStr   = xercesc::XMLString::transcode("maxTime");
  const XMLCh* mEkStr  = xercesc::XMLString::transcode("minEkine");
  const XMLCh* mRStr   = xercesc::XMLString::transcode("minRange");
    
  for (i = 0; i < domAttrList->getLength(); i++) {
    xercesc::DOMNode* attrNode = domAttrList->item(i);
    xercesc::DOMElement* attr = (xercesc::DOMElement *) attrNode;

    std::string name = dom2Std (attr->getAttribute (nameStr));
    std::string mASAttribute = dom2Std (attr->getAttribute (mASStr));
    std::string mTRAttribute = dom2Std (attr->getAttribute (mTRStr));
    std::string mTAttribute = dom2Std (attr->getAttribute (mTStr));
    std::string mEkAttribute = dom2Std (attr->getAttribute (mEkStr));
    std::string mRAttribute = dom2Std (attr->getAttribute (mRStr));

    // computes the values
    double maxAllowedStep = -1.0;
    double maxTrackLength = -1.0;
    double maxTime = -1.0;
    double minEkine = -1.0;
    double minRange = -1.0;
    if (!mASAttribute.empty()) {
      maxAllowedStep = xmlSvc->eval(mASAttribute, false);
    }
    if (!mTRAttribute.empty()) {
      maxTrackLength = xmlSvc->eval(mTRAttribute, true);
    }
    if (!mTAttribute.empty()) {
      maxTime = xmlSvc->eval(mTAttribute, true);
    }
    if (!mEkAttribute.empty()) {
      minEkine = xmlSvc->eval(mEkAttribute, true);
      }
    if (!mRAttribute.empty()) {
      minRange = xmlSvc->eval(mRAttribute, true);
    }
    
    // creates the attribute and register it
    m_attributeSet[name] =
      new SimAttribute
      (maxAllowedStep, maxTrackLength, maxTime, minEkine, minRange);
  }
  xercesc::XMLString::release((XMLCh**) &mASStr);
  xercesc::XMLString::release((XMLCh**) &mTRStr);
  xercesc::XMLString::release((XMLCh**) &mTStr);
  xercesc::XMLString::release((XMLCh**) &mEkStr);
  xercesc::XMLString::release((XMLCh**) &mRStr);   
  xercesc::XMLString::release((XMLCh**) &SimStr);
  

  // go through the tree of elements and fill in the logvol2Sim map
  const XMLCh* LVStr = xercesc::XMLString::transcode("LogVols");
  const XMLCh* ItemStr = xercesc::XMLString::transcode("Item");
  
  xercesc::DOMNodeList* domLogvolsList = document->getElementsByTagName(LVStr);
  if (domLogvolsList->getLength() > 0) {
    xercesc::DOMNode* logvolsNode = domLogvolsList->item(0);
    xercesc::DOMElement* logvolsElement = (xercesc::DOMElement *) logvolsNode;

    xercesc::DOMNodeList* domLogvolList = logvolsElement->getElementsByTagName(ItemStr);
    unsigned int i;
    const XMLCh* partStr = xercesc::XMLString::transcode("particle");
    const XMLCh* attrStr = xercesc::XMLString::transcode("attr");
    const XMLCh* CutStr  = xercesc::XMLString::transcode("Cut");
    
    for (i = 0; i < domLogvolList->getLength(); i++) {
      xercesc::DOMNode* logvolNode = domLogvolList->item(i);
      xercesc::DOMElement* logvol = (xercesc::DOMElement *) logvolNode;

      std::string name = dom2Std (logvol->getAttribute (nameStr));

      PartAttr* partattr = new PartAttr();
      m_logvol2Sim[name]=partattr;

      xercesc::DOMNodeList* domLogvolNode = logvol->getElementsByTagName(CutStr);
      unsigned int j;
      for (j = 0; j < domLogvolNode->getLength(); j++) {
        xercesc::DOMNode* cutNode = domLogvolNode->item(j);
        xercesc::DOMElement* cut = (xercesc::DOMElement *) cutNode;
        std::string dompart = dom2Std (cut->getAttribute(partStr));
        int particle = (int) xmlSvc->eval(dompart, false);
        std::string attr = dom2Std (cut->getAttribute (attrStr));
        // register the association
        partattr->operator[](particle) = m_attributeSet[attr];
      }
    }
    xercesc::XMLString::release((XMLCh**) &partStr);
    xercesc::XMLString::release((XMLCh**) &attrStr);
    xercesc::XMLString::release((XMLCh**) &CutStr);
  }
   
    xercesc::XMLString::release((XMLCh**) &LVStr);
    xercesc::XMLString::release((XMLCh**) &ItemStr);
       
  ///////////////////////////////////////////////////////////
  // the part below deals with the production cuts per region

  //create a temporary map to store ProductionCut definitions
  std::map<std::string, Prcuts> regcut;

  // go through the tree of elements and fill in the ProductionCut sets
  const XMLCh* ProdCutStr = xercesc::XMLString::transcode("ProductionCut");
  
  xercesc::DOMNodeList* domPrCutList = document->getElementsByTagName(ProdCutStr);
  const XMLCh* GaStr = xercesc::XMLString::transcode("gammaCut");
  const XMLCh* ElStr = xercesc::XMLString::transcode("electronCut");
  const XMLCh* PoStr = xercesc::XMLString::transcode("positronCut");
  const XMLCh* PrStr = xercesc::XMLString::transcode("ProtonCut");
  const XMLCh* APStr = xercesc::XMLString::transcode("antiProtonCut");
  const XMLCh* NeStr = xercesc::XMLString::transcode("neutronCut");
  const XMLCh* ANStr = xercesc::XMLString::transcode("antiNeutronCut");
  
  unsigned int ii;
  for (ii = 0; ii < domPrCutList->getLength(); ii++) {
    xercesc::DOMNode* prcutNode = domPrCutList->item(ii);
    xercesc::DOMElement* prcut = (xercesc::DOMElement *) prcutNode;
    std::string name = dom2Std (prcut->getAttribute (nameStr));
    std::string atrgammacut = dom2Std (prcut->getAttribute (GaStr));
    std::string atrelectroncut = dom2Std (prcut->getAttribute (ElStr));
    std::string atrpositroncut = dom2Std (prcut->getAttribute (PoStr));
    std::string atrprotoncut = dom2Std (prcut->getAttribute (PrStr));
    std::string atraprotoncut = dom2Std (prcut->getAttribute (APStr));
    std::string atrneutroncut = dom2Std (prcut->getAttribute (NeStr));
    std::string atraneutroncut = dom2Std (prcut->getAttribute (ANStr));

    Prcuts tempcuts;

    // set default values
    tempcuts.gammacut = -1.0;
    tempcuts.electroncut = -1.0;
    tempcuts.positroncut = -1.0;
    tempcuts.protoncut = -1.0;
    tempcuts.aprotoncut = -1.0;
    tempcuts.neutroncut = -1.0;
    tempcuts.aneutroncut = -1.0;
    // get values (if any)

    if (!atrgammacut.empty()) {
      tempcuts.gammacut = xmlSvc->eval(atrgammacut, false);
    }
    if (!atrelectroncut.empty()) {
      tempcuts.electroncut = xmlSvc->eval(atrelectroncut, true);
    }
    if (!atrpositroncut.empty()) {
      tempcuts.positroncut = xmlSvc->eval(atrpositroncut, true);
    }
    if (!atrprotoncut.empty()) {
      tempcuts.protoncut = xmlSvc->eval(atrprotoncut, true);
      }
    if (!atraprotoncut.empty()) {
      tempcuts.aprotoncut = xmlSvc->eval(atraprotoncut, true);
    }
    if (!atrneutroncut.empty()) {
      tempcuts.neutroncut = xmlSvc->eval(atrneutroncut, true);
      }
    if (!atraneutroncut.empty()) {
      tempcuts.aneutroncut = xmlSvc->eval(atraneutroncut, true);
    }
    
    // fill in the temporary map 
    regcut[name] = tempcuts;
  }
  xercesc::XMLString::release((XMLCh**) &GaStr);
  xercesc::XMLString::release((XMLCh**) &ElStr); 
  xercesc::XMLString::release((XMLCh**) &PoStr);
  xercesc::XMLString::release((XMLCh**) &PrStr); 
  xercesc::XMLString::release((XMLCh**) &APStr);
  xercesc::XMLString::release((XMLCh**) &NeStr);
  xercesc::XMLString::release((XMLCh**) &ANStr);
  xercesc::XMLString::release((XMLCh**) &ProdCutStr);
  
  // go through the tree of elements 
  // the following few lines fixed  by SE 3-11-2005.
  // the fix is to avoid the mixing the terms Region and Regions.
  //
  const XMLCh* RegnsStr = xercesc::XMLString::transcode("Regions");
  const XMLCh* RegStr = xercesc::XMLString::transcode("Region");
  const XMLCh* VolStr = xercesc::XMLString::transcode("Volume");
  const XMLCh* ProdStr = xercesc::XMLString::transcode("prodcut");
 
  xercesc::DOMNodeList* domRegionsList = document->getElementsByTagName(RegnsStr);

  msg << MSG::DEBUG << "SimSvc Production  regions  list length \""
      << (int) domRegionsList ->getLength() << "\" ..." << endmsg;
  // only 1 regions element is allowed in the following. A regions can have
  // many many region elements. Each region element may have different volumes.
  // end of fixes by SE.
  if (domRegionsList->getLength() > 0) {
    xercesc::DOMNode* regionsNode = domRegionsList->item(0);
    xercesc::DOMElement* regionsElement = (xercesc::DOMElement*) regionsNode;
    xercesc::DOMNodeList* domRegionList = regionsElement->getElementsByTagName(RegStr);
    msg << MSG::DEBUG << "SimSvc Production  region  list length \""
        << (int) domRegionList ->getLength() << "\" ..." << endmsg;
    
    unsigned int i;
    for (i = 0; i < domRegionList->getLength(); i++) {
      xercesc::DOMNode* regionNode = domRegionList->item(i);
      xercesc::DOMElement* region = (xercesc::DOMElement*) regionNode;
      std::string regname = dom2Std (region->getAttribute (nameStr));
      std::string prcut = dom2Std (region->getAttribute (ProdStr));

      msg << MSG::DEBUG << "SimSvc region name prcut  \""
          << regname<<"  "<<prcut << "\" ..." << endmsg;

      xercesc::DOMNodeList* domRegionNode = region->getElementsByTagName(VolStr);
      unsigned int j;
      std::vector<std::string> volvect;
      
      for (j = 0; j < domRegionNode->getLength(); j++) 
        {
        xercesc::DOMNode* volNode = domRegionNode->item(j);
        xercesc::DOMElement* vol = (xercesc::DOMElement*) volNode;

        std::string volname = dom2Std (vol->getAttribute (nameStr));
        msg << MSG::DEBUG << "SimSvc vol name    \""
            << volname <<"  "<< "\" ..." << endmsg;
  
        volvect.push_back(volname);        
        }
      
      RegionCuts rcut(regname,
                      volvect,
                      regcut[prcut].gammacut,
                      regcut[prcut].electroncut,
                      regcut[prcut].positroncut,
                      regcut[prcut].protoncut,
                      regcut[prcut].aprotoncut,
                      regcut[prcut].neutroncut,
                      regcut[prcut].aneutroncut);
      m_regionsDefs.push_back(rcut);
    }
  }
  xercesc::XMLString::release((XMLCh**) &nameStr); 
  xercesc::XMLString::release((XMLCh**) &RegStr);
  xercesc::XMLString::release((XMLCh**) &VolStr);
  xercesc::XMLString::release((XMLCh**) &ProdStr);

  xmlSvc->releaseDoc(iovDoc);

}
 

// -----------------------------------------------------------------------
//  hasSimAttribute (ILVolume*)
// -----------------------------------------------------------------------
bool SimulationSvc::hasSimAttribute (const ILVolume* vol) const {
  if (0 != vol) {
    // try first to find an attribute associated directly to the logical volume
    std::string bnn = vol->name();
    Dict::const_iterator it = m_logvol2Sim.find (bnn);
    if (it != m_logvol2Sim.end()) {
      return true;
    }    
  }
  return false;
}

// -----------------------------------------------------------------------
//  hasSimAttribute (std::string)
// -----------------------------------------------------------------------
bool SimulationSvc::hasSimAttribute (const std::string volname) const {

  // try first to find an attribute associated directly to the logical volume
  Dict::const_iterator it = m_logvol2Sim.find (volname);
  if (it != m_logvol2Sim.end()) {
    return true;
  }    
  return false;
}

// -----------------------------------------------------------------------
//  simAttribute (ILVolume*)
// -----------------------------------------------------------------------
const SimulationSvc::PartAttr*
SimulationSvc::simAttribute (const ILVolume* vol) const {
  PartAttr* part = 0;
  
  if (0 != vol) {
    // try first to find an attribute associated directly to the logical volume
    std::string bnn = vol->name();
    Dict::const_iterator it = m_logvol2Sim.find (bnn);
    if (it != m_logvol2Sim.end()) 
      {
        part=it->second;
      } 
    else 
      {
        MsgStream msg (msgSvc(), name());
        msg << MSG::WARNING << "No SimAttribute for " 
            << vol->name() << endmsg;
      } 
  }
  return part;
}

// -----------------------------------------------------------------------
//  simAttribute (std::string)
// -----------------------------------------------------------------------
const SimulationSvc::PartAttr*
SimulationSvc::simAttribute (std::string volname) const {
  PartAttr* part = 0;
  
  // try first to find an attribute associated directly to the logical volume
  Dict::const_iterator it = m_logvol2Sim.find (volname);
  if (it != m_logvol2Sim.end()) 
    {
      part=it->second;
    } 
  else 
    {
      MsgStream msg (msgSvc(), name());
      msg << MSG::WARNING << "No SimAttribute for " 
          << volname << endmsg;
    }
  return part;
}

//------------------------------------------------------------------------
// regionsDefs()
//------------------------------------------------------------------------

const std::vector<RegionCuts>* SimulationSvc::regionsDefs () const
{
  return &m_regionsDefs;
}


// -----------------------------------------------------------------------
// Query interface
// -----------------------------------------------------------------------
StatusCode
SimulationSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
  if (IID_ISimulationSvc.versionMatch(riid))  {
    *ppvInterface = (ISimulationSvc*)this;
  } else {
    // Interface is not directly availible: try out a base class
    return Service::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}
