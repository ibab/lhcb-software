// $Id: Relation2.h,v 1.7 2004-01-14 16:30:25 ibelyaev Exp $
// =============================================================================
// CV Stag $Name: not supported by cvs2svn $
// =============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.6  2004/01/14 15:13:03  ibelyaev
//  few tricks to make POOL/ROOT happy
//
// Revision 1.5  2003/11/23 12:42:59  ibelyaev
//  update to remove multiple and virtual inheritance
//
// =============================================================================
#ifndef RELATIONS_Relation2_H 
#define RELATIONS_Relation2_H 1
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
#include "Relations/IRelation2D.h"
#include "Relations/Relation.h"

namespace Relations
{
  /** @class Relation2 Relation2.h Relations/Relation2.h
   *  
   *  @brief Implementation of ordinary bidirectional relations 
   * 
   *  Implementation of ordinary bidirectional relations 
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
   *  @see IRelation
   *  @warning for the current implementation the actual type of 
   *           FROM should differ from the actual type of TO
   *  
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
   *  @date   23/03/2002
   */
  
  template<class FROM,class TO>
  class Relation2 : public  IRelation2D<FROM,TO> 
  {  
  public:
    /// short cut for own     type
    typedef Relation2<FROM,TO>               OwnType        ;
    /// short cut for inverse type
    typedef Relation2<TO,FROM>               InvType        ;
    /// short cut for interface 
    typedef IRelation2D<FROM,TO>             IBase          ;
    /// actual implementations of direct  type 
    typedef Relation<FROM,TO>                Base           ;
    /// actual implementations of inverse type 
    typedef Relation<TO,FROM>                InvBase        ;
    /// shortcut for direct base type 
    typedef Base                             Direct         ;
    /// shortcut for inverse base type 
    typedef InvBase                          Inverse        ;
    /// shortcut for direct subinterface 
    typedef typename IBase::DirectType       DirectType     ;
    /// shortcut for inverse subinterface 
    typedef typename IBase::InverseType      InverseType    ;
    
    /// import "Range" type from interface 
    typedef typename IBase::Range            Range          ;
    /// import "From"  type from interface 
    typedef typename IBase::From             From           ;
    /// import "To"    type from interface 
    typedef typename IBase::To               To             ;
    
  public:
    

    /// the default constructor
    Relation2( const size_t reserve = 0  )
      :  m_direct ( reserve ) , m_inverse( reserve ) 
    {
      /// set cross-links 
      m_direct  .setInverseBase ( m_inverse .directBase () ) ;
      m_inverse .setInverseBase ( m_direct  .directBase () ) ;
    };
    
    /** constructor from "inverted object"
     *  @param copy object to be inverted
     *  @param int artificial argument to distinguish from copy constructor
     */
    Relation2 ( const InvType& inv   , int flag ) 
      : m_direct  ( inv.m_inverse )
      , m_inverse ( inv.m_direct  ) 
    {
      m_direct  .setInverseBase( m_inverse .directBase () ) ;
      m_inverse .setInverseBase( m_direct  .directBase () );
    };
    
    /// destructor (virtual)
    virtual ~Relation2() {} ;
    
  public:  // major functional methods (fast, 100% inline)
    
    /// retrive all relations from the object (fast,100% inline)
    inline   Range       i_relations
    ( const  From&       object    ) const
    { return m_direct.i_relations ( object ) ; }
    
    /// retrive all relations from ALL objects (fast,100% inline)
    inline   Range        i_relations () const
    { return m_direct.i_relations () ; }
    
    /// make the relation between 2 objects (fast,100% inline method) 
    inline   StatusCode i_relate
    ( const  From&      object1 ,
      const  To&        object2 )
    { return m_direct.i_relate   ( object1 , object2 ) ; }
    
    /// remove the concrete relation between objects (fast,100% inline method)
    inline   StatusCode i_remove 
    ( const  From&      object1 ,
      const  To&        object2 )
    { return m_direct.i_remove ( object1 , object2 ) ; }
    
    /// remove all relations FROM the defined object (fast,100% inline method)
    inline   StatusCode i_removeFrom 
    ( const  From&      object )
    { return m_direct.i_removeFrom ( object ) ; }    
    
    /// remove all relations TO the defined object (fast,100% inline method)
    inline   StatusCode i_removeTo
    ( const  To&        object )
    { return m_direct.i_removeTo( object ) ; }
    
    /// remove ALL relations form ALL  object to ALL objects  (fast,100% inline)
    inline  StatusCode i_clear() 
    { return m_direct.i_clear() ; };

    /// rebuild ALL relations form ALL  object to ALL objects(fast,100% inline)
    inline  StatusCode i_rebuild() 
    { return m_direct.i_rebuild() ; };
    
  public:  // abstract methods from interface
    
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
    virtual  Range       relations
    ( const  From&       object    ) const 
    { return i_relations( object ) ; }
    
    /** retrive all relations from ALL objects 
     *
     *  @see IRelation
     *  @see RelationBase
     *  @param object  smart reference to the object
     *  @return pair of iterators for output relations
     */
    virtual  Range        relations () const
    { return i_relations () ; }
    
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
    { return i_relate ( object1 , object2 ) ; }
    
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
    { return i_remove ( object1 , object2 ) ; }
    
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
    { return i_removeFrom ( object ) ; }
    
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
    ( const  To&        object ) { return i_removeTo( object ) ; }
    
    /** remove ALL relations from ALL  object to ALL objects 
     *
     *  @see IRelationBase 
     *  @return status code
     */
    virtual  StatusCode clear() { return i_clear () ; };

    /** rebuild ALL relations from ALL  object to ALL objects 
     *
     *  @see IRelationBase 
     *  @return status code
     */
    virtual  StatusCode rebuild() { return i_rebuild () ; };
    
    /** update the object after POOL/ROOT reading 
     *  @see IUpdateable 
     *  @param flag    0 - update after read, 1 - update before write 
     *  @return status code
     */
    virtual StatusCode update( int flag ) 
    {
      if( 0 == flag ) { return i_rebuild() ; }
      return StatusCode::SUCCESS ;
    };
 
  public:  // abstract methods from interface
    
    /** get the "direct" interface 
     *  @see IRelation2D
     *  @return pointer to the 'direct' interface 
     */
    virtual       DirectType*  direct ()        { return &m_direct ; }
    
    /** get the "direct" interface  (const-version)
     *  @see IRelation2D
     *  @return pointer to the 'direct' interface 
     */
    virtual const DirectType*  direct () const  { return &m_direct ; }
    
    /** get the "inverse" interface 
     *  @see IRelation2D
     *  @return pointer to the 'inverse' interface 
     */
    virtual       InverseType* inverse ()       { return &m_inverse ; }
    
    /** get the "inverse" interface  (const version)
     *  @see IRelation2D
     *  @return pointer to the 'inverse' interface 
     */
    virtual const InverseType* inverse () const { return &m_inverse ; }
    
  public:
    
    /** query the interface
     *  @see    IRelation
     *  @see    IRelation2D
     *  @see    IInterface
     *  @param  id  interface identifier
     *  @param  ret placeholder for returned interface 
     *  @return status code
     */
    virtual StatusCode queryInterface
    ( const InterfaceID& id , void** ret )
    {
      if( 0 == ret  )          { return StatusCode::FAILURE ; } // RETURN !!!
      if( IInterface::interfaceID()             == id )
        { *ret = static_cast<IInterface*>        ( this ); }
      else if( IBase::interfaceID()             == id )
        { *ret = static_cast<IBase*>             ( this ); }
      else if( IBase::DirectType::interfaceID() == id )
        { *ret = static_cast<typename IBase::DirectType*> ( this ); }
      else                     { return StatusCode::FAILURE ; } //  RETURN !!!
      ///
      addRef() ;
      return StatusCode::SUCCESS ;
    };
    
    /** increase the reference counter (artificial)
     *  @see    IInterface
     *  @see    DataObject
     *  @return current number of references
     */
    virtual unsigned long addRef  () { return 1 ; }
    
    /** release the reference counter (artificial)
     *  @see    IInterface
     *  @see    DataObject
     *  @return current number of references
     */
    virtual unsigned long release () { return 1 ; }
    
  public: // other methods 
    
    /** reserve the relations (for efficiency reasons)
     *  @param num number of relations to be reserved
     *  @return status code
     */
    inline StatusCode reserve ( const size_t num ) 
    { return m_direct.reserve( num ) ; };
    
  public:
    
    /** copy constructor is publc, 
     *  @attention it is not recommended for normal usage 
     *  @param copy object to be copied 
     */
    Relation2 ( const OwnType& copy ) 
      : m_direct  ( copy.m_direct  )
      , m_inverse ( copy.m_inverse ) 
    {
      /// set cross-links 
      m_direct  .setInverseBase ( m_inverse  .directBase () ) ;
      m_inverse .setInverseBase ( m_direct   .directBase () );
    };
    
  private:
    
    /** assignement operator is private!
     *  @param copy object to be copied 
     */
    Relation2& operator=( const OwnType& copy );

  private:
    
    Direct    m_direct  ;  ///< direct table 
    Inverse   m_inverse ;  ///< the "inverse table"
    
  };
  
  
}; // end of namespace Relations

// ============================================================================
// The End 
// ============================================================================
#endif // RELATIONS_Relation2_H
// ============================================================================
