// $Id: Relation1D.h,v 1.9 2006-06-11 19:37:02 ibelyaev Exp $
// =============================================================================
// CVS tag $Name: not supported by cvs2svn $ ; version $Revision: 1.9 $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.8  2006/06/11 17:46:06  ibelyaev
//  fix for OLD gcc
//
// Revision 1.7  2006/06/11 15:23:46  ibelyaev
//  The major  upgrade: see doc/release.notes
//
// ============================================================================
#ifndef RELATIONS_Relation1D_H
#define RELATIONS_Relation1D_H 1
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
#include "GaudiKernel/SmartRef.h"
// ============================================================================
// From Relations
// ============================================================================
#include "Relations/RelationUtils.h"
#include "Relations/IRelation.h"
#include "Relations/RelationBase.h"
#include "Relations/Relation.h"
// ============================================================================
namespace LHCb
{
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
   *           FROM should differ from the actual type of TO
   *  
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   25/01/2002
   */
  
  template<class FROM,class TO>
  class Relation1D 
    : public DataObject
    , public Relations::BaseTable 
    , public IRelation<FROM,TO>
  {    
  public: 
    /// short cut for own     type
    typedef Relation1D<FROM,TO>              OwnType        ;
    /// short cut for inverse type
    typedef Relation1D<TO,FROM>              InvType        ;
    /// short cut for interface 
    typedef IRelation<FROM,TO>               IBase          ;
    /// import "Range" type from the base 
    typedef typename IBase::Range            Range          ;
    /// import "From"  type from the base 
    typedef typename IBase::From             From           ;
    /// import "From"  type from the base 
    typedef typename IBase::From_            From_          ;
    /// import "To"    type from the base 
    typedef typename IBase::To               To             ;
    /// import "To"    type from the base 
    typedef typename IBase::To_              To_            ;
    /// short cut for the actual implementation type 
    typedef typename Relations::Relation<FROM,TO> Base      ;
    // shortcut for "direct" interface 
    typedef typename IBase::DirectType       IDirect        ;
    // shortcut for "inverse" interface 
    typedef typename IBase::InverseType      IInverse       ;
  public:
    /// the default constructor
    Relation1D 
    ( const size_t reserve = 0 ) 
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
    Relation1D 
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
    Relation1D 
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
    Relation1D 
    ( const OwnType& copy  )
      : DataObject    ( copy         )
      , Relations::BaseTable ( copy  )  
      , IBase         ( copy         ) 
      , m_base        ( copy.m_base  )
    {
#ifdef COUNT_INSTANCES 
      Relations::InstanceCounter::instance().increment( type() ) ;
#endif // COUNT_INSTANCES
    };
    /// destructor (virtual)
    virtual ~Relation1D() 
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
    inline   Range i_relations ( From_ object ) const
    { return m_base.i_relations ( object ) ; }
    /// retrive all relations from ALL objects (fast,100% inline)
    inline   Range i_relations () const { return m_base.i_relations () ; }
    /// make the relation between 2 objects (fast,100% inline method) 
    inline   StatusCode i_relate ( From_ object1 , To_ object2 )
    { return m_base.i_relate   ( object1 , object2 ) ; }
    /// remove the concrete relation between objects (fast,100% inline method)
    inline   StatusCode i_remove ( From_ object1 , To_ object2 )
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
     *   - the call for i_sort() is mandatory 
     */
    inline   void i_push ( From_ object1 , To_ object2 )
    { m_base.i_push   ( object1 , object2 ) ; }
    /** (re)sort the table 
     *   mandatory to use after i_push
     */
    inline  void i_sort() { m_base.i_sort() ; }
  public: // abstract methods from interface
    /// retrive all relations from the given object object
    virtual Range      relations ( From_ object ) const 
    { return i_relations( object ) ; }
    /// retrive ALL relations from ALL objects  
    virtual Range      relations () const { return i_relations() ; }    
    /// make the relation between 2 objects
    virtual StatusCode relate ( From_ object1 , To_ object2 ) 
    { return i_relate( object1 , object2 ) ; }
    /// remove the concrete relation between objects
    virtual StatusCode remove ( From_ object1 , To_ object2 ) 
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
    /// query the interface
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
  
} ; // end of namepace LHCb

// =============================================================================
// The End
// =============================================================================
#endif // RELATIONS_Relation1D_H
// =============================================================================
