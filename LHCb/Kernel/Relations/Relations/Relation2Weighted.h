// $Id: Relation2Weighted.h,v 1.3 2005-02-16 19:59:35 ibelyaev Exp $
// =============================================================================
// CV Stag $Name: not supported by cvs2svn $
// =============================================================================
// $Log: not supported by cvs2svn $
// =============================================================================
#ifndef RELATIONS_Relation2Weighted_H 
#define RELATIONS_Relation2Weighted_H 1
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
#include "Relations/IRelationWeighted2D.h"
#include "Relations/RelationWeighted.h"
// =============================================================================

namespace Relations
{
  
  /** @class Relation2Weighted Relation2Weighted.h Relations/Relation2Weighted.h
   *  
   *  @brief Implementation of weighted bidirectional relations 
   * 
   *  Implementation of weighed bidirectional relations 
   *  from objects of type "FROM" to objects of type "TO" withweight "WEIGHT"
   *  
   *  Data types for "FROM", "TO" and "WEIGHT"
   *
   *   - either inherits from ContainedObject/DataObject 
   *   - or have the templated specializations through 
   *     ObjectTypeTraits structure with defined streamer operator 
   *     to/from StreamBuffer class 
   *  
   *  @see ObjectTypeTraits 
   *  @see StreamBuffer 
   *  @see IRelationWeihted
   *  @see IRelationWeihted2D
   *  @warning for the current implementation the actual type of 
   *           FROM should differ from the actual type of TO
   *  
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
   *  @date   23/03/2002
   */
  
  template<class FROM,class TO,class WEIGHT>
  class Relation2Weighted:
    public IRelationWeighted2D<FROM,TO,WEIGHT> 
  {  
  public:
    /// short cut for own     type
    typedef Relation2Weighted<FROM,TO,WEIGHT>               OwnType     ;
    /// short cut for inverse type
    typedef Relation2Weighted<TO,FROM,WEIGHT>               InvType     ;
    /// short cut for interface 
    typedef IRelationWeighted2D<FROM,TO,WEIGHT>             IBase       ;
    /// actual implementations of direct  type 
    typedef RelationWeighted<FROM,TO,WEIGHT>                Base        ;
    /// actual implementations of inverse type 
    typedef RelationWeighted<TO,FROM,WEIGHT>                InvBase     ;
    /// shortcut for direct base type 
    typedef Base                                            Direct      ;
    /// shortcut for inverse base type 
    typedef InvBase                                         Inverse     ;
    /// shortcut for direct subinterface 
    typedef typename IBase::DirectType                      DirectType  ;
    /// shortcut for inverse subinterface 
    typedef typename IBase::InverseType                     InverseType ;
    
    /// import basic types from Interface 
    typedef typename IBase::Range                           Range       ;
    /// import basic types from Interface 
    typedef typename IBase::From                            From        ;
    /// import basic types from Interface 
    typedef typename IBase::To                              To          ;
    /// import basic types from Interface 
    typedef typename IBase::Weight                          Weight      ;    
    
    /// the default constructor
    Relation2Weighted
    ( const size_t reserve = 0  )
      : IBase     (         )   
      , m_direct  ( reserve ) 
      , m_inverse ( reserve ) 
    {
      /// set cross-links 
      m_direct  .setInverseBase ( m_inverse .directBase () ) ;
      m_inverse .setInverseBase ( m_direct  .directBase () ) ;
    };
    
    /** constructor from any "direct" interface 
     *  @param copy object to be copied
     */
    Relation2Weighted 
    ( const DirectType& copy ) 
      : IBase     (          ) 
      , m_direct  ( copy     ) 
      , m_inverse ( copy , 1 ) 
    {
      /// set cross-links 
      m_direct  .setInverseBase ( m_inverse .directBase () ) ;
      m_inverse .setInverseBase ( m_direct  .directBase () ) ;
    }
    
    /** constructor from any "inverse" interface 
     *  @param copy object to be copied
     */
    Relation2Weighted
    ( const InverseType& copy , 
      const int          flag ) 
      : IBase     (             ) 
      , m_direct  ( copy , flag ) 
      , m_inverse ( copy        ) 
    {
      /// set cross-links 
      m_direct  .setInverseBase ( m_inverse .directBase () ) ;
      m_inverse .setInverseBase ( m_direct  .directBase () ) ;
    }
    
    /** copy constructor is publc, 
     *  @attention it is not recommended for normal usage 
     *  @param copy object to be copied 
     */
    Relation2Weighted 
    ( const OwnType& copy ) 
      : IBase     ( copy           ) 
      , m_direct  ( copy.m_direct  )
      , m_inverse ( copy.m_inverse ) 
    {
      /// set cross-links 
      m_direct  .setInverseBase ( m_inverse  .directBase () ) ;
      m_inverse .setInverseBase ( m_direct   .directBase () );
    };
    
    /// destructor (virtual)
    virtual ~Relation2Weighted() {} ;
    
  public:  // major functional methods (fast, 100% inline)
    
    /// retrive all relations from the object (fast,100% inline)
    inline  Range i_relations
    ( const From& object) const
    { return m_direct.i_relations( object ) ; }
    
    /// retrive ALL relations from ALL objects (fast,100% inline)
    inline  Range i_relations() const
    { return m_direct.i_relations() ; }
    
    /// retrive all relations from the object (fast,100% inline)
    inline   Range      i_relations
    ( const  From&      object,
      const  Weight&    threshold ,
      const  bool       flag      ) const 
    { return m_direct.i_relations( object, threshold , flag ) ; }
    
    /// make the relation between 2 objects (fast,100% inline)
    inline   StatusCode i_relate 
    ( const  From&      object1 , 
      const  To&        object2 ,
      const  Weight&    weight  ) 
    { return m_direct.i_relate ( object1 , object2 , weight ) ; }
    
    /// remove the concrete relation between objects (fast,100% inline)
    inline   StatusCode i_remove 
    ( const  From&      object1 , 
      const  To&        object2 ) 
    { return m_direct.i_remove ( object1 , object2 ) ; }
    
    /// remove all relations FROM the defined object (fast,100% inline)
    inline   StatusCode i_removeFrom 
    ( const  From&      object )
    { return m_direct.i_removeFrom ( object ) ; }
    
    /// remove all relations TO the defined object (fast,100% inline)
    inline   StatusCode i_removeTo 
    ( const  To&        object )
    { return m_direct.i_removeTo ( object ) ; }
    
    /// filter out the relations FROM the defined object (fast,100% inline)
    inline   StatusCode i_filterFrom 
    ( const  From&      object    ,
      const  Weight&    threshold ,
      const  bool       flag      )  
    { return m_direct.i_filterFrom ( object , threshold , flag ) ; }
    
    /// filter out the relations TO the defined object (fast,100% inline)
    inline   StatusCode i_filterTo 
    ( const  To&        object    ,
      const  Weight&    threshold ,
      const  bool       flag      )  
    { return m_direct.i_filterTo ( object , threshold , flag ) ; }
    
    /// filter out all relations (fast,100% inline)
    inline   StatusCode i_filter 
    ( const  Weight&    threshold ,
      const  bool       flag      )  
    { return m_direct.i_filter ( threshold , flag ) ; }
    
    /// remove ALL relations from ALL objects to ALL objects (fast,100% inline)
    inline  StatusCode i_clear () { return m_direct.i_clear() ; }

    /// rebuild ALL relations form ALL  object to ALL objects(fast,100% inline)
    inline  StatusCode i_rebuild() 
    { return m_direct.i_rebuild() ; };
    
    /** make the relation between 2 objects (fast,100% inline)
     *  call for i_sort() is mandatory!
     */
    inline   void i_push
    ( const  From&      object1 , 
      const  To&        object2 ,
      const  Weight&    weight  ) 
    { m_direct.i_push ( object1 , object2 , weight ) ; }
        
    /** (re)sort of the table 
     *   mandatory to use after i_push 
     */
    inline void i_sort() { m_direct.i_sort() ; }
    
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
    
    /** get the "direct" interface 
     *  @see IRelationWeighted2D
     *  @return pointer to the 'direct' interface 
     */
    virtual       DirectType*  direct ()        { return &m_direct ; }
    
    /** get the "direct" interface  (const-version)
     *  @see IRelationWeighted2D
     *  @return pointer to the 'direct' interface 
     */
    virtual const DirectType*  direct () const  { return &m_direct  ; }
    
    /** get the "inverse" interface 
     *  @see IRelationWeighted2D
     *  @return pointer to the 'inverse' interface 
     */
    virtual       InverseType* inverse ()       { return &m_inverse ; }
    
    /** get the "inverse" interface  (const version)
     *  @see IRelationWeighted2D
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
    
  public: // other methods 
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
    
      
    /** reserve the relations (for efficiency reasons)
     *  @param num number of relations to be reserved
     *  @return status code
     */
    inline StatusCode reserve ( const size_t num ) 
    { return m_direct.reserve( num ) ; };

  private:
    
    /** assignement operator is private!
     *  @param copy object to be copied 
     */
    Relation2Weighted& operator=( const OwnType& copy );
    
  private:
    
    Direct    m_direct  ;  ///< the direct table 
    Inverse   m_inverse ;  ///< the "inverse table"
    
  };
  
};



// =============================================================================
// The End 
// =============================================================================
#endif // RELATIONS_RELation2Weighted_H
// =============================================================================
