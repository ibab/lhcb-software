// $Id: CaloVector.h,v 1.9 2010-03-18 13:52:39 ibelyaev Exp $ 
// ============================================================================
#ifndef     CALOKERNEL_CALOVECTOR_H
#define     CALOKERNEL_CALOVECTOR_H 1 
// ============================================================================
// from STL 
// ============================================================================
#include <vector> 
// ============================================================================
// from Gaudi 
// ============================================================================
#include "GaudiKernel/Kernel.h"     
#include "GaudiKernel/StatusCode.h" 
// ============================================================================
// Boost 
#include "boost/call_traits.hpp"
// ============================================================================
// forward declaration
namespace LHCb {
  class CaloCellID;                  
}
/** @class CaloVector CaloVector.h CaloKernel/CaloVector.h 
 *  
 *  A dedicated vector for Calo information accessed by CellID.
 *  Allows normal sequencial access, as this inherits form std::vector.
 *  But it allows at the same time direct access by CellID.
 *  Based on the original CaloCollection implementation by Vanya Belyaev    
 *  This is a vector, plus some functions for direct access, and of course 
 *  methods to load the internal vector.
 *  
 *  @author Olivier Callot 
 *  @date XX/XX/XX 
 */

template <class CONTENT, class INDEX_ = LHCb::CaloCellID>
class CaloVector : private std::vector<CONTENT> 
{
public:
  /// type fo content 
  typedef    CONTENT                              Content                ;   
  /// own type 
  typedef    std::vector<CONTENT>                 ContVector             ;
  typedef ContVector                              Vector                 ;
  typedef typename Vector::iterator               iterator               ;
  typedef typename Vector::const_iterator         const_iterator         ;
  typedef typename Vector::reverse_iterator       reverse_iterator       ;
  typedef typename Vector::const_reverse_iterator const_reverse_iterator ;
  typedef std::vector<int>                        Indices                ;
  
  /// type for the argument 
  typedef typename boost::call_traits<const INDEX_>::param_type INDEX ;
  
public: 
  
  /** constructor: 
   *  One specifies a default value, 
   *  returned when the index is invalid or non existent.
   *  @param   def   default value to be returned for wrong/non-existing index 
   */
  CaloVector ( Content  def = Content() )  ///< default value
    : ContVector (   0 )
    , m_cc_def   ( def )
    , m_index    (   0 )
  { }
  
  /// (virtual) destructor
  virtual ~CaloVector() { this -> clear(); }
  
  /// following lines from std::vector interface 
  
  /// sequential access to content container (const version!)
  const_iterator             begin () const { return Vector::begin  (); }
  /// sequential access to content container (const version!)
  const_iterator             end   () const { return Vector::end    (); }
  /// sequential access to content container (const version!)
  const_reverse_iterator     rbegin() const { return Vector::rbegin (); }
  /// sequential access to content container (const version!)
  const_reverse_iterator     rend  () const { return Vector::rend   (); }
  /// size of content container 
  typename Vector::size_type size  () const { return Vector::size   (); }
  /// clear the container 
  void clear () { Vector::clear() ;  m_index.clear(); }  
  
  /** returns the position in the vector, -1 if not present
   *  @param  id index 
   */
  int index ( INDEX id ) const 
  {
    const unsigned int indx = id.index();
    return ( ( m_index.size() > indx ) ? m_index[indx] : -1 ); 
  }
  
  /** Access to the content itself using LHCb::CaloCellID as index. 
   *  Check the boundaries of the index array. 
   *  @param id index 
   */                                         
  inline       Content& operator[]  ( INDEX id )       
  { 
    const int indx = index( id );
    return ( ( 0 > indx ) ? def() : *(begin()+indx) ) ; 
  }
  
  /** Access to the content itself using LHCb::CaloCellID as index.
   *  (const version)
   *  Check the boundaries of the index array. 
   *  @param id index 
   */                                         
  inline const Content& operator[]  ( INDEX id ) const 
  { 
    const int indx = index( id );
    return ( ( 0 > indx ) ? def() : *(begin()+indx) ) ; 
  }
  
  /** Access to the content itself using LHCb::CaloCellID as index. 
   *  Check the boundaries of the index array. 
   *  @param id index 
   */                                         
  inline       Content& operator()  ( INDEX id )       
  { return (*this)[ id ] ; }
  
  /** Access to the content itself using LHCb::CaloCellID as index.
   *  (const version)
   *  Check the boundaries of the index array. 
   *  @param id index 
   */                                         
  inline const Content& operator()  ( INDEX id ) const 
  { return (*this)[ id ] ; }
  
  /** Add one entry. vector is extended, index vector is updated.
   *  @param content   value to be added into CaloVector 
   *  @param id        index 
   *  @return          status code 
   */
  void addEntry ( const Content& content , INDEX id ) 
  {
    const unsigned int indx = id.index() ;
    if ( m_index.size() <= indx  ) 
    {
      const Indices::size_type  num = indx + 1 - m_index.size() ;
      const Indices::value_type val = -1 ;
      m_index.insert ( m_index.end() , num  , val ) ; 
    }
    m_index[indx] = this -> size();
    this -> push_back( content );
  }
  
  /// set size for intermediate container
  void setSize ( const unsigned int Num )
  { m_index.reserve( Num ) ; }
  
  /// extended "reserve" 
  void reserve ( const unsigned int Num ) 
  { Vector::reserve( Num ) ;  m_index.reserve( Num ) ; }

public:
  
  /// sequential access to content container 
  iterator               begin ()       { return Vector::begin  (); }
  /// sequential access to content container 
  iterator               end   ()       { return Vector::end    (); }
  /// sequential access to content container 
  reverse_iterator       rbegin()       { return Vector::rbegin (); }
  /// sequential access to content container 
  reverse_iterator       rend  ()       { return Vector::rend   (); }
  
public:
  // ==========================================================================
  /// access to container (useful in python for iteration)
  inline const Content& at ( const size_t i ) const 
  { return Vector::at ( i ) ; }
  // ==========================================================================
protected:

  /// get default value 
  inline       Content& def()       { return m_cc_def; }
  /// get default value (const version)
  inline const Content& def() const { return m_cc_def; } 
  
private:
  
  /// copying is not allowed!
  CaloVector( const CaloVector& ); 
  /// assignment  is not allowed!
  CaloVector& operator =( const CaloVector& ); 
  
private: 
  
  Content  m_cc_def ;   ///< default value 
  Indices  m_index  ;   ///< array of indices in the vector

};

// ============================================================================
// The End 
// ============================================================================
#endif  ///<  CALOKERNEL_CALOVECTOR_H 
// ============================================================================







