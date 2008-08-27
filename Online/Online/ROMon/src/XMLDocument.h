#ifndef XMLDOCUMENT_H
#define XMLDOCUMENT_H 1

#include "XMLHelpers.h"

#include "xercesc/dom/DOMElement.hpp"
#include "xercesc/dom/DOMDocument.hpp"
#include "xercesc/sax/ErrorHandler.hpp"
#include "xercesc/sax/InputSource.hpp"
#include "xercesc/parsers/XercesDOMParser.hpp"

namespace XML {

  struct XMLDocument {
    typedef const std::string& CTSR;
    xercesc::DOMDocument     *m_doc;
    xercesc::XercesDOMParser *m_parser;
    xercesc::ErrorHandler    *m_errHdlr;
    bool                      m_rdOnly;
    mutable bool              m_update;
  public:
    xercesc::DOMDocument* getDoc(bool throw_if_no_exists=true)  const;
    std::string getfile(const std::string& fname, bool create);
    void printError(CSTR msg, bool throw_exc=true) const;
  
    xercesc::DOMNode* element(CSTR fid, bool print_err=true)  const;
    xercesc::DOMNode* child(xercesc::DOMNode* par, CSTR tag, CSTR attr="", CSTR val="") const;

  public:
    XMLDocument();
    virtual ~XMLDocument();
    int parse(const std::string& tag, xercesc::InputSource& input);
    int parseFile(const std::string& fname);
    /// Parse a memory buffer containing the XML information
    int parseBuffer(const std::string& tag, const void* address, size_t len);

    /// Save DOM catalog to file
    virtual void commit(const std::string& fname);
    /// Check if the catalog is read-only
    virtual bool readOnly() const                 { return m_rdOnly;                }
    /// Check if the catalog should be updated 
    virtual bool dirty() const                    { return m_update;                }
  };
}

#endif // XMLDOCUMENT_H
