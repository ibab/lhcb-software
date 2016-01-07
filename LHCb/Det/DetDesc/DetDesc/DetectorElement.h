#ifndef  DETDESC_DETECTORELEMENT_H
#define  DETDESC_DETECTORELEMENT_H 1

// Some pragmas to avoid warnings in VisualC
#ifdef WIN32
  // Disable warning C4786: identifier was truncated to '255' characters
  // in the debug information
  #pragma warning ( disable : 4786 )
#endif

// Include Files
#include <string>
#include <vector>
#include <map>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartRef.h"

#include "DetDesc/Condition.h"
#include "DetDesc/Services.h"
#include "DetDesc/ILVolume.h"
#include "DetDesc/ParamValidDataObject.h"
#include "DetDesc/DetectorElementException.h"
#include "DetDesc/IDetectorElement.h"
#include "DetDesc/CLIDDetectorElement.h"


// Forward declarations
class IDataProviderSvc;
class IMessageSvc;
template <class T> class DataObjectFactory;
class IUpdateManagerSvc;
class IRegistry;

/** @class DetectorElement DetectorElement.h DetDesc/DetectorElement.h
 *
 *  A "basic" implementation of Detector Description - Detector Element
 *
 *  @author Sebastien Ponce
 *  @author Rado Chytracek
 *  @author Ivan Belyaev
 *  @author Marco Clemencic
 *  @author Juan Palacios
*/

class DetectorElement: public ParamValidDataObject,
                       virtual public IDetectorElement {

  friend class DataObjectFactory<DetectorElement> ;

public:
  /// Type of the internal list of used conditions
  typedef std::map<std::string,SmartRef<Condition> > ConditionMap;

  /// Constructors
  DetectorElement (const std::string& name = "NotYetDefined");

  ///  Destructor
  ~DetectorElement() override;

public:

  /**
   * This method initializes the detector element. It should be overridden
   * and used for computation purposes. This is a kind of hook for adding
   * user code easily in the initialization of a detector element.
   */
  StatusCode initialize() override;

public:

  /// Obtain class ID
  const  CLID& clID()   const override { return classID(); }
  static const   CLID& classID()       { return CLID_DetectorElement; }

  const std::string& name () const override;

  /// Check if the condition called 'name' is in the list of conditionrefs.
  bool hasCondition(const std::string &name) const override;

  /// Return the SmartRef for the condition called 'name'.
  SmartRef<Condition> condition(const std::string &name) const override;

  /// Iterator to the first element of the list of conditions.
  inline  ConditionMap::const_iterator conditionBegin() const
  { return m_de_conditions.begin(); }

  /// Iterator to the last+1 element of the list of conditions.
  inline  ConditionMap::const_iterator conditionEnd() const
  { return m_de_conditions.end(); }

  /// delegation for geometry
  inline        IGeometryInfo* geometry() override;

  /// delegation for geometry  (const version)
  inline  const IGeometryInfo* geometry() const override;

  /// helper member using IGeometryInfo::isInside
  bool isInside(const Gaudi::XYZPoint& globalPoint) const;

  const IDetectorElement* childDEWithPoint(const Gaudi::XYZPoint& globalPoint) const;

  /// delegation for alignnment
  inline        IAlignment*    alignment() override;

  /// delegation for alignment (const version)
  inline  const IAlignment*    alignment() const override;

  // delegation for calibration
  inline        ICalibration*  calibration() override;

  // delegation for calibration (const version)
  inline  const ICalibration*  calibration() const override;

  // delegation for readout
  inline        IReadOut*      readOut() override;

  // delegation for readout (const version)
  inline  const IReadOut*      readOut() const  override;

  // delegation for slow control
  inline        ISlowControl*  slowControl() override;

  // delegation for slow control (const version)
  inline  const ISlowControl*  slowControl() const override;

  // delegation for fast control
  inline        IFastControl*  fastControl() override;

  // delegation for fast control (const version)
  inline  const IFastControl*  fastControl() const override;

  // another way to access: "pseudo-conversion"
  // cast to         IGeometryInfo*
  inline operator       IGeometryInfo*() override;

  // cast to   const IGeometryInfo*
  inline operator const IGeometryInfo*() const override;

  // cast to         IAligment*
  inline operator       IAlignment*   () override;

  // cast to   const IAlignment*
  inline operator const IAlignment*   () const override;

  // cast to         ICalibration*
  inline operator       ICalibration* () override;

  // cast to   const ICalibration*
  inline operator const ICalibration* () const override;

  // cast to         IReadOut*
  inline operator       IReadOut*     () override;

  // cast to   const IReadOut*
  inline operator const IReadOut*     () const override;

  // cast to         ISlowControl*
  inline operator       ISlowControl* () override;

  // cast to   const ISlowControl*
  inline operator const ISlowControl* () const override;

  // cast to         IFastControl*
  inline operator       IFastControl* () override;

  // cast to   const IFastControl*
  inline operator const IFastControl* () const override;

  // cast to         IGeometryInfo&
  // (potentially could throw DetectorElementException)
  inline operator       IGeometryInfo&() override;

  // cast to   const IGeometryInfo&
  // (potentially could throw DetectorElementException)
  inline operator const IGeometryInfo&() const override;

  // cast to         IAlignment&
  // (potentially could throw DetectorElementException)
  inline operator       IAlignment&   () override;

  // cast to   const IAlignment&
  // (potentially could throw DetectorElementException)
  inline operator const IAlignment&   () const override;

  // cast to         ICalibration&
  // (potentially could throw DetectorElementException)
  inline operator       ICalibration& () override;

  // cast to   const ICalibration&
  // (potentially could throw DetectorElementException)
  inline operator const ICalibration& () const  override;

  // cast to         IReadOut&
  // (potentially could throw DetectorElementException)
  inline operator       IReadOut&     () override;

  // cast to   const IReadOut&
  // (potentially could throw DetectorElementException)
  inline operator const IReadOut&     () const override;

  // cast to         ISlowControl&
  // (potentially could throw DetectorElementException)
  inline operator       ISlowControl& () override;

  // cast to   const ISlowControl&
  // (potentially could throw DetectorElementException)
  inline operator const ISlowControl& () const override;

  // cast to         IFastControl&
  // (potentially could throw DetectorElementException)
  inline operator       IFastControl& () override;

  // cast to   const IFastControl&
  // (potentially could throw DetectorElementException)
  inline operator const IFastControl& () const override;

  //  printout (overloaded)
  // (potentially could throw DetectorElementException)
  std::ostream& printOut( std::ostream& ) const override;

  /// reset to the initial state
  IDetectorElement* reset() override;

  virtual MsgStream&    printOut( MsgStream&    ) const;

  // pointer to parent IDetectorElement (const version)
  IDetectorElement*  parentIDetectorElement() const override;

  // (reference to) container of pointers to child detector elements
  IDetectorElement::IDEContainer& childIDetectorElements() const override;

  // iterators for manipulation of daughter elements
  inline IDetectorElement::IDEContainer::iterator childBegin() override{
    return childIDetectorElements().begin();
  }
  inline IDetectorElement::IDEContainer::const_iterator
    childBegin() const override { return childIDetectorElements().begin() ; }
  inline IDetectorElement::IDEContainer::iterator
    childEnd  () override { return childIDetectorElements().end  () ; }
  inline IDetectorElement::IDEContainer::const_iterator
    childEnd  () const override { return childIDetectorElements().end  () ; }

  /**
   * Method used to access the ParamValidDataObject methods from IDetectorElement
   * interface.
   */
  const ParamValidDataObject *params() const override;

  /**
   * Return a sensitive volume identifier for a given point in the
   * global reference frame.
   */

  int sensitiveVolumeID(const Gaudi::XYZPoint& globalPos) const override;

  /// Used to create a link with a given name to the condition at 'path' in the detector data store.
  void createCondition(std::string &name, std::string &path);

  //-- N. Gilardi; 2005.07.08 ---------------------------------------------
  /// Returns list of existing parameter vectors as a vector of their names
  virtual std::vector<std::string> conditionNames() const;
  //-----------------------------------------------------------------------

  ///
  /// specific
  // create "ghost"
  const IGeometryInfo* createGeometryInfo();

  // create "orphan"
  const IGeometryInfo* createGeometryInfo( const std::string& logVol);

  // create "regular"
  const IGeometryInfo* createGeometryInfo
         (const std::string& logVol,
          const std::string& support,
          const ILVolume::ReplicaPath & replicaPath );

  const IGeometryInfo* createGeometryInfo
         (const std::string& logVol,
          const std::string& support,
          const ILVolume::ReplicaPath & replicaPath,
          const std::string& alignmentPath);

  const IGeometryInfo* createGeometryInfo
         (const std::string& logVol,
          const std::string& support,
          const std::string& namePath);

  const IGeometryInfo* createGeometryInfo
         (const std::string& logVol,
          const std::string& support,
          const std::string& namePath,
          const std::string& alignmentPath);

  const IAlignment* createAlignment (const std::string& condition);

  const ICalibration* createCalibration (const std::string& condition);

  const IReadOut* createReadOut (const std::string& condition);

  const ISlowControl* createSlowControl (const std::string& condition);

  const IFastControl* createFastControl (const std::string& condition);

public:

  // Implementation of IInterface
  unsigned long addRef() override;
  unsigned long release() override;
  StatusCode queryInterface( const InterfaceID& riid,
                                     void** ppvInterface ) override;

protected:

  /// specific
  void setGeometry( IGeometryInfo* geoInfo ) { m_de_iGeometry = geoInfo; }

  IDataProviderSvc*  dataSvc () const;
  IMessageSvc*       msgSvc  () const;
  IUpdateManagerSvc* updMgrSvc() const;

  // technicalities

  inline void Assert
  ( bool assertion, const std::string& message =
    "DetectorElement Unknown Exception" ) const ;

  inline void Assert
  ( bool assertion, const char* message ) const ;

private:

  // for IDetectorElement implementation
  IGeometryInfo*         m_de_iGeometry     ;
  IAlignment*            m_de_iAlignment    ;
  ICalibration*          m_de_iCalibration  ;
  IReadOut*              m_de_iReadOut      ;
  ISlowControl*          m_de_iSlowControl  ;
  IFastControl*          m_de_iFastControl  ;

  /// Container of the SmartRefs for the conditions.
  ConditionMap m_de_conditions;

  mutable bool                              m_de_childrensLoaded;
  mutable IDetectorElement::IDEContainer    m_de_childrens;

  /// This defines the type of a userParameter
  enum userParamKind { DOUBLE, INT, OTHER };

  /// reference to services
  DetDesc::Services* m_services;

};

// implementation of the inlines functions
///  output operators to MsgStream ///////////////////////////////////////////
inline MsgStream& operator<< (MsgStream& os, const DetectorElement& de) {
  return de.printOut(os);
}

inline MsgStream& operator<< (MsgStream& os, const DetectorElement* de) {
  if (!de) {
    return os << "DetectorElement* points to NULL"<< endmsg;
  } else {
    return os << *de;
  }
}

//////////////////////////////////////////////////////////////////////////////
inline const IGeometryInfo* DetectorElement::geometry () const {
  return m_de_iGeometry;
}
inline IGeometryInfo* DetectorElement::geometry () {
  return m_de_iGeometry;
}
inline const IAlignment* DetectorElement::alignment () const {
  return m_de_iAlignment;
}
inline IAlignment* DetectorElement::alignment() {
  return m_de_iAlignment;
}
inline const ICalibration* DetectorElement::calibration() const {
  return m_de_iCalibration ;
}
inline ICalibration* DetectorElement::calibration() {
  return m_de_iCalibration ;
}
inline const IReadOut* DetectorElement::readOut() const {
  return m_de_iReadOut     ;
}
inline IReadOut* DetectorElement::readOut() {
  return m_de_iReadOut     ;
}
inline const ISlowControl*  DetectorElement::slowControl() const {
  return m_de_iSlowControl ;
}
inline ISlowControl* DetectorElement::slowControl() {
  return m_de_iSlowControl ;
}
inline const IFastControl*  DetectorElement::fastControl() const {
  return m_de_iFastControl ;
}
inline IFastControl* DetectorElement::fastControl() {
  return m_de_iFastControl ;
}
// "pseudo-casting" //////////////////////////////////////////////////////////
inline DetectorElement::operator const IGeometryInfo*() const {
  return m_de_iGeometry    ;
}
inline DetectorElement::operator IGeometryInfo*() {
  return m_de_iGeometry    ;
}

inline DetectorElement::operator const IAlignment* () const {
  return m_de_iAlignment   ;
}
inline DetectorElement::operator IAlignment* () {
  return m_de_iAlignment   ;
}
inline DetectorElement::operator const ICalibration* () const {
  return m_de_iCalibration ;
}
inline DetectorElement::operator ICalibration* () {
  return m_de_iCalibration ;
}
inline DetectorElement::operator const IReadOut* () const {
  return m_de_iReadOut     ;
}
inline DetectorElement::operator IReadOut* () {
  return m_de_iReadOut     ;
}
inline DetectorElement::operator const ISlowControl* () const {
  return m_de_iSlowControl ;
}
inline DetectorElement::operator ISlowControl* () {
  return m_de_iSlowControl ;
}
inline DetectorElement::operator const IFastControl* () const {
  return m_de_iFastControl ;
}
inline DetectorElement::operator IFastControl* () {
  return m_de_iFastControl ;
}
//////////////////////////////////////////////////////////////////////////////
inline DetectorElement::operator const IGeometryInfo&()    const 
{
  Assert (0 != m_de_iGeometry, "DetectorElement::geometry is not available!");
  return *m_de_iGeometry    ; 
}
//////////////////////////////////////////////////////////////////////////////
inline DetectorElement::operator       IGeometryInfo&()
{ 
  Assert (0 != m_de_iGeometry, "DetectorElement::geometry is not available!");
  return *m_de_iGeometry    ; 
}
//////////////////////////////////////////////////////////////////////////////
inline DetectorElement::operator const IAlignment&   ()    const 
{ 
  Assert (0 != m_de_iAlignment,
          "DetectorElement::alignment is not available!");
  return *m_de_iAlignment   ; 
}
//////////////////////////////////////////////////////////////////////////////
inline DetectorElement::operator       IAlignment&   ()          
{ 
  Assert (0 != m_de_iAlignment,
          "DetectorElement::alignment is not available!");
  return *m_de_iAlignment   ; 
}
//////////////////////////////////////////////////////////////////////////////
inline DetectorElement::operator const ICalibration& ()    const 
{ 
  Assert (0 != m_de_iCalibration,
          "DetectorElement::calibration is not available!");
  return *m_de_iCalibration ; 
}
///////////////////////////////////////////////////////////////////////////////
inline DetectorElement::operator       ICalibration& ()          
{ 
  Assert (0 != m_de_iCalibration,
          "DetectorElement::calibration is not available!");
  return *m_de_iCalibration ; 
}
///////////////////////////////////////////////////////////////////////////////
inline DetectorElement::operator const IReadOut&     ()    const 
{ 
  Assert (0 != m_de_iReadOut, "DetectorElement::readout is not available!");
  return *m_de_iReadOut     ; 
}
///////////////////////////////////////////////////////////////////////////////
inline DetectorElement::operator       IReadOut&     ()          
{ 
  Assert (0 != m_de_iReadOut, "DetectorElement::readout is not available!");
  return *m_de_iReadOut     ; 
}
///////////////////////////////////////////////////////////////////////////////
inline DetectorElement::operator const ISlowControl& ()    const 
{ 
  Assert (0 != m_de_iSlowControl,
          "DetectorElement::slowcontrol is not available!");
  return *m_de_iSlowControl ; 
}
////////////////////////////////////////////////////////////////////////////////
inline DetectorElement::operator       ISlowControl& ()          
{ 
  Assert (0 != m_de_iSlowControl,
          "DetectorElement::slowcontrol is not available!");
  return *m_de_iSlowControl ; 
}
////////////////////////////////////////////////////////////////////////////////
inline DetectorElement::operator const IFastControl& ()    const 
{ 
  Assert (0 != m_de_iFastControl,
          "DetectorElement::fastcontrol is not available!");
  return *m_de_iFastControl ; 
}
///////////////////////////////////////////////////////////////////////////////
inline DetectorElement::operator       IFastControl& ()          
{ 
  Assert (0 != m_de_iFastControl,
          "DetectorElement::fastcontrol is not available!");
  return *m_de_iFastControl ; 
}
/// assertion /////////////////////////////////////////////////////////////////
inline void DetectorElement::Assert 
(bool assertion,
 const std::string& assertionName) const 
{
  if (!assertion) 
  { throw DetectorElementException (assertionName, this); }
}
inline void DetectorElement::Assert 
(bool assertion,  const char* assertionName) const 
{
  if (!assertion) 
  { throw DetectorElementException (assertionName, this); }
}
///////////////////////////////////////////////////////////////////////////////

#endif    //    DETDESC_DETECTORELEMENT_H

