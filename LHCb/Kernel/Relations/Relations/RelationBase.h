// ============================================================================
#ifndef RELATIONS_RELATIONBASE_H 
#define RELATIONS_RELATIONBASE_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <algorithm>
#include <functional>
// ============================================================================
// GaudiKernel
// ============================================================================
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
  // ==========================================================================
  /** @class RelationBase Relations/RelationBase.h
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
    // ========================================================================
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
    typedef typename TypeTraits::From_                      From_       ;
    /// actual "TO"   type 
    typedef typename TypeTraits::To_                        To_         ;  
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
    /// the range   
    typedef typename TypeTraits::Range                      Range       ;
    /// size_type 
    typedef typename Entries::size_type                     size_type   ;
    // ========================================================================
  protected:
    // ========================================================================
    /// comparison criteria for sorting 
    typedef typename TypeTraits::Less                       Less        ;
    /// comparison criteria ( "less" by "From" field )
    typedef typename TypeTraits::LessByFrom                 Less1       ;
    /// equality criteria   ( "equal" by "To" field ) 
    typedef typename TypeTraits::EqualByTo                  Equal       ;
    // ========================================================================
  public:   
    // ========================================================================
    /// retrive all relations 
    inline  IP    i_relations () const 
    { return IP ( m_entries.begin() , m_entries.end() ) ; }
    /// retrive all relations from the object
    inline  IP    i_relations ( From_  object ) const
    { return std::equal_range ( m_entries.begin() , m_entries.end  () , 
                                Entry ( object  ) , Less1()           ) ; }
    /// make the relation between 2 objects
    inline  StatusCode i_relate ( From_ object1 , To_ object2 ) 
    {
      // look for existing relations 
      const Entry entry ( object1 , object2 ) ;
      return i_add ( entry ) ;
    }
    /// add the entry 
    inline  StatusCode i_add ( const Entry& entry ) 
    {
      static const Less _less_ = Less() ;
      // look for existing relations 
      iterator it = std::lower_bound
        ( m_entries.begin () , m_entries.end   () , entry , _less_ ) ;
      // the relation does exist ! 
      if( m_entries.end () != it && !_less_( entry , *it ) ) 
      { return StatusCode ( StatusCode::FAILURE , true ) ; }
      // insert new relation !
      m_entries.insert( it , entry ) ;
      return StatusCode::SUCCESS ;
    }
    /// remove the concrete relation between objects
    inline  StatusCode i_remove ( From_ object1 , To_ object2 ) 
    {
      static const Less _less = Less() ;
      // look for existing relations 
      const Entry ent ( object1 , object2 );
      iterator it  = std::lower_bound
        ( m_entries.begin() , m_entries.end  () , ent , _less ) ;
      // the relation does not exist ! 
      if( m_entries.end() == it || _less( ent , *it ) ) 
      { return StatusCode ( StatusCode::FAILURE , true ) ; }
      // remove existing relation     
      m_entries.erase( it );
      return StatusCode::SUCCESS ; 
    }
    /// remove all relations FROM the defined object
    inline  StatusCode i_removeFrom ( From_ object  )
    {
      // look for all existing relations from the given object  
      IP ip  = i_relations( object ) ;
      // there are no relations 
      if ( ip.second == ip.first ) 
      { return StatusCode ( StatusCode::FAILURE , true ) ; }// RETURN !!!
      // erase relations 
      m_entries.erase( ip.first , ip.second ) ;
      return StatusCode::SUCCESS ;
    }
    /// remove all relations TO the defined object
    inline  StatusCode i_removeTo ( To_ object ) 
    {
      // create the artificial entry 
      Entry entry ;
      entry.m_to = object ;
      // use the predicate "Equal"
      iterator it = std::remove_if ( m_entries.begin () , m_entries.end () , 
                                     std::bind2nd( Equal() , entry ) ) ;
      // no relations are found!
      if( m_entries.end() == it ) 
      { return StatusCode ( StatusCode::FAILURE , true ) ; }// RETURN !!!
      // erase the relations 
      m_entries.erase( it , m_entries.end() ) ;
      return StatusCode::SUCCESS ;
    }
    /// remove ALL relations from ALL  object to ALL objects
    inline  StatusCode i_clear() 
    { m_entries.clear() ; return StatusCode::SUCCESS ; }
    /// reserve the space for relations 
    inline StatusCode i_reserve ( const size_t num  )
    { Relations::reserve ( m_entries ,  num ) ; return StatusCode::SUCCESS ; }
    /// make the relation between 2 objects
    inline  void i_push ( From_ object1 , To_ object2 ) 
    { m_entries.push_back( Entry( object1 , object2 ) ) ; }
    /** (re)sort the whole underlying container 
     *  Call for this method is MANDATORY after usage of i_push 
     */ 
    inline void i_sort() 
    { std::stable_sort( m_entries.begin() , m_entries.end() , Less() ) ; }
    // ========================================================================
  public:
    // ========================================================================
    /** standard/default constructor
     *  @param reserve size of preallocated reserved space
     */ 
    RelationBase
    ( const size_type reserve = 0 ) 
      : BaseTable () 
        , m_entries () 
    { if ( 0 < reserve ) { i_reserve ( reserve ).ignore() ; } ; }
    /// constructor from any "direct" interface 
    RelationBase
    ( const IDirect& copy ) 
      : BaseTable () 
        , m_entries () 
    {
      typename IDirect::Range r = copy.relations() ;
      m_entries.insert ( m_entries.end() , r.begin() , r.end() ) ;
    } 
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
      i_reserve ( r.size() ).ignore();
      // invert all relations    
      for ( typename IInverse::iterator entry = r.begin() ; 
            r.end() != entry ; ++entry )
      { i_push ( entry->to() , entry->from() ) ;  }
      // final sort 
      i_sort() ;      
    }
    /// copy constructor
    RelationBase
    ( const OwnType& copy ) 
      : BaseTable ( copy ) 
      , m_entries ( copy.m_entries ) 
    {} 
    // ========================================================================
  public:
    // ========================================================================
    /** add *SORTED* range into the relation table 
     *  the table 
     *  @see std::merge 
     *  @param range the range to be added 
     *  @return self reference 
     */
    RelationBase& merge ( const Range& range ) 
    {
      if ( range.empty() ) { return *this ; }
      //
      switch ( range.size() )
      {
      case 1 :
        //
        i_add ( range[0] ) . ignore () ;
        break ;                                 // BREAK 
        //
      case 2 : 
        //
        i_add ( range[0] ) . ignore () ;
        i_add ( range[1] ) . ignore () ;
        break ;                                 // BREAK 
        //
      case 3 : 
        //
        i_add ( range[0] ) . ignore () ;
        i_add ( range[1] ) . ignore () ;
        i_add ( range[2] ) . ignore () ;
        break ;                                 // BREAK 
        //
      case 4 : 
        //
        i_add ( range[0] ) . ignore () ;
        i_add ( range[1] ) . ignore () ;
        i_add ( range[2] ) . ignore () ;
        i_add ( range[3] ) . ignore () ;
        break ;                                 // BREAK 
        //
      default: 
        //
        if ( range.size() > 0.1 * m_entries.size() ) 
        {
          Entries tmp ( m_entries.size() + range.size() ) ;
          std::merge 
            ( m_entries . begin () , 
              m_entries . end   () , 
              range     . begin () , 
              range     . end   () , 
              tmp       . begin () , Less() ) ;
          // use std::swap instead of assignement 
          std::swap ( m_entries , tmp ) ;
        }
        else
        {
          for ( typename Range::iterator ientry = range.begin() ; 
                range.end() != ientry ; ++ientry ) 
          { this->i_add ( *ientry ).ignore() ; }
        }
      }                                    // end of swich
      //
      return *this ;              
    }
    // ========================================================================
    /** add 'inverse' range into the relation table 
     *  @param range the range to be added 
     *  @return self reference 
     */
    RelationBase& imerge ( const typename IInverse::Range& range ) 
    {
      if ( range.empty() ) { return *this ; }                       // RETURN 
      // invert all relations    
      i_reserve ( m_entries.size() + range.size()  ) .ignore () ;
      //
      switch ( range.size() ) 
      {
      case 1 : 
        //
        i_relate ( range[0].to () , range[0].from () ) . ignore () ;
        break ;
        //
      case 2 : 
        //
        i_relate ( range[0].to () , range[0].from () ) . ignore () ;
        i_relate ( range[1].to () , range[1].from () ) . ignore () ;
        break ;
        //
      case 3 : 
        //
        i_relate ( range[0].to () , range[0].from () ) . ignore () ;
        i_relate ( range[1].to () , range[1].from () ) . ignore () ;
        i_relate ( range[2].to () , range[2].from () ) . ignore () ;
        break ;
        //
      case 4 : 
        //
        i_relate ( range[0].to () , range[0].from () ) . ignore () ;
        i_relate ( range[1].to () , range[1].from () ) . ignore () ;
        i_relate ( range[2].to () , range[2].from () ) . ignore () ;
        i_relate ( range[3].to () , range[3].from () ) . ignore () ;
        break ;
        //
      default: 
        //
        if ( range.size() > 0.1 * m_entries.size()  ) 
        {
          for ( typename IInverse::iterator entry = range.begin() ; range.end() != entry ; ++entry ) 
          { i_push   ( entry->to() , entry->from() ) ; }
          i_sort () ; 
        }
        else 
        {
          for ( typename IInverse::iterator entry = range.begin() ; range.end() != entry ; ++entry ) 
          { i_relate   ( entry->to() , entry->from() ) . ignore() ; }
        }
        //
      }
      //
      return *this ;                                                 // RETURN 
    }
    // ========================================================================
    /** add *SORTED* range into the relation table 
     *  @see std::merge 
     *  @param range the range to be added 
     *  @return self reference 
     */
    RelationBase& operator+=( const Range& range ) 
    { return merge ( range ) ; }
    // 
  private:
    // ========================================================================
    /// the actual storage of relation links  
    mutable Entries m_entries ;         // the actual storage of relation links  
    // ========================================================================
  } ;
  // ==========================================================================
} // End of name space Relations
// ============================================================================
// The End
// ============================================================================
#endif // RELATIONS_RELATIONBASE_H
// ============================================================================
