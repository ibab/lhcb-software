#ifndef      __DETDESC_SOLID_SOLIDBOOLEAN_H__ 
#define      __DETDESC_SOLID_SOLIDBOOLEAN_H__ 

#include <string> 
#include <deque> 

#include "GaudiKernel/ISolid.h" 

class StatusCode;

class SolidChild;

class HepPoint3D;
class HepVector3D;
class HepTransform3D; 
class HepRotation;

class StreamBuffer;

///
///
///
///   SolidBoolean: a simple helper class (common base) for implementation of 
///                 complex "boolean" solids. 
///                 represents 1 "main"("first") solid and a container of 
///                 "childrens"  placed inside of "main" 
///
///  Author: Vanya Belyaev 
///


class SolidBoolean: public ISolid
{
  
 public:

  //
  typedef std::deque<SolidChild*>         SolidChildrens;
  //
  
  //
  // constructor - "main"("first") solid is mandatory! 
  SolidBoolean( const std::string& name  , 
		ISolid*            solid );
  //
  virtual ~SolidBoolean();
  //  
  
  /// from ISolid   // NB!! isInside still pure absract!!! 
  /// name of the solid 
  inline  const    std::string&  name     (                      ) const { return m_sb_name           ; }; 
  /// type of the solid 
  inline  virtual  std::string   typeName (                      ) const { return "SolidBoolean"      ; };
  /// covering solid (the same as for "main"/"first" solid 
  inline  const    ISolid*       cover    (                      ) const { return first()->cover()    ; };  
  /// the top covering solid (the same as for "main"/"first" solid)
  inline  const    ISolid*       coverTop (                      ) const { return first()->coverTop() ; }; 
  /// overloaded printout 
  inline  virtual  std::ostream& printOut ( std::ostream&        ) const ;
  /// reset to the initial state
  inline  const    ISolid*       reset    (                      ) const ;
  
  /// calculate the intersection points("ticks") with a given line. 
  /// Input - line, paramterised by (Point + Vector * Tick) 
  /// "Tick" is just a value of parameter, at which the intercestion occurs 
  /// Return the number of intersection points (=size of Ticks container)   
  virtual unsigned int intersectionTicks ( const HepPoint3D & Point  ,          // initial point for teh line 
					   const HepVector3D& Vector ,          // vector along the line 
					   ISolid::Ticks    & ticks  ) const ;  // output container of "Ticks"
  /// calculate the intersection points("ticks") with a given line. 
  /// Input - line, paramterised by (Point + Vector * Tick) 
  /// "Tick" is just a value of parameter, at which the intercestion occurs 
  /// Return the number of intersection points (=size of Ticks container)   
  virtual unsigned int intersectionTicks ( const HepPoint3D  & Point   ,          // initial point for teh line 
					   const HepVector3D & Vector  ,          // vector along the line 
					   const ISolid::Tick& tickMin ,          // minimal value for the tick 
					   const ISolid::Tick& tickMax ,          // maximal value for the tick 
					   ISolid::Ticks   &   ticks   ) const ;  // output container of "Ticks"
  // from SolidBoolean 
  // return poiter to the "main"/"first" boolean 
  inline  const  ISolid*       first       ()                    const { return m_sb_first;            } ; 
  // return number of childrens 
  inline         unsigned int  noChildrens ()                    const { return m_sb_childrens.size(); } ;
  // access to the childrens by index 
  inline  const  ISolid*       operator[]  (unsigned int       ) const ;

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

  inline SolidChildrens::const_iterator  childBegin () const { return m_sb_childrens.begin(); }; 
  inline SolidChildrens::const_iterator  childEnd   () const { return m_sb_childrens.end  (); }; 

 protected:
  
  // add child to daughter container 
  StatusCode                             addChild   ( ISolid*               child    , 
						      const HepTransform3D* mtrx     );
  
  // add child to daughter container 
  StatusCode                             addChild   ( ISolid*               child    , 
						      const HepPoint3D&     position , 
                                                      const HepRotation&    rotation );

  // iterators 
  inline SolidChildrens::iterator        childBegin ()       { return m_sb_childrens.begin(); }; 
  inline SolidChildrens::iterator        childEnd   ()       { return m_sb_childrens.end  (); }; 

  ///
 private:
  ///
  std::string                    m_sb_name     ;    
  mutable     ISolid*            m_sb_first    ;  
  mutable     SolidChildrens     m_sb_childrens; 
  //

};

///
///
///

#include "DetDesc/SolidChild.h" 


///
///
///
inline const ISolid* SolidBoolean::operator[]( unsigned int index ) const 
{  return  ( ( index < noChildrens() )  ? *(childBegin()+index) : 0 ); }

//
//
//

inline std::ostream& SolidBoolean::printOut( std::ostream& os ) const 
{
  os << typeName()      << " name=" << name(); 
  os << " First="       << first();
  os << " Booled with " << m_sb_childrens.size() << " solids=(";  
  std::copy( childBegin() , childEnd() , std::ostream_iterator<const ISolid*>(os,",") );
  os << ");" ; 
  return os;
};

///
///
///
/// reset to the initial state
inline  const    ISolid* SolidBoolean::reset    () const 
{
  first()->reset();
  // due to stupid NT-compiler we are forced to write an ugly stuff instead one nice line!
  // std::for_each( childBegin() , childEnd() , std::mem_fun(&ISolid::reset));
  for( SolidChildrens::const_iterator iter = childBegin() ; iter != childEnd() ; ++iter ) { if( (*iter) != 0 ) { (*iter)->reset(); } }
  ///
  return this;
};

///
///
///

#endif   //  __DETDESC_SOLID_SOLIDBOOLEAN_H__ 
