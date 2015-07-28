#include "ROMon/XMLDocument.h"
#include "ROMon/XMLHelpers.h"
#include "RTL/rtl.h"

#include "xercesc/framework/LocalFileFormatTarget.hpp"
#include "xercesc/framework/LocalFileInputSource.hpp"
#include "xercesc/framework/MemBufInputSource.hpp"
#include "xercesc/framework/URLInputSource.hpp"
#include "xercesc/sax/SAXParseException.hpp"
#include "xercesc/sax/EntityResolver.hpp"
#include "xercesc/parsers/XercesDOMParser.hpp"
#include "xercesc/util/PlatformUtils.hpp"
#include "xercesc/util/XercesDefs.hpp"
#include "xercesc/util/XMLUni.hpp"
#include "xercesc/util/XMLURL.hpp"
#include "xercesc/util/XMLString.hpp"
#include "xercesc/dom/DOM.hpp"

#include <memory>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <sys/types.h>
#include <sys/stat.h>

using namespace xercesc;
using namespace std;
using namespace XML;

namespace {
  struct __Init  {
    __Init() { 
      try { XMLPlatformUtils::Initialize();      }
      catch (const XMLException& e)   {
        cout << "Xerces-c error in initialization:" << _toString(e.getMessage()) << endl;
      }
      cout << "XMLPlatformUtils initialized...." << endl;
    }
    ~__Init() {
      XMLPlatformUtils::Terminate(); 
    }
    void verify() {}
  };
  __Init __In__;


  struct ErrHandler : public ErrorHandler    {
    /// Constructor
    ErrHandler() {}
    /// Reset errors (Noop)
    void resetErrors()                          {      }
    /// Warnings callback. Ignore them
    void warning(const SAXParseException& /* e */)    { }
    /// Error handler
    void error(const SAXParseException& e);
    /// Fatal error handler
    void fatalError(const SAXParseException& e);
  };
  struct DTDRedirect : public EntityResolver  {
    InputSource* resolveEntity(const XMLCh* const /* pubId */, const XMLCh* const /* sysId */)  {
      static const char* dtdID = "redirectinmem.dtd";
      static const char* dtd = \
        "\
        <!ELEMENT Cluster (Node*)>\
        <!ELEMENT Node (Missing_Tasks*,Found_Tasks*)>\
        <!ELEMENT Missing_Tasks (Task)*>\
        <!ELEMENT Found_Tasks (Task)*>\
        <!ELEMENT Task EMPTY>\
        <!ATTLIST Cluster       name     CDATA    #REQUIRED  \
                                status   CDATA    #IMPLIED> \
        <!ATTLIST Node          name     CDATA    #REQUIRED  \
                                status   CDATA    #IMPLIED> \
        <!ATTLIST Found_Tasks   count    NMTOKEN  #IMPLIED>  \
        <!ATTLIST Missing_Tasks count    NMTOKEN  #IMPLIED>  \
        <!ATTLIST Task          name     CDATA    #REQUIRED> \
        ";
      static const size_t len = strlen(dtd);
      return new MemBufInputSource((const XMLByte*)dtd,len,dtdID,false);
    }
  };
  void ErrHandler::error(const SAXParseException& e)  {
    string m(_toString(e.getMessage()));
    if (m.find("The values for attribute 'name' must be names or name tokens")!=string::npos ||
        m.find("The values for attribute 'ID' must be names or name tokens")!=string::npos   ||
        m.find("for attribute 'name' must be Name or Nmtoken")!=string::npos                 ||
        m.find("for attribute 'ID' must be Name or Nmtoken")!=string::npos      )
      return;
    string sys(_toString(e.getSystemId()));
    ::lib_rtl_output(LIB_RTL_WARNING,"Error at file \"%s\", line %d, column %d Message:%s",
                     sys.c_str(),e.getLineNumber(),e.getColumnNumber(),m.c_str());
  }
  void ErrHandler::fatalError(const SAXParseException& e)  {
    string m(_toString(e.getMessage()));
    string sys(_toString(e.getSystemId()));
    ::lib_rtl_output(LIB_RTL_WARNING,"Fatal Error at file \"%s\", line %d, column %d Message:%s",
                     sys.c_str(),e.getLineNumber(),e.getColumnNumber(),m.c_str());
    throw runtime_error( "Standard pool exception : Fatal Error on the DOM Parser" );
  }

  const XMLTag EmptyCatalog("<!-- Edited By POOL -->\n"
                            "<!DOCTYPE TASKMONITOR SYSTEM \"InMemory\">\n"
                            "<Cluster>\n"
                            "</Cluster>\n");
}

XML_Init::XML_Init() { __Init in;in.verify(); }
XML_Init::~XML_Init() {}

// ----------------------------------------------------------------------------
XMLDocument::XMLDocument() : m_doc(0),m_parser(0),m_errHdlr(0),m_update(false)
{
}

int XMLDocument::parseFile(const string& fname)   {
  string xmlFile = getfile(fname,false);
  auto_ptr<LocalFileInputSource> input(new LocalFileInputSource(XMLStr(xmlFile)));
  return parse(xmlFile,*input.get());
}

/// Parse a memory buffer containing the XML information
int XMLDocument::parseBuffer(const string& tag, const void* address, size_t len)  {
  XMLStr theTag(tag);
  auto_ptr<MemBufInputSource> input(new MemBufInputSource((XMLByte*)address,len,theTag));
  return parse(tag,*input.get());
}

int XMLDocument::parse(const string& tag,InputSource& input)   {
  try{
    if ( m_parser ) delete m_parser;
    m_parser = new XercesDOMParser;     
    m_parser->setValidationScheme(XercesDOMParser::Val_Auto);
    m_parser->setDoNamespaces(false);      
    //DTDRedirect dtdinmem;
    //m_parser->setEntityResolver(&dtdinmem);
    if ( ! m_errHdlr ) m_errHdlr = new ErrHandler();
    m_parser->setErrorHandler(m_errHdlr);
    m_parser->parse(input);
    /*
      if ( !tag.empty() )  {
      //m_parser->parse(tag.c_str());
      }
      else  {
      const string& s = EmptyCatalog;
      MemBufInputSource src((const XMLByte*)s.c_str(),s.length(),"MemCatalog");
      m_parser->parse(src);
      }
    */
    m_doc = m_parser->getDocument();
    return 1;
  }
  catch (const XMLException& e) {
    printError("XML parse error["+tag+"]: "+_toString(e.getMessage()));
  }
  catch (const DOMException& e) {
    printError("XML parse error["+tag+"]: "+_toString(e.getMessage()));
  }
  catch (...)  {
    printError("UNKNOWN XML parse error in file "+tag);
  }
  return 0;
}
// ----------------------------------------------------------------------------
XMLDocument::~XMLDocument()   {  
  if (m_parser) delete m_parser;
  m_parser = 0; 
  if (m_errHdlr) delete m_errHdlr;
  m_errHdlr = 0;
  m_doc = 0;
}
// ----------------------------------------------------------------------------
DOMDocument* XMLDocument::getDoc(bool throw_if_no_exists)  const  {
  if ( !m_doc && throw_if_no_exists )
    printError("The XML input was not started.",true);
  return m_doc;
}
// ----------------------------------------------------------------------------
void XMLDocument::printError(CSTR msg, bool rethrow)  const  {
  ::lib_rtl_output(LIB_RTL_WARNING,msg.c_str());
  if ( rethrow )  {
    throw runtime_error("XMLDocument> "+msg);
  }
}
// ----------------------------------------------------------------------------
DOMNode* XMLDocument::element(CSTR element_name,bool print_err) const {
  DOMNode* node = getDoc(true)->getElementById(XMLStr(element_name));
  if ( !node && print_err ) printError("Cannot find element:"+element_name);
  return node;
}
// ----------------------------------------------------------------------------
DOMNode* XMLDocument::child(DOMNode* par,CSTR tag,CSTR attr,CSTR val) const {
  for(XMLCollection c(par); c; ++c ) {
    if( c.tag() == tag )  {
      if( !attr.empty() && c.attr(attr) != val) continue;
      return c;
    }
  }
  return 0;
}
// ----------------------------------------------------------------------------
void XMLDocument::commit(const string& fname)    {  
  try {
    if ( dirty() && !readOnly() )  {
      string xmlfile = getfile(fname,true);
      XMLStr ii("LS");
      DOMImplementation *imp = DOMImplementationRegistry::getDOMImplementation(ii);
      XMLFormatTarget   *tar = new LocalFileFormatTarget(xmlfile.c_str());
#if _XERCES_VERSION <= 30000
      DOMWriter         *wr  = ((DOMImplementationLS*)imp)->createDOMWriter();
      wr->setFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true);
      wr->writeNode(tar, *m_doc);
#else
      DOMLSOutput       *out = imp->createLSOutput();
      out->setByteStream(tar);
      DOMLSSerializer   *wr = imp->createLSSerializer();
      wr->getDomConfig()->setParameter(XMLStr("format-pretty-print"), true);
      wr->write(m_doc, out);
      out->release();
#endif
      wr->release();
      delete  tar;
    }
  }
  catch ( exception& e )  {
    printError(string("Cannot open output file:")+e.what());
  }
  catch (...)  {
    printError("Unknown IO rrror.");
  }
}
// ----------------------------------------------------------------------------
string XMLDocument::getfile(const string& fname, bool create)   {  
  string protocol, path;
  XMLURL xerurl;
  try{
    xerurl   = (const XMLCh*)XMLStr(fname);
    protocol = _toString(xerurl.getProtocolName());
    path     = _toString(xerurl.getPath());
  }
  catch (const XMLException& e ) {
    printError(_toString(e.getMessage()));
  }
  if ( protocol.empty() )    {
    printError("Missing protocol.");
  }
  else if ( protocol == "http" || protocol == "ftp" )  {
    m_rdOnly = true;
  }
  else if ( protocol == "file" ) {
    m_rdOnly = false;
    struct stat buff;
    int exist = ::stat(path.c_str(),&buff) != -1;
    if ( create && !exist )  {
      ::lib_rtl_output(LIB_RTL_INFO,"File '%s' does not exist. New file created.",path.c_str());
      ofstream out(path.c_str());
      if( !m_rdOnly && out.is_open() ) {
        out << (CSTR)EmptyCatalog << endl;
      }
      else     {
        printError("Problem creating file "+path);
      }    
      out.close();        
    }
    else if ( exist )  {
      return path;
    }
    else if ( !create )  {
      return "";
    }
  }
  else  {
    printError(protocol + ": protocol not supported.");
  }
  return path;
}
