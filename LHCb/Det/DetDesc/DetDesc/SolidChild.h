#ifndef       DETDESC_SOLIDCHILD_H 
#define       DETDESC_SOLIDCHILD_H 1
/// STD and STL 
#include <functional>
// CLHEP
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Vector/Rotation.h"
/// GaudiKernel
#include "GaudiKernel/ISolid.h" 
/// Detdesc 
#include "DetDesc/SolidException.h" 

class HepRotation; 
class HepTransform3D;

class MsgStream;
class SolidBoolean; 
class StreamBuffer; 


/** @class SolidChild SolidChild.h DetDesc/SolidChils.h
    
    Simple helper class for implementation of "complex" boolean solids
    represent a "solid with position" (e.g. hole in the "mother" solid)
    
    @author Vanya Belyaev 
*/


class SolidChild : public ISolid
{
  ///
  friend class SolidBoolean;
  ///
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
  //
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
  virtual std::ostream& printOut ( std::ostream& os = std::cerr ) const ;
  // printout   
  virtual MsgStream&    printOut ( MsgStream&                   ) const ;
  
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
  
  // "new" method - return solid itself 
  inline const ISolid*             solid        () const { return m_sc_solid; };
  // is solid transformed?  
  inline       bool                simple       () const { return m_sc_simple; };  
  // transfromation
  inline const HepTransform3D&     matrix       () const ; 
  /// Inspectable Interface 
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
  ///
  mutable ISolid*                m_sc_solid   ; 
  bool                           m_sc_simple  ;
  mutable HepTransform3D*        m_sc_matrix  ;  
  ///
};
///
#include "DetDesc/SolidChild.icpp"
///

#endif   //   DETDESC_SOLIDCHILD_H 
