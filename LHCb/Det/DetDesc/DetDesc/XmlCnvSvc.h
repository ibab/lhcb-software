///	$Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/DetDesc/XmlCnvSvc.h,v 1.1.1.1 2000-11-23 13:44:43 ranjard Exp $
#ifndef DETDESC_XMLCNVSVC_H
#define DETDESC_XMLCNVSVC_H 1

/// Include files
#include "GaudiKernel/IXmlSvc.h"
#include "GaudiKernel/ConversionSvc.h"

/// Forward and external declarations
template <class TYPE> class SvcFactory;
class XmlExprParser;

//
// ClassName:    XmlCnvSvc
// 
// Description:  A conversion service for XML based data
//
// Author:       Radovan Chytracek
//
class XmlCnvSvc : public ConversionSvc, virtual public IXmlSvc
{
  
  friend class SvcFactory<XmlCnvSvc>;
  
public:
  
  /// Initialise the service
  virtual StatusCode initialize();
  
  /// Finalise the service
  virtual StatusCode finalize();
  
  virtual StatusCode queryInterface( const IID& riid, void** ppvInterface );

  /// Obligatory implementation of the IXmlSvc interface
  /// Evaluate a numerical expresion
  virtual double eval( char* expr, bool check = true );
  virtual double eval( const char* expr, bool check = true );
  virtual double eval( std::string& expr, bool check = true );
  virtual double eval( const std::string& expr, bool check = true );

public:

  bool allowGenericCnv()
  {
    return m_genericConversion;
  }

protected:
  
  /// Standard Constructor
  /// name   String with service name
  /// svc    Pointer to service locator interface
  /// return Reference to CdfPersCnvSvc
  XmlCnvSvc( const std::string& name, ISvcLocator* svc );
  
  /// Destructor
  virtual ~XmlCnvSvc();

private:

  /// Numerical expressions parser
  XmlExprParser*    m_xp;

  /// Property triggering generic conversion of user defined detector
  /// elements in case the corresponding user defined converter is not
  /// available
  bool              m_genericConversion;
  
};

#endif    // DETDESC_XMLCNVSVC_H

