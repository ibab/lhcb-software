// $Id: Keeper.h,v 1.11 2007-06-01 11:35:26 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.11 $ 
// ============================================================================
// $Log: not supported by cvs2svn $
//
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
// LoKiCore 
// ============================================================================
#include "LoKi/Algs.h"
#include "LoKi/KeeperBase.h"
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
  template <class OBJECT> class UniqueKeeper ;  
  /** @class Keeper Keeper.h LoKi/Keeper.h
   *  
   *  Usefull class to keep valid pointers to the 
   *  objects. It takes care about the const-access 
   *  and the checked modifications.
   *  By construction, the pointers are ALWAYS valid 
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-02-20
   */
  template <class OBJECT>
  class Keeper : public KeeperBase
  {
  public:
    typedef std::vector<const OBJECT*>                        Objects ;
    typedef typename Objects::const_iterator                 iterator ;
    typedef iterator                                   const_iterator ;
    typedef typename Objects::const_reverse_iterator reverse_iterator ;
    typedef reverse_iterator                   const_reverse_iterator ;
    typedef typename Objects::value_type                   value_type ;
  public:
    /// default constructor 
    Keeper() : m_objects() {}
    /// constructor form one object
    Keeper ( const OBJECT* o )
      : m_objects () { addObject ( o ) ; }
    /// constructor form the objects 
    Keeper ( const Objects& objects ) 
      : m_objects( objects ) { removeObject() ; }
    /// templated constructor from another keeper
    template <class OTHER>
    Keeper( const Keeper<OTHER>& o ) 
      : m_objects ( o.begin() , o.end() ) {}
    /// templated constructor from another keeper
    template <class OTHER>
    Keeper( const UniqueKeeper<OTHER>& o ) ;
    /// templated constructor from sequence
    template <class OTHER> 
    Keeper ( OTHER first , 
             OTHER last  ) 
      : m_objects ( first , last ) { removeObject() ; }
    /// templated constructor from the sequence
    template <class OTHER,class PREDICATE> 
    Keeper ( OTHER     first , 
             OTHER     last  , 
             PREDICATE cut   ) 
      : m_objects () { addObjects ( first , last , cut ) ; } ;
    /// protected destructor 
    ~Keeper() { m_objects.clear() ; }
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
    /// make the conversion to the vector (useful for iteration in python)
    const std::vector<const OBJECT*>& toVector () const { return m_objects ; }
  public:
    /// clear the underlying container 
    void clear() { m_objects.clear() ; }
  public:
    /// get the object by index 
    const OBJECT*  object     ( const int index ) const 
    { return  *(begin()+index) ; }
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
    /// get a "slice" of the keeper, in Python style   
    inline Keeper slice( long index1 , long index2 ) const 
    {
      // trivial cases 
      if ( empty() || index1 == index2 ) { return Keeper() ; } // RETURN
      // adjust indices 
      if ( index1 < 0      ) { index1 += size () ; }
      if ( index2 < 0      ) { index2 += size () ; }
      // check 
      if ( index1 < 0      ) { return  Keeper () ; }            // RETURN 
      if ( index2 < index1 ) { return  Keeper () ; }            // RETURN 
      // adjust
      if ( index1 > (long) size () ) { return  Keeper() ; }     // RETURN
      if ( index2 > (long) size () ) { index2  = size() ; }
      // construct the final keeper 
      return Keeper( begin() + index1 , begin() + index2 ) ;    // RETURN 
    }
  public:
    // append the valid objects to the end 
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
    Keeper& operator+= ( const OBJECT* o ) 
    {
      addObject ( o ) ;
      return *this ;
    } ;
    /// adding the another keeper 
    template <class OTHER>
    Keeper& operator+= ( const Keeper<OTHER>& o ) 
    {
      addObjects( o.begin() , o.end() ) ;
      return *this ;
    } ;
    /// adding the another keeper 
    template <class OTHER>
    Keeper& operator+= ( const UniqueKeeper<OTHER>& o ) ;
    /// remove the objecr 
    Keeper& operator-= ( const OBJECT* o ) 
    {
      removeObject ( o ) ;
      return *this ;
    } ;
  public:
    /// accessor to the objects (const)
    const   Objects&   objects() const { return m_objects           ; }
  public:
    /** add the object to the container 
     *  @param o objects to be added 
     *  @return the actual size of container 
     */
    size_t addObject ( const OBJECT* o ) 
    {
      if ( 0 != o ) { m_objects.push_back ( o ) ; }
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
      return size() ;
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
      return size() ;
    }
  public:
    /** sort the container using the sorting criteria 
     *  @param cmp operation used as corting criteria ("strickt less") 
     */
    template <class COMPARISON>
    void sort( COMPARISON cmp ) 
    { std::sort ( m_objects.begin() , m_objects.end() , cmp ) ; }
    /// sort the container using the default ordering of pointers 
    void sort() 
    { std::sort ( m_objects.begin() , m_objects.end() ) ; }
    /** remove the duplicates from the container 
     *  @param cmp comparison criteria used for sorting 
     */
    template <class COMPARISON>
    size_t unique ( COMPARISON cmp ) 
    {
      sort ( cmp ) ;
      m_objects.erase 
        ( std::unique ( m_objects.begin () , 
                        m_objects.end  () , cmp ) , m_objects.end() ) ;
      return size() ;
    } ;
    /// remove the duplicates from the container 
    size_t unique () 
    {
      sort () ;
      m_objects.erase 
        ( std::unique ( m_objects.begin () , 
                        m_objects.end  () ) , m_objects.end() ) ;
      return size() ;
    } ;
  private:
    // assignemenet oprator is disabled 
    Keeper& operator=( const Keeper& ) ;
  private:
    // the actual container of object
    Objects  m_objects ; ///< container of object
  } ;
}  // end of namespace LoKi
// ============================================================================
// "POST"-include 
// ============================================================================
#include "LoKi/UniqueKeeper.h"
// ============================================================================
/// templated constructor form other keeper 
// ============================================================================
template <class OBJECT>
template <class OTHER>
inline LoKi::Keeper<OBJECT>::Keeper
( const LoKi::UniqueKeeper<OTHER>& o ) 
  : m_objects ( o.begin() , o.end() ) 
{}
// ============================================================================
/// adding the another keeper 
// ============================================================================
template <class OBJECT>
template <class OTHER>
inline 
LoKi::Keeper<OBJECT>& 
LoKi::Keeper<OBJECT>::operator+= 
( const LoKi::UniqueKeeper<OTHER>& o ) 
{
  addObjects ( o.begin() , o.end() ) ;
  return *this ;
} ;
// ============================================================================
template <class OBJECT,class OTHER>
inline 
LoKi::Keeper<OBJECT> operator+ 
(  const LoKi::Keeper<OBJECT>& first  , 
   const LoKi::Keeper<OTHER>&  second )
{
  LoKi::Keeper<OBJECT> result( first ) ;
  return result += second ;
} ;
// ============================================================================
template <class OBJECT, class OTHER>
inline 
LoKi::Keeper<OBJECT> operator+ 
(  const LoKi::Keeper<OBJECT>&       first  , 
   const LoKi::UniqueKeeper<OTHER>&  second )
{
  LoKi::Keeper<OBJECT> result( first ) ;
  result.addObjects ( second.begin() , second.end() ) ;  
  return result ;
} ;
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_KEEPER_H
// ============================================================================
