// $Id: SolidBase.h,v 1.3 2002-04-24 10:52:20 ibelyaev Exp $
// ===========================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ===========================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2001/08/13 09:51:36  ibelyaev
// bug fix in 'reset' method
//
// Revision 1.1  2001/08/09 16:47:58  ibelyaev
// update in interfaces and redesign of solids
// 
// ===========================================================================
#ifndef DETDESC_SOLIDBASE_H 
#define DETDESC_SOLIDBASE_H 1
/// DetDesc 
#include "DetDesc/ISolid.h"

/** @class SolidBase SolidBase.h DetDesc/SolidBase.h
 *  
 *  Useful intermediate class for implementation of all solids 
 *
 *  @author Vanya Belyaev
 *  @date   02/08/2001
 */

class SolidBase: public virtual ISolid
{
  
public: 
  
  /** - retrieve the name of the concrete Solid object 
   *  - implementation of ISolid abstract interface 
   *  @see ISolid
   *  @return name of the solid object 
   */
  virtual const std::string& name() const { return m_name ; }
  
  /** - reset solid to its inititial state, remove 
   *    all auxillary data fields and pointers. 
   *  - implementation of ISolid abstract interface  
   *  @see ISolid 
   *  @see SolidBase::~SolidBase()
   *  @return  self-reference 
   */
  virtual ISolid* reset ();

  /** - calculate the intersection points("ticks") of the solid objects 
   *    with given line. 
   *  -# Line is parametrized with parameter \a t : 
   *     \f$ \vec{x}(t) = \vec{p} + t \times \vec{v} \f$ 
   *      - \f$ \vec{p} \f$ is a point on the line 
   *      - \f$ \vec{v} \f$ is a vector along the line  
   *  -# \a tick is just a value of parameter \a t, at which the
   *    intersection of the solid and the line occurs
   *  -# both  \a Point  (\f$\vec{p}\f$) and \a Vector  
   *    (\f$\vec{v}\f$) are defined in local reference system 
   *   of the solid 
   *  - implementation of ISolid abstract interface  
   *  @see ISolid 
   *  @param Point initial point for the line
   *  @param Vector vector along the line
   *  @param tickMin minimum value of Tick 
   *  @param tickMax maximu value of Tick 
   *  @param ticks output container of "Ticks"
   *  @return the number of intersection points
   */
  virtual unsigned int 
  intersectionTicks 
  ( const HepPoint3D  & Point   ,         
    const HepVector3D & Vector  ,         
    ISolid::Ticks     & ticks   ) const ; 
  
  /** - calculate the intersection points("ticks") of the solid objects 
   *    with given line. 
   *  -# Line is parametrized with parameter \a t : 
   *     \f$ \vec{x}(t) = \vec{p} + t \times \vec{v} \f$ 
   *      - \f$ \vec{p} \f$ is a point on the line 
   *      - \f$ \vec{v} \f$ is a vector along the line  
   *  -# \a tick is just a value of parameter \a t, at which the
   *    intersection of the solid and the line occurs
   *  -# both  \a Point  (\f$\vec{p}\f$) and \a Vector  
   *    (\f$\vec{v}\f$) are defined in local reference system 
   *   of the solid 
   *  -# Only intersection ticks within the range 
   *     \a tickMin and \a tickMax are taken into account. 
   *  - implementation of ISolid abstract interface  
   *  @see ISolid 
   *  @param Point initial point for the line
   *  @param Vector vector along the line
   *  @param tickMin minimum value of Tick 
   *  @param tickMax maximu value of Tick 
   *  @param ticks output container of "Ticks"
   *  @return the number of intersection points
   */
  virtual unsigned int 
  intersectionTicks 
  ( const HepPoint3D  & Point   ,         
    const HepVector3D & Vector  ,         
    const ISolid::Tick& tickMin ,         
    const ISolid::Tick& tickMax ,         
    ISolid::Ticks     & ticks   ) const ; 
  
  /** - query the interface 
   *  - implementation of IInterface abstract interface 
   *  @see IInterface 
   *  @param ID unique interafce identifier 
   *  @param ppI placeholder for returned interface
   *  @return status code 
   */
  virtual StatusCode queryInterface
  ( const InterfaceID& ID , void** ppI );
  
  /** - increment the reference count of Interface instance
   *  - implementation of IInterface abstract interface 
   *  @see IInterface 
   *  @return reference count 
   */
  virtual unsigned long addRef  () ;
  
  /** - release interface instance
   *  - implementation of IInterface abstract interface 
   *  @see IInterface 
   *  @return reference count 
   */
  virtual unsigned long release () ;
  
  /** - retrieve the pointer to "the most simplified cover", 
   *    ideally to something like "the bounding box"
   *  - implementation of ISolid interface
   *  @see ISolid 
   *  @see ISolid::cover()
   *  @return pointer to the most simplified cover
   */
  virtual const ISolid* coverTop () const ;
  
  /** - printout to STD/STL stream
   *  - implementation of ISolid interface
   *  @see ISolid 
   *  @param st STD/STL stream
   *  @return reference to the stream
   */
  virtual std::ostream& printOut ( std::ostream& st ) const ;
  
  /** - printout to Gaudi  stream
   *  - implementation of ISolid interface
   *  @see ISolid 
   *  @param st Gaudi stream
   *  @return reference to the stream
   */
  virtual MsgStream&    printOut ( MsgStream&    st ) const ;
 
  /** - serialization for reading
   *  - implementation of ISerialize abstract interface 
   *  @see ISerialize 
   *  @see ISolid  
   *  @param sb reference to stream buffer
   *  @return reference to stream buffer
   */
  virtual StreamBuffer& serialize( StreamBuffer& sb )       ;
  
  /** - serialization for writing
   *  - implementation of ISerialize abstract interface 
   *  @see ISerialize 
   *  @see ISolid  
   *  @param sb reference to stream buffer
   *  @return reference to stream buffer
   */
  virtual StreamBuffer& serialize( StreamBuffer& sb ) const ;
  
  /** - accept the inspector 
   *  - implementation of IInspectable abstract interface   
   *  @see IInspectable
   *  @see IInspector 
   *  @see ISolid
   *  @param pI pointer to Inspector 
   *  @return FALSE if teh object doesn't accept the Inspector.
   */
  virtual bool acceptInspector( IInspector* pI )       ; 
  
  /** - accept the inspector (const version)
   *  - implementation of IInspectable abstract interface   
   *  @see IInspectable
   *  @see IInspector 
   *  @see ISolid
   *  @param pI pointer to Inspector 
   *  @return FALSE if teh object doesn't accept the Inspector.
   */
  virtual bool acceptInspector( IInspector* pI ) const ;
  
protected:
  
  /** standard constructor 
   *  @param Name name of the solid 
   */
  SolidBase( const std::string& Name = "Anonymous Solid" );
  
  /// virtual destructor 
  virtual ~SolidBase();
  
protected:
  
  std::string     m_name  ; ///< name of the solid
  unsigned long   m_count ; ///< reference counter 
  mutable ISolid* m_cover ; ///< pointer to cover 

};

/// ===========================================================================
/// The End 
/// ===========================================================================
#endif ///< DETDESC_SOLIDBASE_H
/// ===========================================================================
