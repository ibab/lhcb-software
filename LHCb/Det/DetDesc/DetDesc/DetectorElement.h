/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $
/// ===========================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.18  2001/12/11 10:02:20  sponce
/// Implementation of usage of the condition database. This includes new objects and associated converters (like condition and XmlConditionCnv).
/// The condition object is extensible, as was detectorElement. Thus, there is a templated XmlUserConditionCnv that could be extended by users as it was the case for detector elements.
///
/// Revision 1.17  2001/12/03 15:18:29  mato
/// Small changes to compile without warnings in Windows
///
/// Revision 1.16  2001/11/20 15:22:19  sponce
/// Lots of changes here :
///    - make use of the new version of GaudiKernel and GaudiSvc. One consequence
///    is the removal of the class XmlAddress
///    - centralization of address creations in conversion services, as suggested
///    by the new architecture
///    - add a parseString method on the XMLParserSvc. This allows to parse XML
///    directly from a string
///    - use of the new Assembly objects in the XML converters
///    - update of the converters to handle the definition of detelem inside
///    detelems, without using detelemrefs
///    - take care of a possible indexing of detelems and parametrized detelems.
///    The numbering is given by adding :<digits> to the name of the element.
///    - add support for polycones in the converters
///    - add code convention compliance to many files
///
/// Revision 1.15  2001/08/10 16:41:28  ibelyaev
/// modifitcations in IDetectorElement and related classes
/// 
/// ===========================================================================
#ifndef  DETDESC_DETECTORELEMENT_H
#define  DETDESC_DETECTORELEMENT_H 1

// Some pragmas to avoid warnings in VisualC
#ifdef WIN32 
  // Disable warning C4786: identifier was truncated to '255' characters
  // in the debug information
  #pragma warning ( disable : 4786 )
#endif

// Include Files
#include <algorithm>
#include <string>
#include <vector>
#include <map>

#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IValidity.h"
#include "GaudiKernel/ITime.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "DetDesc/ILVolume.h"
#include "DetDesc/IPVolume.h"
#include "DetDesc/IDetectorElement.h"
#include "DetDesc/CLIDDetectorElement.h"


// Forward declarations
class IDataProviderSvc;
class IMessageSvc;
template <class T> class DataObjectFactory;


/** @class DetectorElement DetectorElement.h GaudiKernel/DetectorElement.h
 *
 *  A "basic" implementation of Detector Description - Detector Element
 *
 *  @author Sebastien Ponce
 *  @author Rado Chytracek
 *  @author Ivan Belyaev
*/
class DetectorElement: public DataObject,
                       public IDetectorElement,
                       public IValidity {

  friend class DataObjectFactory<DetectorElement> ;  

public:

  /// Constructors 
  DetectorElement (const std::string& name = "NotYetDefined");
  
  DetectorElement (const std::string& name,
                   const ITime& iValidSince,
                   const ITime& iValidTill); 

  ///  Destructor 
  virtual ~DetectorElement(); 


public:

  /// Obtain class ID
  virtual const  CLID& clID()   const  { return classID(); }
  static const   CLID& classID()       { return CLID_DetectorElement; }

  
  virtual const std::string& name () const {
    static std::string s_empty = "";
    IRegistry* pReg = registry();
    return (0!=pReg) ? pReg->identifier() : s_empty;;
  } 

  /// delegation for geometry 
  inline        IGeometryInfo* geometry(); 

  /// delegation for geometry  (const version) 
  inline  const IGeometryInfo* geometry() const; 

  /// delegation for alignnment                
  inline        IAlignment*    alignment(); 

  /// delegation for alignment (const version) 
  inline  const IAlignment*    alignment() const; 

  // delegation for calibration 
  inline        ICalibration*  calibration(); 

  // delegation for calibration (const version) 
  inline  const ICalibration*  calibration() const; 

  // delegation for readout 
  inline        IReadOut*      readOut(); 

  // delegation for readout (const version) 
  inline  const IReadOut*      readOut() const ; 

  // delegation for slow control 
  inline        ISlowControl*  slowControl(); 

  // delegation for slow control (const version) 
  inline  const ISlowControl*  slowControl() const; 

  // delegation for fast control 
  inline        IFastControl*  fastControl(); 

  // delegation for fast control (const version) 
  inline  const IFastControl*  fastControl() const; 

  // another way to access: "pseudo-conversion"
  // cast to         IGeometryInfo*
  inline operator       IGeometryInfo*(); 

  // cast to   const IGeometryInfo* 
  inline operator const IGeometryInfo*() const; 

  // cast to         IAligment*     
  inline operator       IAlignment*   (); 

  // cast to   const IAlignment*    
  inline operator const IAlignment*   () const; 

  // cast to         ICalibration*  
  inline operator       ICalibration* (); 

  // cast to   const ICalibration*  
  inline operator const ICalibration* () const; 

  // cast to         IReadOut*      
  inline operator       IReadOut*     (); 

  // cast to   const IReadOut*      
  inline operator const IReadOut*     () const; 

  // cast to         ISlowControl*  
  inline operator       ISlowControl* (); 

  // cast to   const ISlowControl*  
  inline operator const ISlowControl* () const; 

  // cast to         IFastControl*  
  inline operator       IFastControl* (); 

  // cast to   const IFastControl*  
  inline operator const IFastControl* () const; 

  // cast to         IGeometryInfo&     
  // (potentially could throw DetectorElementException)
  inline operator       IGeometryInfo&(); 

  // cast to   const IGeometryInfo& 
  // (potentially could throw DetectorElementException)
  inline operator const IGeometryInfo&() const; 

  // cast to         IAlignment&    
  // (potentially could throw DetectorElementException)
  inline operator       IAlignment&   (); 

  // cast to   const IAlignment&    
  // (potentially could throw DetectorElementException)
  inline operator const IAlignment&   () const; 

  // cast to         ICalibration&  
  // (potentially could throw DetectorElementException)
  inline operator       ICalibration& ()                ; 

  // cast to   const ICalibration&  
  // (potentially could throw DetectorElementException)
  inline operator const ICalibration& ()          const ; 

  // cast to         IReadOut&      
  // (potentially could throw DetectorElementException)
  inline operator       IReadOut&     ()                ; 

  // cast to   const IReadOut&      
  // (potentially could throw DetectorElementException)
  inline operator const IReadOut&     ()          const ; 

  // cast to         ISlowControl&  
  // (potentially could throw DetectorElementException)
  inline operator       ISlowControl& ()                ; 

  // cast to   const ISlowControl&  
  // (potentially could throw DetectorElementException)
  inline operator const ISlowControl& ()          const ; 

  // cast to         IFastControl&  
  // (potentially could throw DetectorElementException)
  inline operator       IFastControl& ()                ; 

  // cast to   const IFastControl&  
  // (potentially could throw DetectorElementException)
  inline operator const IFastControl& ()          const ; 

  //  printout (overloaded)
  // (potentially could throw DetectorElementException)
  virtual std::ostream& printOut( std::ostream& ) const;  

  // from IValidity interface
  inline        bool    isValid          ()                ;   
  inline        bool    isValid          ( const ITime& )  ;   
  inline const  ITime&  validSince       ()                ;    
  inline const  ITime&  validTill        ()                ;   
  inline        void    setValidity      ( const ITime& , 
                                           const ITime& )  ;  
  inline        void    setValiditySince ( const ITime& )  ;  
  inline        void    setValidityTill  ( const ITime& )  ;   
  StatusCode            updateValidity   ()                ;   // not yet

  /// reset to the initial state
  IDetectorElement* reset() ;    

  virtual MsgStream&    printOut( MsgStream&    ) const;  

  // pointer to parent IDetectorElement (const version)
  virtual IDetectorElement*  parentIDetectorElement() const;

  // (reference to) container of pointers to child detector elements 
  inline virtual IDetectorElement::IDEContainer& childIDetectorElements() const;

  // iterators for manipulation of daughter elements 
  inline virtual IDetectorElement::IDEContainer::iterator childBegin() {
    return childIDetectorElements().begin();
  }
  inline virtual IDetectorElement::IDEContainer::const_iterator
    childBegin() const { return childIDetectorElements().begin() ; }
  inline virtual IDetectorElement::IDEContainer::iterator
    childEnd  () { return childIDetectorElements().end  () ; }
  inline virtual IDetectorElement::IDEContainer::const_iterator
    childEnd  () const { return childIDetectorElements().end  () ; }

  /// IInspectable interface:
//   virtual bool acceptInspector( IInspector* )       ; 
//   virtual bool acceptInspector( IInspector* ) const ; 

  /// serialization for reading 
  virtual StreamBuffer& serialize( StreamBuffer& )       ; 

  /// serialization for writing 
  virtual StreamBuffer& serialize( StreamBuffer& ) const ; 

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
          const std::string& namePath);

  const IAlignment* createAlignment (const std::string& condition);

  const ICalibration* createCalibration (const std::string& condition);

  const IReadOut* createReadOut (const std::string& condition);

  const ISlowControl* createSlowControl (const std::string& condition);

  const IFastControl* createFastControl (const std::string& condition);

  /**
   * This method initializes the detector element. It should be overridden
   * and used for computation purposes. This is a kind of hook for adding
   * user code easily in the initialization of a detector element.
   */
  virtual StatusCode initialize();

  /**
   * This adds a new double userParameter to the detectorElement.
   * This parameter has a name, a type, a comment and a value, given both
   * as a std::string and as a double.
   * If this parameter was already existing, it is replaced by the new one.
   * @param name the name of the parameter
   * @param type the type of the parameter. This is only a clue for the user, it
   * is not used by the detector element itself
   * @param comment a comment on this parameter use
   * @param value the value of the parameter, as a string
   * @param d_value the value of the parameter, as a double
   */
  virtual void addUserParameter (std::string name,
                                        std::string type,
                                        std::string comment,
                                        std::string value,
                                        double d_value);

  /**
   * This adds a new integer userParameter to the detectorElement.
   * This parameter has a name, a type, a comment and a value, given both
   * as a std::string and as an int.
   * If this parameter was already existing, it is replaced by the new one.
   * @param name the name of the parameter
   * @param type the type of the parameter. This is only a clue for the user, it
   * is not used by the detector element itself
   * @param comment a comment on this parameter use
   * @param value the value of the parameter, as a string
   * @param i_value the value of the parameter, as an int
   */
  virtual void addUserParameter (std::string name,
                                        std::string type,
                                        std::string comment,
                                        std::string value,
                                        double d_value,
                                        int i_value);
  
  /**
   * This adds a new non-numerical userParameter to the detectorElement.
   * This parameter has a name, a type, a comment and a value,
   * given as a std::string.
   * If this parameter was already existing, it is replaced by the new one.
   * @param name the name of the parameter
   * @param type the type of the parameter. This is only a clue for the user, it
   * is not used by the detector element itself
   * @param comment a comment on this parameter use
   * @param value the value of the parameter, as a string
   * @param d_value the value of the parameter, as a double
   */
  virtual void addUserParameter (std::string name,
                                        std::string type,
                                        std::string comment,
                                        std::string value);
  
  /**
   * This adds a new double userParameterVector to the detectorElement.
   * This parameter has a name, a type, a comment and a value, given both as a
   * std::vector<std::string> and as a std::vector<double>
   * If this parameter vector was already existing, it is replaced by the new
   * one.
   * @param name the name of the parameter vector
   * @param type the type of the parameter vector. This is only a clue for
   * the user, it is not used by the detector element itself
   * @param comment a comment on this parameter vector use
   * @param value the value of the parameter vector, as a vector of strings
   * @param d_value the value of the parameter vector, as a vector of doubles
   */
  virtual void addUserParameterVector (std::string name,
                                              std::string type,
                                              std::string comment,
                                              std::vector<std::string> value,
                                              std::vector<double> d_value);
  
  /**
   * This adds a new integer userParameterVector to the detectorElement.
   * This parameter has a name, a type, a comment and a value, given both as a
   * std::vector<std::string> and as a std::vector<int>
   * If this parameter vector was already existing, it is replaced by the new
   * one.
   * @param name the name of the parameter vector
   * @param type the type of the parameter vector. This is only a clue for
   * the user, it is not used by the detector element itself
   * @param comment a comment on this parameter vector use
   * @param value the value of the parameter vector, as a vector of strings
   * @param d_value the value of the parameter vector, as a vector of ints
   */
  virtual void addUserParameterVector (std::string name,
                                              std::string type,
                                              std::string comment,
                                              std::vector<std::string> value,
                                              std::vector<double> d_value,
                                              std::vector<int> i_value);
  
  /**
   * This adds a new non-numerical userParameterVector to the detectorElement.
   * This parameter has a name, a type, a comment and a value, given as a
   * std::vector<std::string>.
   * If this parameter vector was already existing, it is replaced by the new
   * one.
   * @param name the name of the parameter vector
   * @param type the type of the parameter vector. This is only a clue for
   * the user, it is not used by the detector element itself
   * @param comment a comment on this parameter vector use
   * @param value the value of the parameter vector, as a vector of strings
   * @param d_value the value of the parameter vector, as a vector of doubles
   * strings by default
   */
  virtual void addUserParameterVector (std::string name,
                                              std::string type,
                                              std::string comment,
                                              std::vector<std::string> value);
  
  /**
   * this gets the type of a parameter.
   * If this parameter does not exist, it raises a DetectorElementException.
   * @param name the name of the parameter
   * @return its type
   */
  virtual std::string userParameterType (std::string name);
  
  /**
   * this gets the comment of a parameter
   * If this parameter does not exist, it raises a DetectorElementException.
   * @param name the name of the parameter
   * @return its comment
   */
  virtual std::string userParameterComment (std::string name);
  
  /**
   * this gets the value of a parameter, as a string
   * If this parameter does not exist, it raises a DetectorElementException.
   * @param name the name of the parameter
   * @return its value, as a string
   */
  virtual std::string userParameterAsString (std::string name);
  
  /**
   * this gets the value of a parameter, as an int
   * If the value is not an int, it raises a DetectorElementException.
   * If this parameter does not exist, it raises a DetectorElementException.
   * @param name the name of the parameter
   * @return its value, as a string
   */
  virtual int userParameterAsInt (std::string name);
  
  /**
   * this gets the value of a parameter, as a double
   * If the value is not a number, it raises a DetectorElementException.
   * If this parameter does not exist, it raises a DetectorElementException.
   * @param name the name of the parameter
   * @return its value, as a double
   */
  virtual double userParameterAsDouble (std::string name);
  
  /**
   * this gets the value of the parameter as a double. This actually is an
   * equivalent of userParameterAsDouble.
   * If the value is not a double, it raises a DetectorElementException.
   * If this parameter does not exist, it raises a DetectorElementException.
   * @param name the name of the parameter
   * @return its value, as a double
   */
  virtual double userParameter (std::string name);
  
  /**
   * this gets the type of a parameter vector
   * If this parameter does not exist, it raises a DetectorElementException.
   * @param name the name of the parameter vector
   * @return its type
   */
  virtual std::string userParameterVectorType (std::string name);
  
  /**
   * this gets the comment of a parameter vector
   * If this parameter does not exist, it raises a DetectorElementException.
   * @param name the name of the parameter vector
   * @return its comment
   */
  virtual std::string userParameterVectorComment (std::string name);
  
  /**
   * this gets the value of a parameter vector, as a vector of string
   * If this parameter does not exist, it raises a DetectorElementException.
   * @param name the name of the parameter vector
   * @return its value, as a string
   */
  virtual std::vector<std::string>
  userParameterVectorAsString (std::string name);
  
  /**
   * this gets the value of a parameter vector, as a vector of int
   * If the parameter vector is not made of ints, it raises a
   * DetectorElementException.
   * If this parameter does not exist, it raises a DetectorElementException.
   * @param name the name of the parameter vector
   * @return its value, as a string
   */
  virtual std::vector<int> userParameterVectorAsInt (std::string name);
  
  /**
   * this gets the value of a parameter as a vector of double.
   * If the parameter vector is not made of numbers, it raises a
   * DetectorElementException.
   * If this parameter does not exist, it raises a DetectorElementException.
   * @param name the name of the parameter
   * @return its value, as a vector of double
   */
  virtual std::vector<double>
  userParameterVectorAsDouble (std::string name);

  /**
   * this gets the value of a parameter as a vector of double. This actually is
   * an equivalent of userParameterVectorAsDouble.
   * If the parameter vector is not made of doubles, it raises a
   * DetectorElementException.
   * If this parameter does not exist, it raises a DetectorElementException.
   * @param name the name of the parameter
   * @return its value, as a vector of double
   */
  virtual std::vector<double> userParameterVector (std::string name);

  /**
   * this returns the list of existing userParameters as a vector of their
   * names
   * @return a list of userParameter names
   */
  virtual std::vector<std::string> userParameters();
  
  /**
   * this returns the list of existing userParameterVectors as a vector of
   * their names
   * @return a list of userParameter names
   */
  virtual std::vector<std::string> userParameterVectors();
  

public:

  virtual StatusCode 
  queryInterface( const InterfaceID& , void** );
  virtual unsigned long addRef  ();
  virtual unsigned long release ();
  
protected:
  
  /// specific 
  void setGeometry( IGeometryInfo* geoInfo ) { m_de_iGeometry = geoInfo; }
  
  static IDataProviderSvc*  dataSvc ();
  static IMessageSvc*       msgSvc  ();
  
private:

  // technicalities 

  inline void Assert (bool assertion,
                      const std::string& name =
                      "DetectorElement Unknown Exception") const;

  // for IDetectorElement implementation 
  IGeometryInfo*         m_de_iGeometry     ;
  IAlignment*            m_de_iAlignment    ;
  ICalibration*          m_de_iCalibration  ;   
  IReadOut*              m_de_iReadOut      ;
  ISlowControl*          m_de_iSlowControl  ;
  IFastControl*          m_de_iFastControl  ;

  mutable bool                              m_de_childrensLoaded;
  mutable IDetectorElement::IDEContainer    m_de_childrens; 

  // for IValidity implementation
  ITime*                 m_de_validSince    ;
  ITime*                 m_de_validTill     ;

  /// reference/object counter 
  static unsigned long   s_count;
  
  /// This defines the type of a userParameter
  enum userParamKind { DOUBLE, INT, OTHER };

  /// This defines a user parameter
  typedef struct _userParam {
    std::string type;
    std::string comment;
    std::string value;
    double d_value;
    int i_value;
    userParamKind kind;
  } UserParam;

  /// this defines a map of UserParam
  typedef std::map<std::string, UserParam> UserParamMap;

  /// This defines a user parameter
  typedef struct _userParamVector {
    std::string type;
    std::string comment;
    std::vector<std::string> value;
    std::vector<double> d_value;
    std::vector<int> i_value;
    userParamKind kind;
  } UserParamVector;

  /// this defines a map of UserParamVector
  typedef std::map<std::string, UserParamVector> UserParamVectorMap;

  /// this is the list of user defined parameters
  UserParamMap m_userParameters;

  /// this is the list of user defined parameter vectors
  UserParamVectorMap m_userParameterVectors;

};

// implementation of the inlines functions
#include "DetDesc/DetectorElement.icpp"

#endif    //    DETDESC_DETECTORELEMENT_H 

