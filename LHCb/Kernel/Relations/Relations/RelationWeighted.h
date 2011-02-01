// $Id: RelationWeighted.h,v 1.16 2009-04-24 15:26:46 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.16 $
// ============================================================================
// $Log: not supported by cvs2svn $
//
// ============================================================================
#ifndef RELATIONS_RelationWeighted_H 
#define RELATIONS_RelationWeighted_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <algorithm>
// ============================================================================
// From Relation
// ============================================================================
#include "Relations/RelationUtils.h"
#include "Relations/IRelationWeighted.h"
#include "Relations/RelationWeightedBase.h"
// ============================================================================

namespace Relations
{  
  /** @class RelationWeighted Relations/RelationWeighted.h
   * 
   *  The simplest implementation of unidirectional weighted relations.
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
   *  @date   28/01/2002
   */
  template<class FROM, class TO, class WEIGHT>
  class RelationWeighted 
    : public BaseWeightedTable  
    , public IRelationWeighted<FROM,TO,WEIGHT> 
  {    
  public: 
    // ========================================================================
    /// shortcut for own type  
    typedef RelationWeighted<FROM,TO,WEIGHT>                OwnType  ;
    /// shortcut for inverse type  
    typedef RelationWeighted<TO,FROM,WEIGHT>                InvType  ;
    /// short cut for interface
    typedef IRelationWeighted<FROM,TO,WEIGHT>               IBase    ;
    /// short cut for the actual implementation type 
    typedef Relations::RelationWeightedBase<FROM,TO,WEIGHT> Base     ;
    /// shortcut for inverse base 
    typedef Relations::RelationWeightedBase<TO,FROM,WEIGHT> InvBase  ;
    /// shortcut for direct base type 
    typedef Base                                            Direct   ;
    /// shortcut for inverse base type 
    typedef InvBase                                         Inverse  ;
    /// shortcut for direct  interface 
    typedef IBase                                           IDirect  ;
    /// shortcut for inverse base type 
    typedef typename IDirect::InverseType                   IInverse ;    
    /// import basic types from Interface 
    typedef typename IBase::Range                           Range   ;
    /// import basic types from Interface 
    typedef typename IBase::From                            From    ;
    /// import basic types from Interface 
    typedef typename IBase::From_                           From_   ;
    /// import basic types from Interface 
    typedef typename IBase::To                              To      ;
    typedef typename IBase::To_                             To_     ;
    /// import basic types from Interface 
    typedef typename IBase::Weight                          Weight  ;
    typedef typename IBase::Weight_                         Weight_ ;
    /// the actual type of the entry
    typedef typename IBase::Entry                           Entry   ;
    // ========================================================================
  public:
    // ========================================================================
    /// the standard/default constructor
    RelationWeighted 
    ( const size_t reserve = 0 ) 
      : BaseWeightedTable () 
      , IBase         (         ) 
      , m_direct      ( reserve ) 
      , m_inverse_aux ( 0       ) 
    {}
    /// constructor from arbitrary "direct" interface 
    RelationWeighted 
    ( const IDirect& copy ) 
      : BaseWeightedTable () 
      , IBase() 
      , m_direct      ( copy ) 
      , m_inverse_aux ( 0    ) 
    {} 
    /** constructor from "inverse interface"
     *  It is an efficient way to "invert" relation 
     *  @param inv interface to be inverted 
     *  @param flag artificial argument, to distinguish from copy constructor
     */
    RelationWeighted
    ( const IInverse&    inv     , 
      const int          flag  ) 
      : BaseWeightedTable () 
      , IBase         (   ) 
      , m_direct      ( inv , flag ) 
      , m_inverse_aux ( 0 )  
    {}
    /** copy constructor is publc, 
     *  but it is not recommended for direct usage 
     *  @param copy object to be copied 
     */
    RelationWeighted 
    ( const OwnType& copy   ) 
      : BaseWeightedTable ( copy ) 
      , IBase         ( copy          ) 
      , m_direct      ( copy.m_direct )
      , m_inverse_aux ( 0             ) 
    {}
    /// destructor (virtual)
    virtual ~RelationWeighted () {} 
    // ========================================================================
  public:  // major functional methods (fast, 100% inline)
    // ========================================================================
    /// retrive all relations from the object (fast,100% inline)
    inline  Range i_relations ( From_ object) const
    {
      typename Base::IP ip = m_direct.i_relations( object );
      return Range(ip.first, ip.second);
    }
    /// retrive ALL relations from ALL objects (fast,100% inline)
    inline  Range i_relations() const
    {
      typename Base::IP ip = m_direct.i_relations();
      return Range(ip.first, ip.second);
    }
    /// retrive all relations from the object (fast,100% inline)
    inline   Range      i_relations
    ( From_ object, Weight_ threshold , const bool flag ) const 
    {
      typename Base::IP ip = m_direct.i_relations( object , threshold , flag );
      return Range(ip.first, ip.second);
    }
    /// retrive all relations in the range from the object (fast,100% inline)
    inline   Range      i_inRange
    ( From_ object , Weight_ low , Weight_ high ) const 
    {
      typename Base::IP ip = m_direct.i_inRange ( object , low , high );
      return Range(ip.first, ip.second);
    }
    /// make the relation between 2 objects (fast,100% inline)
    inline   StatusCode i_relate 
    ( From_ object1 , To_ object2 , Weight_ weight ) 
    {
      const Entry entry ( object1 , object2 , weight ) ;
      return i_add ( entry ) ;
    }
    /// add the entry  
    inline   StatusCode i_add ( const Entry& entry ) 
    {
      StatusCode sc = m_direct.i_add ( entry ) ;
      if ( sc.isFailure() || 0 == m_inverse_aux ) { return sc ; }
      const typename Inverse::Entry 
        ientry ( entry.to() , entry.from() , entry.weight() ) ;
      return m_inverse_aux -> i_add ( ientry );
    }
    /// remove the concrete relation between objects (fast,100% inline)
    inline   StatusCode i_remove ( From_ object1 , To_ object2 ) 
    { 
      StatusCode sc = m_direct.  i_remove ( object1 , object2 ) ; 
      if ( sc.isFailure() || 0 == m_inverse_aux ) { return sc ; }
      return m_inverse_aux -> i_remove ( object2 , object1 ) ; 
    }
    /// remove all relations FROM the defined object (fast,100% inline)
    inline   StatusCode i_removeFrom ( From_ object )
    { 
      StatusCode sc = m_direct.  i_removeFrom ( object ) ; 
      if ( sc.isFailure() || 0 == m_inverse_aux ) { return sc ; }
      return m_inverse_aux ->i_removeTo   ( object ) ; 
    }
    /// remove all relations TO the defined object (fast,100% inline)
    inline   StatusCode i_removeTo ( To_ object )
    { 
      StatusCode sc = m_direct.  i_removeTo   ( object ) ; 
      if ( sc.isFailure() || 0 == m_inverse_aux ) { return sc ; }
      return m_inverse_aux -> i_removeFrom ( object ) ; 
    }
    /// filter out the relations FROM the defined object (fast,100% inline)
    inline   StatusCode i_filterFrom 
    ( From_ object , Weight_ threshold , const bool flag )  
    { 
      StatusCode sc = m_direct.  i_filterFrom ( object , threshold , flag ) ; 
      if ( sc.isFailure() || 0 == m_inverse_aux ) { return sc ; }
      return m_inverse_aux -> i_filterTo   ( object , threshold , flag ) ; 
    }
    /// filter out the relations TO the defined object (fast,100% inline)
    inline   StatusCode i_filterTo 
    ( To_ object , Weight_ threshold , const bool flag )  
    { 
      StatusCode sc =  m_direct.  i_filterTo   ( object , threshold , flag ) ; 
      if ( sc.isFailure() || 0 == m_inverse_aux ) { return sc ; }
      return m_inverse_aux -> i_filterFrom ( object , threshold , flag ) ; 
    }
    /// filter out all relations (fast,100% inline)
    inline   StatusCode i_filter 
    ( Weight_ threshold , const bool flag )  
    { 
      StatusCode sc =  m_direct.  i_filter( threshold , flag ) ; 
      if ( sc.isFailure() || 0 == m_inverse_aux ) { return sc ; }
      return m_inverse_aux -> i_filter( threshold , flag ) ; 
    }
    /// remove ALL relations from ALL objects to ALL objects (fast,100% inline)
    inline  StatusCode i_clear () 
    { 
      StatusCode sc =  m_direct.  i_clear () ; 
      if ( sc.isFailure() || 0 == m_inverse_aux ) { return sc ; }
      return m_inverse_aux -> i_clear () ; 
    }
    /// rebuild ALL relations from ALL object to ALL objects (fast,100% inline)
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
      { i_push( entry->from() , entry->to() , entry->weight() )  ; }
      // (re)sort
      i_sort() ;
      //
      return StatusCode::SUCCESS ;
    }
    /** make the relation between 2 objects (fast,100% inline)
     *  subsequent call for i_sort is mandatory! 
     */
    inline   void i_push ( From_ object1 , To_ object2 , Weight_ weight  ) 
    {
      m_direct.i_push( object1 , object2 , weight );
      if ( 0 != m_inverse_aux ) 
      { m_inverse_aux ->i_push( object2 , object1 , weight ) ; }
    }
    /** (re)sort of the table 
     *   mandatory to use after i_push 
     */
    inline void i_sort() 
    { 
      m_direct.i_sort() ; 
      if ( 0 != m_inverse_aux ) { m_inverse_aux -> i_sort() ; } 
    }
    // ========================================================================
  public: // "merge"
    // ========================================================================
    /** merge with the sorted range of relations 
     *  @attention the range is assumed to be sorted! 
     *  @param range the range to be added 
     *  @return self-reference 
     */
    RelationWeighted& merge ( const Range& range ) 
    {
      if ( range.empty() ) { return *this ; }
      m_direct.merge ( range ) ;
      if ( 0 != m_inverse_aux ) { m_inverse_aux -> imerge ( range ) ; }
      return *this ;
    }
    /** merge with the sorted range of relations 
     *  @attention the range is assumed to be sorted! 
     *  @param range the range to be added 
     *  @return self-reference 
     */
    RelationWeighted& imerge ( const typename IInverse::Range& range ) 
    {
      if ( range.empty() ) { return *this ; }
      m_direct.imerge ( range ) ;
      if ( 0 != m_inverse_aux ) { m_inverse_aux -> merge ( range ) ; }
      return *this ;
    }
    /** merge with the sorted range of relations 
     *  @attention the range is assumed to be sorted! 
     *  @param range the range to be added 
     *  @return self-reference 
     */    
    RelationWeighted& operator+= ( const Range& range ) 
    { return merge ( range ) ; }
    // ========================================================================
  public:  // abstract methods from interface
    // ========================================================================
    /// retrive all relations from the object
    virtual Range relations ( From_ object) const 
    { return i_relations ( object ) ; }
    /// retrive ALL relations from ALL objects
    virtual Range relations() const { return i_relations () ; }    
    /** retrive all relations from the object which has weigth 
     *  larger/smaller than the threshold value 
     */
    virtual  Range      relations
    ( From_ object , Weight_ threshold , const bool flag ) const 
    { return i_relations ( object , threshold , flag ) ; }
    /** retrive all relations from the object which has weigth 
     *  withing the specified range 
     */
    virtual Range inRange ( From_ object , Weight_ low , Weight_ high ) const 
    { return i_inRange ( object , low , high ) ; }
    /// make the relation between 2 objects 
    virtual  StatusCode relate ( From_ object1 , To_ object2 , Weight_ weight  ) 
    { return i_relate( object1 , object2 , weight ) ; }
    /// add the entry 
    virtual  StatusCode add  ( const Entry& entry ) 
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
    virtual  StatusCode rebuild() { return i_rebuild() ;}
    /// update the object after POOL/ROOT reading 
    virtual StatusCode update() { return i_rebuild() ; }

    // ========================================================================
  public:
    // ========================================================================
    /// query the interface 
    virtual StatusCode queryInterface( const InterfaceID& iID , void** ppI )
    {
      if( 0 == ppI  )          { return StatusCode::FAILURE ; }   // RETURN !!!
      if( IInterface::interfaceID() == iID ) 
      { *ppI = static_cast<IInterface*> ( this ); }
      else if( IBase::interfaceID() == iID ) 
      { *ppI = static_cast<IBase*>      ( this ); }
      else                     { return StatusCode::FAILURE ; }   //  RETURN !!!
      ///
      addRef() ;
      return StatusCode::SUCCESS ;
    }
    /// increase the reference counter
    virtual unsigned long addRef  () { return 1 ; }   
    /// release the reference counter
    virtual unsigned long release () { return 1 ; }    
    /// get the pointer to direct table 
    inline Direct*      directBase () { return &m_direct ; }
    /// get the reference to direct table 
    const  Direct&     _direct     () const { return m_direct ; }    
    /** set new inverse table 
     *  @attention the method is not for public usage !!!
     */
    inline void setInverseBase( Inverse* inverse ) { m_inverse_aux = inverse ; }
    /// reserve the relations (for efficiency reasons)
    inline StatusCode reserve ( const size_t num ) 
    {
      if ( 0 != m_inverse_aux ) { m_inverse_aux -> i_reserve( num ) ; }
      return m_direct.i_reserve( num ) ;
    }
    // ========================================================================
  private:
    // ========================================================================
    /// assignement operator is private!
    RelationWeighted& operator= ( const RelationWeighted& copy  );    
    // ========================================================================
  private:
    // ========================================================================
    /// the actual store of relations 
    Direct   m_direct      ;                   // the actual store of relations 
    /// the pointer to inverse table
    Inverse* m_inverse_aux ;                    // the pointer to inverse table
    // ========================================================================
  };
  // ==========================================================================
} // end of namespace Relations
// ============================================================================
// The End 
// ============================================================================
#endif // RELATIONS_RelationWeighted_H
// ============================================================================
