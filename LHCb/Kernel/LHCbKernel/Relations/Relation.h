// $Id: Relation.h,v 1.4 2002-07-25 15:32:13 ibelyaev Exp $
// =============================================================================
// CV Stag $Name: not supported by cvs2svn $
// =============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2002/06/18 16:46:40  ibelyaev
//  bug fix in Relations/Relation.h file
//
// Revision 1.2  2002/04/25 08:44:03  ibelyaev
//  bug fix for Win2K
//
// Revision 1.1  2002/04/03 15:35:17  ibelyaev
// essential update and redesing of all 'Relations' stuff
//
// =============================================================================
#ifndef RELATIONS_Relation_H
#define RELATIONS_Relation_H 1
// Include files
#include "Relations/PragmaWarnings.h"
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

namespace Relations
{
  
  /** @class Relation Relation.h Relations/Relation.h
   *
   *  @brief Implementation of ordinary unidirectional relations 
   * 
   *  Implementation of ordinary unidirectional relations 
   *  from objects of type "FROM" to objects of type "TO".
   *  
   *  Data types for "FROM" and "TO":
   *
   *   - either inherits from ContainedObject/DataObject 
   *   - or have the templated specializations through 
   *     ObjectTypeTraits structure with defined streamer operator 
   *     to/from StreamBuffer class 
   *  
   *  @see ObjectTypeTraits 
   *  @see StreamBuffer 
   *  @see IRelations
   *  @warning for the current implementation the actual type of 
   *           FROM should differ from the actual type of TO
   *  
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   25/01/2002
   */
  
  template<class FROM,class TO>
  class Relation :
    public virtual IRelation<FROM,TO>
  {  
  public:
  
    /// short cut for own     type
    typedef Relation<FROM,TO>                OwnType        ;
    /// short cut for inverse type
    typedef Relation<TO,FROM>                InvType        ;
    /// short cut for interface 
    typedef IRelation<FROM,TO>               IBase          ;
    /// short cut for the actual implementation type 
    typedef Relations::RelationBase<FROM,TO> Base           ;
    /// shortcut for inverse base
    typedef Relations::RelationBase<TO,FROM> InvBase        ;
    /// shortcut for direct base type 
    typedef Base                             Direct         ;
    /// shortcut for inverse base type 
    typedef InvBase                          Inverse        ;
    /// shortcut for direct  interface 
    typedef IBase                            IDirect        ;
    /// shortcut for inverse base type 
    typedef IDirect::InverseType             IInverse       ;
    
  public:
    
    /// the default constructor
    Relation ( const size_t reserve = 0 )
      : m_direct  ( reserve ) 
      , m_inverse ( 0 ) 
    {};
    
    /** constructor from "inverted object"
     *  @param copy object to be inverted
     *  @param int artificial argument to distinguish from copy constructor
     */
    Relation ( const InvType& inv   , int flag ) 
      : m_direct  ( inv.m_direct , flag ) 
      , m_inverse ( 0 )  
    {};

    /** constructor from "inverse interface"
     *  @param copy interafce to be inverted 
     *  @param int artificial argument to distinguish from copy constructor
     */
    Relation ( const IInverse & inv   , int /* flag */ ) 
      : m_direct  (   ) 
      , m_inverse ( 0 )  
    {
      // get all relations 
      IInverse::Range range = inv.relations() ;
      // invert all relations 
      for( IInverse::iterator entry = range.begin() ;
           range.end() != entry ; ++entry )
        { relate( entry->second , entry->first ); }
    };
    
    /// destructor (virtual)
    virtual ~Relation(){};
    
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
      Base::IP ip = m_direct.i_relations( object );
      return Range( ip.first , ip.second );
    };
    
    /** retrive all relations from ALL objects 
     *
     *  @see IRelation
     *  @see RelationBase
     *  @param object  smart reference to the object
     *  @return pair of iterators for output relations
     */
    virtual Range       relations () const
    {
      Base::IP ip = m_direct.i_relations();
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
    {
      StatusCode sc = m_direct.    i_relate( object1 , object2 ) ;
      if( sc.isFailure() || 0 == m_inverse ) { return sc ; }
      return          m_inverse -> i_relate( object2 , object1 ); 
    } ;
    
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
    { 
      StatusCode sc = m_direct.    i_remove( object1 , object2 ) ; 
      if( sc.isFailure() || 0 == m_inverse ) { return sc ; }
      return          m_inverse -> i_remove( object2 , object1 );
    };

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
    virtual  StatusCode removeFrom
    ( const  From&      object )
    { 
      StatusCode sc = m_direct.   i_removeFrom ( object ) ; 
      if( sc.isFailure() || 0 == m_inverse ) { return sc ; }
      return          m_inverse-> i_removeTo   ( object ) ; 
    };
    
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
    virtual  StatusCode removeTo
    ( const  To&        object )
    { 
      StatusCode sc = m_direct.    i_removeTo   ( object ) ; 
      if( sc.isFailure() || 0 == m_inverse ) { return sc ; }
      return          m_inverse -> i_removeFrom ( object ) ; 
    };
    
    /** remove ALL relations form ALL  object to ALL objects 
     *
     *  @see IRelation
     *  @see RelationBase
     *  @param object  smart reference to the object
     *  @return status code
     */
    virtual  StatusCode clear() 
    { 
      StatusCode sc = m_direct.    i_clear () ; 
      if( sc.isFailure() || 0 == m_inverse ) { return sc ; }
      return          m_inverse -> i_clear () ; 
    };

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
    virtual unsigned long addRef  () { return 1 ; }
    
    /** release the reference counter
     *  @see    IInterface
     *  @see    DataObject
     *  @return current number of references
     */
    virtual unsigned long release () { return 1 ; }
    
    /** get the pointer to direct table 
     *  @attention the method is not for public usage !!!
     *  @return pointer to direct base 
     */
    inline Direct*      directBase () { return &m_direct ; }
    
    /** set new inverse table 
     *  @attention the method is not for public usage !!!
     */
    inline void    setInverseBase( Inverse* inverse ) { m_inverse = inverse ; }
    
  private:
    
    /** copy constructor is private! 
     *  @param copy object to be copied 
     */
    Relation ( const OwnType& copy   ) 
      : m_direct  ( copy.m_direct )
      , m_inverse ( 0             ) 
    {}
    
    /** assignement operator is private!
     *  @param copy object to be copied 
     */
    Relation& operator= ( const OwnType& copy  );

  private:
    
    Direct   m_direct  ;  ///< the holder of all direct relations
    Inverse* m_inverse ;  ///< the pointer to inverse table
  };

}; // end of namespace Relations

// =============================================================================
// The End
// =============================================================================
#endif // RELATIONS_Relation_H
// =============================================================================
