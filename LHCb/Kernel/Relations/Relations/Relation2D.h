// $Id: Relation2D.h,v 1.7 2006-06-02 16:18:38 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ ; version $Revision: 1.7 $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.6  2006/02/07 09:22:24  ibelyaev
//  update for Win32
//
// ============================================================================
#ifndef RELATIONS_Relation2D_H 
#define RELATIONS_Relation2D_H 1
// ============================================================================
// Include files
// ============================================================================
#include "Relations/PragmaWarnings.h"
// ============================================================================
// STD & STL 
// ============================================================================
#include <algorithm>
// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartRef.h"
// ============================================================================
// From Relations
// ============================================================================
#include "Relations/RelationUtils.h"
#include "Relations/IRelation.h"
#include "Relations/IRelation2D.h"
#include "Relations/Relation2.h"
// ============================================================================

namespace LHCb
{
  
  /** @class Relation2D Relation2D.h Relations/Relation2D.h
   *  
   ** Templated object which implements bidirectional relations 
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   28/01/2002
   */
  
  template< class FROM, class TO>
  class Relation2D 
   : public DataObject
   , public Relations::BaseTable             
   , public IRelation2D<FROM,TO>
  {
  public:
    
    /// shortcut for own type 
    typedef Relation2D<FROM,TO>              OwnType    ;
    /// shortcut for inverse type 
    typedef Relation2D<TO,FROM>              InvType    ;
    /// shortcut for "direct"(="main") interface
    typedef IRelation2D<FROM,TO>             IBase      ;
    /// shortcut for "direct"(="main") interface
    typedef IRelation<FROM,TO>               IBase1     ;
    /// shortcut for "inverse interface  interface
    typedef IRelation<TO,FROM>               IBase2     ;
    /// shortcut for direct subinterface 
    typedef typename IBase::DirectType       DirectType     ;
    /// shortcut for inverse subinterface 
    typedef typename IBase::InverseType      InverseType    ;
    /// import "Range" type from the base 
    typedef typename IBase::Range            Range      ;
    /// import "From"  type from the base 
    typedef typename IBase::From             From       ;
    /// import "To"    type from the base 
    typedef typename IBase::To               To         ;
    /// shortcut for actual implementation  
    typedef Relations::Relation2<FROM,TO>    Base       ;  
    // shortcut for "direct" interface 
    typedef typename IBase::DirectType       IDirect        ;
    // shortcut for "inverse" interface 
    typedef typename IBase::InverseType      IInverse       ;
    
  public:
    
    /** Standard/default  constructor
     *  @param reserve the map-size to be preallocated
     */
    Relation2D 
    ( const size_t reserve  = 0 ) 
      : DataObject () 
      , Relations::BaseTable  () 
      , IBase      () 
      , m_base     ( reserve ) 
    {
#ifdef COUNT_INSTANCES 
      Relations::InstanceCounter::instance().increment( type() ) ;
#endif // COUNT_INSTANCES
    };
    
    /** constructor from any direct interface
     *  @param copy object to be copied 
     */
    Relation2D 
    ( const IDirect& copy  )
      : DataObject () 
      , Relations::BaseTable  () 
      , IBase      () 
      , m_base     ( copy )
    {
#ifdef COUNT_INSTANCES 
      Relations::InstanceCounter::instance().increment( type() ) ;
#endif // COUNT_INSTANCES
    };
    
    /** constructor from "inverted interface"
     *  @param inv object to be inverted
     *  @param flag artificial argument to distinguisch from 
     *  copy constructor
     */
    Relation2D 
    ( const IInverse& inv  , 
      const int       flag ) 
      : DataObject () 
      , Relations::BaseTable  () 
      , IBase      () 
      , m_base     ( inv  , flag )
    {
#ifdef COUNT_INSTANCES 
      Relations::InstanceCounter::instance().increment( type() ) ;
#endif // COUNT_INSTANCES
    };
    
    /** copy constructor 
     *  @param copy object to be copied 
     */
    Relation2D 
    ( const OwnType& copy  )
      : IInterface    ( copy ) 
      , IUpdateable   ( copy ) 
      , IRelationBase ( copy )
      , DirectType    ( copy ) 
      , DataObject    ( copy          ) 
      , Relations::BaseTable  ( copy  ) 
      , IBase         ( copy          ) 
      , m_base        (  copy.m_base  )
    {
#ifdef COUNT_INSTANCES 
      Relations::InstanceCounter::instance().increment( type() ) ;
#endif // COUNT_INSTANCES
    };
    
    /// destructor (virtual) 
    virtual ~Relation2D()
    {
#ifdef COUNT_INSTANCES 
      Relations::InstanceCounter::instance().decrement( type() ) ;
#endif // COUNT_INSTANCES
    };
    
    /// the type name 
    const std::string& type() const 
    {
      static const std::string s_type( System::typeinfoName( typeid(OwnType) ) ) ;
      return s_type ;
    };
    
    /** object identification (static method) 
     *  @see DataObject 
     *  @return the unique class identifier 
     */ 
    static  const CLID& classID() 
    {
       static const CLID s_clid =
        Relations::clid( System::typeinfoName( typeid(OwnType) ) );
      return s_clid ;
    };
    
    /** object identification (virtual method)
     *  @see DataObject 
     *  @return the unique class identifier 
     */ 
    virtual const CLID& clID()     const { return classID() ; }
    
  public:  // major functional methods (fast, 100% inline)
    
    /// retrive all relations from the object (fast,100% inline)
    inline   Range       i_relations
    ( const  From&       object    ) const
    { return m_base.i_relations ( object ) ; }
    
    /// retrive all relations from ALL objects (fast,100% inline)
    inline   Range        i_relations () const
    { return m_base.i_relations () ; }
    
    /// make the relation between 2 objects (fast,100% inline method) 
    inline   StatusCode i_relate
    ( const  From&      object1 ,
      const  To&        object2 )
    { return m_base.i_relate   ( object1 , object2 ) ; }
    
    /// remove the concrete relation between objects (fast,100% inline method)
    inline   StatusCode i_remove 
    ( const  From&      object1 ,
      const  To&        object2 )
    { return m_base.i_remove ( object1 , object2 ) ; }
    
    /// remove all relations FROM the defined object (fast,100% inline method)
    inline   StatusCode i_removeFrom 
    ( const  From&      object )
    { return m_base.i_removeFrom ( object ) ; }    
    
    /// remove all relations TO the defined object (fast,100% inline method)
    inline   StatusCode i_removeTo
    ( const  To&        object )
    { return m_base.i_removeTo( object ) ; }
    
    /// remove ALL relations form ALL  object to ALL objects  (fast,100% inline)
    inline  StatusCode i_clear() 
    { return m_base.i_clear() ; };
    
    /// rebuild ALL relations form ALL  object to ALL objects(fast,100% inline)
    inline  StatusCode i_rebuild() 
    { return m_base.i_rebuild() ; };
    
    /** make the relation between 2 objects (fast,100% inline method) 
     *  the call for i_sort() is mandatory
     */
    inline   void       i_push
    ( const  From&      object1 ,
      const  To&        object2 )
    { m_base.i_push    ( object1 , object2 ) ; }
    
    /** (re)sort the table 
     *   mandatory to use after i_push
     */
    inline  void i_sort() { m_base.i_sort() ; }
    
  public: // abstract methods from interface 
    
    /** retrive all relations from the given object object
     *  @param  object  the object
     *  @return pair of iterators for output relations
     */
    virtual Range      relations
    ( const From&      object    ) const { return i_relations( object ) ; }
    
    /** retrive ALL relations from ALL objects  
     *  @return pair of iterators for output relations
     */
    virtual Range      relations () const { return i_relations() ; }
    
    /** make the relation between 2 objects
     *  @param  object1 the first object
     *  @param  object2 the second object
     *  @return status  code
     */
    virtual StatusCode relate
    ( const From&      object1 ,
      const To&        object2 ) { return i_relate( object1 , object2 ) ; }
    
    /** remove the concrete relation between objects
     *
     *   - if there are no relations between the given object
     *     the error code will be returned
     *
     *  Example:
     *  @code
     *    From object1 = ... ;
     *    To   object2 = ... ;
     *    irel->remove( object1 , object2 );
     *  @endcode
     *
     *  @param  object1 the first object
     *  @param  object2 the second object
     *  @return status  code
     */
    virtual StatusCode   remove
    ( const From&        object1 ,
      const To&          object2 ) { return i_remove( object1 , object2 ) ; }
    
    /** remove all relations FROM the defined object
     *
     *   - if there are no relations from the given onject
     *     the error code will be returned
     *
     *  Example:
     *  @code
     *    From object1 = ... ;
     *    irel->removeFrom( object1 );
     *  @endcode
     *
     *
     *  @param  object the object
     *  @return status code
     */
    virtual StatusCode   removeFrom
    ( const From&        object ) { return i_removeFrom( object ) ; }
    
    /** remove all relations TO the defined object
     *
     *   - if there are no relations to the given object
     *     the error code will be returned
     *
     *  Example:
     *  @code
     *    To object1 = ... ;
     *    irel->removeTo( object1 );
     *  @endcode
     *
     *  @param  object the object
     *  @return status code
     */
    virtual StatusCode   removeTo
    ( const To&          object ) { return i_removeTo( object ) ; }
    
    /** remove ALL relations from ALL to ALL objects
     *
     *  @return status code
     */
    virtual StatusCode   clear () { return i_clear() ; }
    
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
    
  public:
    
    /// get the "direct" interface 
    inline       typename Base::Direct*  i_direct  ()       
    { return m_base.i_direct() ; }
    
    /// get the "direct" interface  (const-version)
    inline const typename Base::Direct*  i_direct  () const 
    { return m_base.i_direct() ; }
    
    /// get the "inverse" interface 
    inline       typename Base::Inverse* i_inverse ()       
    { return m_base.i_inverse() ; }
    
    /// get the "inverse" interface  (const version)
    inline const typename Base::Inverse* i_inverse () const 
    { return m_base.i_inverse() ; }
    
  public:  // abstract methods from interface
    
    /** get the "direct" interface 
     *  @see IRelation2D
     *  @return pointer to the 'direct' interface 
     */
    virtual       DirectType*  direct ()        { return i_direct() ; }
    
    /** get the "direct" interface  (const-version)
     *  @see IRelation2D
     *  @return pointer to the 'direct' interface 
     */
    virtual const DirectType*  direct () const  { return i_direct() ; }
    
    /** get the "inverse" interface 
     *  @see IRelation2D
     *  @return pointer to the 'inverse' interface 
     */
    virtual       InverseType* inverse ()       { return i_inverse() ; }
    
    /** get the "inverse" interface  (const version)
     *  @see IRelation2D
     *  @return pointer to the 'inverse' interface 
     */
    virtual const InverseType* inverse () const { return i_inverse() ; }
    
    
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
      if( IInterface::interfaceID()  == id )
      { *ret = static_cast<IInterface*> ( this ); }
      else if( IBase::interfaceID()  == id )
      { *ret = static_cast<IBase*>      ( this ); }
      else if( IBase1::interfaceID() == id )
      { *ret = static_cast<IBase1*>     ( this ); }
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
    virtual unsigned long addRef  () { return  DataObject::addRef  () ; }
    
    /** release the reference counter
     *  @see    IInterface
     *  @see    DataObject
     *  @return current number of references 
     */
    virtual unsigned long release () { return  DataObject::release () ; }
    
  public:
    
    /// POOL identifier
    static std::string GUID() { return Relations::guid ( classID() ) ; }
    
  private:
    
    Base m_base ;
    
  };
  
} ; // end of namespace LHCb

// ============================================================================
// The End
// ============================================================================
#endif // RELATIONS_Relation2D_H
// ============================================================================
