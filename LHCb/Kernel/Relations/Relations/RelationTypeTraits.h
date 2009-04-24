// $Id: RelationTypeTraits.h,v 1.7 2009-04-24 15:26:46 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.7 $
// ============================================================================
// $Log: not supported by cvs2svn $
//
// ============================================================================
#ifndef RELATIONS_RELATIONTYPETRAITS_H
#define RELATIONS_RELATIONTYPETRAITS_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL
// ============================================================================
#include <vector>
#include <functional>
// ============================================================================
// Relations
// ============================================================================
#include "Relations/ObjectTypeTraits.h"
#include "Relations/Entry.h"
#include "Relations/Range.h"
#include "Relations/Bases.h"
// ============================================================================
namespace Relations
{  
  // ==========================================================================
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
    // ========================================================================
    /// the actual enum to distinguish the relation type 
    enum Type { weighted = false } ;
    // ========================================================================
    /// the actual type of the entry 
    typedef typename Relations::Entry_<FROM,TO>           Entry      ;
    /// type traits for "FROM" object 
    typedef typename Entry::FromTypes         FromTypes  ;
    /// type traits for "TO" object 
    typedef typename Entry::ToTypes           ToTypes    ;
    ///   
    typedef typename Entry::From_             From_      ;
    typedef typename Entry::From              From       ;
    ///
    typedef typename Entry::To_               To_        ;
    typedef typename Entry::To                To         ;
    /// "less" function object for "From" objects 
    typedef typename Entry::LessF             LessF      ;
    /// "less" function object for "To" objects 
    typedef typename Entry::LessT             LessT      ;
    /// "equality" function object for "To" objects 
    typedef typename Entry::EqualT            EqualT     ;
    /// "less" function object for "From" objects 
    typedef std::less<Entry>                  Less       ;
    /** @struct LessByFrom 
     *  comparison/ordering criteria using only "From" field
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   27/01/2002
     */
    struct LessByFrom: public std::binary_function<Entry,Entry,bool>
    {
      /** comparison/ordering criteria
       *  @param  entry1 the first entry 
       *  @param  entry2 the second entry 
       *  @return true if "From" field of the first entry is less 
       */
      inline bool operator() ( const Entry& entry1 , 
                               const Entry& entry2 ) const
      { return LessF() ( entry1.m_from ,  entry2.m_from ) ; };
    };
    // ========================================================================
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
      { return EqualT() ( entry1.m_to , entry2.m_to ) ; };
    };    
    // ========================================================================
    /// definition of the internal storage
    typedef std::vector<Entry>                Entries  ;    
    ///definition of the standard iterator types
    typedef typename Entries::const_iterator  iterator ;    
    /// the actual type fo Range 
    typedef Relations::Range_<Entries>        Range    ;
    /** technical definitions, useful for  for implementation 
     *  ordinary users should not use them 
     */
    typedef typename Entries::iterator       IT  ;
    typedef typename Entries::const_iterator CIT ;
    typedef std::pair<IT,IT>        IP  ;
    typedef std::pair<CIT,CIT>      CIP ;
  }; // end of struct
  // ==========================================================================
} // end of namespace Relations
// ============================================================================
// The End
// ============================================================================
#endif // RELATIONS_RELATIONTYPETRAITS_H
// ============================================================================
