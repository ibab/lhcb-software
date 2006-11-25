// $Id: Relation2D.h,v 1.9 2006-11-25 18:50:40 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ ; version $Revision: 1.9 $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.8  2006/06/11 15:23:46  ibelyaev
//  The major  upgrade: see doc/release.notes
//
// ============================================================================
#ifndef RELATIONS_Relation2D_H 
#define RELATIONS_Relation2D_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <algorithm>
// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/DataObject.h"
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
    /// import "From"  type from the base 
    typedef typename IBase::From_            From_      ;
    /// import "To"    type from the base 
    typedef typename IBase::To_              To_        ;
    /// import "To"    type from the base 
    typedef typename IBase::To               To         ;
    /// shortcut for actual implementation  
    typedef Relations::Relation2<FROM,TO>    Base       ;  
    // shortcut for "direct" interface 
    typedef typename IBase::DirectType       IDirect        ;
    // shortcut for "inverse" interface 
    typedef typename IBase::InverseType      IInverse       ;
  public:
    /// Standard/default  constructor
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
    /// constructor from any direct interface
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
    /// copy constructor 
    Relation2D 
    ( const OwnType& copy  )
      : DataObject    ( copy          ) 
      , Relations::BaseTable  ( copy  ) 
      , IBase         ( copy          ) 
      , m_base        ( copy.m_base  )
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
    /// object identification (static method) 
    static  const CLID& classID() 
    {
      static const CLID s_clid =
        Relations::clid( System::typeinfoName( typeid(OwnType) ) );
      return s_clid ;
    };
    /// object identification (virtual method)
    virtual const CLID& clID()     const { return classID() ; }    
  public:  // major functional methods (fast, 100% inline)
    /// retrive all relations from the object (fast,100% inline)
    inline Range i_relations ( From_ object ) const
    { return m_base.i_relations ( object ) ; }
    /// retrive all relations from ALL objects (fast,100% inline)
    inline Range i_relations () const { return m_base.i_relations () ; }
    /// make the relation between 2 objects (fast,100% inline method) 
    inline StatusCode i_relate ( From_ object1 , To_ object2 )
    { return m_base.i_relate   ( object1 , object2 ) ; }
    /// remove the concrete relation between objects (fast,100% inline method)
    inline StatusCode i_remove ( From_ object1 , To_ object2 )
    { return m_base.i_remove ( object1 , object2 ) ; }
    /// remove all relations FROM the defined object (fast,100% inline method)
    inline   StatusCode i_removeFrom ( From_ object )
    { return m_base.i_removeFrom ( object ) ; }
    /// remove all relations TO the defined object (fast,100% inline method)
    inline   StatusCode i_removeTo ( To_ object )
    { return m_base.i_removeTo( object ) ; }
    /// remove ALL relations form ALL  object to ALL objects  (fast,100% inline)
    inline  StatusCode i_clear() { return m_base.i_clear() ; };
    /// rebuild ALL relations form ALL  object to ALL objects(fast,100% inline)
    inline  StatusCode i_rebuild() { return m_base.i_rebuild() ; };
    /** make the relation between 2 objects (fast,100% inline method) 
     *  the call for i_sort() is mandatory
     */
    inline void i_push ( From_ object1 , To_ object2 )
    { m_base.i_push    ( object1 , object2 ) ; }
    /** (re)sort the table 
     *   mandatory to use after i_push
     */
    inline  void i_sort() { m_base.i_sort() ; }
  public: // abstract methods from interface
    /// retrive all relations from the given object object
    virtual Range relations ( From_ object ) const 
    { return i_relations( object ) ; }
    /// retrive ALL relations from ALL objects  
    virtual Range relations () const { return i_relations() ; }    
    /// make the relation between 2 objects
    virtual StatusCode relate ( From_ object1 , To_ object2 ) 
    { return i_relate( object1 , object2 ) ; }
    /// remove the concrete relation between objects
    virtual StatusCode   remove ( From_ object1 , To_ object2 ) 
    { return i_remove( object1 , object2 ) ; }
    /// remove all relations FROM the defined object
    virtual StatusCode   removeFrom ( From_ object ) 
    { return i_removeFrom( object ) ; }
    /// remove all relations TO the defined object
    virtual StatusCode   removeTo ( To_ object ) 
    { return i_removeTo( object ) ; }
    /// remove ALL relations from ALL to ALL objects
    virtual StatusCode   clear () { return i_clear() ; }    
    /// rebuild ALL relations from ALL  object to ALL objects 
    virtual  StatusCode rebuild() { return i_rebuild () ; };
    /// update the object after POOL/ROOT reading 
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
    /// get the "direct" interface 
    virtual       DirectType*  direct ()        { return i_direct() ; }    
    /// get the "direct" interface  (const-version)
    virtual const DirectType*  direct () const  { return i_direct() ; }    
    /// get the "inverse" interface 
    virtual       InverseType* inverse ()       { return i_inverse() ; }
    /// get the "inverse" interface  (const version)
    virtual const InverseType* inverse () const { return i_inverse() ; }
  public:
    /// query the interface
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
    /// increase the reference counter
    virtual unsigned long addRef  () { return  DataObject::addRef  () ; }    
    /// release the reference counter
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
