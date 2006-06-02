// $Id: RelationWeighted2D.h,v 1.7 2006-06-02 16:18:39 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ ; version $Revision: 1.7 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.6  2006/02/07 09:22:24  ibelyaev
//  update for Win32
// 
// ============================================================================
#ifndef RELATIONS_RelationWeighted2D_H 
#define RELATIONS_RelationWeighted2D_H 1
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
#include "Relations/IRelationWeighted.h"
#include "Relations/RelationWeightedBase.h"
#include "Relations/Relation2Weighted.h"
// ============================================================================
// Forward declaration
// ============================================================================
class StreamBuffer;
// ============================================================================

namespace LHCb
{
  
  /** @class RelationWeighted2D RelationWeighted2D.h 
   *  
   *  Templated object, which implements the bidirectional 
   *  relations with the weight
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   28/01/2002
   */
  template<class FROM, class TO, class WEIGHT>
  class RelationWeighted2D 
    : public DataObject
    , public Relations::BaseWeightedTable 
    , public IRelationWeighted2D<FROM,TO,WEIGHT>
  {
  public:
    
    /// shortcut for own type 
    typedef RelationWeighted2D<FROM,TO,WEIGHT>              OwnType    ;
    /// shortcut for inverse type 
    typedef RelationWeighted2D<TO,FROM,WEIGHT>              InvType    ;
    /// shortcut for "direct"(="main") interface
    typedef IRelationWeighted2D<FROM,TO,WEIGHT>             IBase      ;
    /// shortcut for "direct"(="main") interface
    typedef IRelationWeighted<FROM,TO,WEIGHT>               IBase1     ;
    /// shortcut for "inverse interface  interface
    typedef IRelationWeighted<TO,FROM,WEIGHT>               IBase2     ;
    /// shortcut for direct subinterface 
    typedef typename IBase::DirectType                      DirectType     ;
    /// shortcut for inverse subinterface 
    typedef typename IBase::InverseType                     InverseType    ;
    /// import "Range" type from the base 
    typedef typename IBase::Range                           Range   ;
    /// import "From"  type from the base 
    typedef typename IBase::From                            From    ;
    /// import "To"    type from the base 
    typedef typename IBase::To                              To      ;
    /// import "Weight" type from the base 
    typedef typename IBase::Weight                          Weight  ;
    /// shortcut for actual implementation  
    typedef Relations::Relation2Weighted<FROM,TO,WEIGHT>    Base       ;
    // shortcut for "direct" interface 
    typedef typename IBase::DirectType                      IDirect        ;
    // shortcut for "inverse" interface 
    typedef typename IBase::InverseType                     IInverse       ;
    
  public:
    
    /** Standard/default  constructor
     *  @param reserve the map-size to be preallocated
     */
    RelationWeighted2D 
    ( const size_t reserve  = 0 ) 
      : DataObject () 
      , Relations::BaseWeightedTable () 
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
    RelationWeighted2D 
    ( const IDirect& copy  )
      : DataObject () 
      , Relations::BaseWeightedTable () 
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
    RelationWeighted2D 
    ( const IInverse& inv  , 
      const int       flag ) 
      : DataObject () 
      , Relations::BaseWeightedTable () 
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
    RelationWeighted2D 
    ( const OwnType& copy  )
      : DataObject ( copy         ) 
      , Relations::BaseWeightedTable ( copy  ) 
      , IBase      ( copy         ) 
      , m_base     ( copy.m_base  )
    {
#ifdef COUNT_INSTANCES 
      Relations::InstanceCounter::instance().increment( type() ) ;
#endif // COUNT_INSTANCES
    };
    
    /// destructor (virtual) 
    virtual ~RelationWeighted2D()
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
    inline   Range i_relations
    ( const  From& object) const 
    { return m_base.i_relations ( object ) ;} 
    
    /// retrive ALL relations from ALL objects (fast,100% inline)
    inline   Range i_relations() const
    { return m_base.i_relations (        ) ;}
    
    /// retrive all relations from the object (fast,100% inline)
    inline   Range      i_relations
    ( const  From&      object,
      const  Weight&    threshold ,
      const  bool       flag      ) const 
    { return m_base.i_relations ( object , threshold , flag ) ;}
    
    /// make the relation between 2 objects (fast,100% inline)
    inline   StatusCode i_relate 
    ( const  From&      object1 , 
      const  To&        object2 ,
      const  Weight&    weight  ) 
    { return m_base.i_relate ( object1 , object2 , weight ) ;}
    
    /// retrive all relations from the object (fast,100% inline)
    inline   Range      i_inRange
    ( const  From&      object ,
      const  Weight&    low    ,
      const  Weight&    high   ) const 
    { return m_base.i_inRange ( object , low , high ) ;}
    
    /// remove the concrete relation between objects (fast,100% inline)
    inline   StatusCode i_remove 
    ( const  From&      object1 , 
      const  To&        object2 ) 
    { return m_base.i_remove ( object1 , object2 ) ; }
    
    /// remove all relations FROM the defined object (fast,100% inline)
    inline   StatusCode i_removeFrom 
    ( const  From&      object )
    { return m_base.i_removeFrom ( object ) ; }
    
    /// remove all relations TO the defined object (fast,100% inline)
    inline   StatusCode i_removeTo 
    ( const  To&        object )
    { return m_base.i_removeTo ( object ) ; }
    
    /// filter out the relations FROM the defined object (fast,100% inline)
    inline   StatusCode i_filterFrom 
    ( const  From&      object    ,
      const  Weight&    threshold ,
      const  bool       flag      )  
    { return m_base.i_filterFrom ( object , threshold , flag ) ; }
    
    /// filter out the relations TO the defined object (fast,100% inline)
    inline   StatusCode i_filterTo 
    ( const  To&        object    ,
      const  Weight&    threshold ,
      const  bool       flag      )  
    { return m_base.i_filterTo ( object , threshold , flag ) ; }
    
    /// filter out all relations (fast,100% inline)
    inline   StatusCode i_filter 
    ( const  Weight&    threshold ,
      const  bool       flag      )  
    { return m_base.i_filter ( threshold , flag ) ; }
    
    /// remove ALL relations from ALL objects to ALL objects (fast,100% inline)
    inline   StatusCode i_clear () { return m_base.i_clear ( ) ; }
    
    /// rebuild ALL relations form ALL  object to ALL objects(fast,100% inline)
    inline  StatusCode i_rebuild() 
    { return m_base.i_rebuild() ; };
    
    /** make the relation between 2 objects (fast,100% inline)
     *  the call for i_sort() is mandatory after usage of this method!
     */
    inline   void i_push
    ( const  From&      object1 , 
      const  To&        object2 ,
      const  Weight&    weight  ) 
    { m_base.i_push ( object1 , object2 , weight ) ;}
    
    /** (re)sort of the table 
     *   mandatory to use after i_push 
     */
    inline   void  i_sort() { m_base.i_sort () ; }
    
  public:  // abstract methods from interface
    
    /** retrive all relations from the object
     *  
     *  - relations are returned in the form of iterator pair:
     *     \n IRelation<FROM,TO,WEIGHT>* irel   = ... ;
     *     \n From                       object = ... ; 
     *     \n Range r = irel->relations( object );
     *  
     *  - the number of related object is:
     *     \n    const unsigned nRel = r.end()  - r.begin() ;
     *     \n // const unsigned nRel = r.second - r.first   ; // the same!
     *
     *  - the related elements could be retrieved using the iterations:
     *     \n for( iterator it = r.begin() ; r.end() != it ; ++it ){
     *     \n /// extract and use the relation
     *     \n To     to     = it->to()     ; // get the "to" object  
     *     \n // To  to     = *it          ; // the same   
     *     \n Weight weight = it->weight() ; // get the weight 
     *     \n From   from   = it->weight() ; // again get the "from" object
     *     \n };
     *
     *  @see    IRelationWeighted1D 
     *  @see    RelationWeighted1DBase
     *  @see    RelationWeighted1DTypeTraits
     *  @param  object  the object
     *  @return pair of iterators for output relations   
     */
    virtual Range relations
    ( const From& object) const { return i_relations ( object ) ; }
    
    /** retrive ALL relations from ALL objects
     *  
     *  @see    IRelationWeighted1D 
     *  @see    RelationWeighted1DBase
     *  @see    RelationWeighted1DTypeTraits
     *  @return pair of iterators for output relations   
     */
    virtual Range relations() const { return i_relations () ; }
    
    /** retrive all relations from the object which has weigth 
     *  larger/smaller than the threshold value 
     *  @param  object    the object
     *  @param  threshold threshold value for the weight 
     *  @param  flag      flag for larger/smaller
     *  @return pair of iterators for output relations   
     */
    virtual  Range      relations
    ( const  From&      object,
      const  Weight&    threshold ,
      const  bool       flag      ) const 
    { return i_relations ( object , threshold , flag ) ; }
    
    /** retrive all relations from the object which has weigth 
     *  withing the specified range 
     *  @param  object  the object
     *  @param  low     low  threshold value for the weight 
     *  @param  high    high threshold value for the weight 
     *  @return pair of iterators for output relations   
     */
    virtual  Range      inRange 
    ( const  From&      object ,
      const  Weight&    low    ,
      const  Weight&    high   ) const 
    { return i_inRange ( object , low , high ) ; }
    
    /** make the relation between 2 objects 
     *  @param  object1 the first object
     *  @param  object2 the second object 
     *  @param  weight  the weigth for the relation 
     *  @return status  code 
     */
    virtual  StatusCode relate 
    ( const  From&      object1 , 
      const  To&        object2 ,
      const  Weight&    weight  ) 
    { return i_relate( object1 , object2 , weight ) ; }
    
    /** remove the concrete relation between objects
     *  @param  object1 the first object
     *  @param  object2 the second object 
     *  @return status  code 
     */
    virtual  StatusCode remove 
    ( const  From&      object1 , 
      const  To&        object2 ) 
    { return i_remove ( object1 , object2 ) ; }
    
    /** remove all relations FROM the defined object
     *  @param  object  smart reference to the object
     *  @return status code 
     */
    virtual  StatusCode removeFrom 
    ( const  From&      object )
    { return i_removeFrom ( object ) ; }
    
    /** remove all relations TO the defined object
     *  @param object  smart reference to the object
     *  @return status code 
     */
    virtual  StatusCode removeTo 
    ( const  To&        object )
    { return i_removeTo( object ) ; }
    
    /** filter out the relations FROM the defined object, which
     *  have a weight larger(smaller)than the threshold weight 
     *  @param  object    the object
     *  @param  threshold threshold value for the weight 
     *  @param  flag      flag for larger/smaller
     *  @return status code 
     */
    virtual  StatusCode filterFrom 
    ( const  From&      object    ,
      const  Weight&    threshold ,
      const  bool       flag      )  
    { return i_filterFrom ( object , threshold , flag ) ; }
    
    /** filter out the relations TO the defined object, which
     *  have a weight larger/smaller than the threshold weight 
     *  @param  object    the object
     *  @param  threshold threshold value for the weight 
     *  @param  flag      flag for larger/smaller
     *  @return status code 
     */
    virtual  StatusCode filterTo 
    ( const  To&        object    ,
      const  Weight&    threshold ,
      const  bool       flag      )  
    { return i_filterTo ( object , threshold , flag ) ; }
    
    /** filter out all relations which
     *  have a weight larger/smaller than the threshold weight 
     *  @param  threshold  threshold value for the weight 
     *  @param  flag       flag for larger/smaller
     *  @return status code 
     */
    virtual  StatusCode filter 
    ( const  Weight&    threshold ,
      const  bool       flag      )  
    { return i_filter ( threshold , flag ) ; }
    
    /** remove ALL relations from ALL objects to ALL objects 
     *  @return status code 
     */
    virtual  StatusCode clear () { return i_clear () ; }
    
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
    
  public:
    
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
#endif // RELATIONS_RelationWeighted2D_H
// ============================================================================
