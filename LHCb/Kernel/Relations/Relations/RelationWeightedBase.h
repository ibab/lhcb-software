// $Id: RelationWeightedBase.h,v 1.6 2006-01-27 13:25:47 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ ; version $Revision: 1.6 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.5  2005/02/16 19:59:35  ibelyaev
//  few minor fixes to enable 'lcgdict' processing
//
// ============================================================================
#ifndef RELATIONS_RELATIONWeightedBASE_H
#define RELATIONS_RELATIONWeightedBASE_H 1
// ============================================================================
// Include files
// ============================================================================
#include "Relations/PragmaWarnings.h"
// ============================================================================
// STD & STL
// ============================================================================
#include <vector>
#include <functional>
#include <algorithm>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/StatusCode.h"
// ============================================================================
// Relations
// ============================================================================
#include "Relations/Relations.h"
#include "Relations/IRelationWeighted.h"
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
  class RelationWeightedBase
  {
  public:
    
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
    /// actual "TO" type
    typedef typename TypeTraits::To                               To;
    /// actual "Weight" type
    typedef typename TypeTraits::Weight                           Weight;
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
    
  protected:
    
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
      { return Equal()( entry1 , entry2 ) && Less2()( entry1 , entry2 ) ; };
    };
    
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
      { return Equal() ( entry1 , entry2 ) && !Less2()( entry1 , entry2 ) ; };
    };
    
  public:
    
    /** retrive all relations from the given object
     *
     *    - the CPU performance is proportional to log(N),
     *      where N is the total number of relations
     *
     *  @see    IRelationWeighted
     *  @see    TypeTraits
     *  @param  object  smart reference to the object
     *  @return pair of iterators for output relations
     */
    inline  IP    i_relations
    ( const From& object      ) const
    { return std::equal_range( m_entries.begin () , 
                               m_entries.end   () , 
                               Entry( object ) , Less1() );};
    
    /** retrive AA relations from the ALL objects
     *
     *  @see    IRelationWeighted
     *  @see    TypeTraits
     *  @return pair of iterators for output relations
     */
    inline  IP    i_relations () const
    { return IP( m_entries.begin() , m_entries.end() ) ;};
    
    /** retrive all relations from the object which has weigth
     *  larger/smaller than the threshold value
     *
     *    - the CPU performance is proportional to log(N) + log(M)
     *      where N is the total number of relations and
     *      M is the mean relation multiplicity
     *
     *  @see    IRelationWeighted
     *  @see    TypeTraits
     *  @param  object     smart reference to the object
     *  @param  threshold  threshold value for the weight
     *  @param  flag       flag for larger/smaller
     *  @return pair of iterators for output relations
     */
    inline  IP      i_relations
    ( const From&   object     ,
      const Weight& threshold  ,
      const bool    flag       ) const
    {
      // find all relations from the given object
      IP ip = i_relations( object );
      // no relations are found !!!
      if( ip.second == ip.first ) { return ip ; }                // RETURN !!!
      // find the appropriate relations
      iterator it = 
        std::lower_bound( ip.first , ip.second ,
                          Entry( object , To() , threshold )  , Less2()  );
      return flag ?  IP( it , ip.second ) : IP( ip.first , it );
    };

    /** retrive all relations from the object which has weigth
     *  int he given range 
     *
     *    - the CPU performance is proportional to log(N) + log(M)
     *      where N is the total number of relations and
     *      M is the mean relation multiplicity
     *
     *  @see    IRelationWeighted
     *  @see    TypeTraits
     *  @param  object     smart reference to the object
     *  @param  low        low  threshold value for the weight
     *  @param  high       high threshold value for the weight
     *  @return pair of iterators for output relations
     */
    inline  IP      i_inRange 
    (  const From&   object , 
       const Weight& low    ,
       const Weight& high   ) const 
    {
      if ( low > high ) { return IP() ; }
      // find all relations from the given object
      IP ip = i_relations( object );
      // no relations are found !!!
      if ( ip.second == ip.first ) { return ip ; }              // RETURN !!!
      // find the appropriate relations
      iterator it1 = std::lower_bound 
        ( ip.first , ip.second , Entry ( object , To() , low  ) , Less2 () ) ;
      iterator it2 = std::lower_bound 
        ( it1      , ip.second , Entry ( object , To() , high ) , Less2 () ) ;
      return IP ( it1 , it2 ) ;
    };

    /** make the relation between 2 objects
     *
     *    - the CPU performance is proportional to log(N) + M
     *      where N is the total number of relations and
     *      M is the mean relation multiplicity
     *
     *    - StatusCode::FAILURE is returned if the relation
     *      i already set, the existing relation remains untouched
     *
     *  @see    IRelationWeighted
     *  @param  object1 smart reference to the first object
     *  @param  object2 smart reference to the second object
     *  @param  weight  weigth for the relation
     *  @return status  code
     */
    inline StatusCode i_relate
    ( const From&     object1 ,
      const To&       object2 ,
      const Weight&   weight  )
    {
      // get all existing relations from object1
      IP ip = i_relations( object1 ) ;
      // does the given relation between object1 and object1 exist ?
      const Entry entry ( object1 , object2 , weight );
      iterator it = std::find_if( ip.first , ip.second ,
                                  std::bind2nd( Equal() , entry ) );
      if( ip.second != it   ) { return StatusCode::FAILURE ; }     // RETURN !!!
      // find the place where to insert the relation and insert it!
      it = std::lower_bound( ip.first , ip.second , entry , Less2() ) ;
      m_entries.insert( it , Entry( object1 , object2 , weight ) ) ;
      return StatusCode::SUCCESS ;
    };
    
    /** remove the concrete relation between objects
     *
     *    - the CPU performance is proportional to log(N) + M
     *      where N is the total number of relations and
     *      M is the mean relation multiplicity
     *
     *    - StatusCode::FAILURE is returned if no appropriate relation
     *      was found
     *
     *  @see    IRelationWeighted
     *  @param  object1  smart reference to the first object
     *  @param  object2  smart reference to the second object
     *  @return status code
     */
    inline  StatusCode i_remove
    ( const From&      object1 ,
      const To&        object2 )
    {
      // get all existing relations form object1
      IP ip = i_relations( object1 );
      // does the given relation between object1 and object1 exist ?
      iterator it = std::find_if( ip.first , ip.second ,
                                  std::bind2nd( Equal() ,
                                                Entry( object1 , object2 ) ) );
      if( ip.second == it   ) { return StatusCode::FAILURE ; }    // RETURN !!!
      // remove the relation
      m_entries.erase( it );
      return StatusCode::SUCCESS ;
    };
    
    /** remove all relations FROM the defined object
     *
     *    - the CPU performance is proportional to log(N)
     *      where N is the total number of relations
     *
     *    - StatusCode::FAILURE is returned if no appropriate relation
     *      was found
     *
     *  @see    IRelationWeighted
     *  @param  object  smart reference to the object
     *  @return status code
     */
    inline  StatusCode i_removeFrom
    ( const From&      object )
    {
      // get all existing relations form object1
      IP ip = i_relations( object );
      // no relations are found !!!
      if( ip.second == ip.first ) { return StatusCode::FAILURE; }// RETURN !!!
      // remove relations
      m_entries.erase( ip.first , ip.second );
      return StatusCode::SUCCESS ;
    };
    
    /** remove all relations TO the defined object
     *
     *    - the CPU performance is proportional to N
     *      where N is the total number of relations
     *
     *    - StatusCode::FAILURE is returned if no appropriate relation
     *      was found
     *
     *  @see       IRelationWeighted
     *  @attention the method is relatively slow
     *  @param object  smart reference to the object
     *  @return status code
     */
    inline  StatusCode i_removeTo
    ( const To&        object )
    {
      iterator it =
        std::remove_if( m_entries.begin() , 
                        m_entries.end  () ,
                        std::bind2nd( Equal() , Entry( From() , object ) ) ) ;
      // no relations are found!
      if( m_entries.end() == it ) { return StatusCode::FAILURE ; }// RETURN !!
      // remove relations
      m_entries.erase( it , m_entries.end() ) ;
      return StatusCode::SUCCESS ;
    };
    
    /** filter out the relations FROM the defined object, which
     *  have a weight larger(smaller)than the threshold weight
     *
     *    - the CPU performance is proportional to log(N) + log(M)
     *      where N is the total number of relations and
     *      M is a mean relation multiplicity
     *
     *    - StatusCode::FAILURE is returned if no appropriate relation
     *      was found
     *
     *  @see    IRelationWeighted
     *  @param  object     smart reference to the object
     *  @param  threshold  threshold value for the weight
     *  @param  flag       flag for larger/smaller
     *  @return status     code
     */
    inline  StatusCode i_filterFrom
    ( const From&      object    ,
      const Weight&    threshold ,
      const bool       flag      )
    {
      // get all relations from the object over/under the threshold
      IP ip = i_relations( object , threshold , !flag );
      // no relations are found!
      if( ip.second == ip.first ) { return StatusCode::FAILURE ; }// RETURN !!!
      // erase relations
      m_entries.erase( ip.first , ip.second );
      return StatusCode::SUCCESS ;
    };
    
    /** filter out the relations TO the defined object, which
     *  have a weight larger/smaller than the threshold weight
     *
     *    - the CPU performance is proportional to N
     *      where N is the total number of relations
     *
     *    - StatusCode::FAILURE is returned if no appropriate relation
     *      was found
     *
     *  @see       IRelationWeighted
     *  @attention the method is relatively slow
     *  @param     object     smart reference to the object
     *  @param     threshold  threshold value for the weight
     *  @param     flag       flag for larger/smaller
     *  @return    status     code
     */
    inline  StatusCode i_filterTo
    ( const To&        object    ,
      const Weight&    threshold ,
      const bool       flag      )
    {
      // remove using predicates
      iterator it = flag ?
        std::remove_if( m_entries.begin () , 
                        m_entries.end   () ,
                        std::bind2nd( Comp1() , Entry( From ()   , 
                                                       object    , 
                                                       threshold ) ) ) :
        std::remove_if( m_entries.begin () , 
                        m_entries.end   () ,
                        std::bind2nd( Comp2() , Entry( From ()   , 
                                                       object    , 
                                                       threshold ) ) ) ;
      // nothing to be removed
      if( m_entries.end() == it ) { return StatusCode::FAILURE ; }// RETURN !!!
      // erase the relations
      m_entries.erase( it , m_entries.end() );
      return StatusCode::SUCCESS ;
    };
    
    /** filter out all relations which
     *  have a weight larger/smaller than the threshold weight
     *
     *    - the CPU performance is proportional to N
     *      where N is the total number of relations
     *
     *    - StatusCode::FAILURE is returned if no appropriate relation
     *      was found
     *
     *  @see       IRelationWeighted
     *  @attention the method  is relatively slow
     *  @param     threshold   threshold value for the weight
     *  @param     flag        flag for larger/smaller
     *  @return    status code
     */
    inline  StatusCode i_filter
    ( const Weight&    threshold ,
      const bool       flag      )
    {
      // remove using the predicates
      iterator it = flag ?
        std::remove_if( m_entries.begin () , 
                        m_entries.end   () ,
                        std::bind2nd( Less2() ,
                                      Entry( From () ,
                                             To   () , threshold ) ) ) :
        std::remove_if( m_entries.begin () , 
                        m_entries.end   () ,
                        std::not1( std::bind2nd( Less2() ,
                                                 Entry( From () ,
                                                        To   ()   ,
                                                        threshold ) ) ) ) ;
      // nothing to be removed
      if( m_entries.end() == it ) { return StatusCode::FAILURE ; } // RETURN 
      // erase the relations
      m_entries.erase( it , m_entries.end() );
      return StatusCode::SUCCESS ;
    };

    /** remove ALL relations from ALL objects to ALL objects 
     *
     *  @see    IRelationWeighted
     *  @return status code
     */
    inline  StatusCode i_clear ()
    {
      m_entries.clear() ;
      return StatusCode::SUCCESS ;
    };
    
    /** reserve the space for relations 
     *  @param   num number of relation to be reserved 
     *  @return  status code 
     */
    inline StatusCode i_reserve ( const size_t num  )
    { m_entries.reserve( num ) ; return StatusCode::SUCCESS ; }

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
    inline void i_push  
    ( const From&     object1 ,
      const To&       object2 ,
      const Weight&   weight  )
    { m_entries.push_back( Entry( object1 , object2 , weight ) ) ; };
    
    /** (re)sort the whole underlying container 
     *  Call for this method is MANDATORY after usage of i_push 
     */ 
    inline void i_sort() 
    { std::stable_sort( m_entries.begin() , m_entries.end() , m_less ) ; };
    
    /** standard/default constructor
     *  @param reserve size of preallocated reserved space
     */
    RelationWeightedBase
    ( const size_t reserve = 0 )
      : m_entries () 
      , m_less    () 
      , m_less1   () 
      , m_less2   () 
      , m_equal   () 
      , m_comp1   () 
      , m_comp2   () 
    { if( 0 < reserve ) { i_reserve( reserve ) ; } };
    
    /// destructor (virtual)
    virtual ~RelationWeightedBase() {} ;

    /** constructor from any "direct" interface 
     *  @param copy object to be copied 
     */
    RelationWeightedBase
    ( const IDirect& copy )
      : m_entries () 
      , m_less    () 
      , m_less1   () 
      , m_less2   () 
      , m_equal   () 
      , m_comp1   () 
      , m_comp2   () 
    {
      typename IDirect::Range r = copy.relations() ;
      m_entries.insert ( m_entries.end() , r.begin() , r.end() ) ;
    } ;
    
    /** constructor from any inverse interface
     *  @param inv relations to be inverted
     *  @param int artificial agument to make the difference 
     *         for stupid MicroSoft compiler
     */
    RelationWeightedBase
    ( const IInverse&    inv    ,
      const int       /* flag*/ )
      : m_entries ()
      , m_less    () 
      , m_less1   () 
      , m_less2   () 
      , m_equal   () 
      , m_comp1   () 
      , m_comp2   () 
    {
      // get all relations from "inv"
      typename IInverse::Range r = inv.relations() ;
      // reserve the space for relations
      i_reserve ( r.size() );
      // invert all relations    
      for ( typename IInverse::iterator entry = r.begin() ; r.end() != entry ; ++entry )
      { i_push ( entry->to() , entry->from() , entry->weight() ) ;  }
      // final sort 
      i_sort() ;      
    };
    
    /** copy constructor 
     *  @param copy object to be copied 
     */
    RelationWeightedBase
    ( const OwnType& copy )
      : m_entries ( copy.m_entries ) 
      , m_less    () 
      , m_less1   () 
      , m_less2   () 
      , m_equal   () 
      , m_comp1   () 
      , m_comp2   () 
    {};
    
  private:
    
    // the actual storage of relations 
    mutable Entries m_entries ; ///< teh actual storage of relations
    
    // comparison criteria for full ordering
    Less            m_less  ; ///< comparison criteria for full ordering
    // comparison criteria ( "less" by "From" value) 
    Less1           m_less1 ; ///< comparison criteria ( "less" by "From") 
    // comparison criteria ( "less" by "Weight" ) 
    Less2           m_less2 ; ///< comparison criteria ( "less" by "Weight") 
    // equality criteria   ( "equal" by "To" value)
    Equal           m_equal ; ///< equality criteria   ( "equal" by "To") 
    // comparison/ordering criteria using "Weight" and "To" fields
    Comp1           m_comp1 ;
    //  comparison/ordering criteria using "Weight" and "To" fields
    Comp1           m_comp2 ;
    
  };
  
}; //  end of namespace Relations


// ============================================================================
// The End
// ============================================================================
#endif // RELATIONS_RELATIONWeightedBASE_H
// ============================================================================
