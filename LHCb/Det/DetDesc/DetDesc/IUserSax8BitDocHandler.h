/// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/DetDesc/IUserSax8BitDocHandler.h,v 1.1.1.1 2000-11-23 13:44:44 ranjard Exp $
#ifndef DETDESC_XMLCNVSVC_IUSERSAX8BITDOCHANDLER_H
#define DETDESC_XMLCNVSVC_IUSERSAX8BITDOCHANDLER_H 1

/// Forward and external declarations
class XmlCnvAttributeList;

class IUserSax8BitDocHandler
{
public:
  ///  Constructors
  IUserSax8BitDocHandler() {}

  ///  Destructor
  virtual ~IUserSax8BitDocHandler() {}
  
  
  ///  User SAX DocumentHandler interface
  
  /// Start of the XML document callback
  virtual void uStartDocument() = 0;

  /// End of the XML document callback
  virtual void uEndDocument() = 0;
  
  /// Parsed character data callback
  virtual void uCharacters( const char* const chars, const unsigned int length ) = 0;
  
  /// White space characters callback
  virtual void uIgnorableWhitespace( const char* const chars, const unsigned int length ) = 0;
  
  /// Start of the XML element callback
  virtual void uStartElement( const char* const name, XmlCnvAttributeList& attributes) = 0;
  
  /// End of the XML element callback
  virtual void uEndElement( const char* const name ) = 0;

  /// White space characters callback
  virtual void uProcessingInstruction( const char* const target, const char* const data ) = 0;

  // This method must implement each Xml converter to provide information
  // about the name of an XML tag it is interested in
  //virtual const char* tag() const = 0;
};

#endif // DETDESC_XMLCNVSVC_IUSERSAX8BITDOCHANDLER_H

