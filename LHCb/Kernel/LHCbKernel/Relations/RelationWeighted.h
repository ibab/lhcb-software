// $Id: RelationWeighted.h,v 1.5 2003-01-22 11:29:16 sponce Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2003/01/17 14:07:01  sponce
// support for gcc 3.2
//
// Revision 1.3  2002/07/25 15:32:14  ibelyaev
//  bug fix in destructors of relation objects
//
// Revision 1.2  2002/04/25 08:44:04  ibelyaev
//  bug fix for Win2K
//
// Revision 1.1  2002/04/03 15:35:18  ibelyaev
// essential update and redesing of all 'Relations' stuff
// 
// ============================================================================
#ifndef RELATIONS_RelationWeighted_H 
#define RELATIONS_RelationWeighted_H 1
// Include files
#include "Relations/PragmaWarnings.h"
// STD & STL 
#include <algorithm>
// from Gaudi
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/MsgStream.h"
// From Relation
#include "Relations/RelationUtils.h"
#include "Relations/IRelationWeighted.h"
#include "Relations/RelationWeightedBase.h"

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
    public virtual IRelationWeighted<FROM,TO,WEIGHT> 
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
 
  public:
    
    /// the standard/default constructor
    RelationWeighted ( const size_t reserve = 0 ) 
      : m_direct  ( reserve ) 
      , m_inverse ( 0       ) 
    {};
    
    /** constructor from inverse object 
     *  @param inv  relation object to be inverted 
     *  @param flag artificial parameter to distinguisch 
     *              fromcopy constructor 
     */
    RelationWeighted( const InvType& inv , int flag ) 
      : m_direct( inv.m_direct , flag ) 
      , m_inverse ( 0 ) 
    {};

    /** constructor from inverse interface
     *  @param inv  relation object to be inverted 
     *  @param flag artificial parameter to distinguisch 
     *              fromcopy constructor 
     */
    RelationWeighted( const IInverse & inv , int /* flag */ ) 
      : m_direct  (   ) 
      , m_inverse ( 0 ) 
    {
      // get all relations 
      typename IInverse::Range range = inv.relations() ;
      // invert all relations 
      for( typename IInverse::iterator entry = range.begin() ;
           range.end() != entry ; ++entry )
        { relate( entry->to() , entry->from() , entry->weight() ); }
    };
    
    /// destructor (virtual)
    virtual ~RelationWeighted () {} ;
    
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
    virtual typename RelationWeighted<FROM, TO, WEIGHT>::Range relations
    ( const typename RelationWeighted<FROM, TO, WEIGHT>::From& object) const
    {
      typedef typename RelationWeighted<FROM, TO, WEIGHT>::Range localRange;
      typename Base::IP ip = m_direct.i_relations( object );
      return localRange(ip.first, ip.second);
    };
    
    /** retrive ALL relations from ALL objects
     *  
     *  @see    IRelationWeighted1D 
     *  @see    RelationWeighted1DBase
     *  @see    RelationWeighted1DTypeTraits
     *  @param  object  the object
     *  @return pair of iterators for output relations   
     */
    virtual typename RelationWeighted<FROM, TO, WEIGHT>::Range relations() const
    {
      typedef typename RelationWeighted<FROM, TO, WEIGHT>::Range localRange;
      typename Base::IP ip = m_direct.i_relations();
      return localRange(ip.first, ip.second);
    };
    
    /** retrive all relations from the object which has weigth 
     *  larger/smaller than the threshold value 
     *  @param  object    the object
     *  @param  threshold threshold value for the weight 
     *  @param  flag      flag for larger/smaller
     *  @return pair of iterators for output relations   
     */
    virtual  typename RelationWeighted<FROM, TO, WEIGHT>::Range      relations
    ( const  typename RelationWeighted<FROM, TO, WEIGHT>::From&      object,
      const  typename RelationWeighted<FROM, TO, WEIGHT>::Weight& threshold ,
      const  bool       flag      ) const 
    {
      typedef typename RelationWeighted<FROM, TO, WEIGHT>::Range localRange;
      typename Base::IP ip = m_direct.i_relations( object , threshold , flag );
      return localRange(ip.first, ip.second);
    };

    /** make the relation between 2 objects 
     *  @param  object1 the first object
     *  @param  object2 the second object 
     *  @param  weight  the weigth for the relation 
     *  @return status  code 
     */
    virtual  StatusCode relate 
    ( const  typename RelationWeighted<FROM, TO, WEIGHT>::From&      object1 , 
      const  typename RelationWeighted<FROM, TO, WEIGHT>::To&        object2 ,
      const  typename RelationWeighted<FROM, TO, WEIGHT>::Weight&    weight  ) 
    {
      StatusCode sc = m_direct.  i_relate ( object1 , object2 , weight );
      if( sc.isFailure() || 0 == m_inverse ) { return sc ; }
      return          m_inverse->i_relate ( object2 , object1 , weight ); 
    };

    /** remove the concrete relation between objects
     *  @param  object1 the first object
     *  @param  object2 the second object 
     *  @return status  code 
     */
    virtual  StatusCode remove 
    ( const  typename RelationWeighted<FROM, TO, WEIGHT>::From&      object1 , 
      const  typename RelationWeighted<FROM, TO, WEIGHT>::To&        object2 ) 
    { 
      StatusCode sc = m_direct.  i_remove ( object1 , object2 ) ; 
      if( sc.isFailure() || 0 == m_inverse ) { return sc ; }
      return          m_inverse->i_remove ( object2 , object1 ) ; 
    };
    
    /** remove all relations FROM the defined object
     *  @param  object  smart reference to the object
     *  @return status code 
     */
    virtual  StatusCode removeFrom 
    ( const  typename RelationWeighted<FROM, TO, WEIGHT>::From&      object )
    { 
      StatusCode sc = m_direct.  i_removeFrom ( object ) ; 
      if( sc.isFailure() || 0 == m_inverse ) { return sc ; }
      return          m_inverse->i_removeTo   ( object ) ; 
    };

    /** remove all relations TO the defined object
     *  @param object  smart reference to the object
     *  @return status code 
     */
    virtual  StatusCode removeTo 
    ( const  typename RelationWeighted<FROM, TO, WEIGHT>::To&        object )
    { 
      StatusCode sc = m_direct.  i_removeTo   ( object ) ; 
      if( sc.isFailure() || 0 == m_inverse ) { return sc ; }
      return          m_inverse->i_removeFrom ( object ) ; 
    };
    
    /** filter out the relations FROM the defined object, which
     *  have a weight larger(smaller)than the threshold weight 
     *  @param  object    the object
     *  @param  threshold threshold value for the weight 
     *  @param  flag      flag for larger/smaller
     *  @return status code 
     */
    virtual  StatusCode filterFrom 
    ( const  typename RelationWeighted<FROM, TO, WEIGHT>::From&      object    ,
      const  typename RelationWeighted<FROM, TO, WEIGHT>::Weight& threshold ,
      const  bool       flag      )  
    { 
      StatusCode sc = m_direct.  i_filterFrom ( object , threshold , flag ) ; 
      if( sc.isFailure() || 0 == m_inverse ) { return sc ; }
      return          m_inverse->i_filterTo   ( object , threshold , flag ) ; 
    };
    
    /** filter out the relations TO the defined object, which
     *  have a weight larger/smaller than the threshold weight 
     *  @param  object    the object
     *  @param  threshold threshold value for the weight 
     *  @param  flag      flag for larger/smaller
     *  @return status code 
     */
    virtual  StatusCode filterTo 
    ( const  typename RelationWeighted<FROM, TO, WEIGHT>::To&        object,
      const  typename RelationWeighted<FROM, TO, WEIGHT>::Weight& threshold ,
      const  bool       flag      )  
    { 
      StatusCode sc =  m_direct.  i_filterTo   ( object , threshold , flag ) ; 
      if( sc.isFailure() || 0 == m_inverse ) { return sc ; }
      return           m_inverse->i_filterFrom ( object , threshold , flag ) ; 
    };
    
    /** filter out all relations which
     *  have a weight larger/smaller than the threshold weight 
     *  @param  threshold  threshold value for the weight 
     *  @param  flag       flag for larger/smaller
     *  @return status code 
     */
    virtual  StatusCode filter 
    ( const  typename RelationWeighted<FROM, TO, WEIGHT>::Weight& threshold ,
      const  bool       flag      )  
    { 
      StatusCode sc =  m_direct.  i_filter( threshold , flag ) ; 
      if( sc.isFailure() || 0 == m_inverse ) { return sc ; }
      return           m_inverse->i_filter( threshold , flag ) ; 
    };

    /** remove ALL relations from ALL objects to ALL objects 
     *  @return status code 
     */
    virtual  StatusCode clear () 
    { 
      StatusCode sc =  m_direct.  i_clear () ; 
      if( sc.isFailure() || 0 == m_inverse ) { return sc ; }
      return           m_inverse->i_clear () ; 
    };
    
    /** query the interface 
     *  @see    IRelationWeighted1D
     *  @see    IInterface 
     *  @param  iid interface identifier 
     *  @param  ppi placeholder 
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
    
    /** set new inverse table 
     *  @attention the method is not for public usage !!!
     */
    inline void    setInverseBase( Inverse* inverse ) { m_inverse = inverse ; }
    
  private:
    
    /** copy constructor is private! 
     *  @param copy object to be copied 
     */
    RelationWeighted ( const OwnType& copy   ) 
      : m_direct  ( copy.m_direct )
      , m_inverse ( 0             ) 
    {}
    
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
