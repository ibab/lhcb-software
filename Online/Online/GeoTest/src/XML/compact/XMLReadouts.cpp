#include "XML/compact/XMLSegmentations.h"
#include "XML/compact/XMLReadouts.h"
#include "XML/lcdd/XMLObjects.h"

using namespace std;
using namespace DetDesc::XML;

/// Constructor to be used when reading the already parsed DOM tree
Readout::Readout(Handle_t e) : RefElement(e) {
}

/// Constructor to be used when reading the already parsed DOM tree
Readout::Readout(const RefElement& e) : RefElement(e) {
}

/// Initializing constructor
Readout::Readout(const Document& doc, Handle_t h) : RefElement(doc,Tag_readout,Tag_empty)
{
  if ( h )  {
    Handle_t id  = h.child(Tag_id,false);
    Handle_t seg = h.child(Tag_segmentation,false);
    if ( h.hasAttr(Attr_name) )
      setAttr(Attr_name,h.attr<cpXMLCh>(Attr_name));
    if ( seg )
      setSegmentation(seg);
    if ( id )
      m_element.append(id.clone(doc,true));
  }
}

const XMLCh* Readout::name() const  {
  return m_element ? attr<cpXMLCh>(Attr_name) : 0;
}

Handle_t Readout::idSpec() const   {
  return m_element.child(Tag_id);
}

void Readout::setupIDDescriptor(Handle_t h)  const   {
  Handle_t id = m_element.child(Tag_id,false);
  if ( h )  {
    if ( id ) 
      m_element->replaceChild(id,clone(h,true));
    else 
      m_element.append(clone(h,true));
    return;
  }
  else if ( id )  {
    m_element->removeChild(id);
  }
}

void Readout::setSegmentation(Handle_t h)   const  {
  Handle_t seg = m_element.child(Tag_segmentation,false);
  if ( h )  {
    Handle_t c = clone(h,true);
    if ( seg ) m_element->replaceChild(seg,c);
    else m_element.append(c);
    return;
  }
  throw runtime_error("Readout::setSegmentation: Cannot assign segmentation [Invalid Handle]");
}

Handle_t Readout::segmentation() const  {
  return m_element.child(Tag_segmentation,false);
}
