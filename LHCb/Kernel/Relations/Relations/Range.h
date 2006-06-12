// $Id: Range.h,v 1.2 2006-06-12 15:27:28 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, verison $Revision: 1.2 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2006/06/11 15:23:45  ibelyaev
//  The major  upgrade: see doc/release.notes
// 
// ============================================================================
#ifndef RELATIONS_RANGE_H 
#define RELATIONS_RANGE_H 1
// ============================================================================
// Include files
// ============================================================================
// Relations 
// ============================================================================
#include "Relations/Bases.h"
// ============================================================================

namespace Relations 
{
  /** @struct Range_
   *
   *  An auxillary structure to provide a little bit
   *  better access to the components of standard
   *  @p std::pair class
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   27/01/2002
   */
  template <class ENTRIES>
  struct Range_ : public Relations::BaseRange 
  {
    // useful types 
    typedef typename ENTRIES::const_iterator         iterator               ;
    typedef typename ENTRIES::const_iterator         const_iterator         ;
    typedef typename ENTRIES::const_reverse_iterator const_reverse_iterator ;
    typedef typename ENTRIES::const_reverse_iterator reverse_iterator       ;
    typedef typename ENTRIES::const_reference        reference              ;
    typedef typename ENTRIES::const_reference        const_reference        ;
    typedef typename ENTRIES::value_type             value_type             ;
    /// default constructor
    Range_ () 
      : Relations::BaseRange() , m_begin() , m_end() {} ;
    /// constructor
    Range_ ( iterator begin , iterator end ) 
      : Relations::BaseRange() 
      , m_begin ( begin ) 
      , m_end   ( end   ) {} ;
    Range_ ( const ENTRIES& e ) 
      : Relations::BaseRange() 
      , m_begin ( e.begin () ) 
      , m_end   ( e.end   () ) {}
    template <class T1,class T2> 
    Range_ ( const std::pair<T1,T2>& r ) 
      : Relations::BaseRange() 
      , m_begin ( r.first  ) 
      , m_end   ( r.second ) {}
    /// begin-iterator (    const version)
    iterator         begin  () const { return this->m_begin ; }
    /// end-iterator   (    const version)
    iterator         end    () const { return this->m_end   ; }
    /// reverse iterator "begin"
    reverse_iterator rbegin () const
    { return const_reverse_iterator ( end   () ) ; }
    /// reverse iterator "end"
    reverse_iterator rend   () const 
    { return const_reverse_iterator ( begin () ) ; }
    /// front element (applied only for 'valid' ranges)
    const_reference  front  () const { return *begin() ; }
    /// back  element (applied only for 'valid' ranges)
    const_reference  back   () const { return (*this)( size() - 1 ) ; }
    /// number of relations 
    size_t           size   () const { return end () -  begin ()    ; }
    /// empty?
    bool             empty  () const { return end () == begin ()    ; }
    /// the access to the items by index 
    const_reference  operator() ( const size_t index ) const
    { return *( begin() + index ) ; } ;
    /// the access to the items by index
    const_reference  operator[] ( const size_t index ) const
    { return (*this) ( index ) ; } ;
    /// the access to the items by index 
    const_reference  at         ( const size_t index ) const
    { return (*this) ( index ) ; } ;
  private:
    iterator m_begin ;
    iterator m_end   ;
  };    
} // end of namespace Relatios

// ============================================================================
// The END 
// ============================================================================
#endif // RELATIONS_RANGE_H
// ============================================================================
