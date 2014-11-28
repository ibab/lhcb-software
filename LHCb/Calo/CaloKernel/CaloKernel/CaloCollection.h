// $Id: CaloCollection.h,v 1.5 2006-06-14 15:54:50 ibelyaev Exp $
// ============================================================================ 
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================ 
// $Log: not supported by cvs2svn $
// ============================================================================ 
#ifndef     CALOKERNEL_CALOCOLLECTION_H
#define     CALOKERNEL_CALOCOLLECTION_H 1  
// ============================================================================ 
// from STL 
// ============================================================================ 
#include <iostream> 
#include <string> 
#include <vector> 
#include <functional> 
// ============================================================================ 
// from Gaudi 
// ============================================================================ 
#include "GaudiKernel/Kernel.h"     
#include "GaudiKernel/StatusCode.h" 
#include "GaudiKernel/MsgStream.h" 
// ============================================================================ 
// Kernel 
// ============================================================================ 
#include "Kernel/CaloCellID.h"
// ============================================================================ 
// from CaloKernel Package 
// ============================================================================ 
#include "CaloKernel/CaloException.h" 
// ============================================================================ 

/** @class CaloCollection CaloCollection.h CaloKernel/CaloCollection.h
 *
 *  a simple adaptor class for implementing the 
 *  container with direct access of the data using LHCb::CaloCellID as an index.
 *  It has 2 interfaces  
 *   - (1) from container (presumably with random-access-iterator (but 
 *  due to smart definition of operator[] even list is acceptable
 *  :-)) and therefore can be used as STL-container
 *  - (2) from std::unary_function, and therefore can be directly 
 *  considered as "functor" for any STL-sequence and algorithm 
 *  (the latter it especially important for container of 
 *  pointers to functions, or using in the "std::transform"
 *  of sequence of LHCb::CaloCellIDs to the sequence of "Content"
 *  
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date    26/11/1999
 */

template 
<
  class CONTENT                          ,  ///< type of content 
  class RETTYPE   = CONTENT              ,  ///< return type     
  class CONTAINER = std::vector<CONTENT> ,  ///< container type  
  class FUNCTOR   = std::unary_function<const LHCb::CaloCellID&,RETTYPE&>  ///< functor
>
class CaloCollection :  public CONTAINER ,  public FUNCTOR 
{
  //
  public: 
  //
  typedef    CONTENT                        Content    ;   
  typedef    RETTYPE                        ReturnType ; 
  typedef    const LHCb::CaloCellID&              Index      ; 
  typedef    typename CONTAINER::size_type  size_type  ; 
  //
public:
  //
  /** constructor
      @param def   the default return value 
      @param num   initial size 
      @param messageService   pointer to message service 
  */
  CaloCollection
  ( Content def        = Content() ,  /* "default" value */
    const unsigned num = 0         ,  /* initial size */    
    IMessageSvc* messageService = 0         )
    : CONTAINER             ( num , def      )
    , m_cc_def              ( def            )
    , m_cc_messageService   ( messageService ) 
  { };
  /// (virtual) destructor 
  virtual ~CaloCollection() { CONTAINER::clear(); };
  //
  public: 
  
  // CONTAINER
  // access to CONTAINER interfce:
  /// access to the content  itself using LHCb::CaloCellID as an index
  inline       Content& operator[]( Index id )       
  { 
    size_type indx = id.index();
    return ( (indx < CONTAINER::size())? *(CONTAINER::begin()+indx) : def() ); 
  };
  /// the "const" version of access to the content  using LHCb::CaloCellID
  inline const Content& operator[]( Index id ) const 
  { 
    size_type indx = id.index();
    return ( (indx < CONTAINER::size())? *(CONTAINER::begin()+indx) : def() ); 
  };
  /// checked access, need to be catched!
  virtual inline  Content&     at( Index id ) 
  {
    size_type indx = id.index(); 
    if ( indx >= CONTAINER::size() ) 
    { throw CaloException("CaloCollection::at() - out_of_range()");} 
    return *(CONTAINER::begin()+indx); 
  }
  /// the "const" version of checked access, need to be catched!
  virtual inline const Content&      at( Index id ) const  
  { 
    size_type indx = id.index(); 
    if ( indx >= CONTAINER::size() ) 
    { throw CaloException("CaloCollection::at() - out_of_range()");} 
    return *(CONTAINER::begin()+indx); 
  }
  /// FUNCTOR! 
  /// access to FUNCTOR facilities
  /// please, pay some attention that return type here CAN BE DIFFERENT from 
  /// the return type of operator [] ! 
  /// this trick could be  heavily used in Geometry business
  virtual       ReturnType& operator() (Index id)       { return (*this)[id];};
  virtual const ReturnType& operator() (Index id) const { return (*this)[id];};
  // printout 
  virtual  std::ostream& fillStream( std::ostream& os ) const 
  { 
    os << " class CaloCollection: "
       << " size=" << CONTAINER::size()
       << "number of  non-default elements="<< CONTAINER::size() - 
      std::count ( CONTAINER::begin() , CONTAINER::end(), def() ) << std::endl ;
    return os ; 
  } ;
  ///
  inline  StatusCode addEntry ( const Content& content , Index id ) 
  {
    size_type indx = id.index(); 
    while ( CONTAINER::size() <= indx ) { CONTAINER::push_back( def() ); }
    *(CONTAINER::begin()+indx) = content; 
    return StatusCode::SUCCESS;
  };
  ///
protected:
  ///
  inline       Content&     def    ()       { return m_cc_def            ; };
  inline const Content&     def    () const { return m_cc_def            ; };
  inline       IMessageSvc* msgSvc () const { return m_cc_messageService ; };
  ///
private:
  /// copy constructor! copying is not allowed! 
  CaloCollection            ( const CaloCollection& );
  /// assignment!       assignment is not allowed! 
  CaloCollection& operator =( const CaloCollection& ); 
  ///
private: 
  ///  
  Content                   m_cc_def;             // "def value" 
  IMessageSvc*              m_cc_messageService;  // Message Service 
  ///
};
// ============================================================================
template <class T1,class T2,class T3,class T4>
inline std::ostream& 
operator <<( std::ostream& os , const CaloCollection<T1,T2,T3,T4>& cc ) 
{ return cc.fillStream ( os ) ; }
// ============================================================================
template <class T1,class T2,class T3,class T4>
inline std::ostream& 
operator <<( std::ostream& os , const CaloCollection<T1,T2,T3,T4>* cc ) 
{ return ( ( 0 == cc ) ? os << " CaloCollection* points to NULL!" : os << *cc); }
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
#endif  //  CALOKERNEL_CALOCOLLECTION_H
// ============================================================================





