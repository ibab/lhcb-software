//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Ex/DetCondExample/src/XmlStringTemperatureCnv.h,v 1.1.1.1 2001-09-14 15:45:03 andreav Exp $
#ifndef DETCOND_XMLSTRINGTEMPERATURECNV_H
#define DETCOND_XMLSTRINGTEMPERATURECNV_H 1

#include "Temperature.h"

#include "GaudiKernel/Converter.h"

template <class TYPE> class CnvFactory;

///---------------------------------------------------------------------------
/** @class XmlStringTemperatureCnv XmlStringTemperatureCnv.h Det/DetCond/XmlStringTemperatureCnv.h

    A converter for Temperature objects stored as XmlStrings.

    @author Andrea Valassi 
    @date August 2001
*///--------------------------------------------------------------------------

class XmlStringTemperatureCnv : public Converter

{

  /// Only factories can access protected constructors
  friend class CnvFactory<XmlStringTemperatureCnv>;

 protected:
  
  /// Constructor
  XmlStringTemperatureCnv( ISvcLocator* svc );
  
  /// Destructor
  ~XmlStringTemperatureCnv() {}; 

 public: 
  
  // Re-implementations of the Converter interface 

  /// Initialize the converter 
  virtual StatusCode initialize(); 
  
  /// Finalize the converter 
  virtual StatusCode finalize(); 
  
  /// Create a transient object from a persistent representation 
  virtual StatusCode createObj (IOpaqueAddress* pAddress,  
				DataObject*&    refpObject);
  
  /// Update a transient object from a persistent representation
  virtual StatusCode updateObj (IOpaqueAddress* pAddress, 
				DataObject*     pObject);
  
 public:

  // Static methods needed by CnvFactory

  /// Storage type for this class
  static const unsigned char& storageType() { return XMLSTRING_StorageType; };
 
  /// Class ID for this class
  static const CLID& classID() { return Temperature::classID(); }

};

#endif // DETCOND_XMLSTRINGTEMPERATURECNV_H

