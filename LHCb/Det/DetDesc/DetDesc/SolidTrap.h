#ifndef     __DETDESC_SOLID_SOLIDTRAP_H__
#define     __DETDESC_SOLID_SOLIDTRAP_H__ 1 


#include <cmath>
#include <iostream> 


#include "GaudiKernel/ISolid.h" 


#include "CLHEP/Geometry/Point3D.h" 
#include "CLHEP/Geometry/Vector3D.h" 
#include "CLHEP/Units/PhysicalConstants.h" 

#include "DetDesc/SolidException.h" 

///
///  class SolidTrap : a simple implementation of General Trapezoid
///
///  Author: Vanya Belyaev 
///  date  : 20 sept 2000 
///

class SolidTrap: public ISolid
{
  ///
  typedef std::vector<HepVector3D>  FACE;   
  typedef std::vector<FACE>         FACES; 
  typedef std::vector<HepPoint3D>   VERTS;
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
  virtual inline std::ostream&     printOut     ( std::ostream&       ) const;
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
 private: 
  ///
  SolidTrap           ( const SolidTrap & );  // no copy-constructor 
  SolidTrap& operator=( const SolidTrap & );  // no assignment 
  ///
  inline const HepPoint3D&            point    ( const VERTS::size_type& i ) const { return m_trap_vertices[i]; };
  ///  
  inline bool                         inside   ( const HepPoint3D& p , const FACE& face ) const;  
  ///
  inline double                       distance ( const HepPoint3D& p , const FACE& face ) const;  
  ///
  void addFace(  const HepPoint3D& p1 , 
		 const HepPoint3D& p2 , 
		 const HepPoint3D& p3 , 
		 const HepPoint3D& p4 ) const;  
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
  mutable FACES        m_trap_faces;    
  ///
  VERTS                m_trap_vertices;
  ///
};

///
///
///

inline const ISolid* SolidTrap::coverTop      () const 
{
  const ISolid* cov = this; 
  while( cov != cov->cover() ){ cov = cov->cover(); } 
  return cov; 
};


///
///
///


inline std::ostream&  SolidTrap::printOut      ( std::ostream&  os ) const
{
  return 
    os << typeName()   << "name=" << name()
       << " (zHalfLength="       << zHalfLength      () / millimeter << "[mm];"  
       <<   "theta="             << theta            () / degree     << "[deg];" 
       <<   "phi="               << phi              () / degree     << "[deg];" 
       <<   "dyAtMinusZ="        << dyAtMinusZ       () / millimeter << "[mm];"  
       <<   "dxAtMinusZMinusY="  << dxAtMinusZMinusY () / millimeter << "[mm];" 
       <<   "dxAtMinusZPlusY="   << dxAtMinusZPlusY  () / millimeter << "[mm];" 
       <<   "alphaAtMinusZ="     << alphaAtMinusZ    () / degree     << "[deg]"
       <<   "dyAtPlusZ="         << dyAtPlusZ        () / millimeter << "[mm];"  
       <<   "dxAtPlusZMinusY="   << dxAtPlusZMinusY  () / millimeter << "[mm];" 
       <<   "dxAtPlusZPlusY="    << dxAtPlusZPlusY   () / millimeter << "[mm];" 
       <<   "alphaAtPlusZ="      << alphaAtPlusZ     () / degree     << "[deg]";
};

///
///
/// reset to the initial state
inline const   ISolid* SolidTrap::reset() const
{
  if( 0 != m_trap_cover ) { delete m_trap_cover ; m_trap_cover = 0 ; } 
  return this;
};
///
///
///

inline   bool  SolidTrap::isInside( const HepPoint3D& point) const
{
  ///
  if( abs( point.z() ) > zHalfLength() ) { return false ; }  
  ///
  /// loop over all faces 
  for( FACES::const_iterator pface = m_trap_faces.begin() ; m_trap_faces.end() != pface ; ++pface ) 
    { 
      ///
      if( !inside( point , *pface ) ) { return false; } 
      ///
    } 
  ///
  return true;
  ///
};

///
///
///

inline bool SolidTrap::inside( const HepPoint3D& p , const SolidTrap::FACE& face ) const
{
  ///
  double alpha = distance( p , face ) ; 
  ///
  return alpha > 0 ? true : false ; 
  ///
}; 

///
/// "distance" along the normal to the face
///

inline double  SolidTrap::distance( const HepPoint3D& p , const SolidTrap::FACE& face ) const
{
  ///
  if( 4 != face.size() ) { throw SolidException("SolidTrap::inside wrond dimension of FACE!") ; }
  ///
  const HepVector3D& p0 = face[1] ;  // point on the face
  const HepVector3D& n  = face[3] ;  // (unit) normal to the face 
  ///
  return p0*n - p*n ; 
  ///
}; 

///
///
///


#endif //     __DETDESC_SOLID_SOLIDTRAP_H__
 



















