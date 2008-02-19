// $Id: Entry.h,v 1.3 2008-02-19 15:26:09 ibelyaev Exp $
// ============================================================================
#ifndef RELATIONS_ENTRY_H 
#define RELATIONS_ENTRY_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <functional>
#include <utility>
// ============================================================================
// Relations
// ============================================================================
#include "Relations/ObjectTypeTraits.h"
// ============================================================================
namespace Relations 
{
  // ==========================================================================
  /** @struct Entry_
   *  An auxillary class to implement the relations
   *  @see std::pair 
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   27/01/2002
   */
  template <class FROM, class TO>
  struct Entry_ 
  {
  public:
    /// type traits for "FROM" object 
    typedef Relations::ObjectTypeTraits<FROM> FromTypes  ;
    /// type traits for "TO" object 
    typedef Relations::ObjectTypeTraits<TO>   ToTypes    ;
  public:
    typedef typename FromTypes::Input         From_      ;
    typedef typename FromTypes::Output        From       ;
    typedef typename FromTypes::Inner         FromInner  ;
    typedef typename ToTypes::Input           To_        ;
    typedef typename ToTypes::Output          To         ;
    typedef typename ToTypes::Inner           ToInner    ;
  public: 
    /// "less" function object for "From" objects 
    typedef typename FromTypes::Less          LessF  ;
    /// "less" function object for "To" objects 
    typedef typename ToTypes::Less            LessT  ;
    /// "equality" function object for "To" objects
    typedef typename std::equal_to<ToInner>   EqualT ;
  public:
    typedef FromInner   First  ;
    typedef ToInner     Second ;      
  public:
    /// constructor
    Entry_ ()                  
      : m_from (   ) , m_to (   ) {}
    /// constructor 
    Entry_ ( From_ f , To_ t ) 
      : m_from ( f ) , m_to ( t ) {}
    /// constructor 
    Entry_ ( From_ f ) 
      : m_from ( f ) , m_to (   ) {}
  public:
    /// accessor to the "FROM" object ( const     version )
    inline From from   () const { return m_from ; }
    /// accessor to the "TO"   object ( const     version )
    inline To   to     () const { return m_to   ; }
    /// the conversion operator       ( const     version )
    inline operator To () const { return m_to   ; }
  public:
    /// for Python @warning  "from" is reserved keyword in Python 
    inline From _from  () const { return from  () ; }
    /// for Python
    inline To   _to    () const { return to    () ; }
  public:
    /// comparison operator 
    inline bool operator<( const Entry_& entry ) const 
    {
      return 
        LessF() ( this->m_from  , entry.m_from  ) ? true  :
        LessF() ( entry.m_from  , this->m_from  ) ? false :
        LessT() ( this->m_to    , entry.m_to    )         ; }  
  public:
    mutable FromInner m_from ;
    mutable ToInner   m_to   ;
  } ;
  // ==========================================================================
} // end of namespace Relations
// ============================================================================
// The END 
// ============================================================================
#endif // RELATIONS_ENTRY_H
// ============================================================================
