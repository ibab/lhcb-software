//$Id: XmlStringCnvSvc.h,v 1.3 2001-11-23 18:08:59 andreav Exp $
#ifndef DETCOND_XMLSTRINGCNVSVC_H
#define DETCOND_XMLSTRINGCNVSVC_H 1

/// Include files
#include "GaudiKernel/ConversionSvc.h"

/// Forward and external declarations
template <class ConcreteService> class SvcFactory;

///---------------------------------------------------------------------------
/** @class XmlStringCnvSvc XmlStringCnvSvc.h Det/DetCond/XmlStringCnvSvc.h

    A simple conversion service for XML-like strings.
    This is only introduced to test the ConditionsDBCnvSvc containing 
    string data of an arbitrary type (XML, Objy).
    
    @author Andrea Valassi 
    @date August 2001
*///--------------------------------------------------------------------------

class XmlStringCnvSvc : public ConversionSvc
{
  
  /// Only factories can access protected constructors
  friend class SvcFactory<XmlStringCnvSvc>;

 protected:
  
  /// Constructor
  XmlStringCnvSvc( const std::string& name, ISvcLocator* svc );
  
  /// Destructor
  virtual ~XmlStringCnvSvc();

 public:

  // Reimplemented from ConversionSvc

  /// Initialise the service
  virtual StatusCode initialize();
  
  /// Finalise the service
  virtual StatusCode finalize();
  
  /// Create an address using explicit arguments to identify a single object.
  virtual StatusCode createAddress( unsigned char svc_type,
                                    const CLID& clid,
                                    const std::string* par, 
                                    const unsigned long* ip,
                                    IOpaqueAddress*& refpAddress);

};

#endif    // DETCOND_XMLSTRINGCNVSVC_H


