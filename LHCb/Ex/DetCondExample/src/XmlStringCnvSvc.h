//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Ex/DetCondExample/src/XmlStringCnvSvc.h,v 1.1.1.1 2001-09-14 15:45:03 andreav Exp $
#ifndef DETCOND_XMLSTRINGCNVSVC_H
#define DETCOND_XMLSTRINGCNVSVC_H 1

/// Include files
#include "GaudiKernel/ConversionSvc.h"

/// Forward and external declarations
template <class TYPE> class SvcFactory;

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
  
  /// Create a transient representation from another rep of this object.
  virtual StatusCode createObj(IOpaqueAddress* pAddress, 
			       DataObject*&    refpObject);
  
  /// Update a transient representation from another rep of this object.
  virtual StatusCode updateObj(IOpaqueAddress* pAddress, 
			       DataObject*     pObject);
  
};

#endif    // DETCOND_XMLSTRINGCNVSVC_H


