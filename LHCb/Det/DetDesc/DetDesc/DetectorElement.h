#ifndef  DETDESC_DETECTORELEMENT_H
#define  DETDESC_DETECTORELEMENT_H 1
// STD and STL 
#include <algorithm>
/// GaudiKernel
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IDataDirectory.h"
#include "GaudiKernel/ILVolume.h"
#include "GaudiKernel/IPVolume.h"
#include "GaudiKernel/IDetectorElement.h"
#include "GaudiKernel/IValidity.h"
#include "GaudiKernel/ITime.h"
#include "GaudiKernel/ISvcLocator.h"
// GaudiKernel
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
/// DetDesc 
#include "DetDesc/CLIDDetectorElement.h"
///
class IDataProviderSvc;
class IMessageSvc;
template <class T> class DataObjectFactory;
///


/** @class DetectorElement DetectorElement.h GaudiKernel/DetectorElement.h

    A "basic" implementation of Detector Description - Detector Element

    @author Rado Chytracek
    @author Ivan Belyaev
*/

///
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
  DetectorElement( const std::string& name  = "NotYetDefined" );
  
  DetectorElement( const std::string& name         ,
                   const ITime&       iValidSince  , 
                   const ITime&       iValidTill   ); 
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
  /// 
  virtual MsgStream&    printOut( MsgStream&    ) const;  
  // pointer to parent IDetectorElement
  inline virtual       IDetectorElement*  parentIDetectorElement()       
    { return ( 0 != parent() ) ? dynamic_cast<IDetectorElement*>     ( parent() ) : 0 ; };  
  // pointer to parent IDetectorElement (const version)
  inline virtual const IDetectorElement*  parentIDetectorElement() const  
    { return ( 0 != parent() ) ? dynamic_cast<const IDetectorElement*>( parent() ) : 0 ; };  
  // (reference to) container of pointers to child detector elements 
  inline virtual       IDetectorElement::IDEContainer&  childIDetectorElements()       ;
  inline virtual const IDetectorElement::IDEContainer&  childIDetectorElements() const ;
  // iterators for manipulation of daughter elements 
  inline virtual IDetectorElement::IDEContainer::iterator        childBegin()       { return childIDetectorElements().begin() ; }
  inline virtual IDetectorElement::IDEContainer::const_iterator  childBegin() const { return childIDetectorElements().begin() ; }
  inline virtual IDetectorElement::IDEContainer::iterator        childEnd  ()       { return childIDetectorElements().end  () ; }
  inline virtual IDetectorElement::IDEContainer::const_iterator  childEnd  () const { return childIDetectorElements().end  () ; }
  /// IInspectable interface:
  virtual bool acceptInspector( IInspector* )       ; 
  virtual bool acceptInspector( IInspector* ) const ; 
  /// serialization for reading 
  virtual StreamBuffer& serialize( StreamBuffer& )       ; 
  /// serialization for writing 
  virtual StreamBuffer& serialize( StreamBuffer& ) const ; 
  ///
  /// specific   
  // create "ghost" 
  const IGeometryInfo* createGeometryInfo();
  // create "orphan"
  const IGeometryInfo* createGeometryInfo( const std::string&            logVol      );
  // create "regular"
  const IGeometryInfo* createGeometryInfo( const std::string&            logVol      , 
                                           const std::string&            support     ,
                                           const ILVolume::ReplicaPath & replicaPath );
  const IGeometryInfo* createGeometryInfo( const std::string&            logVol      , 
                                           const std::string&            support     ,
                                           const std::string&            namePath    );
 protected:
  /// specific 
  void setGeometry( IGeometryInfo* geoInfo ) { m_de_iGeometry = geoInfo; }
  ///
  inline IMessageSvc*       msgSvc         () const { return m_de_msgSvc ; }  
  inline IMessageSvc*       messageService () const { return msgSvc()    ; }
  inline ISvcLocator*       svcLoc         () const { return m_de_svcLoc ; } 
  inline ISvcLocator*       svcLocator     () const { return svcLoc()    ; } 
  ///
 private:
  ///
  // technicalities 
  inline IDataProviderSvc*  dataSvc () const { return m_de_dataSvc ; } 
  ////
  inline void Assert( bool assertion , const std::string& name = "DetectorElement Unknown Exception" ) const; 
  ///
 private:   
  // for IDetectorElement implementation 
  IGeometryInfo*         m_de_iGeometry     ;
  IAlignment*            m_de_iAlignment    ;
  ICalibration*          m_de_iCalibration  ;   
  IReadOut*              m_de_iReadOut      ;
  ISlowControl*          m_de_iSlowControl  ;
  IFastControl*          m_de_iFastControl  ;
  //
  mutable bool                              m_de_childrensLoaded;
  mutable IDetectorElement::IDEContainer    m_de_childrens; 
  // for IValidity implementation
  ITime*                 m_de_validSince    ;
  ITime*                 m_de_validTill     ; 
  //  
 private: 
  // technicalities:
  IDataProviderSvc*      m_de_dataSvc       ;
  IMessageSvc*           m_de_msgSvc        ; 
  ISvcLocator*           m_de_svcLoc        ;
  //
};
///
#include "DetDesc/DetectorElement.icpp"
///

#endif    //    DETDESC_DETECTORELEMENT_H 

