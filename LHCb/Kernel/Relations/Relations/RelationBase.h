// $Id: RelationBase.h,v 1.7 2006-02-07 09:22:24 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ ; version $Revision: 1.7 $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef RELATIONS_RELATIONBASE_H 
#define RELATIONS_RELATIONBASE_H 1
// ============================================================================
// Include files
// ============================================================================
#include "Relations/PragmaWarnings.h"
// ============================================================================
// STD & STL 
// ============================================================================
#include <algorithm>
#include <functional>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/StatusCode.h"
// ============================================================================
// Relation
// ============================================================================
#include "Relations/RelationTypeTraits.h"
#include "Relations/IRelation.h"
#include "Relations/Reserve.h"
// ============================================================================

namespace Relations
{
  /** @class RelationBase RelationBase.h Relations/RelationBase.h
   *  
   *  Implementation of ordinary unidirectional relation 
   *
   *  @warning for the current implementation the actual type of 
   *  "FROM" should differ from the actual type of "TO"
   *  
   *  @warning users should not deal with this class directly!
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   27/01/2002
   */  
  template<class FROM,class TO>
  class RelationBase : BaseTable 
  {  
  public:
    
    /// short cut to own type 
    typedef RelationBase<FROM,TO>                           OwnType     ;
    /// short cut to type of inverse relations 
    typedef RelationBase<TO,FROM>                           InvType     ;
    /// shortcut to type traits structure 
    typedef Relations::RelationTypeTraits<FROM,TO>          TypeTraits  ;
    /// shortcut for "direct" interface 
    typedef IRelation<FROM,TO>                              IDirect     ;
    /// shortcut for "inverse" interface 
    typedef IRelation<TO,FROM>                              IInverse    ;    
    /// actual "FROM" type 
    typedef typename TypeTraits::From                       From        ;
    /// actual "TO"   type 
    typedef typename TypeTraits::To                         To          ;  
    /// Entry type  
    typedef typename TypeTraits::Entry                      Entry       ;
    /// container  type  
    typedef typename TypeTraits::Entries                    Entries     ;
    /// iterator type (internal)
    typedef typename Entries::iterator                      iterator    ;
    /// iterator type (internal)   
    typedef typename TypeTraits::IT                         IT          ;
    /// iterator pair type type (internal)   
    typedef typename TypeTraits::IP                         IP          ;
    /// const iterator type (internal)   
    typedef typename TypeTraits::CIT                        CIT         ;
    /// size_type 
    typedef typename Entries::size_type                     size_type   ;
    
  protected: 
    
    /// comparison criteria for sorting 
    typedef typename TypeTraits::Less                       Less        ;
    /// comparison criteria ( "less" by "From" field )
    typedef typename TypeTraits::LessByFrom                 Less1       ;
    /// equality criteria   ( "equal" by "To" field ) 
    typedef typename TypeTraits::EqualByTo                  Equal       ;
    
  public:
    
    /** retrive all relations 
     *
     *  @see IRelation 
     *  @return the pair of iterators for output relations   
     */
    inline  IP    i_relations () const 
    { return IP( m_entries.begin() , m_entries.end() ) ;};
    
    /** retrive all relations from the object
     *
     *   - the CPU performance is proportional to log(N), 
     *     where N is the total number of relations
     *
     *  @see IRelation 
     *  @param  object  smart reference to the object
     *  @return the pair of iterators for output relations   
     */
    inline  IP    i_relations
    ( const From& object      ) const
    { return std::equal_range( m_entries.begin() , 
                               m_entries.end  () , 
                               Entry ( object  ) , 
                               Less1()           ) ;};
    
    /** make the relation between 2 objects
     * 
     *   - StatusCode::FAILURE is returned if the relation
     *     between the given objects  is already set
     *
     *   - the CPU performance is proportional to log(N)
     *     for location of the object plus some overhead for 
     *     list operation, which is more or less constant for 
     *     std::deque implementation of the underlying relation 
     *     store and proportional to N for std::vector implementation, 
     *     where N is the total number of relations 
     *
     *  @see IRelation 
     *  @see RelationTypeTraits
     *  @param object1  the first object
     *  @param object2  the second object 
     *  @return status code 
     */
    inline  StatusCode i_relate 
    ( const From&      object1 , 
      const To&        object2 ) 
    {
      Less _less_ = Less() ;
      // look for existing relations 
      const Entry ent ( object1 , object2 ) ;
      iterator it = std::lower_bound( m_entries.begin () , 
                                      m_entries.end   () , ent , _less_ ) ;
      // the relation does exist ! 
      if( m_entries.end () != it && !_less_( ent , *it ) ) 
        { return StatusCode::FAILURE; }
      // insert new relation !
      m_entries.insert( it , ent ) ;
      return StatusCode::SUCCESS ;
    };
    
    /** remove the concrete relation between objects
     *
     *   - StatusCode::FAILURE is returned if the relation
     *     between the given objects  is already set
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
     *  @see RelationTypeTraits
     *  @param object1  the first object
     *  @param object2  to the second object 
     *  @return status code 
     */
    inline  StatusCode i_remove 
    ( const From&      object1 , 
      const To&        object2 ) 
    {
      const Less _less = Less() ;
      // look for existing relations 
      const Entry ent ( object1 , object2 );
      iterator it  = std::lower_bound( m_entries.begin() , 
                                       m_entries.end  () , ent , _less ) ;
      // the relation does not exist ! 
      if( m_entries.end() == it || _less( ent , *it ) ) 
        { return StatusCode::FAILURE ; }
      // remove existing relation     
      m_entries.erase( it );
      return StatusCode::SUCCESS ; 
    };
    
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
     *  @see    IRelation 
     *  @param  object  to the object
     *  @return status code 
     */
    inline  StatusCode i_removeFrom 
    ( const From&      object  )
    {
      // look for all existing relations from the given object  
      IP ip  = i_relations( object ) ;
      // there are no relations 
      if( ip.second == ip.first ) { return StatusCode::FAILURE ; }// RETURN !!!
      // erase relations 
      m_entries.erase( ip.first , ip.second ) ;
      return StatusCode::SUCCESS ;
    };
    
    /** remove all relations TO the defined object
     *  @attention the method is relatively "slow"
     *
     *   - StatusCode::FAILURE is returned if there are no relations
     *     from the given object
     *
     *   - the CPU performance is proportional to 
     *     the total number of relations 
     *
     *  @see    IRelation 
     *  @param  object the object
     *  @return status code 
     */
    inline  StatusCode i_removeTo 
    ( const To&        object  ) 
    {
      // use the predicate "Equal"
      iterator it = std::remove_if
        ( m_entries.begin () , 
          m_entries.end   () , 
          std::bind2nd( Equal() , Entry( From() , object ) ) ) ;
      // no relations are found!
      if( m_entries.end() == it ) { return StatusCode::FAILURE ; }// RETURN !!!
      // erase the relations 
      m_entries.erase( it , m_entries.end() ) ;
      return StatusCode::SUCCESS ;
    };

    /** remove ALL relations from ALL  object to ALL objects
     *
     *  @see    IRelation 
     *  @return status code 
     */
    inline  StatusCode i_clear() 
    { m_entries.clear() ; return StatusCode::SUCCESS ; }

    /** reserve the space for relations 
     *  @param   num number of relation to be reserved 
     *  @return  status code 
     */
    inline StatusCode i_reserve ( const size_t num  )
    { m_entries.reserve( num ) ; return StatusCode::SUCCESS ; }


    /** make the relation between 2 objects
     * 
     *   - StatusCode::FAILURE is returned if the relation
     *     between the given objects  is already set
     *
     *   - the CPU performance is proportional to log(N)
     *     for location of the object plus some overhead for 
     *     list operation, which is more or less constant for 
     *     std::deque implementation of the underlying relation 
     *     store and proportional to N for std::vector implementation, 
     *     where N is the total number of relations 
     *
     *  @see IRelation 
     *  @see RelationTypeTraits
     *  @param object1  the first object
     *  @param object2  the second object 
     *  @return status code 
     */
    inline  void       i_push   
    ( const From&      object1 , 
      const To&        object2 ) 
    { m_entries.push_back( Entry( object1 , object2 ) ) ; }
    
    /** (re)sort the whole underlying container 
     *  Call for this method is MANDATORY after usage of i_push 
     */ 
    inline void i_sort() 
    { std::stable_sort( m_entries.begin() , m_entries.end() , Less() ) ; };
    
    /** standard/default constructor
     *  @param reserve size of preallocated reserved space
     */ 
    RelationBase
    ( const size_type reserve = 0 ) 
      : BaseTable () 
      , m_entries () 
    { if ( 0 < reserve ) { i_reserve( reserve ) ; } ; };
    
    /** constructor from any "direct" interface 
     *  @param copy object to be copied  
     */
    RelationBase
    ( const IDirect& copy ) 
      : BaseTable () 
      , m_entries () 
    {
      typename IDirect::Range r = copy.relations() ;
      m_entries.insert ( m_entries.end() , r.begin() , r.end() ) ;
    } ;
    
    /** constructor from any "inverse" interface  
     *  @param inv object to be inverted
     *  @param int artificial agument to make the difference 
     *         for stupid MicroSoft compiler
     */
    RelationBase
    (  const IInverse&   inv     , 
       const int      /* flag */ ) 
      : BaseTable ()  
      , m_entries ()
    {
      // get all relations from "inv"
      typename IInverse::Range r = inv.relations() ;
      // reserve the space for relations
      i_reserve ( r.size() );
      // invert all relations    
      for ( typename IInverse::iterator entry = r.begin() ; 
            r.end() != entry ; ++entry )
      { i_push ( entry->to() , entry->from() ) ;  }
      // final sort 
      i_sort() ;      
    };
    
    /** copy constructor
     *  @param copy object to be copied  
     */
    RelationBase
    ( const OwnType& copy ) 
      : BaseTable ( copy ) 
      , m_entries ( copy.m_entries ) 
    {} ;
    
  private:
    
    // the actual storage of references 
    mutable Entries m_entries ; ///< the actual storage of references 
    
  };
  
}; // End of name space Relations 


// ============================================================================
// The End
// ============================================================================
#endif // RELATIONS_RELATIONBASE_H
// ============================================================================
