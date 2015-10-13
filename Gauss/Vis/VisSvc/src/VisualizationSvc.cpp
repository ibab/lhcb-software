// Include Files
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"

#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMNode.hpp>

#include <boost/regex.hpp>

using namespace xercesc;

#include "XmlTools/IXmlSvc.h"

#include <DetDesc/Material.h>
#include <DetDesc/ILVolume.h>
#include <DetDesc/LogVolumeException.h>

#include "VisualizationSvc.h"


// -----------------------------------------------------------------------
// Instantiation of a static factory class used by clients to create
// instances of this service
// -----------------------------------------------------------------------

DECLARE_SERVICE_FACTORY(VisualizationSvc)

// -----------------------------------------------------------------------
// build a standard string from a DOMString
// -----------------------------------------------------------------------
  const std::string dom2Std (const XMLCh* aString) {
  char *cString = xercesc::XMLString::transcode(aString);
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
VisualizationSvc::VisualizationSvc (const std::string& name, ISvcLocator* svc) :
  Service (name, svc) {
  declareProperty ("ColorDbLocation", m_colorDbLocation="empty" );
}

// -----------------------------------------------------------------------
// Initialize the service.
// -----------------------------------------------------------------------
StatusCode VisualizationSvc::initialize() {
  // Before anything we have to initialize grand mother
  StatusCode status = Service::initialize();
  if (!status.isSuccess()) {
    return status;
  }
  if( m_colorDbLocation.empty() || "empty" == m_colorDbLocation ) {
    if ( 0 != getenv("XMLVISROOT") ) {
      m_colorDbLocation  = getenv("XMLVISROOT");
      m_colorDbLocation += "/xml/colors.xml";
    } else {
      m_colorDbLocation = "";
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
void VisualizationSvc::clear () {
  m_attributeSet.clear();
  m_material2Vis.clear();
  m_logvol2Vis.clear();
  m_logvol_regex_2Vis.clear();
}

// -----------------------------------------------------------------------
// reload
// ------------------------------------------------------------------------
void VisualizationSvc::reload () {
  MsgStream log (msgSvc(), "VisualizationSvc");

  // erases the old set of attributes
  clear();

  // gets the XmlSvc
  IXmlSvc* xmlSvc;
  StatusCode status = serviceLocator()->service("XmlCnvSvc", xmlSvc, true);
  if (status.isFailure()) {
    log << MSG::ERROR << "Unable to get XmlCnvSvc. The visualization "
        << "attributes will not be loaded." << endmsg;
    return;
  }
  log << MSG::INFO << "Loading visualization attributes file \""
      << m_colorDbLocation << "\" ..." << endmsg;

  // parses the file containing the color definitions
  IOVDOMDocument* iovDoc = xmlSvc->parse(m_colorDbLocation.c_str());
  if (!iovDoc) {
    log << MSG::ERROR << "Unable to parse file " << m_colorDbLocation
        << ". The visualization attributes will not be loaded." << endmsg;
    return;
  }
  DOMDocument* document = iovDoc->getDOM();
  if (!document) {
    log << MSG::ERROR << "Document does not exist " << m_colorDbLocation
        << ". The visualization attributes will not be loaded." << endmsg;
    xmlSvc->releaseDoc(iovDoc);
    return;
  }

  // go through the tree of elements and fill in the attribute sets
  XMLCh* xs = xercesc::XMLString::transcode("VisAtt");
  DOMNodeList* domAttrList = document->getElementsByTagName(xs);
  if(!domAttrList) {
    xmlSvc->releaseDoc(iovDoc);
    return;
  }

  xercesc::XMLString::release(&xs);
  unsigned int i;
  for (i = 0; i < domAttrList->getLength(); i++) {
    DOMNode* attrNode = domAttrList->item(i);
    DOMElement* attr = (DOMElement*) attrNode;

    xs = xercesc::XMLString::transcode("name");
    std::string name = dom2Std (attr->getAttribute (xs));
    xercesc::XMLString::release(&xs);

    xs = xercesc::XMLString::transcode("visible");
    std::string visibleAttribute = dom2Std (attr->getAttribute (xs));
    xercesc::XMLString::release(&xs);

    xs = xercesc::XMLString::transcode("opened");
    std::string openedAttribute = dom2Std (attr->getAttribute (xs));
    xercesc::XMLString::release(&xs);

    xs = xercesc::XMLString::transcode("mode");
    std::string modeAttribute = dom2Std (attr->getAttribute (xs));
    xercesc::XMLString::release(&xs);

    // computes the values
    VisAttribute::Visibility visible = VisAttribute::NO_VISIBILITY;
    if ("Yes" == visibleAttribute) {
      visible = VisAttribute::VISIBLE;
    } else if ("No" == visibleAttribute) {
      visible = VisAttribute::NOT_VISIBLE;
    }
    VisAttribute::OpenStatus opened = VisAttribute::NO_STATUS;
    if ("Yes" == openedAttribute) {
      opened = VisAttribute::OPENED;
    } else if ("No" == openedAttribute) {
      opened = VisAttribute::CLOSED;
    }
    VisAttribute::DisplayMode mode = VisAttribute::NO_MODE;
    if ("Plain" == modeAttribute) {
      mode = VisAttribute::PLAIN;
    } else if ("WireFrame" == modeAttribute) {
      mode = VisAttribute::WIRE_FRAME;
    }

    // Looks whether a color node exists
    Color color;
    xs = xercesc::XMLString::transcode("Color");
    DOMNodeList* domColorList = attr->getElementsByTagName(xs);
    xercesc::XMLString::release(&xs);

    if (domColorList && domColorList->getLength() > 0) {
      DOMNode* colorNode = domColorList->item(0);
      DOMElement* colorElement = (DOMElement*) colorNode;

      xs = xercesc::XMLString::transcode("R");
      std::string RAttribute = dom2Std (colorElement->getAttribute (xs));
      xercesc::XMLString::release(&xs);

      xs = xercesc::XMLString::transcode("G");
      std::string GAttribute = dom2Std (colorElement->getAttribute (xs));
      xercesc::XMLString::release(&xs);

      xs = xercesc::XMLString::transcode("B");
      std::string BAttribute = dom2Std (colorElement->getAttribute (xs));
      xercesc::XMLString::release(&xs);

      xs = xercesc::XMLString::transcode("A");
      std::string AAttribute = dom2Std (colorElement->getAttribute (xs));
      xercesc::XMLString::release(&xs);

      // computes the values
      float red = 0.0;
      float green = 0.0;
      float blue = 0.0;
      float alpha = 0.0;
      if (!RAttribute.empty()) {
        red = (float)xmlSvc->eval(RAttribute, false);
      }
      if (!GAttribute.empty()) {
        green = (float)xmlSvc->eval(GAttribute, false);
      }
      if (!BAttribute.empty()) {
        blue = (float)xmlSvc->eval(BAttribute, false);
      }
      if (!AAttribute.empty()) {
        alpha = (float)xmlSvc->eval(AAttribute, false);
      }

      // creates the color
      color = Color (red, green, blue, alpha);
    }

    // creates the attribute and register it
    m_attributeSet[name] = VisAttribute(visible, opened, mode, color);
  }

  // go through the tree of elements and fill in the material2Vis map
  xs = xercesc::XMLString::transcode("Materials");
  DOMNodeList* domMaterialsList = document->getElementsByTagName(xs);
  xercesc::XMLString::release(&xs);

  if (domMaterialsList && domMaterialsList->getLength() > 0) {
    DOMNode* materialsNode = domMaterialsList->item(0);
    DOMElement* materialsElement = (DOMElement*) materialsNode;

    xs = xercesc::XMLString::transcode("Item");
    DOMNodeList* domMaterialList = materialsElement->getElementsByTagName(xs);
    xercesc::XMLString::release(&xs);

    if(domMaterialList) {
      for (unsigned int ii = 0; ii < domMaterialList->getLength(); ii++) {
        DOMNode* materialNode = domMaterialList->item(ii);
        DOMElement* material = (DOMElement*) materialNode;

        xs = xercesc::XMLString::transcode("name");
        std::string name = dom2Std (material->getAttribute (xs));
        xercesc::XMLString::release(&xs);

        xs = xercesc::XMLString::transcode("attr");
        std::string attr = dom2Std (material->getAttribute (xs));
        xercesc::XMLString::release(&xs);

        // register the association
        m_material2Vis[name] = attr;
      }
    }
  }

  // go through the tree of elements and fill in the logvol2Vis map
  xs = xercesc::XMLString::transcode("LogVols");
  DOMNodeList* domLogvolsList = document->getElementsByTagName(xs);
  xercesc::XMLString::release(&xs);

  if (domLogvolsList->getLength() > 0) {
    DOMNode* logvolsNode = domLogvolsList->item(0);
    DOMElement* logvolsElement = (DOMElement*) logvolsNode;

    xs = xercesc::XMLString::transcode("Item");
    DOMNodeList* domLogvolList = logvolsElement->getElementsByTagName(xs);
    xercesc::XMLString::release(&xs);

    for (unsigned int ii = 0; ii < domLogvolList->getLength(); ++ii) {
      DOMNode* logvolNode = domLogvolList->item(ii);
      DOMElement* logvol = (DOMElement*) logvolNode;

      xs = xercesc::XMLString::transcode("name");
      std::string sname = dom2Std (logvol->getAttribute (xs));
      xercesc::XMLString::release(&xs);

      xs = xercesc::XMLString::transcode("regex");
      std::string sregex = dom2Std (logvol->getAttribute (xs));
      xercesc::XMLString::release(&xs);

      xs = xercesc::XMLString::transcode("attr");
      std::string attr = dom2Std (logvol->getAttribute (xs));
      xercesc::XMLString::release(&xs);

      // register the association
      if(sname.size()) {
        m_logvol2Vis[sname] = attr;
      } else if(sregex.size()) {
        m_logvol_regex_2Vis[sregex] = attr;
      } else {
        MsgStream log(msgSvc(), name());
        log << MSG::WARNING << "LogVol with empty name or regex attribute."
            << endmsg;
      }
    }
  }

  xmlSvc->releaseDoc(iovDoc);

}

// -----------------------------------------------------------------------
//  visAttribute
// -----------------------------------------------------------------------
const VisAttribute
VisualizationSvc::visAttribute (const Material* mat) const {
  VisAttribute attr;

  if (0 != mat) {
    Dictionnary::const_iterator it =
      m_material2Vis.find (mat->registry()->identifier());
    if (it != m_material2Vis.end()) {
      AttributeSet::const_iterator it2 = m_attributeSet.find (it->second);
      if (it2 != m_attributeSet.end()) {
        attr = it2->second;
      } else {
        MsgStream log(msgSvc(), name());
        log << MSG::WARNING << "VisAttribute " << it->second << " unknown but"
            << " used for material " << mat->name() << "." << endmsg;
      }
    }
  }
  return attr;
}

// -----------------------------------------------------------------------
//  visAttribute
// -----------------------------------------------------------------------

const VisAttribute
VisualizationSvc::visAttribute (const ILVolume* vol) const {
  VisAttribute attr;

  if (0 != vol) {

    // try first to find an attribute associated directly to the logical volume
    const std::string bnn = vol->name();
    {
      Dictionnary::const_iterator it = m_logvol2Vis.find (bnn);
      if (it != m_logvol2Vis.end()) {
        AttributeSet::const_iterator it2 = m_attributeSet.find (it->second);
        if (it2 != m_attributeSet.end()) {
          attr = it2->second;
          // If the attribute is complete, just return
          if (attr.color().isValid() &&
              VisAttribute::NO_VISIBILITY != attr.visible() &&
              VisAttribute::NO_STATUS != attr.openStatus() &&
              VisAttribute::NO_MODE != attr.displayMode()) {
            return attr;
          }
        } else {
          MsgStream log(msgSvc(), name());
          log << MSG::WARNING << "VisAttribute " << it->second
              << " unknown but"
              << " used for logical volume " << vol->name() << "." << endmsg;
          return attr;
        }
      }
    }

    // look in LogVol Vis XMLs with regular expression :
    {
      Dictionnary::const_iterator it;
      for(it=m_logvol_regex_2Vis.begin();it!=m_logvol_regex_2Vis.end(); ++it) {
        boost::regex re(it->first);
        if(boost::regex_search(bnn,re)) {
          //printf("debug : for \"%s\", found \"%s\" with value \"%s\"\n",
          //       bnn.c_str(),it->first.c_str(),it->second.c_str());

          AttributeSet::const_iterator it2 = m_attributeSet.find (it->second);
          if (it2 != m_attributeSet.end()) {
            attr = it2->second;
            // If the attribute is complete, just return
            if (attr.color().isValid() &&
                VisAttribute::NO_VISIBILITY != attr.visible() &&
                VisAttribute::NO_STATUS != attr.openStatus() &&
                VisAttribute::NO_MODE != attr.displayMode()) {
              return attr;
            }
          } else {
            MsgStream log(msgSvc(), name());
            log << MSG::WARNING << "VisAttribute " << it->second
                << " unknown but"
                << " used for logical volume " << vol->name() << "." << endmsg;
            return attr;
          }

          break;
        }
      }
    }

    // either we don't have an attribute or it may be interesting to
    // complete it using the material
    try {
      const VisAttribute attr2 = visAttribute(vol->material());
      attr.merge(attr2);
    } catch (LogVolumeException ex) {
      // This occurs when the material retrieval raises an exception
      // We display the exception message and return an invalid color
      MsgStream log (msgSvc(), "VisualizationSvc");
      log << MSG::WARNING
          << "Exception received in VisualizationSvc::logvolColor : "
          << endmsg;
      ex.printOut (log);
      log << MSG::WARNING << "Visualization attribute will be corrupted."
          << endmsg;
    }
  }

  return attr;

}

// -----------------------------------------------------------------------
// Query interface
// -----------------------------------------------------------------------
StatusCode
VisualizationSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
  if (IID_IVisualizationSvc.versionMatch(riid))  {
    *ppvInterface = (IVisualizationSvc*)this;
    addRef();
    return StatusCode::SUCCESS;
  } else {
    // Interface is not directly availible: try out a base class
    return Service::queryInterface(riid, ppvInterface);
  }
}
