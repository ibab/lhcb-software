// $Id: RelationWeighted2D.h,v 1.14 2009-04-24 15:26:46 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ ; version $Revision: 1.14 $
// ============================================================================
// $Log: not supported by cvs2svn $
//
// ============================================================================
#ifndef RELATIONS_RelationWeighted2D_H 
#define RELATIONS_RelationWeighted2D_H 1
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
  /** @class RelationWeighted2D Relations/RelationWeighted2D.h 
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
    // ========================================================================
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
    typedef typename IBase::From_                           From_   ;
    /// import "To"    type from the base 
    typedef typename IBase::To                              To      ;
    typedef typename IBase::To_                             To_     ;
    /// import "Weight" type from the base 
    typedef typename IBase::Weight                          Weight  ;
    typedef typename IBase::Weight_                         Weight_ ;
    /// shortcut for actual implementation  
    typedef Relations::Relation2Weighted<FROM,TO,WEIGHT>    Base    ;
    // shortcut for "direct" interface 
    typedef typename IBase::DirectType                      IDirect ;
    // shortcut for "inverse" interface 
    typedef typename IBase::InverseType                     IInverse ; 
    /// the actual type of the entry
    typedef typename IBase::Entry                           Entry    ;
    // ========================================================================
 public:
    // ========================================================================
    /// Standard/default  constructor
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
    }
    /// constructor from any direct interface
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
    }
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
    }
    /// copy constructor 
    RelationWeighted2D 
    ( const OwnType& copy  )
      : DataObject    ( copy ) 
      , Relations::BaseWeightedTable ( copy  ) 
      , IBase      ( copy         ) 
      , m_base     ( copy.m_base  )
    {
#ifdef COUNT_INSTANCES 
      Relations::InstanceCounter::instance().increment( type() ) ;
#endif // COUNT_INSTANCES
    }
    /// destructor (virtual) 
    virtual ~RelationWeighted2D()
    {
#ifdef COUNT_INSTANCES 
      Relations::InstanceCounter::instance().decrement( type() ) ;
#endif // COUNT_INSTANCES
    }
    /// the type name 
    const std::string& type() const 
    {
      static const std::string s_type( System::typeinfoName( typeid(OwnType) ) ) ;
      return s_type ;
    }
    /// object identification (static method) 
    static  const CLID& classID() 
    {
      static const CLID s_clid = 
        Relations::clid( System::typeinfoName( typeid(OwnType) ) );
      return s_clid ;
    }
    /// object identification (virtual method)
    virtual const CLID& clID()     const { return classID() ; }    
    // ========================================================================
  public:  // major functional methods (fast, 100% inline)
    // ========================================================================
    /// retrive all relations from the object (fast,100% inline)
    inline   Range i_relations ( From_ object) const 
    { return m_base.i_relations ( object ) ;}
    /// retrive ALL relations from ALL objects (fast,100% inline)
    inline   Range i_relations() const { return m_base.i_relations () ;}
    /// retrive all relations from the object (fast,100% inline)
    inline   Range      i_relations
    ( From_ object , Weight_ threshold , const bool flag ) const 
    { return m_base.i_relations ( object , threshold , flag ) ;}
    /// make the relation between 2 objects (fast,100% inline)
    inline   StatusCode i_relate 
    ( From_ object1 , To_ object2 , Weight_ weight ) 
    {
      const Entry entry ( object1 , object2 , weight ) ;
      return i_add ( entry ) ;
    }
    /// add the entry 
    inline   StatusCode i_add ( const Entry& entry ) 
    { return m_base.i_add ( entry ) ; }
    /// retrive all relations from the object (fast,100% inline)
    inline   Range      i_inRange
    ( From_ object , Weight_ low , Weight_ high ) const 
    { return m_base.i_inRange ( object , low , high ) ;}
    /// remove the concrete relation between objects (fast,100% inline)
    inline   StatusCode i_remove ( From_ object1 , To_ object2 ) 
    { return m_base.i_remove ( object1 , object2 ) ; }
    /// remove all relations FROM the defined object (fast,100% inline)
    inline   StatusCode i_removeFrom ( From_ object )
    { return m_base.i_removeFrom ( object ) ; }
    /// remove all relations TO the defined object (fast,100% inline)
    inline   StatusCode i_removeTo ( To_ object )
    { return m_base.i_removeTo ( object ) ; }
    /// filter out the relations FROM the defined object (fast,100% inline)
    inline   StatusCode i_filterFrom 
    ( From_ object , Weight_ threshold , const bool flag )  
    { return m_base.i_filterFrom ( object , threshold , flag ) ; }
    /// filter out the relations TO the defined object (fast,100% inline)
    inline   StatusCode i_filterTo 
    ( To_ object , Weight_ threshold , const bool flag )  
    { return m_base.i_filterTo ( object , threshold , flag ) ; }
    /// filter out all relations (fast,100% inline)
    inline   StatusCode i_filter ( Weight_ threshold , const bool flag )  
    { return m_base.i_filter ( threshold , flag ) ; }
    /// remove ALL relations from ALL objects to ALL objects (fast,100% inline)
    inline   StatusCode i_clear () { return m_base.i_clear ( ) ; }   
    /// rebuild ALL relations form ALL  object to ALL objects(fast,100% inline)
    inline  StatusCode i_rebuild() { return m_base.i_rebuild() ; }
    /** make the relation between 2 objects (fast,100% inline)
     *  the call for i_sort() is mandatory after usage of this method!
     */
    inline   void i_push ( From_ object1 , To_ object2 , Weight_ weight  ) 
    { m_base.i_push ( object1 , object2 , weight ) ;}
    /** (re)sort of the table 
     *   mandatory to use after i_push 
     */
    inline   void  i_sort() { m_base.i_sort () ; }
    // ========================================================================
  public: // merge 
    // ========================================================================
    /** merge with the sorted range of relations 
     *  @attention the range is assumed to be sorted! 
     *  @param range the range to be added 
     *  @return self-reference 
     */
    RelationWeighted2D& merge ( const Range& range ) 
    { m_base.merge ( range ) ; return *this ; }
    /** merge with the sorted range of relations 
     *  @attention the range is assumed to be sorted! 
     *  @param range the range to be added 
     *  @return self-reference 
     */
    RelationWeighted2D& imerge ( const typename IInverse::Range& range ) 
    { m_base.imerge ( range ) ; return *this ; }
    /** merge with the sorted range of relations 
     *  @attention the range is assumed to be sorted! 
     *  @param range the range to be added 
     *  @return self-reference 
     */
    RelationWeighted2D& operator+=( const Range& range ) 
    { return merge ( range ) ; }
    // ========================================================================
 public:  // abstract methods from interface
    // ========================================================================
    /// retrive all relations from the object
    virtual Range relations ( From_ object ) const 
    { return i_relations ( object ) ; }
    /// retrive ALL relations from ALL objects
    virtual Range relations() const { return i_relations () ; }    
    /** retrive all relations from the object which has weigth 
     *  larger/smaller than the threshold value 
     */
    virtual  Range      relations
    ( From_ object, Weight_ threshold , const bool flag ) const 
    { return i_relations ( object , threshold , flag ) ; }
    /** retrive all relations from the object which has weigth 
     *  withing the specified range 
     */
    virtual  Range inRange ( From_ object , Weight_ low , Weight_ high ) const 
    { return i_inRange ( object , low , high ) ; }
    /// make the relation between 2 objects 
    virtual  StatusCode relate ( From_ object1 , To_ object2 , Weight_ weight ) 
    { return i_relate( object1 , object2 , weight ) ; }
    /// add the entry 
    virtual  StatusCode add ( const Entry& entry ) 
    { return i_add ( entry ) ; }
    /// remove the concrete relation between objects
    virtual  StatusCode remove ( From_ object1 , To_ object2 ) 
    { return i_remove ( object1 , object2 ) ; }
    /// remove all relations FROM the defined object
    virtual  StatusCode removeFrom ( From_ object )
    { return i_removeFrom ( object ) ; }
    /// remove all relations TO the defined object
    virtual  StatusCode removeTo ( To_ object ) { return i_removeTo( object ) ; }
    /** filter out the relations FROM the defined object, which
     *  have a weight larger(smaller)than the threshold weight 
     */
    virtual  StatusCode filterFrom 
    ( From_ object , Weight_ threshold , const bool flag )  
    { return i_filterFrom ( object , threshold , flag ) ; }
    /** filter out the relations TO the defined object, which
     *  have a weight larger/smaller than the threshold weight 
     */
    virtual  StatusCode filterTo 
    ( To_ object , Weight_ threshold , const bool flag )  
    { return i_filterTo ( object , threshold , flag ) ; }
    /** filter out all relations which
     *  have a weight larger/smaller than the threshold weight 
     */
    virtual  StatusCode filter ( Weight_ threshold , const bool flag )  
    { return i_filter ( threshold , flag ) ; }
    /// remove ALL relations from ALL objects to ALL objects 
    virtual  StatusCode clear () { return i_clear () ; }    
    /// rebuild ALL relations from ALL  object to ALL objects 
    virtual  StatusCode rebuild() { return i_rebuild () ; };   
    /// update the object after POOL/ROOT reading 
    virtual StatusCode update( int flag ) 
    {
      if( 0 == flag ) { return i_rebuild() ; }
      return StatusCode::SUCCESS ;
    }
    // ========================================================================
  public:    
    // ========================================================================
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
    // ========================================================================
  public:  // abstract methods from interface
    // ========================================================================
    /// get the "direct" interface
    virtual       DirectType*  direct ()        { return i_direct() ; }
    /// get the "direct" interface  (const-version)
    virtual const DirectType*  direct () const  { return i_direct() ; }    
    /// get the "inverse" interface 
    virtual       InverseType* inverse ()       { return i_inverse() ; }    
    /// get the "inverse" interface  (const version)
    virtual const InverseType* inverse () const { return i_inverse() ; }    
    // ========================================================================
  public:
    // ========================================================================
    /// query the interface
    virtual StatusCode queryInterface
    ( const InterfaceID& id , void** ret )
    {
      if ( 0 == ret  )          { return StatusCode::FAILURE ; } // RETURN !!!
      if ( IInterface::interfaceID()  == id )
      { *ret = static_cast<IInterface*> ( this ); }
      else if ( IBase::interfaceID()  == id )
      { *ret = static_cast<IBase*>      ( this ); }
      else if ( IBase1::interfaceID() == id )
      { *ret = static_cast<IBase1*>     ( this ); }
      else                     { return StatusCode::FAILURE ; } //  RETURN !!!
      ///
      addRef() ;
      return StatusCode::SUCCESS ;
    }
    // ========================================================================
  public:
    // ========================================================================
    /// increase the reference counter
    virtual unsigned long addRef  () { return  DataObject::addRef  () ; }
    /// release the reference counter
    virtual unsigned long release () { return  DataObject::release () ; }
    // ========================================================================
  public:
    // ========================================================================
    /// POOL identifier
    static std::string GUID() { return Relations::guid ( classID() ) ; }
    // ========================================================================
  private:
    // ========================================================================
    // The assigenement opeartor is disabled 
    RelationWeighted2D& operator=( const RelationWeighted2D& ) ; // no assignement
    // ========================================================================
  private:
    // ========================================================================
    /// the actual storage of relations 
    Base m_base ;
    // ========================================================================
  };
  // ==========================================================================
} // end of namespace LHCb 
// ============================================================================
// The End  
// ============================================================================
#endif // RELATIONS_RelationWeighted2D_H
// ============================================================================
