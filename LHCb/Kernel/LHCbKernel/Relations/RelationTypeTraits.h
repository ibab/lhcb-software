// $Id: RelationTypeTraits.h,v 1.14 2003-05-15 17:21:33 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.13  2003/05/15 15:22:10  ibelyaev
//  few modification to conform SLT interface for 'Range' types
//
// Revision 1.12  2003/01/17 14:07:01  sponce
// support for gcc 3.2
//
// Revision 1.11  2002/10/29 08:53:29  ibelyaev
//  add the reverse itereators to relations
//
// ============================================================================
#ifndef RELATIONS_RELATIONTYPETRAITS_H
#define RELATIONS_RELATIONTYPETRAITS_H 1
// Include files
#include "Relations/PragmaWarnings.h"
// STD & STL
#include <vector>
// #include <deque>
#include <functional>
// GaudiKernel
#include "GaudiKernel/GaudiException.h"
// Relations
#include "Relations/ObjectTypeTraits.h"

namespace Relations
{

  /** @struct RelationTypeTraits RelationTypeTraits.h
   *
   *  Type traits structure for definition of 
   *  data types and operation
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   06/02/2002
   */
  template < class FROM, class TO>
  struct RelationTypeTraits
  {
    /// type traits version  
    enum { version = 1 };
    /// type traits for "FROM" object 
    typedef Relations::ObjectTypeTraits<FROM>  FromTypeTraits ;
    /// type traits for "TO" object 
    typedef Relations::ObjectTypeTraits<TO>    ToTypeTraits   ;
    /// actual type of "FROM" object 
    typedef typename FromTypeTraits::Type      From           ;
    /// actual type of "TO" object 
    typedef typename ToTypeTraits::Type        To             ;    
    /// "less" function object for "From" objects 
    typedef typename FromTypeTraits::Less      LessF          ;
    /// "less" function object for "To" objects 
    typedef typename ToTypeTraits::Less        LessT          ;
    /// "equality" function object for "To" objects 
    typedef typename std::equal_to<To>         EqualT         ;
    
    /** @struct Entry
     *  @brief An auxillary class to implement the relations
     *
     *  For efficiency reason 
     *  the inheritance from @p std::pair is public, 
     *  to allow comparision functors to have a direct 
     *  access to @ std::pair - fields
     *  
     *  @see std::pair 
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   27/01/2002
     */
    struct Entry:
      public std::pair<From,To>
    {
      /// shortcut to the own base
      typedef std::pair<From,To>  Pair ;
      /// constructor
      Entry( const From& f = From () , 
             const To&   t = To   () ): Pair( f , t ) {};
      /// accessor to the "FROM" object ( const     version )
      inline From  from   () const { return Pair::first   ; }
      /// accessor to the "FROM" object ( non-const version )
      inline From& from   ()       { return Pair::first   ; }
      /// accessor to the "TO"   object ( const     version )
      inline To    to     () const { return Pair::second  ; }
      /// accessor to the "TO"   object ( non-const version )
      inline To&   to     ()       { return Pair::second  ; }
      /// the conversion operator       ( const     version )
      inline operator To  () const { return Pair::second  ; }
      /// the conversion operator       ( non-const version )
      inline operator To& ()       { return Pair::second  ; }
      /// comparison operator 
      inline bool operator<( const Entry& entry ) const 
      {
        return 
          LessF() ( Pair::first  , entry.first  ) ? true  :
          LessF() ( entry.first  , Pair::first  ) ? false :
          LessT() ( Pair::second , entry.second )         ; }  
    };
    
    /// "less" function object for "From" objects 
    typedef std::less<Entry>                     Less    ;

    /** @struct LessByFrom 
     *  comparison/ordering criteria using only "From" field
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   27/01/2002
     */
    struct LessByFrom: public std::binary_function<Entry,Entry,bool>
    {
      /** comparison/ordering criteria
       *  @param  entry1 the first entry 
       *  @param  entry2 the secons entry 
       *  @return true if "From" field of the first entry is less 
       */
      inline bool operator() ( const Entry& entry1 , 
                               const Entry& entry2 ) const
      { return LessF() ( entry1.first ,  entry2.first ) ; };
    };
    
    /** @struct  EqualByTo
     *  equality criteria using only "To" field
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   27/01/2002
     */
    struct EqualByTo: public std::binary_function<Entry,Entry,bool>
    {
      /** comparison criteria
       *  @param  entry1 the first entry 
       *  @param  entry2 the secons entry 
       *  @return true if "To" fields are equal 
       */
      inline bool operator() ( const Entry& entry1 , 
                               const Entry& entry2 ) const
      { return EqualT() ( entry1.second , entry2.second ) ; };
    };
    
    /** definition of the internal storage
     *  alternatively one could use @p std::deque
     */
    typedef std::vector<Entry>      Entries;
    
    /** definition of the standard iterator types
     *  @attention the true  @p iterator type is indeed  @p const_iterator !
     */
    typedef typename Entries::const_iterator         iterator  ;    
    typedef std::pair<iterator,iterator>             RangeBase ;
    
    /** @struct Range
     *
     *  An auxillary structure to provide a little bit
     *  better access to the components of standard
     *  @p std::pair class
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   27/01/2002
     */
    struct Range : public RangeBase 
    {
      /// short cut for own base class
      typedef RangeBase Base;
      typedef typename Entries::const_iterator         iterator               ;
      typedef typename Entries::const_iterator         const_iterator         ;
      typedef typename Entries::const_reverse_iterator const_reverse_iterator ;
      typedef typename Entries::const_reverse_iterator reverse_iterator       ;
      typedef typename Entries::const_reference        reference              ;
      typedef typename Entries::const_reference        const_reference        ;
      typedef typename Entries::value_type             value_type             ;
      /// default constructor
      Range()                                : Base()              {} ;
      /// constructor
      Range( iterator begin , iterator end ) : Base( begin , end ) {} ;
      /// the aliases for standard "first" and "second"
      /// begin-iterator (    const version)
      inline iterator         begin  () const { return Base::first           ; }
      /// end-iterator   (    const version)
      inline iterator         end    () const { return Base::second          ; }
      /// reverse iterator "begin"
      inline reverse_iterator rbegin () const
      { return const_reverse_iterator ( end   () ) ; }
      /// reverse iterator "end"
      inline reverse_iterator rend   () const 
      { return const_reverse_iterator ( begin () ) ; }
      /** front element (applied only for 'valid' ranges)
       *  @warning for invalid index the reference 
       *           to "default" element is returned 
       */
      inline const_reference  front  () const { return (*this)( 0 )          ; }
      /** back  element (applied only for 'valid' ranges)
       *  @warning for invalid index the reference 
       *           to "default" element is returned 
       */
      inline const_reference  back   () const { return (*this)( size() - 1 ) ; }
      /// number of relations 
      inline size_t           size   () const { return end () -  begin ()    ; }
      /// empty?
      inline bool             empty  () const { return end () == begin ()    ; }
      /** the access to the items by index 
       *  @warning for invalid index the reference 
       *           to "default" element is returned 
       *  @param  index the index of the element 
       *  @return the value 
       */
      inline const_reference  operator() ( const size_t index ) const
      {
        if ( index < size() ) { return *( begin() + index ) ; }
        static const value_type s_default = value_type()  ;
        return s_default ;
      };
      /** the access to the items by index 
       *  @warning for invalid index the reference 
       *           to "default" element is returned 
       *  @param  index the index of the element 
       *  @return the value 
       */
      inline const_reference  operator[] ( const size_t index ) const
      { return (*this) ( index ) ; } ;
      /** the access to the items by index 
       *  @warning for invalid index the reference 
       *           to "default" element is returned
       *  @param  index the index of the element 
       *  @return the value 
       */
      inline const_reference  at         ( const size_t index ) const
      { 
        if( index >= size() ) 
          { throw GaudiException("RelationTypeTraits::Range: index error" , 
                                 "*Relations*"                            ,
                                 StatusCode::FAILURE                      ) ; } 
        return (*this) ( index ) ; 
      } ;
    };
    
    /** technical definitions, useful for  for implementation 
     *  ordinary users should not use them 
     */
    typedef typename Entries::iterator       IT  ;
    typedef typename Entries::const_iterator CIT ;
    typedef std::pair<IT,IT>        IP  ;
    typedef std::pair<CIT,CIT>      CIP ;

  }; //< end of struct
  
}; // end of the namespace

// ============================================================================
// The End
// ============================================================================
#endif // RELATIONS_RELATIONTYPETRAITS_H
// ============================================================================
