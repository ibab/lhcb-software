// $Id: Surface.h,v 1.3 2005-12-07 13:19:07 cattanem Exp $
#ifndef     DETDESC_SURFACE_H 
#define     DETDESC_SURFACE_H  1
/// STL
#include <string>
/// GaudiKernel
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartRefVector.h"
/// DetDesc
#include "DetDesc/CLIDSurface.h"
///
class TabulatedProperty;
class MsgStream;
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
  inline const unsigned int    model       () const ;  
  inline Surface&           setModel       ( const unsigned int ); 
  /// "finish"
  inline const unsigned int    finish      () const ; 
  inline Surface&           setFinish      ( const unsigned int ); 
  /// "type"
  inline const unsigned int    type        () const ; 
  inline Surface&           setType        ( const unsigned int ); 
  /// "value" - (NB: ugly name!)
  inline const double          value       () const ; 
  inline Surface&           setValue       ( const double       ); 
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
///
#include "DetDesc/Surface.icpp"
///

#endif  //  DETDESC_SURFACE_H 







