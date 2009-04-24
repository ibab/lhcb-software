// $Id: WEntry.h,v 1.3 2009-04-24 15:26:46 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.3 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2006/06/11 19:37:02  ibelyaev
//  remove some extra classes + fix all virtual bases
//
// ============================================================================
#ifndef RELATIONS_WENTRY_H 
#define RELATIONS_WENTRY_H 1
// ============================================================================
// Include files
// ============================================================================
// Relations
// ============================================================================
#include "Relations/ObjectTypeTraits.h"
// ============================================================================


namespace Relations 
{
  /** @struct WEntry_ 
   *  
   *
   *  @author Ivan BELYAEV
   *  @date   2006-06-11
   */
  template <class FROM,class TO, class WEIGHT>
  struct WEntry_ 
  {
  public:
    // ========================================================================
    /// type traits for "FROM" object 
    typedef Relations::ObjectTypeTraits<FROM>   FromTypes   ;
    /// type traits for "TO" object 
    typedef Relations::ObjectTypeTraits<TO>     ToTypes     ;
    /// type traits for "WEIGHT" object 
    typedef Relations::ObjectTypeTraits<WEIGHT> WeightTypes ;
    // ========================================================================
  public:
    // ========================================================================
    typedef typename FromTypes::Input           From_       ;
    typedef typename FromTypes::Output          From        ;
    typedef typename FromTypes::Inner           FromInner   ;
    typedef typename ToTypes::Input             To_         ;
    typedef typename ToTypes::Output            To          ;
    typedef typename ToTypes::Inner             ToInner     ;
    typedef typename WeightTypes::Input         Weight_     ;
    typedef typename WeightTypes::Output        Weight      ;
    typedef typename WeightTypes::Inner         WeightInner ;
    // ========================================================================
  public:
    // ========================================================================
    /// "less" function object for "From" objects 
    typedef typename FromTypes::Less            LessF       ;
    /// "less" function object for "To" objects 
    typedef typename ToTypes::Less              LessT       ;
    /// "less" function object for "Weight" objects 
    typedef typename WeightTypes::Less          LessW       ;
    /// "equality" function object for "To" objects
    typedef typename std::equal_to<ToInner>     EqualT      ;
    // ========================================================================
  public:
    // ========================================================================
    typedef FromInner   First  ;
    typedef ToInner     Second ;      
    typedef WeightInner Third  ;      
    // ========================================================================
  public:
    // ========================================================================
    /// constructor
    WEntry_ ( ) 
      : m_from (   )  , m_to (   ) , m_weight (   ) {} ;
    WEntry_ ( From_ f ) 
      : m_from ( f )  , m_to (   ) , m_weight (   ) {} ;
    WEntry_ ( From_ f , To_    t ) 
      : m_from ( f )  , m_to ( t ) , m_weight (   ) {} ;
    WEntry_ ( From_ f , To_    t   , Weight_    w ) 
      : m_from ( f )  , m_to ( t ) , m_weight ( w ) {} ;
    // ========================================================================
  public:
    // ========================================================================
    /// accessor to the "FROM" object ( const     version )
    inline From     from   () const { return m_from ; }
    /// accessor to the "TO"   object ( const     version )
    inline To       to     () const { return m_to   ; }
    /// accessor to the "WEIGHT"   object (     const version )
    inline Weight   weight () const { return m_weight ; }
    /// the conversion operator       ( const     version )
    inline operator To     () const { return m_to   ; }
    // ========================================================================
  public:
    // ========================================================================
    /// for Python @warning  "from" is reserved keyword in Python 
    inline From _from  () const { return from  () ; }
    /// for Python
    inline To   _to    () const { return to    () ; }
    /// for Python
    inline Weight  _weight () const { return weight () ; }
    // ========================================================================
  public:
    // ========================================================================
    /// comparison operator 
    inline bool operator<( const WEntry_& entry ) const 
    { 
      static const LessF _lessF = LessF() ;
      static const LessT _lessT = LessT() ;
      static const LessW _lessW = LessW() ;
      return 
        _lessF ( this->m_from   , entry.m_from   ) ? true  :
        _lessF ( entry.m_from   , this->m_from   ) ? false :
        _lessW ( this->m_weight , entry.m_weight ) ? true  : 
        _lessW ( entry.m_weight , this->m_weight ) ? false :
        _lessT ( this->m_to     , entry.m_to     )         ; }
    // ========================================================================
  public:
    // ========================================================================
    mutable FromInner m_from ;
    mutable ToInner   m_to   ;
    mutable WeightInner m_weight ;
    // ========================================================================
  } ;
  // ==========================================================================
} // end of namespace Relations 
// ============================================================================
// The END 
// ============================================================================
#endif // RELATIONS_WENTRY_H
// ============================================================================
