// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/DetDesc/ISolid.h,v 1.3 2001-03-20 23:45:38 mato Exp $
#ifndef DETDESC_ISOLID_H
#define DETDESC_ISOLID_H 1 
// STL
#include  <iostream>
#include  <string> 
#include  <vector>
#if defined(__GNUC__) &&  ( __GNUC__ == 2 &&  __GNUC_MINOR__ == 91 )
#include  <vector> 
#else
#include  <deque> 
#endif 
// Include files 
#include "GaudiKernel/IInspectable.h" 
#include "GaudiKernel/IInspector.h" 

// forward declaration (from CLHEP)
class HepPoint3D;  
class HepVector3D;  
class StreamBuffer;

/** @class ISolid ISolid.h DetDesc/ISolid.h
    
    Interface to Solid. 

    @author Vanya Belyaev 
*/

class ISolid : public virtual IInspectable 
{
 public:
  
  /// define useful types for dealing with intersection with line 
  typedef double             Tick  ; 

#if defined (__GNUC__) && ( __GNUC__ == 2 && __GNUC_MINOR__ == 91 )
  /// define useful types for dealing with intersection with line 
  typedef std::vector<Tick>  Ticks ;
#else 
  /// define useful types for dealing with intersection with line 
  typedef std::deque<Tick>   Ticks ;
#endif 
  
 public:
  
  ///  return the specific name of the solid 
  virtual const std::string&        name         (                      ) const = 0; 
  
  /// return the name of TYPE for given solid 
  virtual       std::string         typeName     (                      ) const = 0;
  
  ///  check for the given point (local frame)
  virtual       bool                isInside     ( const HepPoint3D   & ) const = 0;
  
  /// return pointer to "simplified" solid - "cover"
  virtual const ISolid*             cover        (                      ) const = 0;
  
  /// return pointer to "the most simplified cover" - probably, something like "gabarite box"
  virtual const ISolid*             coverTop     (                      ) const = 0; 
  
  /// printout   
  virtual       std::ostream&       printOut     ( std::ostream&        ) const = 0;
  
  /// reset to the initial ("after constructor") state
  virtual const ISolid*             reset        (                      ) const = 0;

  /** calculate the intersection points("ticks") with a given line. 
      Input - line, paramterised by  x_vect = Point + Vector * T 
      "tick" is just a value of T, at which the intersection occurs
      @param Point initial point for the line
      @param Vector vector along the line
      @param ticks output container of "Ticks"
      @return the number of intersection points (=size of Ticks container)
  */
  virtual       unsigned int        intersectionTicks ( const HepPoint3D & Point  , 
							const HepVector3D& Vector , 
							Ticks            & ticks  ) const = 0 ; 
  
  /** calculate the intersection points("ticks") with a given line. 
      Input - line, paramterised by  x_vect = Point + Vector * T 
      "tick" is just a value of T, at which the intersection occurs 
      @param Point initial point for the line
      @param Vector vector along the line
      @param tickMin minimal value of "Tick"  
      @param tickMax maximal value of "Tick"  
      @param ticks output container of "Ticks"
      @return the number of intersection points (=size of Ticks container)
              between tickMin and tickMax
  */
  virtual       unsigned int        intersectionTicks ( const HepPoint3D & Point   , 
							const HepVector3D& Vector  ,
                                                        const Tick       & tickMin ,      
                                                        const Tick       & tickMax , 
							Ticks            & ticks   ) const = 0 ;
  
  /// destructor 
  virtual  ~ISolid(){};

  /// serialization for reading
  virtual StreamBuffer& serialize( StreamBuffer& )        = 0 ;

  /// serialization for writing 
  virtual StreamBuffer& serialize( StreamBuffer& ) const  = 0 ;

};

/// output operator to std::ostream 
inline std::ostream& operator<<( std::ostream&  os , const ISolid&  solid  ) 
{ return solid.printOut(os); }

/// output operator to std::ostream 
inline std::ostream& operator<<( std::ostream&  os , const ISolid*  solid  ) 
{ return ( ( 0 == solid ) ? ( os << "ISolid* points to NULL" ) : ( os << *solid ) ); }

/// input/output operators from/to StreamBuffer 
inline StreamBuffer& operator<<( StreamBuffer& sb , const ISolid& solid ) { return solid.serialize( sb ) ; } 
inline StreamBuffer& operator>>( StreamBuffer& sb ,       ISolid& solid ) { return solid.serialize( sb ) ; } 

 
#endif   // DETDESC_ISOLID_H





