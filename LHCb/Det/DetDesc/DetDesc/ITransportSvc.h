// $Id: ITransportSvc.h,v 1.5 2004-07-09 14:19:41 cattanem Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
#ifndef DETDESC_ITRANSPORTSVC_H 
#define DETDESC_ITRANSPORTSVC_H 1

// Framework include files
#include "GaudiKernel/IService.h"
#include "DetDesc/ILVolume.h" 

// Forward declarations
class HepPoint3D;
class HepVector3D;
class IGeometryInfo;

/** Declaration of the unique interface identifier  
 *  ( interface id, major version, minor version) 
 */
static const InterfaceID IID_ITransportSvc( "ITransportSvc" , 2 , 0);

/** @class ITransportSvc ITransportSvc.h DetDesc/ITransportSvc.h
 *
 *  Definition of abstract interface for Transport Service
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 */
class ITransportSvc : virtual public IService
{
 public:
  
  /// Retrieve the unique interface ID
  static const InterfaceID& interfaceID() { return IID_ITransportSvc; }
  
  /** Estimate the distance between 2 points in units 
   *  of radiation length units
   *  @param point1 first  point
   *  @param point2 second point
   *  @param threshold threshold value
   *  @param alternativeGeometry source of alternative geometry information
   *  @param geometryGuess a guess for navigation
   */
  virtual double 
  distanceInRadUnits
  ( const HepPoint3D& point1                         ,
    const HepPoint3D& point2                         ,
    double            threshold                 = 0  ,
    IGeometryInfo*    alternativeGeometry       = 0  ,
    IGeometryInfo*    geometryGuess             = 0  )  = 0 ;
  
  /** general method ( returns the "full history" of the volume 
   *  boundary intersections 
   *  with different material properties between 2 points )
   *  @see ILVolume 
   *  @see IPVolume 
   *  @see ISolid  
   *  @see IGeometryInfo
   *  @see Material
   *  @param point   initial point on the line
   *  @param vector  direction vector of the line 
   *  @param tickMin minimal value of line paramater
   *  @param tickMax maximal value of line parameter 
   *  @param intersept (output) container of intersections
   *  @param threshold threshold value
   *  @param alternativeGeometry  source of alternative geometry information
   *  @param geometryGuess a guess for navigation
   */
  virtual unsigned long 
  intersections
  ( const HepPoint3D& point                          ,
    const HepVector3D&       vector                  ,
    const ISolid::Tick&      tickMin                 ,
    const ISolid::Tick&      tickMax                 ,
    ILVolume::Intersections& intersept               ,
    double                   threshold           = 0 ,
    IGeometryInfo*           alternativeGeometry = 0 ,
    IGeometryInfo*           geometryGuess       = 0 ) = 0  ;
  
protected:

  /// virtual destructor 
  virtual ~ITransportSvc(){}; 

};

// ============================================================================
// the End 
// ============================================================================
#endif  //  GAUDIKERNEL_ITRANSPORTSVC_H
// ============================================================================







