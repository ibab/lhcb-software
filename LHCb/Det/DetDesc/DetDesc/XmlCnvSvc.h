///	$Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/DetDesc/XmlCnvSvc.h,v 1.3 2001-01-25 12:12:29 mato Exp $
#ifndef DETDESC_XMLCNVSVC_H
#define DETDESC_XMLCNVSVC_H

/// Include files
#include "DetDesc/IXmlSvc.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/ConversionSvc.h"
#include "GaudiKernel/ICnvManager.h"
#include "GaudiKernel/System.h"
#include "CLHEP/Evaluator/Evaluator.h"

/// Forward and external declarations
template <class TYPE> class SvcFactory;

/** @class XmlCnvSvc XmlCnvSvc.h DetDesc/XmlCnvSvc.h

   A conversion service for XML based data. It is based in the generic conversion 
   service and implements in addition the IXmlSvc interface.

   @author Radovan Chytracek
   @author Pere Mato
*/
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
  virtual double eval( const char* expr, bool check = true ) const;
  virtual double eval( const std::string& expr, bool check = true ) const;
  virtual bool addParameter( const std::string& name, const std::string& value );
  virtual bool addParameter( const char* name, const char* value );
  virtual bool addParameter( const char* name, double value );
  virtual bool removeParameter( const std::string& name );
  virtual bool removeParameter( const char* name );

public:

  bool allowGenericCnv() { return m_genericConversion; }

protected:
  
  /** Standard Constructor
      @param name   String with service name
      @param svc    Pointer to service locator interface
      @return Reference to CdfPersCnvSvc
  */
  XmlCnvSvc( const std::string& name, ISvcLocator* svc );
  
  /// Destructor
  virtual ~XmlCnvSvc();

private:

  /// Numerical expressions parser
  HepTool::Evaluator  m_xp;

  /** Property triggering generic conversion of user defined detector
      elements in case the corresponding user defined converter is not
      available
  */
  bool  m_genericConversion;
};

#endif    // DETDESC_XMLCNVSVC_H













