// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/DetDesc/DetectorElement.h,v 1.4 2001-01-25 15:36:43 ibelyaev Exp $
#ifndef  GAUDIKERNEL_DETECTORELEMENT_H
#define  GAUDIKERNEL_DETECTORELEMENT_H 

// Include files
#include <algorithm>

#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/MsgStream.h"

#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/ILVolume.h"
#include "GaudiKernel/IPVolume.h"
#include "GaudiKernel/IDetectorElement.h"

#include "GaudiKernel/IValidity.h"
#include "GaudiKernel/ITime.h"

// Forward declarations

class IDataProviderSvc;
class IMessageSvc;
template <class T> class DataObjectFactory;



// unique ID of the class
#include "DetDesc/CLIDDetectorElement.h"


/** @class DetectorElement DetectorElement.h GaudiKernel/DetectorElement.h

    A "basic" implementation of Detector Description - Detector Element

    @author Rado Chytracek
    @author Ivan Belyaev
*/

class DetectorElement: public DataObject,
                       public IDetectorElement,
		       public IValidity 
{
  ///
  friend class DataObjectFactory<DetectorElement> ;  
  ///
 public:
  ///
  /// Constructors 
  DetectorElement( const std::string& name  = "" ,
		   IDataProviderSvc*  datasvc     = 0  ,
		   IMessageSvc*       messsvc     = 0  );  
  
  DetectorElement( const std::string& name ,
		   const ITime&       iValidSince     , 
		   const ITime&       iValidTill      , 
		   IDataProviderSvc*  datasvc     = 0 ,
		   IMessageSvc*       messsvc     = 0 );  
  ///  Destructor 
  virtual ~DetectorElement(); 
public:
    /// Obtain class ID
  virtual const  CLID& clID()   const  { return classID(); }
  static const   CLID& classID()       { return CLID_DetectorElement; }

  ///
  inline virtual const std::string& name () const { return DataObject::fullpath() ; } 

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
  
  
  
  //   
  // from IValidity interface
  //   
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
  
  //
  //
  //
  // specific 

  void setGeometry( IGeometryInfo* geoInfo ) { m_de_iGeometry = geoInfo; }
  
  
  /*
  //
  // specific 
  //
  // create "ghost" 
  const IGeometryInfo* createGeometryInfo( IDataProviderSvc*            datasvc = 0 ,
					   IMessageSvc*                 messsvc = 0 ); 
  // create "orphan"
  const IGeometryInfo* createGeometryInfo( const std::string&           logVol      ,
					   IDataProviderSvc*            datasvc = 0 ,
					   IMessageSvc*                 messsvc = 0 );
  // create "regular"
  const IGeometryInfo* createGeometryInfo( const std::string&           logVol      , 
					   const std::string&           support     ,
					   const ILVolume::ReplicaPath& replicaPath ,
					   IDataProviderSvc*            datasvc = 0 ,
					   IMessageSvc*                 messsvc = 0 );
  */  
  
  ///  const std::string& nameToBeRegistered() const { return m_de_name; } 
  
  //
  virtual MsgStream&    printOut( MsgStream&    ) const;  
  //

  
  //
  // some functions to simplify the navigation 
  // (according to feedback after release 3) 
  //
  
  // pointer to parent IDetectorElement
  inline virtual       IDetectorElement*  parentIDetectorElement()       ;  
  
  // pointer to parent IDetectorElement (const version)
  inline virtual const IDetectorElement*  parentIDetectorElement() const ; 
  
  // (reference to) container of pointers to child detector elements 
  inline virtual       IDetectorElement::IDEContainer&  childIDetectorElements()   ;

  // iterators for manipulation of daughter elements 
  
  // begin iterator 
  inline virtual       IDetectorElement::IDEContainer::iterator  childBegin()      ;
  
  // end   iterator 
  inline virtual       IDetectorElement::IDEContainer::iterator  childEnd  ()      ;
 

  ///  
  /// IInspectable interface:
  ///
  virtual bool acceptInspector( IInspector* )       ; 
  ///
  virtual bool acceptInspector( IInspector* ) const ; 
  ///

  ///
  /// serialization for reading 
  virtual StreamBuffer& serialize( StreamBuffer& )       ; 
  /// serialization for writing 
  virtual StreamBuffer& serialize( StreamBuffer& ) const ; 

 protected:
  
  inline IMessageSvc*       msgSvc () const; 
// Obsoleted name, kept due to the backwards compatibility
  inline IMessageSvc*       messageService () const; 

 private:
  
  // technicalities 
  IDataProviderSvc*         dataSvc() ;
  //
  class DetectorElementException; // exception
  inline void Assert( bool assertion , const std::string& name = "DetectorElement Unknown Exception" ) const; 
  
 private: 
  
  // just name, nothing more
  std::string            m_de_name          ;         
  
  // for IDetectorElement implementation 

  IGeometryInfo*         m_de_iGeometry     ;
  IAlignment*            m_de_iAlignment    ;
  ICalibration*          m_de_iCalibration  ;   
  IReadOut*              m_de_iReadOut      ;
  ISlowControl*          m_de_iSlowControl  ;
  IFastControl*          m_de_iFastControl  ;
  
  mutable bool                              m_de_parentLoaded;
  mutable IDetectorElement*                 m_de_parent;

  mutable bool                              m_de_childrensLoaded;
  mutable IDetectorElement::IDEContainer    m_de_childrens; 

  // for IValidity implementation
  
  ITime*                 m_de_validSince    ;
  ITime*                 m_de_validTill     ; 
  
 private: 
  
  // technicalities:
  mutable IDataProviderSvc*  m_de_dataSvc       ;
  IMessageSvc*               m_de_messageSvc    ; 
  //
  
};

//
//
//  output operators to MsgStream
//
inline MsgStream&    operator<<( MsgStream&    os ,  const DetectorElement& de ) { return de.printOut(os); };
inline MsgStream&    operator<<( MsgStream&    os ,  const DetectorElement* de )
{ return ( (0 == de) ? (os << "DetectorElement* points to NULL") : ( os << *de ) ) ; };

//
//
//

inline const IGeometryInfo* DetectorElement::geometry   () const { return  m_de_iGeometry    ; };
inline       IGeometryInfo* DetectorElement::geometry   ()       { return  m_de_iGeometry    ; };
inline const IAlignment*    DetectorElement::alignment  () const { return  m_de_iAlignment   ; };
inline       IAlignment*    DetectorElement::alignment  ()       { return  m_de_iAlignment   ; };
inline const ICalibration*  DetectorElement::calibration() const { return  m_de_iCalibration ; };
inline       ICalibration*  DetectorElement::calibration()       { return  m_de_iCalibration ; };
inline const IReadOut*      DetectorElement::readOut    () const { return  m_de_iReadOut     ; };
inline       IReadOut*      DetectorElement::readOut    ()       { return  m_de_iReadOut     ; };
inline const ISlowControl*  DetectorElement::slowControl() const { return  m_de_iSlowControl ; };
inline       ISlowControl*  DetectorElement::slowControl()       { return  m_de_iSlowControl ; };
inline const IFastControl*  DetectorElement::fastControl() const { return  m_de_iFastControl ; };
inline       IFastControl*  DetectorElement::fastControl()       { return  m_de_iFastControl ; };

// "pseudo-casting" 
inline DetectorElement::operator const IGeometryInfo*()    const { return  m_de_iGeometry    ; };
inline DetectorElement::operator       IGeometryInfo*()          { return  m_de_iGeometry    ; };
inline DetectorElement::operator const IAlignment*   ()    const { return  m_de_iAlignment   ; };
inline DetectorElement::operator       IAlignment*   ()          { return  m_de_iAlignment   ; };
inline DetectorElement::operator const ICalibration* ()    const { return  m_de_iCalibration ; };
inline DetectorElement::operator       ICalibration* ()          { return  m_de_iCalibration ; };
inline DetectorElement::operator const IReadOut*     ()    const { return  m_de_iReadOut     ; };
inline DetectorElement::operator       IReadOut*     ()          { return  m_de_iReadOut     ; };
inline DetectorElement::operator const ISlowControl* ()    const { return  m_de_iSlowControl ; };
inline DetectorElement::operator       ISlowControl* ()          { return  m_de_iSlowControl ; };
inline DetectorElement::operator const IFastControl* ()    const { return  m_de_iFastControl ; };
inline DetectorElement::operator       IFastControl* ()          { return  m_de_iFastControl ; };

//
//
// 
inline DetectorElement::operator const IGeometryInfo&()    const 
{
  Assert( 0 != m_de_iGeometry , "DetectorElement::geometry is not available!" );
  return *m_de_iGeometry    ; 
};
//
inline DetectorElement::operator       IGeometryInfo&()          
{ 
  Assert( 0 != m_de_iGeometry , "DetectorElement::geometry is not available!" );
  return *m_de_iGeometry    ; 
};
//
inline DetectorElement::operator const IAlignment&   ()    const 
{ 
  Assert( 0 != m_de_iAlignment , "DetectorElement::alignment is not available!" );
  return *m_de_iAlignment   ; 
};
//
inline DetectorElement::operator       IAlignment&   ()          
{ 
  Assert( 0 != m_de_iAlignment , "DetectorElement::alignment is not available!" );
  return *m_de_iAlignment   ; 
};
//
inline DetectorElement::operator const ICalibration& ()    const 
{ 
  Assert( 0 != m_de_iCalibration , "DetectorElement::calibration is not available!" );
  return *m_de_iCalibration ; 
};
//
inline DetectorElement::operator       ICalibration& ()          
{ 
  Assert( 0 != m_de_iCalibration , "DetectorElement::calibration is not available!" );
  return *m_de_iCalibration ; 
};
//
inline DetectorElement::operator const IReadOut&     ()    const 
{ 
  Assert( 0 != m_de_iReadOut , "DetectorElement::readout is not available!" );
  return *m_de_iReadOut     ; 
};
//
inline DetectorElement::operator       IReadOut&     ()          
{ 
  Assert( 0 != m_de_iReadOut , "DetectorElement::readout is not available!" );
  return *m_de_iReadOut     ; 
};
//
inline DetectorElement::operator const ISlowControl& ()    const 
{ 
  Assert( 0 != m_de_iSlowControl , "DetectorElement::slowcontrol is not available!" );
  return *m_de_iSlowControl ; 
};
//
inline DetectorElement::operator       ISlowControl& ()          
{ 
  Assert( 0 != m_de_iSlowControl , "DetectorElement::slowcontrol is not available!" );
  return *m_de_iSlowControl ; 
};
//
inline DetectorElement::operator const IFastControl& ()    const 
{ 
  Assert( 0 != m_de_iFastControl , "DetectorElement::fastcontrol is not available!" );
  return *m_de_iFastControl ; 
};
//
inline DetectorElement::operator       IFastControl& ()          
{ 
  Assert( 0 != m_de_iFastControl , "DetectorElement::fastcontrol is not available!" );
  return *m_de_iFastControl ; 
};

//
//
//


//
//
//  from IValidity Interface:
inline bool DetectorElement::isValid ()                 { return validSince() < validTill() ;};
inline bool DetectorElement::isValid ( const ITime& t ) { return validSince() < t &&  t < validTill() ;};

//
//
// technicalities 
//
// Exception class
//
class DetectorElement::DetectorElementException
{
  //
 public:
  //
  explicit DetectorElementException ( const std::string&      name      = "DetectorElementException::UNKNOWN" , 
				      const DetectorElement*  detElem   = 0                                   , 
				      IMessageSvc*            messSvc   = 0                                   )
    : m_dee_exceptionName   ( name    ) 
    , m_dee_detectorElement ( detElem ) 
    , m_dee_messageSvc      ( messSvc ) 
    { print(); };
  //
  inline void print() const 
    { 
      MsgStream log( m_dee_messageSvc , "DetectorElementException");
      log << MSG::ERROR << m_dee_exceptionName; 
      if( 0 != m_dee_detectorElement ) { log << " for " << m_dee_detectorElement; }
      log << endreq;
    };
  //
 private:
  //
  std::string             m_dee_exceptionName    ;   // exception name 
  const DetectorElement*  m_dee_detectorElement  ;   // "author" for the exception 
  IMessageSvc*            m_dee_messageSvc       ;   // message service 
  //
};

//
//
//


inline       IMessageSvc*   DetectorElement::msgSvc  () const { return m_de_messageSvc; }; 
// Obsoleted name, kept due to the backwards compatibility
inline       IMessageSvc*   DetectorElement::messageService  () const { return m_de_messageSvc; }; 

//
//
// assertion 
inline void DetectorElement::Assert( bool assertion , const std::string& assertionName ) const
{ 
  if( !assertion ) 
    { throw DetectorElement::DetectorElementException( assertionName , this , msgSvc() ) ; } 
};

//
//
//

// pointer to parent IDetectorElement
inline       IDetectorElement*  DetectorElement::parentIDetectorElement()       
{
  if( m_de_parentLoaded ) { return m_de_parent; }
  DataObject* pObj = parent(); 
  IDetectorElement* ide = 0 ;
  try   { if( 0 != pObj ) { ide = dynamic_cast<IDetectorElement*>(pObj) ; } }
  catch (...)             { ide = 0                                     ;   } 
  if( 0 != ide ) { m_de_parentLoaded = true; } 
  m_de_parent = ide; 
  return m_de_parent;
};  

// pointer to parent IDetectorElement (const version)
inline const IDetectorElement*  DetectorElement::parentIDetectorElement() const 
{
  const DataObject* pObj = parent(); 
  const IDetectorElement* ide = 0 ;
  try   { if( 0 != pObj ) { ide = dynamic_cast<const IDetectorElement*>(pObj) ; } }
  catch (...)             { ide = 0                                           ;   } 
  return ide;
};  
  

//
//
// (reference to) container of pointers to child detector elements 
inline       IDetectorElement::IDEContainer&  DetectorElement::childIDetectorElements()
{
  if( m_de_childrensLoaded ) { return m_de_childrens; } 
  //
  IDataDirectory*  dir = this->directory(); 
  if( 0 != dir )
    {
      IDataDirectory::DirIterator it = dir->begin();
      while( dir->end() != it ) 
	{
	  IDetectorElement* ide = 0;
	  try{  
	    DataObject* pDo = 0 ; 
	    StatusCode sc = StatusCode::FAILURE;
            if( 0 != dataSvc() )
	      { sc = dataSvc()->retrieveObject( (DataObject*) this , (*it++)->name() , pDo ); }
	    if( sc.isSuccess() && 0 != pDo ) { ide = dynamic_cast<IDetectorElement*>(pDo); }
	  }
	  catch(...) { ide = 0 ; } 
	  m_de_childrens.push_back( ide  );
	}
    }
  //
  m_de_childrensLoaded = true; 
  //
  return m_de_childrens;
};

///
///
///

///
///
/// begin iterator 
inline IDetectorElement::IDEContainer::iterator  DetectorElement::childBegin() { return childIDetectorElements().begin(); } 

///
///
/// end   iterator 
inline IDetectorElement::IDEContainer::iterator  DetectorElement::childEnd  () { return childIDetectorElements().end()  ; } 

///
///
///
#endif    //  GAUDIKERNEL_DETECTORELEMENT_H 
