// $Id: RichVector.h,v 1.2 2002-04-18 11:43:46 cattanem Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/04/04 14:33:51  jonrob
// First version of Rich Kernel package
//
// Revision 1.1.1.1  2001/11/25 14:07:38  ibelyaev
// New Package: substitution of the  previous RichGen package
//
// Revision 1.8  2001/10/30 11:30:04  ibelyaev
// update for new compiler, RichHypotheses namespace is added
//
// Revision 1.7  2001/07/12 21:52:05  ibelyaev
// minor change to please Win2k
//
// Revision 1.6  2001/07/02 17:49:10  ibelyaev
// update in RichException and RichVector
//
// Revision 1.5  2001/06/23 14:39:31  ibelyaev
//  fix CVS-keywords and remove long lines
//
// ============================================================================
#ifndef     RICHKERNEL_RICHVECTOR_H
#define     RICHKERNEL_RICHVECTOR_H 1
// ============================================================================
// from STL 
#include <vector> 
// from Gaudi 
#include "GaudiKernel/Kernel.h"     
#include "GaudiKernel/StatusCode.h" 
// forward declaration
class RichSmartID;

/** @class RichVector RichVector.h RichKernel/RichVector.h
 *  
 *  A dedicated vector for Rich information accessed by SmartID.
 *  Allows normal sequencial access, as this inherits form std::vector.
 *  But it allows at the same time direct access by SmartID.
 *  Based on the original RichCollection implementation by Vanya Belyaev
 *  This is a vector, plus some functions for direct access, and of course 
 *  methods to load the internal vector.
 *  
 *  @author Andy Presland
 *  @date XX/XX/XX 
 */

template <class CONTENT, class INDEX   = const RichSmartID&  >
class RichVector : private std::vector<CONTENT>
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
  
public: 
  
  /** constructor: 
      One specifies a default value, 
      returned when the index is invalid or non existent.
      @param   def   default value to be returned for wrong/non-existing index 
  */
  RichVector ( Content  def = Content() )  ///< default value
    : ContVector (   0 )
    , m_cc_def   ( def )
    , m_index    (   0 )
  { };
  
  /// (virtual) destructor
  virtual ~RichVector() { clear (); }
  
  /**
     following lines from std::vector interface 
  */
  
  /// sequential access to content container 
  inline iterator               begin ()       { return Vector::begin  (); }
  /// sequential access to content container (const version!)
  inline const_iterator         begin () const { return Vector::begin  (); }
  /// sequential access to content container 
  inline iterator               end   ()       { return Vector::end    (); }
  /// sequential access to content container (const version!)
  inline const_iterator         end   () const { return Vector::end    (); }
  /// sequential access to content container 
  inline reverse_iterator       rbegin()       { return Vector::rbegin (); }
  /// sequential access to content container (const version!)
  inline const_reverse_iterator rbegin() const { return Vector::rbegin (); }
  /// sequential access to content container 
  inline reverse_iterator       rend  ()       { return Vector::rend   (); }
  /// sequential access to content container (const version!)
  inline const_reverse_iterator rend  () const { return Vector::rend   (); }
  /// size of content container 
  inline Vector::size_type      size  () const { return Vector::size   (); }
  /// clear the container 
  inline void                   clear ()      
  { Vector::clear() ;  m_index.clear(); }
  
  
  /** returns the position in the vector, -1 if not present
      @param  id index 
  */
  inline const int index( INDEX id ) const 
  {
    unsigned int indx = id.index();
    return ( ( m_index.size() > indx ) ? m_index[indx] : -1 ); 
  };

  /**  Access to the content itself using RichSmartID as index.
       Check the boundaries of the index array. 
       @param id index 
  */                                         
  inline       Content& operator[]  ( INDEX id )       
  { 
    int indx = index( id );
    return ( (0 <= indx ) ? *(begin()+indx) : def() ); 
  };

  /**  Access to the content itself using RichSmartID as index.
       (const version)
       Check the boundaries of the index array. 
       @param id index 
  */                                         
  inline const Content& operator[]  ( INDEX id ) const { 
    int indx = index( id );
    return ( (0 <= indx ) ? *(begin()+indx) : def() ); 
  };

  /**  Add one entry. vector is extended, index vector is updated.
       @param content   value to be added into RichVector
       @param id        index 
       @return          status code 
  */
  inline       StatusCode  addEntry ( const Content& content , INDEX id ) 
  {
    unsigned int indx = id.index() ; 
    while( m_index.size() <= indx ) { m_index.push_back( -1 ); }
    m_index[indx] =  size();
    push_back( content );
    return StatusCode::SUCCESS;
  };

  /**  set size for intermediate container
       standart "reserve" acts on base container!
  */
  inline void setSize( const unsigned int Num )
  { m_index.reserve( Num ) ; }
  
protected:

  /// get default value 
  inline       Content& def()       { return m_cc_def; };
  /// get default value (const version)
  inline const Content& def() const { return m_cc_def; };    
  
private:
  
  /// copying is not allowed!
  RichVector( const RichVector& );
  /// assignment  is not allowed!
  RichVector& operator =( const RichVector& );
  
private: 
  
  Content          m_cc_def ;   ///< default value 
  std::vector<int> m_index  ;   ///< array of indices in the vector

};

// ============================================================================
// The End 
// ============================================================================
#endif  ///<  RICHKERNEL_RICHVECTOR_H
// ============================================================================







