#ifndef DETDESC_XMLCNVSVC_ISAX8BITDOCHANDLER_H
#define DETDESC_XMLCNVSVC_ISAX8BITDOCHANDLER_H 1

#define NOTIFY_ALL "__All_XML_Tags__"

class XmlCnvAttributeList;

class ISax8BitDocHandler
{
public:
  // -----------------------------------------------------------------------
  //  Constructors
  // -----------------------------------------------------------------------
  ISax8BitDocHandler() {}
  virtual ~ISax8BitDocHandler() {}
  
  
  // -----------------------------------------------------------------------
  //  Implementations of the SAX DocumentHandler interface
  // -----------------------------------------------------------------------
  virtual void startDocument( const char* info ) = 0;
  
  virtual void endDocument( const char* info ) = 0;
  
  virtual void endElement( const char* const name ) = 0;
  
  virtual void characters( const char* const chars, const unsigned int length ) = 0;
  
  virtual void ignorableWhitespace( const char* const chars, const unsigned int length ) = 0;
  
  virtual void processingInstruction( const char* const target, const char* const data ) = 0;
  
  virtual void startElement( const char* const name, XmlCnvAttributeList& attributes) = 0;
  
  // This method must implement each Xml converter to provide information
  // about the name of an XML tag it is interested in
  // In case the client wants to be notified about each XML tag it should return
  // NOTIFY_ALL value defined above
  virtual const char* tag() const = 0;
};

#endif // DETDESC_XMLCNVSVC_ISAX8BITDOCHANDLER_H

