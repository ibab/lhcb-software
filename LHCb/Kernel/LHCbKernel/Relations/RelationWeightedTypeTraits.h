// $Id: RelationWeightedTypeTraits.h,v 1.2 2002-04-03 15:35:18 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef RELATIONS_RELATIONWEIGTEDTYPETRAITS_H
#define RELATIONS_RELATIONWEIGTEDTYPETRAITS_H 1
// Include files
// STD & STL
#include <vector>
// Relations
#include "Relations/ObjectTypeTraits.h"


namespace Relations
{
  /** @struct RelationWeightedTypeTraits RelationWeightedTypeTraits.h
   *
   *  Type traits for relations with the weight ("link attributes") 
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   06/02/2002
   */
  template < class FROM, class TO, class WEIGHT>
  struct RelationWeightedTypeTraits
  {
    /// type traits version  
    enum { version = 1 } ;
    /// type traits for "FROM" object 
    typedef Relations::ObjectTypeTraits<FROM>   FromTypeTraits   ;
    /// type traits for "TO" object 
    typedef Relations::ObjectTypeTraits<TO>     ToTypeTraits     ;
    /// type traits for "WEIGHT" object 
    typedef Relations::ObjectTypeTraits<WEIGHT> WeightTypeTraits ;
    /// actual type of "FROM" object 
    typedef FromTypeTraits::Type                From             ;
    /// actual type of "TO" object 
    typedef ToTypeTraits::Type                  To               ;
    /// actual type of "WEIGHT" object 
    typedef WeightTypeTraits::Type              Weight           ;
    
    /// "less" function object for "From" objects 
    typedef FromTypeTraits::Less                LessF            ;
    /// "less" function object for "To" objects 
    typedef ToTypeTraits::Less                  LessT            ;
    /// "less" function object for "Weight" objects 
    typedef WeightTypeTraits::Less              LessW            ;
    /// "equality" function object for "To" objects 
    typedef std::equal_to<To>                   EqualT           ;
    
    /** @struct Entry 
     *  An auxillary class to implement the relations.
     *
     *  For efficiency reason 
     *  the inheritance from std::pair is public, 
     *  to allow comparision functors to have a direct 
     *  access to std::pair - fields
     * 
     *  @see std::pair 
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   27/01/2002
     */
    struct Entry: public std::pair<std::pair<From,Weight>,To>
    {
      /// shortcut to the own base
      typedef std::pair<From,Weight>  Pair    ;
      typedef std::pair<Pair,To>      Triplet ;
      /// constructor
      Entry( const From&   f = From   () ,  
             const To&     t = To     () , 
             const Weight& w = Weight () )
        : Triplet( Pair( f , w ) , t )  {};
      /// accessor to the "FROM" object     (     const version )
      inline From     from   () const { return first.first  ; }
      /// accessor to the "FROM" object     ( non-const version )
      inline From&    from   ()       { return first.first  ; }
      /// accessor to the "WEIGHT"   object (     const version )
      inline Weight   weight () const { return first.second ; }
      /// accessor to the "WEIGHT"   object ( non-const version )
      inline Weight&  weight ()       { return first.second ; }
      /// accessor to the "TO"   object     (     const version ) 
      inline To       to     () const { return second       ; }
      /// accessor to the "TO"   object     ( non-const version ) 
      inline To&      to     ()       { return second       ; }      
      /// the conversion operator           (     const version ) 
      inline operator To     () const { return second       ; }
      /// the conversion operator           ( non-const version ) 
      inline operator To&    ()       { return second       ; }
      /// comparison operator 
      inline bool operator<( const Entry& entry ) const 
      { 
        return 
          LessF() (       first.first  , entry.first.first  ) ? true  :
          LessF() ( entry.first.first  ,       first.first  ) ? false :
          LessW() (       first.second , entry.first.second ) ? true  : 
          LessW() ( entry.first.second ,       first.second ) ? false :
          LessT() (             second ,       entry.second )         ; }
    };
    
    /// "less" function object for "Entry" objects 
    typedef std::less<Entry>                  Less ;
    
    /** @struct  LessByFrom
     *
     *  comparison/ordering criteria using only "From" field
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   27/01/2002
     */
    struct LessByFrom: public std::binary_function<Entry,Entry,bool>
    {
      inline bool operator() ( const Entry& entry1 , 
                               const Entry& entry2 ) const 
      { return LessF() ( entry1.first.first , entry2.first.first ) ; }
      
    };
    
    /** @struct  LessByWeight
     *
     *  comparison/ordering criteria using only "Weight" field
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   27/01/2002
     */
    struct LessByWeight: public std::binary_function<Entry,Entry,bool>
    {
      /** comparison/ordering criteria
       *  @param  entry1 the first entry
       *  @param  entry2 the secons entry
       *  @return true   if "Weight" field of the first entry is less
       */
      inline bool operator() ( const Entry& entry1 ,
                               const Entry& entry2 ) const
      { return LessW() ( entry1.first.second ,  entry2.first.second ); }
    };
    
    /** @struct  EqualByTo
     *
     *  equality criteria using only "To" field
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   27/01/2002
     */
    struct EqualByTo: public std::binary_function<Entry,Entry,bool>
    {
      /** comparison criteria
       *  @param  entry1 the first entry
       *  @param  entry2 the second entry
       *  @return true   if "To" fields are equal
       */
      inline bool operator() ( const Entry& entry1 ,
                               const Entry& entry2 ) const
      { return EqualT() ( entry1.second , entry2.second ) ; };
    };
    
    /// definition of the internal storage 
    typedef std::vector<Entry>      Entries;
    
    /** definition of the standard iterator type.
     *  @warning The true type of 'iterator' is 'const_iterator'
     */
    typedef Entries::const_iterator iterator ;

    /** @struct Range
     *
     *  An auxilalry structure to provide a little bit
     *  better access to the components of standard
     *  std::pair class
     *
     *  @see std::pair
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   27/01/2002
     */
    struct Range : public std::pair<iterator,iterator>
    {
      /// short cut for own base class
      typedef std::pair<iterator,iterator> Base;
      /// constructor
      Range( iterator begin , iterator end ) : Base( begin , end ) {};
      /// the aliases for standard "first" and "second"
      iterator& begin ()       { return first  ; }
      iterator  begin () const { return first  ; }
      iterator& end   ()       { return second ; }
      iterator  end   () const { return second ; }
    };
    
    /** technical definitions, useful and needed for implementation 
     */
    typedef Entries::iterator       IT  ;
    typedef Entries::const_iterator CIT ;
    typedef std::pair<IT,IT>        IP  ;
    typedef std::pair<CIT,CIT>      CIP ;
    
  }; //< end of struct

};

// ============================================================================
// The End
// ============================================================================
#endif // RELATIONS_RELATIONWEIGHTEDTYPETRAITS
// ============================================================================
