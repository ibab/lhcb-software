// $Id: RelationBase.h,v 1.1 2002-03-18 19:32:18 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef RELATIONS_RELATIONBASE_H 
#define RELATIONS_RELATIONBASE_H 1
// Include files
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
    typedef TypeTraits::From                       From        ;
    /// actual "TO"   type 
    typedef TypeTraits::To                         To          ;  
    /// Entry type  
    typedef TypeTraits::Entry                      Entry       ;
    /// container  type  
    typedef TypeTraits::Entries                    Entries     ;
    /// iterator type (internal)
    typedef Entries::iterator                      iterator    ;
    /// iterator type (internal)   
    typedef TypeTraits::IT                         IT          ;
    /// iterator pair type type (internal)   
    typedef TypeTraits::IP                         IP          ;
    /// const iterator type (internal)   
    typedef TypeTraits::CIT                        CIT         ;
    /// size_type 
    typedef Entries::size_type                     size_type   ;
    
  protected: 
    
    /// comparison criteria for sorting 
    typedef TypeTraits::Less                       Less        ;
    /// comparion criteria ( "less" by "From" field )
    typedef TypeTraits::LessByFrom                 Less1       ;
    /// equality criteria  ( "equal" by "To" field ) 
    typedef TypeTraits::EqualByTo                  Equal       ;
    
  public:
    
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
    { return std::equal_range( begin() , end() , Entry( object ) , Less1() ) ;};
    
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
      Less  cmp;
      iterator it = std::lower_bound( begin() , end() , ent , cmp ) ;
      // the relation does exist ! 
      if( end () != it && !cmp( ent , *it ) ) { return StatusCode::FAILURE; }
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
      Less  cmp ;
      iterator it  = std::lower_bound( begin() , end() , ent , cmp ) ;
      // the relation does not exist ! 
      if( end() == it || cmp( ent , *it ) ) { return StatusCode::FAILURE ; }
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
    inline  StatusCode i_remove 
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
    inline  StatusCode i_remove 
    ( const To&        object  ) 
    {
      // use the predicate "Equal"
      iterator it = 
        std::remove_if( begin() , end() , 
                        std::bind2nd( Equal() , Entry( From() , object ) ) ) ;
      // no relations are found!
      if( end() == it ) { return StatusCode::FAILURE ; }          // RETURN !!!
      // erase the relations 
      m_entries.erase( it , end() ) ;
      return StatusCode::SUCCESS ;
    };
    
    /// standard/default constructor 
    RelationBase( const size_type reserve = 0 ) 
      : m_entries() 
    { if( reserve ) { Relations::reserve( m_entries , reserve ) ; } ; };
    
    /// destructor (virtual)
    virtual ~RelationBase() { m_entries.clear(); }
    
    /** copy constructor
     *  @param copy object to be copied  
     */
    RelationBase( const OwnType& copy ) : m_entries( copy.m_entries ) {} ;
    
    /** constructor from the inverse type! 
     *  @attention it is indeed the effective way to get the inverse relations!
     *  @param inv object to be inversed!
     */
    RelationBase(  const InvType& inv ) : m_entries()
    {
      // reserve the space for relations
      m_entries.reserve( inv.end() - inv.begin() );
      /// invert the relations    
      for( InvType::CIT it = inv.begin() ; inv.end() != it ; ++it ) 
        { i_relate( it->to() , it->from() ) ;  }
    };
    
    /// accessor to "begin" iterator  (const     version)
    inline iterator begin ()  const { return m_entries.begin () ; }
    /// accessor to "begin" iterator  (non-const version)
    inline iterator begin ()        { return m_entries.begin () ; }
    /// accessor to "end"   iterator  (const     version)
    inline iterator end   ()  const { return m_entries.end   () ; }
    /// accessor to "end"   iterator  (non-const version)
    inline iterator end   ()        { return m_entries.end   () ; }
    
    /** accessor to the container of entries 
     *  @return reference to the whole store of relations  
     */
    inline       Entries& i_entries ()        { return m_entries ; }
    
    /** accessor to the container of entries (const version)
     *  @return reference to the whole store of relations  
     */
    inline const Entries& i_entries () const  { return m_entries ; }
    
  private:
    
    mutable Entries m_entries ;
    
  };
  
}; // End of name space Relations 


// ============================================================================
// The End
// ============================================================================
#endif // RELATIONS_RELATIONBASE_H
// ============================================================================
