// $Id: Selected.h,v 1.5 2006-11-25 19:12:56 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.5 $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2006/05/02 14:29:10  ibelyaev
//  censored
//
// ============================================================================
#ifndef LOKI_SELECTED_H 
#define LOKI_SELECTED_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <algorithm>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/HashMap.h"
// ============================================================================
// LoKi 
// ============================================================================
#include "LoKi/NamedRange.h"
#include "LoKi/select.h"
// ============================================================================

// ============================================================================
/** @file
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23 
 */
// ============================================================================

namespace LoKi
{  
  /** @class Selected_ Selected.h LoKi/Selected.h
   * 
   *  Storage of "Selected"/"Useful"/"Tagged" objects
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-24
   */
  template <class CONTAINER>
  class Selected_ 
  {
  public:
    
    /// actual container type 
    typedef          CONTAINER                              Container ;
    /// underlying map 
    typedef typename GaudiUtils::HashMap<std::string,Container> Map   ;
    /// return type 
    typedef typename LoKi::NamedRange_<Container>           Range     ;
    /// actual iterator type 
    typedef typename Range::iterator                        iterator  ;
    /// own type 
    typedef          Selected_<Container>                   _Self     ;
    
    typedef typename Map::const_iterator                    map_iterator ;
    typedef typename Map::const_iterator                    mao_iterator ;
    
  public:
    
    /// Standard constructor
    Selected_() : m_map() {}
    
    // destructor 
    virtual ~Selected_() { m_map.clear() ; } ;
    
    /** get all particled tagged objects
     *  @param name  tag to be selected
     *  @return selected range of objects 
     */
    Range get( const std::string& name ) const 
    {
      const Container& cont = m_map( name );
      // return valid range 
      return Range( cont.begin() , cont.end() , name );
    };
    
    /** get all particled tagged objects
     *  @param name  tag to be selected
     *  @return selected range of objects 
     */
    Range operator() ( const std::string& name ) const 
    { return get( name ); };
    
    /** add objects to the tagged container 
     *  @param name     tag for objects 
     *  @param storage  storage of objects 
     *  @param cut      cut to be applied 
     *  @return selected range of objects 
     */
    template <class STORAGE, class CUT> 
    Range add( const std::string& name    , 
               const STORAGE&     storage , 
               const CUT&         cut     ) 
    { return add( name , storage.begin() , storage.end() , cut ) ; };
    
    /** add objects to the tagged container 
     *  @param name     tag for objects 
     *  @param first    begin of sequence of objects  
     *  @param last     end of sequece of object 
     *  @param cut      cut to be applied 
     *  @return selected range of objects 
     */
    template <class OBJECT, class CUT> 
    Range add( const std::string& name    , 
               OBJECT             first   , 
               OBJECT             last    , 
               const CUT&         cut     ) 
    {
      // get the container 
      Container& cont = m_map[ name ] ;
      // append approprate objects to the constainer
      LoKi::select( first , last  , std::back_inserter( cont ) , cut ) ;
      // return valid range 
      return Range( cont.begin() , cont.end() , name );      
    };
    
    /** add the object to the tagged container 
     *  @param name     tag for objects 
     *  @param storage  storage of objects 
     *  @param cut      cut to be applied 
     *  @return selected range of objects 
     */
    template <class OBJECT> 
    Range add( const std::string& name   , 
               const OBJECT&      object ) 
    {
      // get the container 
      Container& cont = m_map[ name ];
      // append object to the container 
      cont.push_back( object );
      // return valid range 
      return Range( cont.begin() , cont.end() , name );      
    };
    
    /// sequential access though iterators 
    map_iterator begin () const { return m_map.begin () ; }
    /// sequential access though iterators 
    map_iterator end   () const { return m_map.end   () ; }
    
    /// clear the selected particles 
    _Self& clear() { m_map.clear() ; return *this ;}
    
  private:
    
    /// copy constructor is desibled 
    Selected_        ( const _Self& );
    /// assignement      is desibled 
    _Self& operator= ( const _Self& );
    
  private:
    
    // actual representation of the storage 
    Map m_map;
    
  };
  
} // enfd of namespace LoKi

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_SELECTED_H
// ============================================================================
