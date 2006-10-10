// $Id: UniqueKeeper.h,v 1.4 2006-10-10 09:03:21 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2006/05/02 14:29:10  ibelyaev
//  censored
//
// ============================================================================
#ifndef LOKI_UNIQUEKEEPER_H 
#define LOKI_UNIQUEKEEPER_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <set>
#include <algorithm>
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
 *  @date 2006-02-20 
 */
// ============================================================================

namespace LoKi 
{  
  template <class OBJECT> class Keeper ;
  
  /** @class UniqueKeeper UniqueKeeper.h LoKi/UniqueKeeper.h
   *  
   *  Usefull class to keep valid UNIQUE pointer to the 
   *  objects. It takes care about the const-access 
   *  and the checked modifications.
   *  By construction, the pointers are ALWAYS valid 
   *
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-02-21
   */
  template <class OBJECT>
  class UniqueKeeper 
  {
  public: 
    typedef typename std::set<const OBJECT*>                  Objects ;
    typedef typename Objects::const_iterator                 iterator ;
    typedef iterator                                   const_iterator ;
    typedef typename Objects::const_reverse_iterator reverse_iterator ;
    typedef reverse_iterator                   const_reverse_iterator ;
    typedef typename Objects::value_type                   value_type ;
  public:
    /// the defualt constructor 
    UniqueKeeper() : m_objects() {}
    /// constructor from one objects
    UniqueKeeper ( const OBJECT* o ) 
      : m_objects() { addObject( o ) ; }
    /// constructor form objects 
    UniqueKeeper ( const Objects& objects )
      : m_objects ( objects ) { removeObject() ; }
    /// templated constructor form other unique keeper 
    template <class OTHER> 
    UniqueKeeper ( const UniqueKeeper<OTHER>& o ) 
      : m_objects ( o.begin() , o.end() ) {}
    /// templated constructor form other keeper 
    template <class OTHER> 
    UniqueKeeper ( const Keeper<OTHER>& o ) ;
    /// templated constructor from sequence
    template <class OTHER> 
    UniqueKeeper ( OTHER first , 
                   OTHER last  ) 
      : m_objects ( first , last ) { removeObject() ; }
    /// templated constructor from the sequence
    template <class OTHER,class PREDICATE> 
    UniqueKeeper ( OTHER     first , 
                   OTHER     last  , 
                   PREDICATE cut   ) 
      : m_objects () 
    { addObjects ( first , last , cut ) ; } ;
    /// protected destructor 
    ~UniqueKeeper() { m_objects.clear() ; }    
  public:
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
  public:
    /// get the object by index (it is not an optimal operation!)
    const OBJECT*  object     ( const int index ) const 
    {
      iterator it = begin() ;
      std::advance ( it , index ) ;
      return  *it ; 
    } ;
    /// get the object by index 
    const OBJECT*  operator() ( const int index ) const 
    { return object ( index ) ; }
    /// get the object by index 
    const OBJECT*  operator[] ( const int index ) const 
    { return object ( index ) ; }
    /// get the object by index 
    const OBJECT*  at         ( const int index ) const 
    { return object ( index ) ; }
  public:
    /// append the valid objects to the end 
    void push_back ( const OBJECT* o ) { addObject ( o ) ; } ;
    /** insert object into the container
     *  @param o object to be inserted 
     */
    void insert    ( const OBJECT* o ) { addObject ( o ) ; } ;
    /** insert the object at the fake position 
     *  (added to mimic STL behaviour) 
     *  @param o object to be inserted 
     */
    template <class POSITION>
    void insert
    ( POSITION      /* p */ ,
      const OBJECT*    o    ) { addObject ( o ) ; }
    /** insert the sequence of objects at the fake position 
     *  (added to mimic STL behaviour) 
     *  @param first 'begin'-iterator to the sequence 
     *  @param last   'end'-iterator to the sequence 
     */
    template <class POSITION, class OTHER>
    void insert
    ( POSITION /* p */ , 
      OTHER    first   , 
      OTHER    last    ) { addObjects ( first , last ) ; }
    /** insert the sequence of objects at the fake position 
     *  (added to mimic STL behaviour) 
     *  @param first 'begin'-iterator to the sequence 
     *  @param last   'end'-iterator to the sequence 
     *  @param cut selection criteria 
     */
    template <class POSITION, class OTHER,class PREDICATE>
    void insert
    ( POSITION /* p */ , 
      OTHER     first  , 
      OTHER     last   , 
      PREDICATE cut    ) { addObjects ( first , last , cut ) ; }
  public:
    /// adding an object 
    UniqueKeeper& operator+= ( const OBJECT* o ) 
    {
      addObject ( o ) ;
      return *this ;
    } ;
    /// adding the another keeper 
    template <class OTHER>
    UniqueKeeper& operator+= ( const UniqueKeeper<OTHER>& o ) 
    {
      addObjects( o.begin() , o.end() ) ;
      return *this ;
    } ;
    /// adding the another keeper 
    template <class OTHER>
    UniqueKeeper& operator+= ( const Keeper<OTHER>& o ) ;
    /// remove the objecr 
    UniqueKeeper& operator-= ( const OBJECT* o ) 
    {
      removeObject ( o ) ;
      return *this ;
    } ;
  public:
    /// accessor to the objects (const)
    const   Objects&   objects() const { return m_objects ; }
  public:
    /** add the object to the container 
     *  @param o objects to be added 
     *  @return the actual size of container 
     */
    size_t addObject ( const OBJECT* o ) 
    {
      if ( 0 != o ) { m_objects.insert ( o ) ; }
      return size() ;
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
      m_objects.insert( first , last ) ;
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
      for ( ; first != last ; ++first ) 
      { if ( cut ( *first ) ) { addObject( *first ) ; } } ;
      return size() ;
    } ;    
    /** remove the object for the container 
     *  @param o object to be removed 
     *  @return the actual size of container 
     */
    size_t removeObject ( const OBJECT* o = 0 )
    {
      typename Objects::iterator i0 = m_objects.find( o ) ;
      if ( m_objects.end() != i0 ) { m_objects.erase( i0 ) ; }
      return size() ;
    }
  public:
    /// sort the container using the default ordering of pointers 
    void   sort   () {}
    /// remove the duplicates from the container 
    size_t unique () { return size() ; }
  private:
    UniqueKeeper& operator=( const UniqueKeeper& other ) ;
  private:
    Objects m_objects ;
  };
  
  
}; // end of the namespace LoKi


// ============================================================================
// "POST"-include 
// ============================================================================
#include "LoKi/Keeper.h"
// ============================================================================

// ============================================================================
/// templated constructor form other keeper 
// ============================================================================
template <class OBJECT>
template <class OTHER> 
inline LoKi::UniqueKeeper<OBJECT>::UniqueKeeper 
( const LoKi::Keeper<OTHER>& o ) 
  : m_objects ( o.begin() , o.end() ) 
{}
// ============================================================================
/// adding the another keeper 
// ============================================================================
template <class OBJECT>
template <class OTHER>
inline LoKi::UniqueKeeper<OBJECT>& 
LoKi::UniqueKeeper<OBJECT>::operator+= 
( const LoKi::Keeper<OTHER>& o ) 
{
  addObjects( o.begin() , o.end() ) ;
  return *this ;
} ;
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_UNIQUEKEEPER_H
// ============================================================================
