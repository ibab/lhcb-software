/// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/DetDesc/XmlLVolumeCnv.h,v 1.2 2001-01-22 09:55:38 ibelyaev Exp $
#ifndef DETDESC_XMLCNVSVC_XMLLVOLUMECNV_H
#define DETDESC_XMLCNVSVC_XMLLVOLUMECNV_H

// Include files
#include "DetDesc/XmlGenericCnv.h"

#include <string>
#include <vector>

#include "CLHEP/Geometry/Transform3D.h"

#include "GaudiKernel/Converter.h"

#include "DetDesc/ISax8BitDocHandler.h"

// Forward and extern declarations
class     ISvcLocator;
class     ISolid;
template <class TYPE> class CnvFactory;

class LVolume;

extern const    CLID&   CLID_LogicalVolume;

class   XmlLVolumeCnv : public XmlGenericCnv, public ISax8BitDocHandler
{
  
  // Friend needed for instantiation
  friend class CnvFactory<XmlLVolumeCnv>;

public:

  /// This structure keeps temporary information for each phys. vol.
  /// we have processed
  typedef struct _PVitem
  {
      std::string m_pvName;
      std::string m_lvName;
      HepPoint3D  m_translation;
      HepRotation m_rotation;
  } PVitem;
  
  /// This structure keeps temporary information for each solid
  /// we have processed inside boolean solids
  typedef struct _SolidItem
  {
    ISolid*         m_solid;
    HepPoint3D      m_translation;
    HepRotation     m_rotation;
    //      HepTransform3D* m_transform;
  } SolidItem;

  ///  Parameterized physical volume type
  typedef struct _ParamPV
  {
    unsigned long m_number;
    PVitem        m_initialPos;
    HepVector3D   m_stepTranslation;
    HepRotation   m_stepRotation;
  } ParamPV;
  
public:
  
  // Create the transient representation of an object.
  virtual StatusCode createObj(IOpaqueAddress* pAddress, DataObject*& refpObject);
  
  // Update the transient object from the other representation.
  virtual StatusCode updateObj(IOpaqueAddress* pAddress, DataObject* refpObject);
  
  // Convert the transient object to the requested representation.
  virtual StatusCode createRep(DataObject* pObject, IOpaqueAddress*& refpAddress);
  
  // Update the converted representation of a transient object.
  virtual StatusCode updateRep(IOpaqueAddress* pAddress, DataObject* pObject);
  
  static const unsigned char& storageType()
  {
    return XML_StorageType;
  }
  
  static const CLID& classID()
  {
    return CLID_LogicalVolume;
  }
  
  // -----------------------------------------------------------------------
  //  Implementations of the SAX 8 bit DocumentHandler interface
  // -----------------------------------------------------------------------
  virtual void startDocument( 
                              const char* //info
                            )
  {
  }
  
  virtual void endDocument( 
                            const char* //info
                          )
  {
  }
  
  virtual void ignorableWhitespace( 
                                    const char* const  //chars
                                   ,const unsigned int //length
                                  )
  {
  }
  
  virtual void processingInstruction( 
                                      const char* const //target
                                     ,const char* const //data
                                    )
  {
  }
  
  virtual void characters(
                           const char* const chars
                          ,const unsigned int length
                         );
  
  virtual void startElement(
                             const char* const name
                            ,XmlCnvAttributeList& attributes
                           );
  
  virtual void endElement(
                           const char* const name
                         );
  
  // Implementation of the obligatory method to provide a hint to generic XML
  // converter about the XML tag we want to be notfied about
  virtual const char* tag() const;
  
protected:
  
  // Standard Constructor
  XmlLVolumeCnv( ISvcLocator* svcs );
  
  // Standard Destructor
  virtual ~XmlLVolumeCnv()                                                 {
  }
  
private:

  /// Local helper methods
  void doBoolean( SolidItem&  siRef, XmlCnvAttributeList& attributes);
  
  void doBoolTranslation( SolidItem&  siRef, XmlCnvAttributeList& attributes);
  void doBoolRPhiZTranslation( SolidItem&  siRef, XmlCnvAttributeList& attributes);
  void doBoolRThPhiTranslation( SolidItem&  siRef, XmlCnvAttributeList& attributes);
  
  void doBoolRotation( SolidItem&  siRef, XmlCnvAttributeList& attributes);
  void doBoolAxisRotation( SolidItem&  siRef, XmlCnvAttributeList& attributes);
  
  HepPoint3D doTranslation( std::string x, std::string y, std::string z );
  HepPoint3D doRPhiZTranslation( std::string r, std::string phi, std::string z );
  HepPoint3D doRThPhiTranslation( std::string r, std::string theta, std::string phi );
  
  HepRotation doRotation( std::string rx, std::string ry, std::string rz );
  HepRotation doAxisRotation( std::string axtheta, std::string axphi, std::string angle );
  
  HepTransform3D* doTransformation( std::string x,  std::string y,  std::string z,
                                    std::string rx, std::string ry, std::string rz );

  // Get & Manipulate transformation context
  inline std::string& transContext()
  {
    return m_transContext;
  }

  inline void setTransContext( std::string& context )
  {
    m_transContext = context;
  }

  inline void setTransContext( const std::string& context )
  {
    m_transContext = context;
  }
  
  inline void setTransContext( const char* context )
  {
    m_transContext = context;
  }

  inline void setTransContext( char* context )
  {
    m_transContext = context;
  }

  /// Indicator whether we're inside the interesting tag or not
  bool                   m_tagRead;

  /// Remember context for figuring out what transformation is being found
  std::string            m_transContext;

  /// Information needed inside callbacks
  /// Name of the converted volume
  std::string            m_volName;

  /// Material name
  std::string            m_materialName;

  /// Vector of physical volumes found
  std::vector<PVitem>    m_pvstore;

  /// Simple solid found
  ISolid*                m_solid;

  /// Needed for boolean volumes to keep all daughter solids found
  std::vector<SolidItem> m_bstore;
  std::string            m_bsName;
  bool                   m_insideBoolean;

  /// Needed for a parameterized volume
  bool                   m_insideParameterized;
  std::vector<ParamPV>   m_ppvstore;
};


#endif // DETDESC_XMLCNVSVC_XMLLVOLUMECNV_H

