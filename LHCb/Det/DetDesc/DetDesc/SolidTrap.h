#ifndef     DETDESC_SOLIDTRAP_H
#define     DETDESC_SOLIDTRAP_H 1 
/// STD and STL 
#include <cmath>
#include <iostream> 
/// CLHEP
#include "CLHEP/Geometry/Point3D.h" 
#include "CLHEP/Geometry/Vector3D.h" 
#include "CLHEP/Geometry/Plane3D.h" 
#include "CLHEP/Units/PhysicalConstants.h" 
/// GaudiKernel
#include "GaudiKernel/ISolid.h" 
/// DetDesc 
#include "DetDesc/SolidException.h" 
#include "DetDesc/SolidPolyHedronHelper.h" 
///
class ISolidFromStream;
class StreamBuffer; 


/** @class SolidTrap SolidTrap.h DetDesc/SolidTrap.h

    A simple implementation of TRAP
    
    @author Vanya Belyaev 
*/

class SolidTrap: public  virtual ISolid                ,
                 private virtual SolidPolyHedronHelper
{
  ///
  friend class ISolidFromStream;
  ///
  typedef std::vector<HepPoint3D> VERTICES; 
  ///
 public: 
  ///
  /// constructor 
  SolidTrap( const std::string&  name             , /* name of this solid */ 
             const double        zHalfLength      , /* half length along z-axes for given solid */
             const double        theta            , /* polar angle of "axe" of this trapezoid   */  
             const double        phi              , /* asimuthal angle of the "axe" of this trapezoid */   
             const double        dyAtMinusZ       ,  
             const double        dxAtMinusZMinusY , 
             const double        dxAtMinusZPlusY  ,
             const double        alphaAtMinusZ    ,
             const double        dyAtPlusZ        ,  
             const double        dxAtPlusZMinusY  , 
             const double        dxAtPlusZPlusY   ,
             const double        alphaAtPlusZ    );
  ///
  /// destructor 
  virtual ~SolidTrap();
  ///
  /// functions from ISolid:
  /// name of this solid 
  inline  const  std::string&      name         ()                      const { return m_trap_name  ; };
  /// type of this solid 
  inline         std::string       typeName     ()                      const { return "SolidTrap"  ; };
  /// the notorious "isInside" method 
  inline         bool              isInside     ( const HepPoint3D&   ) const;
  /// covering solid 
  inline const   ISolid*           cover        ()                      const;
  /// the top covering solid 
  inline const   ISolid*           coverTop     ()                      const;
  //// printout   
  virtual std::ostream&     printOut   ( std::ostream& os = std::cerr ) const;
  virtual MsgStream&        printOut   ( MsgStream&                   ) const;
  /// reset to the initial state
  inline const   ISolid*           reset        ()                      const; 
  /// calculate the intersection points("ticks") with a given line. 
  /// Input - line, paramterised by (Point + Vector * Tick) 
  /// "Tick" is just a value of parameter, at which the intercestion occurs 
  /// Return the number of intersection points (=size of Ticks container)   
  virtual inline  unsigned int intersectionTicks ( const HepPoint3D&  Point  ,          // initial point for teh line 
                                                   const HepVector3D& Vector ,          // vector along the line 
                                                   ISolid::Ticks   &  ticks  ) const ;  // output container of "Ticks"
  /// calculate the intersection points("ticks") with a given line. 
  /// Input - line, parametrized by (Point + Vector * Tick) 
  /// "Tick" is just a value of parameter, at which the intercestion occurs 
  /// Return the number of intersection points (=size of Ticks container)   
  virtual inline  unsigned int intersectionTicks ( const HepPoint3D&   Point   ,          // initial point for teh line 
                                                   const HepVector3D&  Vector  ,          // vector along the line 
                                                   const ISolid::Tick& tickMin ,          // minimal value for the tick 
                                                   const ISolid::Tick& tickMax ,          // maximal value for tick 
                                                   ISolid::Ticks   &   ticks   ) const ;  // output container of "Ticks"
  ///
  /// function specific for SolidTrd
  ///
  /// half size in z  
  inline const double&                zHalfLength      () const { return m_trap_zHalfLength      ; };
  /// polar and asimutal angle of the "axes" :-)) of thei general trapezoid 
  inline const double&                theta            () const { return m_trap_theta            ; };
  inline const double&                phi              () const { return m_trap_phi              ; }; 
  /// parameters of the bottom face 
  inline const double&                dyAtMinusZ       () const { return m_trap_dyAtMinusZ       ; }; 
  inline const double&                dxAtMinusZMinusY () const { return m_trap_dxAtMinusZMinusY ; };
  inline const double&                dxAtMinusZPlusY  () const { return m_trap_dxAtMinusZPlusY  ; };
  inline const double&                alphaAtMinusZ    () const { return m_trap_alphaAtMinusZ    ; }; 
  /// parameters of the top face
  inline const double&                dyAtPlusZ        () const { return m_trap_dyAtPlusZ        ; }; 
  inline const double&                dxAtPlusZMinusY  () const { return m_trap_dxAtPlusZMinusY  ; };
  inline const double&                dxAtPlusZPlusY   () const { return m_trap_dxAtPlusZPlusY   ; };
  inline const double&                alphaAtPlusZ     () const { return m_trap_alphaAtPlusZ     ; }; 
  ///

  /// serialization for reading
  StreamBuffer& serialize( StreamBuffer& )       ; 
  /// serialization for writing
  StreamBuffer& serialize( StreamBuffer& ) const ; 
  ///

  ///
  /// Inspectable Interface 
  ///
  virtual bool acceptInspector( IInspector* )       ; 
  ///
  virtual bool acceptInspector( IInspector* ) const ; 
  ///

 protected:
  ///
  SolidTrap();
  ///
 private: 
  ///
  SolidTrap           ( const SolidTrap & );  // no copy-constructor 
  SolidTrap& operator=( const SolidTrap & );  // no assignment 
  ///
  void        makeAll();
  ///
  inline      const HepPoint3D& point( const VERTICES::size_type& indx ) const { return m_trap_vertices[indx]; }  
  ///
 private:
  ///
  /// name of this solid 
  std::string          m_trap_name              ; 
  ///
  double               m_trap_zHalfLength       ; 
  ///
  double               m_trap_theta             ; 
  double               m_trap_phi               ; 
  double               m_trap_dyAtMinusZ        ; 
  double               m_trap_dxAtMinusZMinusY  ; 
  double               m_trap_dxAtMinusZPlusY   ; 
  double               m_trap_alphaAtMinusZ     ; 
  ///
  double               m_trap_dyAtPlusZ         ; 
  double               m_trap_dxAtPlusZMinusY   ; 
  double               m_trap_dxAtPlusZPlusY    ; 
  double               m_trap_alphaAtPlusZ      ; 
  double               m_trap_sinAlphaAtPlusZ   ; 
  double               m_trap_cosAlphaAtPlusZ   ; 
  ///
  mutable ISolid*      m_trap_cover;
  ///
  VERTICES             m_trap_vertices; 
  ///
};
///
#include  "DetDesc/SolidTrap.icpp"
///


#endif //     DETDESC_SOLIDTRAP_H
 



















