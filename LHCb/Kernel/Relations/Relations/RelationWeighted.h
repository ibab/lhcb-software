// $Id: RelationWeighted.h,v 1.5 2005-03-14 09:47:14 cattanem Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2005/02/16 19:59:35  ibelyaev
//  few minor fixes to enable 'lcgdict' processing
//
// ============================================================================
#ifndef RELATIONS_RelationWeighted_H 
#define RELATIONS_RelationWeighted_H 1
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
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/MsgStream.h"
// ============================================================================
// From Relation
// ============================================================================
#include "Relations/RelationUtils.h"
#include "Relations/IRelationWeighted.h"
#include "Relations/RelationWeightedBase.h"
// ============================================================================

namespace Relations
{
  
/** @class RelationWeighted RelationWeighted.h Relations/RelationWeighted.h
 * 
 *  The simplest implementation of unidirectional weighted relations.
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date   28/01/2002
 */
  template<class FROM, class TO, class WEIGHT>
  class RelationWeighted :
    public IRelationWeighted<FROM,TO,WEIGHT> 
  {
    
  public:
    
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
    typedef typename IBase::To                              To      ;
    /// import basic types from Interface 
    typedef typename IBase::Weight                          Weight  ;
 
  public:
    
    /// the standard/default constructor
    RelationWeighted 
    ( const size_t reserve = 0 ) 
      : IBase     (         ) 
      , m_direct  ( reserve ) 
      , m_inverse ( 0       ) 
    {};
    
    /** constructor from arbitrary "direct" interface 
     *  @param copy object to be copied
     */
    RelationWeighted 
    ( const IDirect& copy ) 
      : IBase() 
      , m_direct  ( copy ) 
      , m_inverse ( 0    ) 
    {} ;
    
    /** constructor from "inverse interface"
     *  It is an efficient way to "invert" relation 
     *  @param inv interface to be inverted 
     *  @param flag artificial argument, to distinguish from copy constructor
     */
    RelationWeighted
    ( const IInverse&    inv     , 
      const int          flag  ) 
      : IBase     (   ) 
      , m_direct  ( inv , flag ) 
      , m_inverse ( 0 )  
    {};
    
    /** copy constructor is publc, 
     *  but it is not recommended for direct usage 
     *  @param copy object to be copied 
     */
    RelationWeighted 
    ( const OwnType& copy   ) 
      : IBase     ( copy          ) 
      , m_direct  ( copy.m_direct )
      , m_inverse ( 0             ) 
    {};
    
    /// destructor (virtual)
    virtual ~RelationWeighted () {} ;
    
  public:  // major functional methods (fast, 100% inline)
    
    /// retrive all relations from the object (fast,100% inline)
    inline  Range i_relations
    ( const From& object) const
    {
      typename Base::IP ip = m_direct.i_relations( object );
      return Range(ip.first, ip.second);
    };
    
    /// retrive ALL relations from ALL objects (fast,100% inline)
    inline  Range i_relations() const
    {
      typename Base::IP ip = m_direct.i_relations();
      return Range(ip.first, ip.second);
    };
    
    /// retrive all relations from the object (fast,100% inline)
    inline   Range      i_relations
    ( const  From&      object,
      const  Weight&    threshold ,
      const  bool       flag      ) const 
    {
      typename Base::IP ip = m_direct.i_relations( object , threshold , flag );
      return Range(ip.first, ip.second);
    };
    
    /// make the relation between 2 objects (fast,100% inline)
    inline   StatusCode i_relate 
    ( const  From&      object1 , 
      const  To&        object2 ,
      const  Weight&    weight  ) 
    {
      StatusCode sc = m_direct.  i_relate ( object1 , object2 , weight );
      if( sc.isFailure() || 0 == m_inverse ) { return sc ; }
      return          m_inverse->i_relate ( object2 , object1 , weight ); 
    };

    /// remove the concrete relation between objects (fast,100% inline)
    inline   StatusCode i_remove 
    ( const  From&      object1 , 
      const  To&        object2 ) 
    { 
      StatusCode sc = m_direct.  i_remove ( object1 , object2 ) ; 
      if( sc.isFailure() || 0 == m_inverse ) { return sc ; }
      return          m_inverse->i_remove ( object2 , object1 ) ; 
    };
    
    /// remove all relations FROM the defined object (fast,100% inline)
    inline   StatusCode i_removeFrom 
    ( const  From&      object )
    { 
      StatusCode sc = m_direct.  i_removeFrom ( object ) ; 
      if( sc.isFailure() || 0 == m_inverse ) { return sc ; }
      return          m_inverse->i_removeTo   ( object ) ; 
    };

    /// remove all relations TO the defined object (fast,100% inline)
    inline   StatusCode i_removeTo 
    ( const  To&        object )
    { 
      StatusCode sc = m_direct.  i_removeTo   ( object ) ; 
      if( sc.isFailure() || 0 == m_inverse ) { return sc ; }
      return          m_inverse->i_removeFrom ( object ) ; 
    };
    
    /// filter out the relations FROM the defined object (fast,100% inline)
    inline   StatusCode i_filterFrom 
    ( const  From&      object    ,
      const  Weight&    threshold ,
      const  bool       flag      )  
    { 
      StatusCode sc = m_direct.  i_filterFrom ( object , threshold , flag ) ; 
      if( sc.isFailure() || 0 == m_inverse ) { return sc ; }
      return          m_inverse->i_filterTo   ( object , threshold , flag ) ; 
    };
    
    /// filter out the relations TO the defined object (fast,100% inline)
    inline   StatusCode i_filterTo 
    ( const  To&        object    ,
      const  Weight&    threshold ,
      const  bool       flag      )  
    { 
      StatusCode sc =  m_direct.  i_filterTo   ( object , threshold , flag ) ; 
      if( sc.isFailure() || 0 == m_inverse ) { return sc ; }
      return           m_inverse->i_filterFrom ( object , threshold , flag ) ; 
    };
    
    /// filter out all relations (fast,100% inline)
    inline   StatusCode i_filter 
    ( const  Weight&    threshold ,
      const  bool       flag      )  
    { 
      StatusCode sc =  m_direct.  i_filter( threshold , flag ) ; 
      if( sc.isFailure() || 0 == m_inverse ) { return sc ; }
      return           m_inverse->i_filter( threshold , flag ) ; 
    };
    
    /// remove ALL relations from ALL objects to ALL objects (fast,100% inline)
    inline  StatusCode i_clear () 
    { 
      StatusCode sc =  m_direct.  i_clear () ; 
      if( sc.isFailure() || 0 == m_inverse ) { return sc ; }
      return           m_inverse->i_clear () ; 
    };

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
      StatusCode sc =  i_clear()  ; if( sc.isFailure() ) { return sc ; }
      // 4) reserve space for new relations 
      sc = reserve( _e.size() )   ; if( sc.isFailure() ) { return sc ; }
      // 5) build new relations 
      for ( typename _Entries::const_iterator entry = _e.begin() ; _e.end() != entry ; ++entry )
      { i_push( entry->from() , entry->to() , entry->weight() )  ; }
      // (re)sort
      i_sort() ;
      //
      return StatusCode::SUCCESS ;
    };
    
    /** make the relation between 2 objects (fast,100% inline)
     *  subsequent call for i_sort is mandatory! 
     */
    inline   void i_push 
    ( const  From&      object1 , 
      const  To&        object2 ,
      const  Weight&    weight  ) 
    {
      m_direct.i_push( object1 , object2 , weight );
      if ( 0 != m_inverse ) { m_inverse->i_push( object2 , object1 , weight ) ; }
    };
    
    /** (re)sort of the table 
     *   mandatory to use after i_push 
     */
    inline void i_sort() 
    { m_direct.i_sort() ; if ( 0 != m_inverse ) { m_inverse->i_sort() ; } }
    
    
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
     *  @see    IRelationWeighted1D
     *  @see    IInterface 
     *  @param  iID interface identifier 
     *  @param  ppI placeholder 
     *  @return status code
     */
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

    /** get the reference to direct table 
     *  @attention the method is not for public usage !!!
     *  @return reference to direct base 
     */
    const  Direct&     _direct     () const { return m_direct ; }
    
    /** set new inverse table 
     *  @attention the method is not for public usage !!!
     */
    inline void    setInverseBase( Inverse* inverse ) { m_inverse = inverse ; }
    
    /** reserve the relations (for efficiency reasons)
     *  @param num number of relations to be reserved
     *  @return status code
     */
    inline StatusCode reserve ( const size_t num ) 
    {
      if( 0 != m_inverse ) { m_inverse->i_reserve( num ) ; }
      return m_direct.i_reserve( num ) ;
    };
    
  private:
    
    /** assignement operator is private!
     *  @param copy object to be copied 
     */
    RelationWeighted& operator= ( const OwnType& copy  );
    
    
  private:
    
    Direct   m_direct  ; ///< the actual relation store
    Inverse* m_inverse ; /// the pointer to inverse table 
    
  };
  

}; // end of namespace Relations 


// ============================================================================
// The End 
// ============================================================================
#endif // RELATIONS_RelationWeighted_H
// ============================================================================
