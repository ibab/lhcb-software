// $Id: RelationBase.h,v 1.4 2005-01-27 14:48:48 cattanem Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ ; version $Revision: 1.4 $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2005/01/27 06:54:03  ibelyaev
//  std::sort -> std::table_sort
//
// Revision 1.2  2005/01/26 16:27:29  ibelyaev
//  add 'power input' option to speed-up the filling
//
// ============================================================================
#ifndef RELATIONS_RELATIONBASE_H 
#define RELATIONS_RELATIONBASE_H 1
// Include files
#include "Relations/PragmaWarnings.h"
// STD & STL 
#include <algorithm>
#include <functional>
// GaudiKernel
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/StatusCode.h"
// Relation
#include "Relations/RelationTypeTraits.h"
#include "Relations/Reserve.h"

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
  class RelationBase
  {  
  public:
    
    /// short cut to own type 
    typedef RelationBase<FROM,TO>                  OwnType     ;
    /// short cut to type of inverse relations 
    typedef RelationBase<TO,FROM>                  InvType     ;
    /// shortcut to type traits structure 
    typedef Relations::RelationTypeTraits<FROM,TO> TypeTraits  ;
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
                               m_less1           ) ;};
    
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
      // look for existing relations 
      const Entry ent ( object1 , object2 ) ;
      iterator it = std::lower_bound( m_entries.begin () , 
                                      m_entries.end   () , ent , m_less ) ;
      // the relation does exist ! 
      if( m_entries.end () != it && !m_less( ent , *it ) ) 
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
      // look for existing relations 
      const Entry ent ( object1 , object2 );
      iterator it  = std::lower_bound( m_entries.begin() , 
                                       m_entries.end  () , ent , m_less ) ;
      // the relation does not exist ! 
      if( m_entries.end() == it || m_less( ent , *it ) ) 
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
      iterator it = 
        std::remove_if( m_entries.begin () , 
                        m_entries.end   () , 
                        std::bind2nd( m_equal , Entry( From() , object ) ) ) ;
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
    { std::stable_sort( m_entries.begin() , m_entries.end() , m_less ) ; };
    
    /** standard/default constructor
     *  @param reserve size of preallocated reserved space
     */ 
    RelationBase
    ( const size_type reserve = 0 ) 
      : m_entries () 
      , m_less    ()
      , m_less1   ()
      , m_equal   ()
    { if( 0 < reserve ) { i_reserve( reserve ) ; } ; };
    
    /** copy constructor
     *  @param copy object to be copied  
     */
    RelationBase
    ( const OwnType& copy ) 
      : m_entries ( copy.m_entries ) 
      , m_less    ()
      , m_less1   ()
      , m_equal   ()
    {} ;
    
    /** constructor from the inverse type! 
     *  @attention it is indeed the most effective way to 
     *             get the inverse relations!
     *  @param inv object to be inverted
     *  @param int artificial agument to make the difference 
     *         for stupid MicroSoft compiler
     */
    RelationBase
    (  const InvType&    inv     , 
       const int      /* flag */ ) 
      : m_entries ()
      , m_less    ()
      , m_less1   ()
      , m_equal   ()
    {
      // get all relations from "inv"
      typename InvType::IP ip = inv.i_relations() ;
      // reserve the space for relations
      m_entries.reserve( ip.second - ip.first  );
      // invert all relations    
      for( typename InvType::CIT it = ip.first ; ip.second != it ; ++it ) 
        { i_relate( it->to() , it->from() ) ;  }
    };
    
  private:
    
    // the actual storage of references 
    mutable Entries m_entries ; ///< the actual storage of references 
    
    // comparison criteria for sorting 
    Less            m_less  ; ///< comparison criteria for sorting 
    // comparison criteria ( "less" by "From" field )
    Less1           m_less1 ; ///< comparison criteria ( "less" by "From" )
    // equality criteria   ( "equal" by "To" field ) 
    Equal           m_equal ; ///< equality criteria   ( "equal" by "To"  )
    
  };
  
}; // End of name space Relations 


// ============================================================================
// The End
// ============================================================================
#endif // RELATIONS_RELATIONBASE_H
// ============================================================================
