// $Id: ISolid.h,v 1.6 2002-04-24 10:52:08 ibelyaev Exp $ 
// ===========================================================================
// CVS tag $Name: not supported by cvs2svn $
// ===========================================================================
// $Log: not supported by cvs2svn $
// Revision 1.5  2001/08/13 09:51:35  ibelyaev
// bug fix in 'reset' method
//
// Revision 1.4  2001/08/09 16:47:57  ibelyaev
// update in interfaces and redesign of solids
//
// ===========================================================================
#ifndef DETDESC_ISOLID_H
#define DETDESC_ISOLID_H 1
/// STD & STL
#include  <iostream>
#include  <string>
#include  <vector>
/// Gaudi Kernel
#include "GaudiKernel/ISerialize.h"
#include "GaudiKernel/IInspectable.h"
#include "GaudiKernel/IInspector.h"
#include "GaudiKernel/MsgStream.h"
/// forward declaration
class HepPoint3D;        ///< CLHEP
class HepVector3D;       ///< CLHEP
class StreamBuffer;      ///< GaudiKernel

/// Declaration of the interface ID ( interface id, major & minor versions)
static const InterfaceID IID_ISolid( "ISolid" , 4 , 0 );

/** @interface ISolid ISolid.h "DetDesc/ISolid.h"
 *
 *  An abstract interface to any Solid object
 *
 *  @version 3 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date   xx/xx/xxxx 
 */

class ISolid : public virtual IInterface   ,
               public virtual ISerialize   ,
               public virtual IInspectable
{
public:
  
  /** useful type definition for dealing 
   *  with intersections of the solid and the line 
   */
  typedef double             Tick  ;
  typedef std::vector<Tick>  Ticks ;
  
public:
  
  /** retrieve the uninque interface identifier 
   *  @see Interface::interfaceID()
   *  @return uniqie interface identifier 
   */
  static const InterfaceID& interfaceID() { return IID_ISolid; }
  
public:
  
  /** retrieve the specific name of the solid object  
   *  @return specific name of the solid
   */
  virtual const std::string& name     () const = 0;
  
  /** retrieve the specific type of the solid object  
   *  @return type of the solid
   */
  virtual       std::string  typeName () const = 0;
  
  /** check for the given 3D-point. 
   *  Point coordinated are in the local reference 
   *  frame of the solid.   
   *  @param point point (in local reference system of the solid)
   *  @return true if the point is inside the solid
   */
  virtual bool isInside ( const HepPoint3D   & point ) const = 0;
  
  /** retrieve the pointer to "simplified" solid.
   *  @see ISolid::coverTop()
   *  @return pointer to "simplified" solid - "cover"
   */
  virtual const ISolid* cover    () const = 0;
  
  /** retrieve the pointer to "the most simplified cover", 
   *  ideally to something like "the bounding box"
   *  @see ISolid::cover()
   *  @return pointer to the most simplified cover
   */
  virtual const ISolid* coverTop () const = 0;

  /** printout to STD/STL stream
   *  @param os STD/STL stream
   *  @return reference to the stream
   */
  virtual std::ostream& printOut ( std::ostream& os = std::cout ) const = 0;
  
  /** printout to Gaudi  stream
   *  @param os Gaudi stream
   *  @return reference to the stream
   */
  virtual MsgStream&    printOut ( MsgStream&    os ) const = 0;
  
  /** reset solid to its inititial state, remove 
   *  all auxillary data fields and pointers. 
   *  @see ISolid::~ISolid()
   *  @return self-reference 
   */
  virtual ISolid* reset()  = 0;
  
  /** calculate the intersection points("ticks") of the solid objects 
   *  with given line. 
   *  - Line is parametrized with parameter \a t :
   *     \f$ \vec{x}(t) = \vec{p} + t \times \vec{v} \f$ 
   *      - \f$ \vec{p} \f$ is a point on the line 
   *      - \f$ \vec{v} \f$ is a vector along the line  
   *  - \a tick is just a value of parameter \a t, at which the
   *    intersection of the solid and the line occurs
   *  - both  \a Point  (\f$\vec{p}\f$) and \a Vector  
   *    (\f$\vec{v}\f$) are defined in local reference system 
   *   of the solid 
   *  @see ISolid::intersectionTicks()
   *  @param Point initial point for the line
   *  @param Vector vector along the line
   *  @param ticks output container of "Ticks"
   *  @return the number of intersection points
   */
  virtual unsigned int
  intersectionTicks 
  ( const HepPoint3D & Point  ,
    const HepVector3D& Vector ,
    Ticks            & ticks  ) const = 0 ;
  
  /** calculate the intersection points("ticks") of the solid objects 
   *  with given line. 
   *  - Line is parametrized with parameter \a t : 
   *     \f$ \vec{x}(t) = \vec{p} + t \times \vec{v} \f$ 
   *      - \f$ \vec{p} \f$ is a point on the line 
   *      - \f$ \vec{v} \f$ is a vector along the line  
   *  - \a tick is just a value of parameter \a t, at which the
   *    intersection of the solid and the line occurs
   *  - both  \a Point  (\f$\vec{p}\f$) and \a Vector  
   *    (\f$\vec{v}\f$) are defined in local reference system 
   *   of the solid 
   *  Only intersection ticks within the range 
   *   \a tickMin and \a tickMax are taken into account.
   *  @see ISolid::intersectionTicks()
   *  @param Point initial point for the line
   *  @param Vector vector along the line
   *  @param tickMin minimum value of Tick 
   *  @param tickMax maximu value of Tick 
   *  @param ticks output container of "Ticks"
   *  @return the number of intersection points
   */
  virtual unsigned int
  intersectionTicks 
  ( const HepPoint3D & Point   ,
    const HepVector3D& Vector  ,
    const Tick       & tickMin ,
    const Tick       & tickMax ,
    Ticks            & ticks   ) const = 0 ;
  
  /** virtual destructor
   *  @see ISolid::reset()
   */
  virtual  ~ISolid(){};
  
};

// ============================================================================
/** output operator to STD/STL stream
 *  @param  os      reference to output stream
 *  @param  solid   reference to ISolid object
 *  @return reference to the stream
 */
// ============================================================================
inline std::ostream& operator<<( std::ostream&  os , const ISolid&  solid  )
{ return solid.printOut(os); }

// ============================================================================
/** output operator to STD/STL stream
 *  @param  os      reference to output stream
 *  @param  solid   pointer to ISolid object
 *  @return reference to the stream
 */
// ============================================================================
inline std::ostream& operator<<( std::ostream&  os , const ISolid*  solid  )
{ return ((0 == solid) ? (os<<"ISolid* points to NULL"):(os<<(*solid)));}

// ============================================================================
/** output operator to Gaudi stream
 *  @param  os      reference to output stream
 *  @param  solid   reference to ISolid object
 *  @return reference to the stream
 */
// ============================================================================
inline MsgStream&    operator<<( MsgStream&     os , const ISolid&  solid  )
{ return solid.printOut(os); }

// ============================================================================
/** output operator to Gaudi stream
 *  @param  os      reference to output stream
 *  @param  solid   pointer  to ISolid object
 *  @return reference to the stream
 */
// ============================================================================
inline MsgStream&    operator<<( MsgStream&     os , const ISolid*  solid  )
{ return ((0 == solid) ? (os<<"ISolid* points to NULL"):(os<<(*solid)));}

// ============================================================================
// The End 
// ============================================================================
#endif   ///< DETDESC_ISOLID_H
// ============================================================================





