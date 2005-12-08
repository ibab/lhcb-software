// $Id: SolidBoolean.h,v 1.19 2005-12-08 19:20:01 jpalac Exp $ 
// ===========================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ===========================================================================
#ifndef      DETDESC_SOLIDBOOLEAN_H 
#define      DETDESC_SOLIDBOOLEAN_H  1 
/// STL & STD 
#include <string> 
#include <vector> 
#include <algorithm> 
/// DetDesc 
#include "DetDesc/SolidChild.h" 

// Forward declarations
class StatusCode;
class MsgStream;


/** @class SolidBoolean SolidBoolean.h "DetDesc/SolidBoolean.h"
 *
 *  A simple helper class (common base) for implementation of 
 *  complex "boolean" solids. 
 *  represents 1 "main"("first") solid and a container of 
 *  "children"  placed inside of "main" 
 *  
 *  @author Vanya Belyaev 
 */

class SolidBoolean: public virtual SolidBase
{  
 public:
  //
  typedef std::vector<SolidChild*>         SolidChildrens;
  //

  /** retrieve the specific type of the solid
   *  @return specific type of the solid
   */
  virtual  std::string   typeName () const { return "SolidBoolean"; };
  
  /** retrieve the pointer to "simplified" solid - "cover"
   *  @return pointer to "simplified" solid - "cover"
   */
  virtual const ISolid* cover () const { return first()->cover() ; };  

  /** retrieve the pointer to "the most simplified cover"
   *  probably, something like "gabarite box"
   *  @return pointer to the most simplified cover
   */
  virtual const ISolid* coverTop () const { return first()->coverTop() ; };
  
  /** printout to STD/STL stream
   *  @param os STD/STL stream
   *  @return reference to the stream
   */
  virtual  std::ostream& printOut ( std::ostream& os = std::cout ) const ;
  
  /** printout to Gaudi  stream
   *  @param os Gaudi stream
   *  @return reference to the stream
   */
  virtual  MsgStream&    printOut ( MsgStream&    os             ) const ;
  
  /** reset to the initial ("after constructor") state
   */
  virtual ISolid* reset() ;
  
  /** calculate the intersection points("ticks") with a given line.
   *  Input - line, paramterised by  x_vect = Point + Vector * T
   *  "tick" is just a value of T, at which the intersection occurs
   *  @param Point initial point for the line
   *  @param Vector vector along the line
   *  @param ticks output container of "Ticks"
   *  @return the number of intersection points (=size of Ticks container)
   */
  virtual unsigned int intersectionTicks( const Gaudi::XYZPoint & Point,
                                          const Gaudi::XYZVector& Vector,
                                          ISolid::Ticks& ticks  ) const ;

  virtual unsigned int intersectionTicks( const Gaudi::Polar3DPoint  & Point,
                                          const Gaudi::Polar3DVector & Vector,
                                          ISolid::Ticks     & ticks) const ; 

  virtual unsigned int intersectionTicks( const Gaudi::RhoZPhiPoint  & Point,
                                          const Gaudi::RhoZPhiVector & Vector,
                                          ISolid::Ticks     & ticks) const ; 

  /** calculate the intersection points("ticks") with a given line.
   *  Input - line, paramterised by  x_vect = Point + Vector * T
   *  "tick" is just a value of T, at which the intersection occurs
   *  @param Point initial point for the line
   *  @param Vector vector along the line
   *  @param tickMin minimal value of "Tick"
   *  @param tickMax maximal value of "Tick"
   *  @param ticks output container of "Ticks"
   *  @return the number of intersection points (=size of Ticks container)
   *  between tickMin and tickMax
   */

  virtual unsigned int intersectionTicks( const Gaudi::XYZPoint& Point,
                                          const Gaudi::XYZVector & Vector,
                                          const ISolid::Tick& tickMin,
                                          const ISolid::Tick& tickMax,
                                          ISolid::Ticks& ticks   ) const ; 

  virtual unsigned int intersectionTicks( const Gaudi::Polar3DPoint& Point,
                                          const Gaudi::Polar3DVector & Vector,
                                          const ISolid::Tick& tickMin,
                                          const ISolid::Tick& tickMax,
                                          ISolid::Ticks& ticks   ) const ; 

  virtual unsigned int intersectionTicks( const Gaudi::RhoZPhiPoint& Point,
                                          const Gaudi::RhoZPhiVector & Vector,
                                          const ISolid::Tick& tickMin,
                                          const ISolid::Tick& tickMax,
                                          ISolid::Ticks& ticks   ) const ; 

  /** poiter to the "main"/"first" boolean 
   *  @return poiter to the "main"/"first" boolean 
   */
  const  ISolid* first () const { return m_sb_first; } ; 
  
  /** number of childrens 
   *  @return number of childrens 
   */
  unsigned int  noChildrens () const { return m_sb_childrens.size(); } ;

  /** access to the childrens by index 
   *  @param index index of child solid 
   *  @return pointer to child solid 
   */
  const ISolid* operator[]  ( unsigned int index ) const 
  {  return  ( ( index < noChildrens() )  ? *(childBegin()+index) : 0 ) ; } ;
  
  /** acess to constant iterator
   *  @return "begin" iterator
   */  
  SolidChildrens::const_iterator  
  childBegin () const { return m_sb_childrens.begin(); }; 
  
  /** acess to constant iterator
   *  @return "end" iterator
   */  
  SolidChildrens::const_iterator  
  childEnd   () const { return m_sb_childrens.end  (); }; 
  
  ///
protected:
  
  /** constructor - "main"("first") solid is mandatory! 
   *  @param name name of the solid 
   *  @param solid pointer to teh "first"/"main" solid
   *  @exception SolidException NULL pointer to ISolid
   */
  SolidBoolean( const std::string& name  , 
                ISolid*            solid );
  
  /// destructor 
  virtual ~SolidBoolean();

  /** constructor - "main"("first") solid is mandatory! 
   *  @param name name of the solid 
   */
  SolidBoolean ( const std::string& name="Undefined" );
  
  /** add child to daughter container 
   *  @param child pointer to solid 
   *  @param mtrx  pointer to transformation 
   *  @return status code 
   */
  StatusCode addChild   
  ( ISolid*               child , 
    const Gaudi::Transform3D* mtrx  );
  
  /** add child to daughter container 
   *  @param child    pointer to solid 
   *  @param position position 
   *  @param rotation rotation 
   */
  StatusCode addChild   
  ( ISolid*               child    , 
    const Gaudi::XYZPoint&     position , 
    const Gaudi::Rotation3D&    rotation );

  /** acess to iterator
   *  @return "begin" iterator
   */
  SolidChildrens::iterator 
  childBegin () { return m_sb_childrens.begin(); }; 
  
  /** acess to iterator
   *  @return "begin" iterator
   */
  SolidChildrens::iterator 
  childEnd   () { return m_sb_childrens.end  (); };

private:

  // default constructor is disabled 
  // SolidBoolean() ;
  // assignement operator is disabled 
  SolidBoolean& operator=(SolidBoolean & ) ;

  template<class aPoint, class aVector>
  unsigned int intersectionTicksImpl( const aPoint  & Point,
                                      const aVector & Vector,
                                      const ISolid::Tick& tickMin,
                                      const ISolid::Tick& tickMax,
                                      ISolid::Ticks&  ticks) const;

  template<class aPoint, class aVector>
  unsigned int intersectionTicksImpl( const aPoint  & Point,
                                      const aVector & Vector,
                                      ISolid::Ticks& ticks ) const;
  
protected:
  
  /** set bounding parameters 
   *  @return status code 
   */
  StatusCode setBP();

private:
  ///
  std::string                    m_sb_name     ;    
  mutable     ISolid*            m_sb_first    ;  
  mutable     SolidChildrens     m_sb_childrens; 
  //
};

/// ===========================================================================
#endif   ///< DETDESC_SOLIDBOOLEAN_H
/// ===========================================================================
