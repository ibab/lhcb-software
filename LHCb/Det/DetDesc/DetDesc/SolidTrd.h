#ifndef     __DETDESC_SOLID_SOLIDTRD_H__
#define     __DETDESC_SOLID_SOLIDTRD_H__ 1 

#include <cmath>
#include <iostream> 


#include "GaudiKernel/ISolid.h" 


#include "CLHEP/Geometry/Point3D.h" 
#include "CLHEP/Geometry/Vector3D.h" 
#include "CLHEP/Units/PhysicalConstants.h" 

#include "DetDesc/SolidPolyHedronHelper.h" 

class StreamBuffer;
class ISolidFromStream;

///
///  class SolidTrd : a simple implementation of Trapezoid
///
///  Author: Vanya Belyaev 
///

class SolidTrd: public  virtual ISolid , 
                private virtual SolidPolyHedronHelper 
{
  //
  friend class ISolidFromStream;
  //
 public:
  //
  // constructor, all sizes in mm, no any default values! 
  SolidTrd( const std::string& name         ,
	    const double       zHalfLength  , 
	    const double       XHalfLength1 , 
	    const double       YHalfLength1 , 
            const double       XHalfLength2 , 
            const double       YHalfLength2 );
  //
  // destructor 
  virtual ~SolidTrd();
  //
  // functions from ISolid:
  // name of this solid 
  inline  const  std::string&      name         ()                      const { return m_trd_name  ; };
  // type of this solid 
  inline         std::string       typeName     ()                      const { return "SolidTrd"; };
  // the notorious "isInside" method 
  inline         bool              isInside     ( const HepPoint3D& p ) const { return SolidPolyHedronHelper::isInside( p ) ; }  
  // covering solid 
  inline const   ISolid*           cover        ()                      const;
  // the top covering solid 
  inline const   ISolid*           coverTop     ()                      const;
  /// printout   
  virtual inline std::ostream&     printOut     ( std::ostream&       ) const;
  /// reset to the initial state
  inline const   ISolid*           reset        ()                      const; 
  
  /// calculate the intersection points("ticks") with a given line. 
  /// Input - line, paramterised by (Point + Vector * Tick) 
  /// "Tick" is just a value of parameter, at which the intercestion occurs 
  /// Return the number of intersection points (=size of Ticks container)   
  virtual inline  unsigned int intersectionTicks ( const HepPoint3D& Point  ,          // initial point for teh line 
						   const HepVector3D& Vector ,          // vector along the line 
						   ISolid::Ticks   & ticks  ) const ;  // output container of "Ticks"
  /// calculate the intersection points("ticks") with a given line. 
  /// Input - line, paramterised by (Point + Vector * Tick) 
  /// "Tick" is just a value of parameter, at which the intercestion occurs 
  /// Return the number of intersection points (=size of Ticks container)   
  virtual inline  unsigned int intersectionTicks ( const HepPoint3D&   Point   ,          // initial point for teh line 
						   const HepVector3D&   Vector  ,          // vector along the line 
                                                   const ISolid::Tick& tickMin ,          // minimal value for the tick 
                                                   const ISolid::Tick& tickMax ,          // maximal value for tick 
						   ISolid::Ticks   &   ticks   ) const ;  // output container of "Ticks"
  //
  // function specific for SolidTrd
  //
  // half size in x at point 1 
  inline       double                xHalfLength1() const { return m_trd_xHalfLength1; };
  // half size in x at point 2 
  inline       double                xHalfLength2() const { return m_trd_xHalfLength2; };
  // half size in y at point 1 
  inline       double                yHalfLength1() const { return m_trd_yHalfLength1; };
  // half size in y at point 2 
  inline       double                yHalfLength2() const { return m_trd_yHalfLength2; };
  // half size in z  
  inline       double                zHalfLength () const { return m_trd_zHalfLength; };
  
  // full size in x at point 1 
  inline       double                xLength1    () const { return m_trd_xHalfLength1 * 2 ; };
  // full size in x at point 2 
  inline       double                xLength2    () const { return m_trd_xHalfLength2 * 2 ; };
  // full size in y at point 1 
  inline       double                yLength1    () const { return m_trd_yHalfLength1 * 2 ; };
  // full size in y at point 2 
  inline       double                yLength2    () const { return m_trd_yHalfLength2 * 2 ; };
  // full size in z  
  inline       double                zLength     () const { return m_trd_zHalfLength  * 2 ; };
  //

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
  SolidTrd();
  ///
 private:
  //
  void makeAll() ; 
  //
 private:
  //
  SolidTrd           ( const SolidTrd & );  // no copy-constructor 
  SolidTrd& operator=( const SolidTrd & );  // no assignment 
  //
  // members
  //
  std::string            m_trd_name  ;
  double                 m_trd_zHalfLength  ;
  double                 m_trd_xHalfLength1 ;
  double                 m_trd_xHalfLength2 ;
  double                 m_trd_yHalfLength1 ;
  double                 m_trd_yHalfLength2 ;
  //
  mutable ISolid*        m_trd_cover;
  //
};


//
//
//

inline const ISolid* SolidTrd::coverTop      () const 
{
  const ISolid* cov = this; 
  while( cov != cov->cover() ){ cov = cov->cover(); } 
  return cov; 
};

//
//
//

inline std::ostream&  SolidTrd::printOut      ( std::ostream&  os ) const
{
  os << typeName()   << "name=" << name();
  os << " (zLength="        << zLength        () / millimeter << "[mm];";
  os <<   "xsize1="         << xLength1       () / millimeter << "[mm];";
  os <<   "ysize1="         << yLength1       () / millimeter << "[mm];";
  os <<   "xsize2="         << xLength2       () / millimeter << "[mm];";
  os <<   "ysize2="         << yLength2       () / millimeter << "[mm])";
  return os;  
};

//
//
//


///
///
///
inline  unsigned int SolidTrd::intersectionTicks ( const HepPoint3D&  Point  ,          // initial point for teh line 
						   const HepVector3D& Vector ,          // vector along the line 
						   ISolid::Ticks   &  ticks  ) const    // output container of "Ticks"
{ return SolidPolyHedronHelper::intersectionTicks( Point, Vector , ticks ); } 

/// calculate the intersection points("ticks") with a given line. 
/// Input - line, parametrized by (Point + Vector * Tick) 
/// "Tick" is just a value of parameter, at which the intercestion occurs 
/// Return the number of intersection points (=size of Ticks container)   
inline  unsigned int SolidTrd::intersectionTicks ( const HepPoint3D&   Point   ,          // initial point for teh line 
						   const HepVector3D&  Vector  ,          // vector along the line 
						   const ISolid::Tick& tickMin ,          // minimal value for the tick 
						   const ISolid::Tick& tickMax ,          // maximal value for tick 
						   ISolid::Ticks   &   ticks   ) const    // output container of "Ticks"
{ return SolidPolyHedronHelper::intersectionTicks( Point, Vector , tickMin , tickMax , ticks ); } 
///
///
///
///


///
///
/// reset to the initial state
inline const   ISolid* SolidTrd::reset() const
{
  if( 0 != m_trd_cover ) { delete m_trd_cover ; m_trd_cover = 0 ; } 
  return this;
};
///
///
///

///
///
///

#endif //   __DETDESC_SOLID_SOLIDTRD_H__




