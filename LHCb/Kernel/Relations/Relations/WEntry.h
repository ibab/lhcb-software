// $Id: WEntry.h,v 1.1 2006-06-11 15:23:47 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef RELATIONS_WENTRY_H 
#define RELATIONS_WENTRY_H 1
// ============================================================================
// Include files
// ============================================================================
// Relations
// ============================================================================
#include "Relations/Entry.h"
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
  struct WEntry_ : public Relations::Entry_<FROM,TO> 
  {
  public:
    /// type traits for "WEIGHT" object 
    typedef Relations::ObjectTypeTraits<WEIGHT> WeightTypes  ;
  public:
    typedef typename WeightTypes::Input         Weight_      ;
    typedef typename WeightTypes::Output        Weight       ;
    typedef typename WeightTypes::Inner         WeightInner  ;
  public:
    /// "less" function object for "Weight" objects 
    typedef typename WeightTypes::Less          LessW        ;
  public:
    typedef WeightInner Third  ;      
  protected:
    typedef Relations::Entry_<FROM,TO>  _Base ;
  public:
    /// constructor
    WEntry_ ( ) 
      : Entry_<FROM,TO> (       ) , m_weight (   ) {} ;
    WEntry_ ( typename _Base::From_ f ) 
      : Entry_<FROM,TO> ( f )     , m_weight (   ) {} ;
    WEntry_ ( typename _Base::From_ f , 
              typename _Base::To_   t ) 
      : Entry_<FROM,TO> ( f , t ) , m_weight (   ) {} ;
    WEntry_ ( typename _Base::From_ f , 
              typename _Base::To_   t , Weight_ w ) 
      : Entry_<FROM,TO> ( f , t ) , m_weight ( w ) {} ;
  public:
    /// accessor to the "WEIGHT"   object (     const version )
    inline Weight   weight () const { return m_weight ; }
  public:
    /// for Python
    inline Weight  _weight () const { return weight () ; }
  public:
    /// comparison operator 
    inline bool operator<( const WEntry_& entry ) const 
    { 
      typedef typename _Base::LessF LessF ;
      typedef typename _Base::LessT LessT ;
      static const LessF _lessF = LessF() ;
      static const LessT _lessT = LessT() ;
      static const LessW _lessW = LessW() ;
      return 
        _lessF ( _Base::m_from   , entry.m_from   ) ? true  :
        _lessF ( entry.m_from    , _Base::m_from  ) ? false :
        _lessW ( this->m_weight  , entry.m_weight ) ? true  : 
        _lessW ( entry.m_weight  , this->m_weight ) ? false :
        _lessT ( _Base::m_to     , entry.m_to     )         ; }
  public:
    mutable WeightInner m_weight ;
  } ;
  
} // end of namespace relations 
// ============================================================================
// The END 
// ============================================================================
#endif // RELATIONS_WENTRY_H
// ============================================================================
