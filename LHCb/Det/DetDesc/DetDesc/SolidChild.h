#ifndef       __DETDESC_SOLID_SOLIDCHILD_H__ 
#define       __DETDESC_SOLID_SOLIDCHILD_H__ 


#include <functional>

#include "GaudiKernel/ISolid.h" 

#include "DetDesc/SolidException.h" 

#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Vector/Rotation.h"

class HepRotation; 
class HepTransform3D;

class ISolid; 
class SolidBoolean; 
class StreamBuffer; 

///
/// 
///   class SolidChild::  siple helper class for implementation of "complex" boolean solids
///                     represent a "solid with position" (e.g. hole in the "mother" solid)
///
///
///   Author: Vanya Belyaev 
///

class SolidChild : public ISolid
{

  friend class SolidBoolean;
  
 protected: 

  // constructor
  SolidChild();
  
  // constructor                  ( protected ! )  
  SolidChild( ISolid*               Solid      , 
	      const HepTransform3D* mtrx   = 0 );
  
  // another form of constructor  ( protected ! )
  SolidChild( ISolid*               Solid                     ,
	      const HepPoint3D&     Position                  , 
              const HepRotation&    Rotation = HepRotation()  ); 

  // copy constructor             ( protected ! )
  SolidChild           ( const SolidChild&     Solid );
 
  // assignment                   ( protected ! ) 
  SolidChild& operator=( const SolidChild&     Solid );

 public: 
  
  // destructor 
  virtual ~SolidChild();

 public:
  
  // delegate all functions from ISolid to solid itself
  inline const std::string&        name         () const { return solid()->name()     ; };  
  inline       std::string         typeName     () const { return solid()->typeName() ; };
  inline const ISolid*             cover        () const { return solid()->cover()    ; };  
  inline const ISolid*             coverTop     () const { return solid()->coverTop() ; }; 

  // printout   
  virtual      std::ostream&       printOut     ( std::ostream&        ) const ;
  
  // except this notorious "is inside method" 
  inline       bool                isInside     ( const HepPoint3D   & ) const ;
  
  /// reset to the initial state
  inline const ISolid*             reset        (                      ) const ; 
  
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
                                                   const ISolid::Tick& tickMin , 
                                                   const ISolid::Tick& tickMax , 
						   ISolid::Ticks   &   ticks   ) const ;  // output container of "Ticks"
  
  // specific

  // "new" method - return solid itself 
  inline const ISolid*             solid        () const { return m_sc_solid; };

  // is solid transformed?  
  inline       bool                simple       () const { return m_sc_simple; };  

  // transfromation
  inline const HepTransform3D&     matrix       () const ; 
  //

  ///
  /// Inspectable Interface 
  ///
  virtual bool acceptInspector( IInspector* )       ; 
  ///
  virtual bool acceptInspector( IInspector* ) const ; 
  ///

 protected:
  
  /// serialization for reading
  virtual StreamBuffer& serialize( StreamBuffer& s )       ; 
  
  /// serialization for writing 
  virtual StreamBuffer& serialize( StreamBuffer& s ) const ;  
  /// 
 private:

  mutable ISolid*                m_sc_solid   ; 
  bool                           m_sc_simple  ;
  mutable HepTransform3D*        m_sc_matrix  ;  

};

//
//
//

inline const HepTransform3D& SolidChild::matrix() const
{
  if( 0 == m_sc_matrix ) { m_sc_matrix = new  HepTransform3D; }
  return *m_sc_matrix;
};

//
//
//

inline std::ostream& SolidChild::printOut     ( std::ostream& os ) const
{ return os << "SolidChild with " << solid(); };


inline bool SolidChild::isInside( const HepPoint3D& point ) const
{ return  ( simple() ? solid()->isInside(point) : solid()->isInside( matrix() * point ) ) ; }; 

//
//
// 

class Solid_isInside: public std::unary_function<const ISolid*,bool>
{
  
 public:
  //
  explicit Solid_isInside( const HepPoint3D& pointInMotherSystem ) 
    : point( pointInMotherSystem ){};
  //
  inline bool operator() ( const ISolid* solid ) const  
    { return ( 0 == solid ? false : solid->isInside(point) ); };
  //
 private:
  //
  HepPoint3D point;
  //
};


///
///
///

/// reset to the initial state
inline const ISolid* SolidChild::reset        () const  { if( 0 != solid() ) { solid()->reset() ; } return this; }


/// calculate the intersection points("ticks") with a given line. 
/// Input - line, paramterised by (Point + Vector * Tick) 
/// "Tick" is just a value of parameter, at which the intercestion occurs 
/// Return the number of intersection points (=size of Ticks container)   
inline  unsigned int SolidChild::intersectionTicks ( const HepPoint3D & point  ,          // initial point for teh line 
						     const HepVector3D & vect   ,          // vector along the line 
						     ISolid::Ticks    & ticks  ) const   // output container of "Ticks"
{
  return solid()->intersectionTicks(  simple() ? point : matrix() * point , 
                                      simple() ? vect  : matrix() * vect  , 
				      ticks                               ) ;
};

///
///
///

/// calculate the intersection points("ticks") with a given line. 
/// Input - line, paramterised by (Point + Vector * Tick) 
/// "Tick" is just a value of parameter, at which the intercestion occurs 
/// Return the number of intersection points (=size of Ticks container)   
inline  unsigned int SolidChild::intersectionTicks ( const HepPoint3D &  point   ,          // initial point for teh line 
						     const HepVector3D &  vect    ,          // vector along the line 
                                                     const ISolid::Tick& tickMin , 
                                                     const ISolid::Tick& tickMax , 
						     ISolid::Ticks    &  ticks   ) const   // output container of "Ticks"
{
  return solid()->intersectionTicks(  simple() ? point : matrix() * point , 
                                      simple() ? vect  : matrix() * vect  , 
				      tickMin , tickMax , ticks           ) ;
};

///
///
///


#endif   //   __DETDESC_SOLID_SOLIDCHILD_H__ 
