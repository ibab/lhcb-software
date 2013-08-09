#include "ROMon/XMLHelpers.h"
using namespace XML;
using namespace xercesc;

XMLElement::XMLElement(DOMNode* n) : m_node((DOMElement*)n) {
  if ( m_node && m_node->getNodeType() != DOMNode::ELEMENT_NODE ) m_node = 0;
}

XMLElement& XMLElement::operator()(DOMNode* n) {
  if ( 0 != (m_node=(DOMElement*)n) && m_node->getNodeType() != DOMNode::ELEMENT_NODE ) m_node = 0;
  return *this;
}

XMLCollection::XMLCollection(DOMNode* n, bool use_children, const std::string& t) : XMLElement(n), m_tag(t) {
  if ( use_children )  {
    if ( m_node ) m_node = (DOMElement*)m_node->getFirstChild();
    if ( m_node && m_node->getNodeType() != DOMNode::ELEMENT_NODE ) ++(*this);
    if ( !m_tag.empty() && tag()!=m_tag ) ++(*this);
  }
}

XMLCollection& XMLCollection::operator++()   {
  while(m_node)  {
    m_node = (DOMElement*)m_node->getNextSibling();
    if ( m_node && m_node->getNodeType() == DOMNode::ELEMENT_NODE ) {
      if (  !m_tag.empty() && tag() == m_tag ) return *this;
      else if ( m_tag.empty() ) return *this;
    }
  }
  return *this;
}
