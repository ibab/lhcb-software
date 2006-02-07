// $Id: Relation.h,v 1.8 2006-02-07 13:42:53 ibelyaev Exp $
// =============================================================================
// CV Stag $Name: not supported by cvs2svn $ ; version $Revision: 1.8 $ 
// =============================================================================
// $Log: not supported by cvs2svn $
// =============================================================================
#ifndef RELATIONS_Relation_H
#define RELATIONS_Relation_H 1
// =============================================================================
// Include files
// =============================================================================
#include "Relations/PragmaWarnings.h"
// =============================================================================
// STD & STL
// =============================================================================
#include <algorithm>
// =============================================================================
// from Gaudi
// =============================================================================
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/MsgStream.h"
// =============================================================================
// From Relations
// =============================================================================
#include "Relations/RelationUtils.h"
#include "Relations/IRelation.h"
#include "Relations/RelationBase.h"
// =============================================================================

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
  class Relation : public BaseTable , public IRelation<FROM,TO>
  {  
  public:
  
    /// short cut for own     type
    typedef          Relation<FROM,TO>                OwnType        ;
    /// short cut for inverse type
    typedef          Relation<TO,FROM>                InvType        ;
    /// short cut for interface 
    typedef          IRelation<FROM,TO>               IBase          ;
    /// short cut for the actual implementation type 
    typedef typename Relations::RelationBase<FROM,TO> Base           ;
    /// shortcut for inverse base
    typedef typename Relations::RelationBase<TO,FROM> InvBase        ;
    /// shortcut for direct base type 
    typedef          Base                             Direct         ;
    /// shortcut for inverse base type 
    typedef          InvBase                          Inverse        ;
    /// shortcut for direct  interface 
    typedef          IBase                            IDirect        ;
    /// shortcut for inverse base type 
    typedef typename IDirect::InverseType             IInverse       ;
    
    /// basic types from Interface 
    typedef typename IBase::Range Range ;
    /// basic types from Interface 
    typedef typename IBase::From  From  ;
    /// basic types from Interface 
    typedef typename IBase::To    To    ;

  public:
    
    /// the default constructor
    Relation 
    ( const size_t reserve = 0 )
      : BaseTable     (   ) 
      , IBase         (   ) 
      , m_direct      ( reserve ) 
      , m_inverse_aux ( 0 ) 
    {};
    
    /** constructor from arbitrary "direct" interface 
     *  @param copy object to be copied
     */
    Relation 
    ( const IDirect& copy ) 
      : BaseTable     (      ) 
      , IBase         (      ) 
      , m_direct      ( copy ) 
      , m_inverse_aux ( 0    ) 
    {}
    
    /** constructor from "inverse interface"
     *  It is an efficient way to "invert" relation 
     *  @param inv interface to be inverted 
     *  @param flag artificial argument, to distinguish from copy constructor
     */
    Relation 
    ( const IInverse&    inv     , 
      const int          flag  ) 
      : BaseTable     (   ) 
      , IBase         (   ) 
      , m_direct      ( inv , flag ) 
      , m_inverse_aux ( 0 )  
    {}
    
    /** copy constructor is publc, 
     *  but it is not recommended for direct usage 
     *  @param copy object to be copied 
     */
    Relation 
    ( const OwnType& copy   ) 
      : BaseTable     ( copy          ) 
      , IBase         ( copy          ) 
      , m_direct      ( copy.m_direct )
      , m_inverse_aux ( 0             ) 
    {}
    
    /// destructor (virtual)
    virtual ~Relation(){};
    
  public:  // major functional methods (fast, 100% inline)
    
    /// retrive all relations from the object (fast,100% inline)
    inline   Range       i_relations
    ( const  From&       object    ) const
    {
      typename Base::IP ip = m_direct.i_relations( object );
      return Range( ip.first , ip.second );
    };
    
    /// retrive all relations from ALL objects (fast,100% inline)
    inline   Range        i_relations () const
    {
      typename Base::IP ip = m_direct.i_relations();
      return Range( ip.first , ip.second );
    };
    
    /// make the relation between 2 objects (fast,100% inline method) 
    inline   StatusCode i_relate
    ( const  From&      object1 ,
      const  To&        object2 )
    {
      StatusCode sc = m_direct.    i_relate( object1 , object2 ) ;
      if ( sc.isFailure() || 0 == m_inverse_aux ) { return sc ; }
      return m_inverse_aux  -> i_relate( object2 , object1 ); 
    } ;
    
    /// remove the concrete relation between objects (fast,100% inline method)
    inline   StatusCode i_remove 
    ( const  From&      object1 ,
      const  To&        object2 )
    { 
      StatusCode sc = m_direct.    i_remove( object1 , object2 ) ; 
      if ( sc.isFailure() || 0 == m_inverse_aux ) { return sc ; }
      return m_inverse_aux  -> i_remove( object2 , object1 );
    };
    
    /// remove all relations FROM the defined object (fast,100% inline method)
    inline   StatusCode i_removeFrom 
    ( const  From&      object )
    { 
      StatusCode sc = m_direct.   i_removeFrom ( object ) ; 
      if ( sc.isFailure() || 0 == m_inverse_aux ) { return sc ; }
      return m_inverse_aux -> i_removeTo   ( object ) ; 
    };
    
    /// remove all relations TO the defined object (fast,100% inline method) 
    inline   StatusCode i_removeTo
    ( const  To&        object )
    { 
      StatusCode sc = m_direct.    i_removeTo   ( object ) ; 
      if ( sc.isFailure() || 0 == m_inverse_aux ) { return sc ; }
      return m_inverse_aux -> i_removeFrom ( object ) ; 
    };
    
    /// remove ALL relations form ALL  object to ALL objects (fast,100% inline)
    inline  StatusCode i_clear() 
    { 
      StatusCode sc = m_direct.    i_clear () ; 
      if ( sc.isFailure() || 0 == m_inverse_aux ) { return sc ; }
      return m_inverse_aux -> i_clear () ; 
    };
    
    /// rebuild ALL relations form ALL  object to ALL objects (fast,100% inline)
    inline  StatusCode i_rebuild ()
    { 
      typedef typename IBase::TypeTraits::Entry _Entry   ;
      typedef typename std::vector<_Entry>      _Entries ;
      // 1) get all relations
      Range r = i_relations();
      // 2) copy them into temporary storage 
      _Entries _e ( r.begin() , r.end() );
      // 3) clear all existing rleations 
      StatusCode sc =  i_clear()  ; if ( sc.isFailure() ) { return sc ; }
      // 4) reserve space for new relations 
      sc = reserve( _e.size() )   ; if ( sc.isFailure() ) { return sc ; }
      // 5) build new relations 
      for ( typename _Entries::const_iterator entry = _e.begin() ; _e.end() != entry ; ++entry ) 
      { i_push ( entry->from() , entry->to() )  ; }
      // (re)sort
      i_sort() ;
      //
      return StatusCode::SUCCESS  ;
    };
    
    /** make the relation between 2 objects (fast,100% inline method) 
     *  - Call for i_sort() is mandatory! 
     */
    inline   void       i_push   
    ( const  From&      object1 ,
      const  To&        object2 )
    {
      m_direct.i_push( object1 , object2 ) ;
      if ( 0 != m_inverse_aux ) 
      { m_inverse_aux -> i_push ( object2 , object1 ) ; }
    } ;

    /** (re)sort of the table 
     *   mandatory to use after i_push 
     */
    inline void i_sort() 
    { 
      m_direct.i_sort() ; 
      if ( 0 != m_inverse_aux ) { m_inverse_aux -> i_sort() ; } 
    }
    
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
    virtual Range       relations
    ( const From&       object    ) const { return i_relations( object ) ; }
    
    /** retrive all relations from ALL objects 
     *
     *  @see IRelation
     *  @see RelationBase
     *  @return pair of iterators for output relations
     */
    virtual Range        relations () const { return i_relations () ; }
    
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
      const  To&        object2 ) { return i_relate( object1 , object2 ) ; }
    
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
      const  To&        object2 ) { return i_remove ( object1 , object2 ) ; }
    
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
    ( const  From&      object     ) { return i_removeFrom ( object ) ; }
    
    /** remove all relations TO the defined object
     *  ("fast", 100% inline method) 
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
    
    /** remove ALL relations form ALL  object to ALL objects 
     *
     *  @see IRelationBase 
     *  @return status code
     */
    virtual  StatusCode clear()  { return i_clear() ; }
    
    /** rebuild ALL relations form ALL  object to ALL objects 
     *
     *  @see IRelationBase
     *  @return status code
     */
    virtual  StatusCode rebuild() { return i_rebuild() ;}
    
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

  public:
    
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
    
    /** get the pointer to direct table 
     *  @attention the method is not for public usage !!!
     *  @return pointer to direct base 
     */
    inline Direct*      directBase ()       { return &m_direct ; }
    
    /** get the reference to direct table 
     *  @attention the method is not for public usage !!!
     *  @return direct base 
     */
    const  Direct&     _direct     () const { return m_direct ; }
    
    /** set new inverse table 
     *  @attention the method is not for public usage !!!
     */
    inline void    setInverseBase( Inverse* inverse ) 
    { m_inverse_aux = inverse ; }
    
    /** reserve the relations (for efficiency reasons)
     *  @param num number of relations to be reserved
     *  @return status code
     */
    inline StatusCode reserve ( const size_t num ) 
    {
      if ( 0 != m_inverse_aux ) { m_inverse_aux->i_reserve( num ) ; }
      return m_direct.i_reserve( num ) ;
    };
    
  private:
    
    /** assignement operator is private!
     *  @param copy object to be copied 
     */
    Relation& operator= ( const OwnType& copy  );

  private:
    
    Direct   m_direct  ;  ///< the holder of all direct relations
    Inverse* m_inverse_aux ;  ///< the pointer to inverse table
  };

}; // end of namespace Relations

// =============================================================================
// The End
// =============================================================================
#endif // RELATIONS_Relation_H
// =============================================================================
