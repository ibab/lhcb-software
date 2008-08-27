#include "XMLHelpers.h"
using namespace XML;
using namespace xercesc;

XMLElement::XMLElement(DOMNode* n) : m_node((DOMElement*)n) {
  if ( m_node && m_node->getNodeType() != DOMNode::ELEMENT_NODE ) m_node = 0;
}

XMLCollection::XMLCollection(DOMNode* n, bool use_children) : XMLElement(n) {
  if ( use_children )  {
    if ( m_node ) m_node = (DOMElement*)m_node->getFirstChild();
    if ( m_node && m_node->getNodeType() != DOMNode::ELEMENT_NODE ) ++(*this);
  }
}

XMLCollection& XMLCollection::operator++()   {
  while(m_node)  {
    m_node = (DOMElement*)m_node->getNextSibling();
    if ( m_node && m_node->getNodeType() == DOMNode::ELEMENT_NODE ) {
      return *this;
    }
  }
  return *this;
}
