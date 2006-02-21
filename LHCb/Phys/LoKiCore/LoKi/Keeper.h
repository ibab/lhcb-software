// $Id: Keeper.h,v 1.1 2006-02-21 14:45:51 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef LOKI_KEEPER_H 
#define LOKI_KEEPER_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <vector>
#include <algorithm>
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
#include "Event/Vertex.h"
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/Algs.h"
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
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot et al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-02-20 
 */
// ============================================================================
namespace LoKi
{
  /** @class Keeper Keeper.h LoKi/Keeper.h
   *  
   *  Usefull class to keep valid the pointer to the 
   *  objects. It takes care about the const-access 
   *  and the checked modifications.
   *  By construction, the pouinters are ALWAYS valid 
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-02-20
   */
  template <class OBJECT>
  class Keeper
  {
  public:
    typedef std::vector<const OBJECT*>                        Objects ;
    typedef typename Objects::const_iterator                 iterator ;
    typedef iterator                                   const_iterator ;
    typedef typename Objects::const_reverse_iterator reverse_iterator ;
    typedef reverse_iterator                   const_reverse_iterator ;
  protected:
    /// default constructor 
    Keeper() 
      : m_objects() {}
    /// constructor form the objects 
    Keeper ( const Objects& objects ) 
      : m_objects( objects ) 
    { removeObject() ; }
    /// templated constructor from sequence
    template <class OTHER> 
    Keeper ( OTHER first , 
             OTHER last  ) 
      : m_objects ( first , last ) 
    { removeObject() ; }
    /// templated constructor from sequence
    template <class OTHER,class PREDICATE> 
    Keeper ( OTHER     first , 
             OTHER     last  , 
             PREDICATE cut   ) 
      : m_objects () 
    { 
      LoKi::Algs::copy_if
        ( first , last , std::back_inserter( m_objects ) , cut );
      removeObject() ; 
    };
    /// protected destructor 
    ~Keeper() { m_objects.clear() ; }
  public:
    /// accessor to the objects 
    const Objects&   objects() const { return m_objects           ; }
    /// size of the container 
    size_t           size   () const { return m_objects.size   () ; }
    /// empty container ?
    bool             empty  () const { return m_objects.empty  () ; }
    //
    iterator         begin  () const { return m_objects.begin  () ; }
    //
    iterator         end    () const { return m_objects.end    () ; }
    //
    reverse_iterator rbegin () const { return m_objects.rbegin () ; }
    //
    reverse_iterator rend   () const { return m_objects.rend   () ; }
    //
   public:
    /** add the object to the container 
     *  @param o objects to be added 
     *  @return the actual size of container 
     */
    size_t addObject ( const OBJECT* o ) 
    {
      m_objects.push_back ( o ) ;
      return removeObject() ;
    } ;
    /** add the objects from the sequence to the container 
     *  @param first 'begin'-iterator of the sequence of objects 
     *  @param last  'end'-iterator of the sequence of objects 
     *  @return the actual size of container 
     */
    template<class OTHER>
    size_t addObjects 
    ( OTHER first , 
      OTHER last  ) 
    {
      m_objects.insert ( m_objects.end() , first , last ) ;
      return removeObject() ;
    } ;
    /** add the objects from the sequence to the container 
     *  @param first 'begin'-iterator of the sequence of objects 
     *  @param last  'end'-iterator of the sequence of objects 
     *  @param cut only objects which satisfy the criteri will be added 
     *  @return the actual size of container 
     */
    template<class OTHER, class PREDICATE>
    size_t addObjects 
    ( OTHER     first , 
      OTHER     last  , 
      PREDICATE cut ) 
    {
      LoKi::Algs::copy_if
        ( first , last , std::back_inserter ( m_objects ) , cut ) ;
      return removeObject() ;
    } ;
    /** remove the objects for the container 
     *  @param cut   objects which satisfy the criteri will be removed  
     *  @return the actual size of container 
     */
    template <class PREDICATE>
    size_t removeObjects ( PREDICATE cut ) 
    {
      m_objects.erase
        ( std::remove_if ( m_objects.begin () , 
                           m_objects.end   () , cut ) , m_objects.end() ) ;
      return m_objs.size() ;
    }
    /** remove the object for the container 
     *  @param o object to be removed 
     *  @return the actual size of container 
     */
    size_t removeObject ( const OBJECT* o = 0 )
    {
      m_objects.erase
        ( std::remove ( m_objects.begin () ,
                        m_objects.end   () , o ) , m_objects.end() ) ;
      return m_objects.size() ;
    }
  private:
    // the actual container of object
    Objects  m_objects ; ///< container of object
  } ;
  
} ; // end of namespace LoKi 


// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_KEEPER_H
// ============================================================================
