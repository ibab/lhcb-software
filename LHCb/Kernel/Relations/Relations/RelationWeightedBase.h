// $Id: RelationWeightedBase.h,v 1.15 2009-04-24 15:26:46 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ ; version $Revision: 1.15 $
// ============================================================================
// $Log: not supported by cvs2svn $
//
// ============================================================================
#ifndef RELATIONS_RELATIONWeightedBASE_H
#define RELATIONS_RELATIONWeightedBASE_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL
// ============================================================================
#include <vector>
#include <functional>
#include <algorithm>
// ============================================================================
// Relations
// ============================================================================
#include "Relations/IRelationWeighted.h"
#include "Relations/RelationWeightedTypeTraits.h"
#include "Relations/Reserve.h"
// ============================================================================

namespace Relations
{
  /** @class RelationWeightedBase RelationWeightedBase.h
   *
   *  The useful helper class for effective  implementation
   *  of weighted unidirectional relations
   *  
   *  @warning for the current implementation the actual type of 
   *  "FROM" should differ from the actual type of "TO"
   *  
   *  @warning users should not deal with this class directly!
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   27/01/2002
   */
  template<class FROM, class TO, class WEIGHT>
  class RelationWeightedBase : public BaseWeightedTable 
  {
  public:
    // ========================================================================
    /// shortcut for own type
    typedef RelationWeightedBase<FROM,TO,WEIGHT>                  OwnType    ;
    /// shortcut for inverse type
    typedef RelationWeightedBase<TO,FROM,WEIGHT>                  InvType    ;
    /// short cut for type traits
    typedef Relations::RelationWeightedTypeTraits<FROM,TO,WEIGHT> TypeTraits ;
    /// shortcut for "direct" interface 
    typedef IRelationWeighted<FROM,TO,WEIGHT>                     IDirect    ;
    /// shortcut for "inverse" interface 
    typedef IRelationWeighted<TO,FROM,WEIGHT>                     IInverse   ;    
    /// actual "FROM" type
    typedef typename TypeTraits::From                             From;
    /// actual "FROM" type
    typedef typename TypeTraits::From_                            From_;
    /// actual "TO" type
    typedef typename TypeTraits::To                               To  ;
    typedef typename TypeTraits::To_                              To_ ;
    /// actual "Weight" type
    typedef typename TypeTraits::Weight                           Weight;
    typedef typename TypeTraits::Weight_                          Weight_ ;
    /// Entry type 
    typedef typename TypeTraits::Entry                            Entry;
    /// container type 
    typedef typename TypeTraits::Entries                          Entries;
    /// iterator type 
    typedef typename Entries::iterator                            iterator;
    /// iterator type (internal) 
    typedef typename Entries::iterator                            IT;
    /// iterator pair type (internal) 
    typedef typename TypeTraits::IP                               IP;
    /// const_iterator type (internal) 
    typedef typename TypeTraits::CIT                              CIT;    
    /// range 
    typedef typename TypeTraits::Range                            Range  ;
    // ========================================================================
  protected:
    // ========================================================================
    /// comparison criteria for full ordering
    typedef typename TypeTraits::Less                             Less;
    /// comparison criteria ( "less" by "From" value) 
    typedef typename TypeTraits::LessByFrom                       Less1;
    /// comparison criteria ( "less" by "Weight" value) 
    typedef typename TypeTraits::LessByWeight                     Less2;
    /// equality criteria   ( "equal" by "To" value)
    typedef typename TypeTraits::EqualByTo                        Equal;
    /** @struct  Comp1
     *  comparison/ordering criteria using "Weight" and "To" fields
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   27/01/2002
     */
    struct Comp1: public std::binary_function<Entry,Entry,bool>
    {
      /** comparison/ordering criteria
       *  @param  entry1 the first entry
       *  @param  entry2 the secons entry
       *  @return true   if "Weight" field of the first entry is less
       *                 and "To" fields are equal!
       */
      inline bool operator() ( const Entry& entry1 ,
                               const Entry& entry2 ) const
      { return Equal()( entry1 , entry2 ) && Less2()( entry1 , entry2 ) ; }
    } ;
    /** @struct  Comp2
     *  comparison/ordering criteria using "Weight" and "To" fields
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   27/01/2002
     */
    struct Comp2: public std::binary_function<Entry,Entry,bool>
    {
      /** comparison/ordering criteria
       *  @param  entry1 the first entry
       *  @param  entry2 the secons entry
       *  @return true   if "Weight" field of the first entry is less
       *                 and "To" fields are equal!
       */
      inline bool operator() ( const Entry& entry1 ,
                               const Entry& entry2 ) const
      { return Equal() ( entry1 , entry2 ) && !Less2()( entry1 , entry2 ) ; }
    } ;
    // ========================================================================
  public:
    // ========================================================================
    /// retrive all relations from the given object
    inline  IP    i_relations ( From_ object      ) const
    { return std::equal_range ( m_entries.begin () , m_entries.end () , 
                                Entry( object ) , Less1() ) ; } ;
    /// retrive ALL relations from the ALL objects
    inline  IP    i_relations () const
    { return IP( m_entries.begin() , m_entries.end() ) ;};    
    /// retrive all relations from the object which has weigth
    inline  IP    i_relations
    ( From_ object , Weight_ threshold  , const bool flag ) const
    {
      // find all relations from the given object
      IP ip = i_relations( object );
      // no relations are found !!!
      if( ip.second == ip.first ) { return ip ; }                // RETURN !!!
      // find the appropriate relations
      Entry entry ;
      entry.m_from   = object    ;
      entry.m_weight = threshold ;
      iterator it = 
        std::lower_bound( ip.first , ip.second , entry , Less2()  );
      return flag ?  IP( it , ip.second ) : IP( ip.first , it );
    }
    /// retrive all relations from the object which has weigth
    inline  IP i_inRange ( From_ object , Weight_ low , Weight_ high ) const 
    {
      if ( low > high ) { return IP() ; }
      // find all relations from the given object
      IP ip = i_relations( object );
      // no relations are found !!!
      if ( ip.second == ip.first ) { return ip ; }              // RETURN !!!
      // find the appropriate relations
      Entry entry ( object ) ;
      entry.m_weight = low   ;
      iterator it1 = std::lower_bound( ip.first , ip.second , entry , Less2() ) ;
      entry.m_weight = high  ;
      iterator it2 = std::lower_bound( it1      , ip.second , entry , Less2() ) ;
      return IP ( it1 , it2 ) ;
    }
    /// make the relation between 2 objects
    inline StatusCode i_relate ( From_ object1 , To_ object2 , Weight_ weight  )
    {
      const Entry entry ( object1 , object2 , weight );
      return i_add ( entry ) ;
    }
    /// add the entry 
    inline StatusCode i_add ( const Entry& entry )
    {
      // get all existing relations from object1
      IP ip = i_relations( entry.from() ) ;
      // does the given relation between object1 and object2 exist ?
      iterator it = std::find_if 
        ( ip.first , ip.second , std::bind2nd( Equal() , entry ) );
      if ( ip.second != it   ) { return StatusCode::FAILURE ; }     // RETURN !!!
      // find the place where to insert the relation and insert it!
      it = std::lower_bound( ip.first , ip.second , entry , Less2() ) ;
      m_entries.insert( it , entry ) ;
      return StatusCode::SUCCESS ;
    }
    /// remove the concrete relation between objects
    inline  StatusCode i_remove ( From_ object1 , To_ object2 )
    {
      // get all existing relations form object1
      IP ip = i_relations( object1 );
      // does the given relation between object1 and object1 exist ?
      iterator it = 
        std::find_if ( ip.first , ip.second , 
                       std::bind2nd( Equal() , Entry( object1 , object2 ) ) );
      if ( ip.second == it   ) { return StatusCode::FAILURE ; }    // RETURN !!!
      // remove the relation
      m_entries.erase( it );
      return StatusCode::SUCCESS ;
    }
    /// remove all relations FROM the defined object
    inline  StatusCode i_removeFrom ( From_ object )
    {
      // get all existing relations form object1
      IP ip = i_relations( object );
      // no relations are found !!!
      if( ip.second == ip.first ) { return StatusCode::FAILURE; }// RETURN !!!
      // remove relations
      m_entries.erase( ip.first , ip.second );
      return StatusCode::SUCCESS ;
    }
    /// remove all relations TO the defined object
    inline  StatusCode i_removeTo ( To_ object )
    {
      // create the artificial entry 
      Entry entry ;
      entry.m_to = object ;
      iterator it = std::remove_if ( m_entries.begin() , m_entries.end  () ,
                                     std::bind2nd( Equal() , entry ) ) ;
      // no relations are found!
      if ( m_entries.end() == it ) { return StatusCode::FAILURE ; }// RETURN !!
      // remove relations
      m_entries.erase( it , m_entries.end() ) ;
      return StatusCode::SUCCESS ;
    }
    /** filter out the relations FROM the defined object, which
     *  have a weight larger(smaller)than the threshold weight
     */
    inline  StatusCode i_filterFrom
    ( From_ object , Weight_ threshold , const bool flag )
    {
      // get all relations from the object over/under the threshold
      IP ip = i_relations( object , threshold , !flag );
      // no relations are found!
      if ( ip.second == ip.first ) { return StatusCode::FAILURE ; }// RETURN !!!
      // erase relations
      m_entries.erase( ip.first , ip.second );
      return StatusCode::SUCCESS ;
    }
    /** filter out the relations TO the defined object, which
     *  have a weight larger/smaller than the threshold weight
     */
    inline  StatusCode i_filterTo
    ( To_ object , Weight_ threshold , const bool flag )
    {
      Entry entry ;
      entry.m_to     = object    ;
      entry.m_weight = threshold ;      
      // remove using predicates
      iterator it = flag ?
        std::remove_if ( m_entries.begin () , m_entries.end () ,
                         std::bind2nd ( Comp1() , entry ) ) :
        std::remove_if ( m_entries.begin () , m_entries.end () ,
                         std::bind2nd ( Comp2() , entry ) ) ;
      // nothing to be removed
      if ( m_entries.end() == it ) { return StatusCode::FAILURE ; }// RETURN !!!
      // erase the relations
      m_entries.erase( it , m_entries.end() );
      return StatusCode::SUCCESS ;
    }
    /** filter out all relations which
     *  have a weight larger/smaller than the threshold weight
     */
    inline  StatusCode i_filter ( Weight_ threshold , const bool flag )
    {
      Entry entry ;
      entry.m_weight = threshold ;
      // remove using the predicates
      iterator it = flag ?
        std::remove_if ( m_entries.begin () , m_entries.end () ,
                         std::bind2nd ( Less2() , entry  ) ) :
        std::remove_if ( m_entries.begin () , m_entries.end   () ,
                         std::bind1st ( Less2() , entry  ) ) ;
      // nothing to be removed
      if ( m_entries.end() == it ) { return StatusCode::FAILURE ; } // RETURN 
      // erase the relations
      m_entries.erase( it , m_entries.end() );
      return StatusCode::SUCCESS ;
    }
    /// remove ALL relations from ALL objects to ALL objects
    inline  StatusCode i_clear ()
    {
      m_entries.clear() ;
      return StatusCode::SUCCESS ;
    }
    /// reserve the space for relations 
    inline StatusCode i_reserve ( const size_t num  )
    { Relations::reserve ( m_entries , num ) ; return StatusCode::SUCCESS ; }
    /** make the relation between 2 objects, but brute force.
     *  The relation entry in *APPENDED* to the end 
     *  of the internal container, keeping it in INVALID state
     *
     *  This method is added to provide more 
     *  efficient fill for Relation tables.
     *
     *  The subsequent call of "sort" is MANDATORY!
     *  
     *  @param  object1 smart reference to the first object
     *  @param  object2 smart reference to the second object
     *  @param  weight  weigth for the relation
     */
    inline void i_push ( From_ object1 , To_ object2 , Weight_ weight  )
    { m_entries.push_back( Entry( object1 , object2 , weight ) ) ; }
    /** (re)sort the whole underlying container 
     *  Call for this method is MANDATORY after usage of i_push 
     */ 
    inline void i_sort() 
    { std::stable_sort ( m_entries.begin() , m_entries.end() , Less() ) ; }
    /// standard/default constructor
    RelationWeightedBase
    ( const size_t reserve = 0 )
      : BaseWeightedTable () 
      , m_entries () 
    { if ( 0 < reserve ) { i_reserve ( reserve ).ignore() ; } }
    /// destructor (virtual)
    virtual ~RelationWeightedBase() {} ;
    /// constructor from any "direct" interface 
    RelationWeightedBase
    ( const IDirect& copy )
      : BaseWeightedTable () 
      , m_entries         () 
    {
      typename IDirect::Range r = copy.relations() ;
      m_entries.insert ( m_entries.end() , r.begin() , r.end() ) ;
    } 
    /** constructor from any inverse interface
     *  @param inv relations to be inverted
     *  @param int artificial agument to make the difference 
     *         for stupid MicroSoft compiler
     */
    RelationWeightedBase
    ( const IInverse&    inv    ,
      const int       /* flag*/ )
      : BaseWeightedTable () 
      , m_entries         ()
    {
      // get all relations from "inv"
      typename IInverse::Range r = inv.relations() ;
      // reserve the space for relations
      i_reserve ( r.size() ).ignore();
      // invert all relations    
      for ( typename IInverse::iterator entry = r.begin() ; r.end() != entry ; ++entry )
      { i_push ( entry->to() , entry->from() , entry->weight() ) ;  }
      // final sort 
      i_sort() ;      
    }
    /// copy constructor 
    RelationWeightedBase
    ( const OwnType& copy )
      : BaseWeightedTable ( copy ) 
      , m_entries         ( copy.m_entries ) 
    {}
    // ========================================================================
  public:
    // ========================================================================
    /** add *SORTED* range into the relation table 
     *  @see std::merge 
     *  @param range the range to be added 
     *  @return self reference 
     */
    RelationWeightedBase& merge ( const Range& range ) 
    {
      if ( range.empty() ) { return *this ; }
      Entries tmp ( m_entries.size() + range.size() ) ;
      std::merge 
        ( m_entries . begin () , 
          m_entries . end   () , 
          range     . begin () , 
          range     . end   () , 
          tmp       . begin () , Less() ) ;
      m_entries = tmp ;
      return *this ;              
    }
    // ========================================================================
    /** add 'inverse' range into the relation table 
     *  @param range the range to be added 
     *  @return self reference 
     */
    RelationWeightedBase& imerge ( const typename IInverse::Range& range ) 
    {
      if ( range.empty() ) { return *this ; }                       // RETURN 
      // invert all relations    
      i_reserve ( m_entries.size() + range.size()  ) .ignore () ;
      for ( typename IInverse::iterator entry = range.begin() ; 
            range.end() != entry ; ++entry ) 
      { i_push ( entry->to() , entry->from() , entry->weight() ) ;  }
      /// final sorting of the container
      i_sort () ;
      return *this ;                                                 // RETURN 
    }
    /** add *SORTED* range into the relation table 
     *  @see std::merge 
     *  @param range the range to be added 
     *  @return self reference 
     */
    RelationWeightedBase& operator+=( const Range& range ) 
    { return merge ( range ) ; }
    // ========================================================================
  private:
    // ========================================================================
    /// the actual storage of relation links  
    mutable Entries m_entries ;         // the actual storage of relation links  
    // ========================================================================
  };
  // ==========================================================================
} //  end of namespace Relations
// ============================================================================
// The End
// ============================================================================
#endif // RELATIONS_RELATIONWeightedBASE_H
// ============================================================================
