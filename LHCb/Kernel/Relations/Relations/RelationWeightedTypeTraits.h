// $Id: RelationWeightedTypeTraits.h,v 1.6 2009-04-24 15:26:46 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.6 $
// ============================================================================
// $Log: not supported by cvs2svn $
//
// ============================================================================
#ifndef RELATIONS_RELATIONWEIGTEDTYPETRAITS_H
#define RELATIONS_RELATIONWEIGTEDTYPETRAITS_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL
// ============================================================================
#include <vector>
// ============================================================================
// Relations
// ============================================================================
#include "Relations/ObjectTypeTraits.h"
#include "Relations/WEntry.h"
#include "Relations/Range.h"
#include "Relations/Bases.h"
// ============================================================================
namespace Relations
{
  // ==========================================================================
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
    // ========================================================================
    /// the actual enum to distinguish the relation type 
    enum Type { weighted = true } ;
    // ========================================================================  
    /// the actual type of the entry 
    typedef typename Relations::WEntry_<FROM,TO,WEIGHT> Entry ;
    /// type traits for "FROM" object 
    typedef typename Entry::FromTypes            FromTypes   ;
    /// type traits for "TO" object 
    typedef typename Entry::ToTypes              ToTypes     ;
    /// type traits for "WEIGHT" object 
    typedef typename Entry::WeightTypes          WeightTypes ;
    /// actual type of "FROM" object 
    typedef typename Entry::From_                From_       ;
    typedef typename Entry::From                 From        ;
    /// actual type of "TO" object 
    typedef typename Entry::To_                  To_         ;
    typedef typename Entry::To                   To          ;
    /// actual type of "WEIGHT" object 
    typedef typename Entry::Weight_              Weight_     ;
    typedef typename Entry::Weight               Weight      ;
    /// "less" function object for "From" objects 
    typedef typename Entry::LessF                LessF       ;
    /// "less" function object for "To" objects 
    typedef typename Entry::LessT                LessT       ;
    /// "less" function object for "Weight" objects 
    typedef typename Entry::LessW                LessW       ;
    /// "equality" function object for "To" objects 
    typedef typename Entry::EqualT               EqualT      ;
    /// "less" function object for "Entry" objects 
    typedef std::less<Entry>                     Less ;
    /** @struct  LessByFrom
     *  comparison/ordering criteria using only "From" field
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   27/01/2002
     */
    struct LessByFrom: public std::binary_function<Entry,Entry,bool>
    {
      inline bool operator() ( const Entry& entry1 , 
                               const Entry& entry2 ) const 
      { return LessF() ( entry1.m_from , entry2.m_from ) ; }
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
       *  @return true   if "Weight" field of the first entry is less
       */
      inline bool operator() ( const Entry& entry1 ,
                               const Entry& entry2 ) const
      { return LessW() ( entry1.m_weight ,  entry2.m_weight ); }
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
       *  @param  entry2 the second entry
       *  @return true   if "To" fields are equal
       */
      inline bool operator() ( const Entry& entry1 ,
                               const Entry& entry2 ) const
      { return EqualT() ( entry1.m_to , entry2.m_to ) ; };
    };
    /// definition of the internal storage 
    typedef std::vector<Entry>               Entries  ;
    /// definition of the standard iterator type.
    typedef typename Entries::const_iterator iterator ;
    /// the actual type for range 
    typedef Relations::Range_<Entries>       Range    ;
    /// technical definitions, useful and needed for implementation 
    typedef typename Entries::iterator       IT  ;
    typedef typename Entries::const_iterator CIT ;
    typedef std::pair<IT,IT>        IP  ;
    typedef std::pair<CIT,CIT>      CIP ;
  }; //< end of struct
} // end of namespace Relations
// ============================================================================
// The End
// ============================================================================
#endif // RELATIONS_RELATIONWEIGHTEDTYPETRAITS
// ============================================================================
