// $Id: Relation1D.h,v 1.1 2002-03-18 19:32:17 ibelyaev Exp $
// =============================================================================
// CV Stag $Name: not supported by cvs2svn $
// =============================================================================
// $Log: not supported by cvs2svn $
// =============================================================================
#ifndef RELATIONS_RELATIONOBJECT_H
#define RELATIONS_RELATIONOBJECT_H 1
// Include files
// STD & STL
#include <algorithm>
// from Gaudi
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/MsgStream.h"
// From Relations
#include "Relations/RelationUtils.h"
#include "Relations/IRelation.h"
#include "Relations/RelationBase.h"
#include "Relations/Apply.h"

/** @class Relation1D Relation1D.h Relations/Relation1D.h
 *
 *  @brief Implementation of ordinary unidirectional relations 
 * 
 *  Implementation of ordinary unidirectional relations 
 *  from objects of type "FROM" to objects of type "TO".
 *  
 *  Data types for "FROM" and "TO":
 *
 *   - either inherits form ContainedObject/DataObject 
 *   - or have the templated specializations through 
 *     ObjectTypeTraits structure with defined streamer operator 
 *     to/from StreamBuffer class 
 *  
 *  @see DataObject 
 *  @see ContainedObject 
 *  @see ObjectTypeTraits 
 *  @see StreamBuffer 
 *  @warning for the current implementation the actual type of 
 *           FROM shoudl differ from the actual type of TO
 *  
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   25/01/2002
 */

template<class FROM,class TO>
class Relation1D :
  public virtual IRelation<FROM,TO> ,
  public         DataObject
{
  
public:
  
  /// short cut for own     type
  typedef Relation1D<FROM,TO>              OwnType        ;
  /// short cut for inverse type
  typedef Relation1D<TO,FROM>              InvType        ;
  /// short cut for interface 
  typedef IRelation<FROM,TO>               IBase          ;
  /// short cut for the actual implementation type 
  typedef Relations::RelationBase<FROM,TO> Base           ;
  
public:
  
  /// the default constructor
  Relation1D ( const Base::size_type reserve = 0 ) 
    : DataObject () , m_base ( reserve ) {};
  
  /** constructor from "inverted object"
   *  @param copy object to be inverted
   */
  Relation1D ( const InvType& inv ) 
    : DataObject () , m_base ( inv.m_base ) {};
  
  /// destructor (virtual)
  virtual ~Relation1D(){};
  
  /** object identification (static method)
   *  @see DataObject
   *  @see ObjectTypeTraits
   *  @see RelationWeightedTypeTraits
   *  @return the unique class identifier
   */
  static  const CLID& classID()
  {
    static const CLID s_clid =
      Relations::clid( "Relation1D"          ,
                       FromTypeTraits:: id() ,
                       ToTypeTraits::   id() );
    return s_clid ;
  };
  
  /** object identification (virtual method)
   *  @see DataObject
   *  @return the unique class identifier
   */
  virtual const CLID& clID()     const { return classID() ; }

  /** object serialization for writing
   *  @see DataObject
   *  @param  s reference to the output stream
   *  @return   reference to the output stream
   */
  virtual StreamBuffer& serialize ( StreamBuffer& s ) const
  {
    // serialize the base class
    DataObject::serialize( s );
    // serialize number of relations
    s << m_base.i_entries().size() ;
    // serialize all relations
    for( Base::CIT it = m_base.i_entries().begin() ; 
         m_base.i_entries().end() != it ; ++it )
      { 
        s << Relations::apply( (*it).first  , this ) 
          << Relations::apply( (*it).second , this ) ;
      }
    return s ;
  };
  
  /** object serialization for reading
   *  @see DataObject
   *  @param  s reference to the input  stream
   *  @return   reference to the input  stream
   */
  virtual StreamBuffer& serialize ( StreamBuffer& s )
  {
    // reset existing relations
    m_base.i_entries().clear();
    // serialize the base class
    DataObject::serialize( s );
    // get the total number of relations
    typename Base::Entries::size_type num ;
    s >> num ;
    while( num-- > 0 )
      {
        // serialize all relations
        From from ; To to ; 
        s >> Relations::apply( from , this )  
          >> Relations::apply( to   , this )  ;
        m_base.i_relate( from , to );
      }
    ///
    return s ;
  };
  
  /** retrive all relations from the object
   *
   *   - the CPU performance is proportional to log(N), 
   *     where N is the total number of relations
   *
   *  @see IRelation
   *  @see RelationBase
   *  @param object  smart reference to the object
   *  @return pair of iterators for output relations
   */
  virtual Range       relations
  ( const From&       object    ) const
  {
    Base::IP ip = m_base.i_relations( object );
    return Range( ip.first , ip.second );
  };

  /** make the relation between 2 objects
   *
   *   - StatusCode::FAILURE is returned if the relation
   *     between the given objects  is already set
   *
   *   - the CPU performance is proportional to log(N)
   *     for location of the object plus some overhead for 
   *     list operations, which is more or less constant for 
   *     std::deque implementation of the underlying relation 
   *     store and proportional to N for std::vector implementation, 
   *     where N is the total number of relations 
   *
   *  @see IRelation
   *  @see RelationTypeTraits
   *  @see RelationBase
   *  @param object1  the first object
   *  @param object2  the second object
   *  @return status code
   */
  virtual  StatusCode relate
  ( const  From&      object1 ,
    const  To&        object2 )
  { return m_base.i_relate( object1 , object2 ) ; }

  /** remove the concrete relation between objects
   *
   *   - StatusCode::FAILURE is returned if the relation
   *     between the given objects  is already set
   *
   *   - the CPU performance is proportional to log(N)
   *     for location of the object plus some overhead for 
   *     list operations, which is more or less constant for 
   *     std::deque implementation of the underlying relation 
   *     store and proportional to N for std::vector implementation, 
   *     where N is the total number of relations 
   *
   *  @see IRelation
   *  @see RelationTypeTraits
   *  @see RelationBase
   *  @param object1  smart reference to the first object
   *  @param object2  smart reference to the second object
   *  @return status code
   */
  virtual  StatusCode remove
  ( const  From&      object1 ,
    const  To&        object2 )
  { return m_base.i_remove( object1 , object2 ) ; }

  /** remove all relations FROM the defined object
   *
   *   - StatusCode::FAILURE is returned if there are no relations
   *     from the given object
   *
   *   - the CPU performance is proportional to log(N)
   *     for location of the object plus some overhead for 
   *     list operations, which is more or less constant for 
   *     std::deque (or std::list) implementation of the 
   *     underlying relation store and proportional to N
   *     for std::vector implementation, where N is the 
   *     total number of relations 
   *
   *  @see IRelation
   *  @see RelationBase
   *  @param object  smart reference to the object
   *  @return status code
   */
  virtual  StatusCode remove
  ( const  From&      object )
  { return m_base.i_remove( object ) ; }

  /** remove all relations TO the defined object
   *
   *   - StatusCode::FAILURE is returned if there are no relations
   *     from the given object
   *
   *   - the CPU performance is proportional to 
   *     the total number of relations 
   *   
   *  @see IRelation
   *  @see RelationBase
   *  @param object  smart reference to the object
   *  @return status code
   */
  virtual  StatusCode remove
  ( const  To&        object )
  { return m_base.i_remove( object ) ; }

  /** query the interface
   *  @see    IRelation
   *  @see    IInterface
   *  @param  id  interface identifier
   *  @param  ret placeholder for returned interface 
   *  @return status code
   */
  virtual StatusCode queryInterface
  ( const InterfaceID& id , void** ret )
  {
    if( 0 == ret  )          { return StatusCode::FAILURE ; } // RETURN !!!
    if( IInterface::interfaceID() == id )
      { *ret = static_cast<IInterface*> ( this ); }
    else if( IBase::interfaceID() == id )
      { *ret = static_cast<IBase*>      ( this ); }
    else                     { return StatusCode::FAILURE ; } //  RETURN !!!
    ///
    addRef() ;
    return StatusCode::SUCCESS ;
  };

  /** increase the reference counter
   *  @see    IInterface
   *  @see    DataObject
   *  @return current number of references
   */
  virtual unsigned long addRef  ()
  { return  DataObject::addRef  () ; }

  /** release the reference counter
   *  @see    IInterface
   *  @see    DataObject
   *  @return current number of references
   */
  virtual unsigned long release ()
  { return  DataObject::release () ; }

private:
  
  Base m_base ;  ///< the holder of all relations 
  
};

// =============================================================================
// The End
// =============================================================================
#endif // RELATIONS_Relation1D_H
// =============================================================================
