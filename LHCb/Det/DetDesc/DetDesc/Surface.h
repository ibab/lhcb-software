#ifndef     DETDESC_SURFACE_H 
#define     DETDESC_SURFACE_H  1
/// STL
#include <string>
/// GaudiKernel
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartRefVector.h"
#include "GaudiKernel/MsgStream.h"
/// DetDesc
#include "DetDesc/CLIDSurface.h"
///
class TabulatedProperty;
/// 

/** @class Surface Surface.h DetDesc/Surface.h
    
    Class to define optical surfaces and thier non-trivial
    optical properties.
    Class is later converted into combination of Geant4 
    classes: G4OpticalSurface, G4LogicalSkinSurface
    G4LogicalBorderSurface
 
    @author  Vanya Belyaev
    @date    28/02/2001
*/


class Surface: public DataObject
{
  ///
public:
  ///
  typedef SmartRefVector<TabulatedProperty> Tables;
  ///
public:
  ///
  /// constructor
  Surface( const std::string& Name = "");
  virtual ~Surface();
  /// 
  virtual const CLID& clID    () const { return Surface::classID() ; }
  static  const CLID& classID ()       { return CLID_Surface       ; }

  /// Fill the output stream (ASCII)
  virtual std::ostream& fillStream ( std::ostream& s ) const ;
  /// Fill the output stream (ASCII)
  virtual MsgStream&    fillStream ( MsgStream&    s ) const ;
  ///
  /// accessors: (naming from Geant4, except for last)
  /// "model" 
  inline unsigned int    model       () const ;  
  inline Surface&     setModel       ( const unsigned int ); 
  /// "finish"
  inline unsigned int    finish      () const ; 
  inline Surface&     setFinish      ( const unsigned int ); 
  /// "type"
  inline unsigned int    type        () const ; 
  inline Surface&     setType        ( const unsigned int ); 
  /// "value" - (NB: ugly name!)
  inline double          value       () const ; 
  inline Surface&     setValue       ( const double       ); 
  /// name of first  physical volume 
  inline const std::string&    firstVol    () const ;
  inline Surface&           setFirstVol    ( const std::string& ); 
  /// name of second physical volume 
  inline const std::string&    secondVol   () const ;
  inline Surface&           setSecondVol   ( const std::string& ); 
  /// tables of optical prorties 
  inline const Tables& tabulatedProperties () const ;
  inline       Tables& tabulatedProperties ()       ;
  ///
private:
  ///
  unsigned int m_model      ; /// G4OpticalSurfaceModel
  /// 
  unsigned int m_finish     ; /// G4OpticalSurfaceFinish
  ///
  unsigned int m_type       ; /// G4OpticalSurfaceType 
  ///
  double       m_value      ; /// used for "model"
  ///
  std::string  m_firstVol   ; /// G4PhysicalVolume
  /// 
  std::string  m_secondVol  ; /// G4PhysicalVolume
  ///
  Tables       m_props      ; /// tables of properties 
  ///
};
/////////////////////////////////////////////////////////////////////////
inline std::ostream& operator<<( std::ostream& os , const Surface& surf )        
{ return surf.fillStream( os ); }
/////////////////////////////////////////////////////////////////////////
inline MsgStream&    operator<<( MsgStream&    os , const Surface& surf )          
{ return surf.fillStream( os ); }
/////////////////////////////////////////////////////////////////////////
inline std::ostream& operator<<( std::ostream& os , const Surface* surf )          
{ return surf ? (os<<*surf) : (os<<" Surface* points to NULL!"<<std::endl) ; }
/////////////////////////////////////////////////////////////////////////
inline MsgStream&    operator<<( MsgStream&    os , const Surface* surf )           
{ return surf ? (os<<*surf) : (os<<" Surface* points to NULL!"<<endmsg) ; }
/////////////////////////////////////////////////////////////////////////


/// "model" //////////////////////////////////////////////////////////////
inline unsigned int Surface::   model     () const { return m_model;  }  
//////////////////////////////////////////////////////////////////////////
inline Surface&     Surface::setModel     ( const unsigned int val )
{ m_model  = val ; return *this; }  
/// "finish" /////////////////////////////////////////////////////////////
inline unsigned int Surface::   finish    () const { return m_finish; }
//////////////////////////////////////////////////////////////////////////
inline Surface&     Surface::setFinish    ( const unsigned int val ) 
{ m_finish = val ; return *this; }  
/// "type" ///////////////////////////////////////////////////////////////
inline unsigned int Surface::   type      () const { return m_type  ; }
//////////////////////////////////////////////////////////////////////////
inline Surface&     Surface::setType      ( const unsigned int val ) 
{ m_type   = val ; return *this; }  
/// "value" - (NB: ugly name!) ///////////////////////////////////////////
inline double       Surface::   value     () const { return m_value ; }
//////////////////////////////////////////////////////////////////////////
inline Surface&     Surface::setValue     ( const double       val ) 
{ m_value  = val ; return *this; }  
/// name of first  physical volume ///////////////////////////////////////
inline const std::string& Surface::   firstVol  () const { return m_firstVol ; }
//////////////////////////////////////////////////////////////////////////
inline Surface&           Surface::setFirstVol  ( const std::string& val ) 
{ m_firstVol  = val ; return *this; }  
/// name of second physical volume ///////////////////////////////////////
inline const std::string& Surface::   secondVol () const { return m_secondVol; }
//////////////////////////////////////////////////////////////////////////
inline Surface&           Surface::setSecondVol ( const std::string& val ) 
{ m_secondVol = val ; return *this; }  
/// tables of optical prorties ///////////////////////////////////////////
inline const Surface::Tables& Surface::tabulatedProperties() const { return m_props; }
//////////////////////////////////////////////////////////////////////////
inline       Surface::Tables& Surface::tabulatedProperties()       { return m_props; }
//////////////////////////////////////////////////////////////////////////

#endif  //  DETDESC_SURFACE_H
