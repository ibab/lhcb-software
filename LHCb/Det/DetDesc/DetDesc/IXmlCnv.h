/// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/DetDesc/IXmlCnv.h,v 1.1.1.1 2000-11-23 13:44:42 ranjard Exp $

/// Include files
#include <string>

#include "GaudiKernel/Kernel.h"

class ISax8BitDocHandler;
class IUserSax8BitDocHandler;

class IXmlCnv
{
public:

  IXmlCnv() : m_context( "" ), m_asciiHandler( 0 ) {
  }

  virtual ~IXmlCnv() {
  }

  /// Register & get a client who receives ASCII data
  inline virtual void set8BitDocHandler( ISax8BitDocHandler& handler )
  {
    m_asciiHandler = &handler;
  }

  inline virtual ISax8BitDocHandler* get8BitDocHandler() const
  {
    return m_asciiHandler;
  }
  
  /// Set & get user IUserSax8BitDocHandler
  /// Only used by converters containing <specific> tag and for detector elements
  /// which have class ID different than CLID_DetectorElement( e.g. 2)
  inline virtual void setUserSaxDocHandler( IUserSax8BitDocHandler& handler )
  {
    m_userDocHandler = &handler;
  }

  inline virtual IUserSax8BitDocHandler* getUserSaxDocHandler() const
  {
    return m_userDocHandler;
  }

  /// Get & set the current tag context
  inline virtual std::string context() const
  {
    return m_context;
  }

  inline virtual void setContext( std::string& context )
  {
    m_context = context;
  }
  
  /// We need for each run a new SAX so...
  virtual StatusCode initParser() = 0;
  virtual StatusCode finiParser() = 0;

  /// Calling this method must use try{...}catch(...){...} block inside
  /// to catch exceptions thrown by XML4CParser or XML converters
  virtual StatusCode parse( const char* fileName ) = 0;

private:
  
  /// Name of the tag inside we currently are
  /// This is useful for PCDATA content model to get a clue which tag holds them
  std::string             m_context;

  /// This is our ASCII SAX handler client
  ISax8BitDocHandler*     m_asciiHandler;

  IUserSax8BitDocHandler* m_userDocHandler;
};

