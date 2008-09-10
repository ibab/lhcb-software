#include "xercesc/dom/DOM.hpp"
#include "xercesc/util/XMLString.hpp"

#include <string>

namespace XML {

  typedef const std::string& CSTR;
  inline std::string _toString(const XMLCh *toTranscode)  {
    char * buff = xercesc::XMLString::transcode(toTranscode);
    std::string tmp(buff==0 ? "" : buff);
    xercesc::XMLString::release(&buff);
    return tmp;
  }
  struct XMLStr  {
    XMLCh* m_xml;
    XMLStr(CSTR c)                                      { m_xml=xercesc::XMLString::transcode(c.c_str());  }
    ~XMLStr()                                           { if (m_xml) xercesc::XMLString::release(&m_xml);  }
    operator const XMLCh*() const                       { return m_xml;                               }
  };
  struct XMLTag : public XMLStr {
    std::string m_str;
    XMLTag(CSTR s) : XMLStr(s), m_str(s)                {                                             }
    ~XMLTag()                                           {  }
    operator CSTR () const                              { return m_str;                               }
  };
  inline bool operator==(const XMLTag& b, CSTR c)       {  return c==b.m_str;                         }
  inline bool operator==(CSTR c, const XMLTag& b)       {  return c==b.m_str;                         }

  struct XMLElement {
    xercesc::DOMElement* m_node;
    XMLElement() : m_node(0)                            {                                             }
    XMLElement(xercesc::DOMNode* n);
    operator bool()  const                              { return 0 != m_node;                                        }
    XMLElement& operator ()(xercesc::DOMNode* n);
    operator xercesc::DOMNode* () const                 { return m_node;                                             }
    operator xercesc::DOMElement* () const              { return m_node;                                             }
    xercesc::DOMElement* operator->() const             { return m_node;                                             }
    std::string attr(const XMLTag& tag) const           { return m_node ? _toString(m_node->getAttribute(tag)) : ""; }
    std::string attr(CSTR tag)  const                   { return attr(XMLTag(tag));                                  }
    std::string tag() const                             { return m_node ? _toString(m_node->getTagName()) : "";      }
    std::string value() const                           { return m_node ? _toString(m_node->getNodeValue()) : "";    }
  };
  struct XMLCollection : public XMLElement  {
    std::string m_tag;
    XMLCollection(xercesc::DOMNode* n, bool use_children=true, const std::string& tag="");
    XMLCollection& operator++();
  };
}
