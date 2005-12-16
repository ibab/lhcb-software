// $Id: CaloCollection.h,v 1.4 2005-12-16 17:02:42 odescham Exp $
// ============================================================================ 
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================ 
// $Log: not supported by cvs2svn $
// Revision 1.3  2003/12/09 10:14:42  cattanem
// Changes for GAUDI_v13r0
//
// Revision 1.2  2002/03/18 18:16:21  ibelyaev
//  small update for LHCbKernel package
//
// Revision 1.1.1.1  2001/11/25 14:07:38  ibelyaev
// New Package: substitution of the  previous CaloGen package
//
// Revision 1.3  2001/06/23 14:39:31  ibelyaev
//  fix CVS-keywords and remove long lines
// ============================================================================ 
#ifndef     CALOKERNEL_CALOCOLLECTION_H
#define     CALOKERNEL_CALOCOLLECTION_H 1  
// from STL 
#include <iostream> 
#include <string> 
#include <vector> 
#include <functional> 
// from Gaudi 
#include "GaudiKernel/Kernel.h"     
#include "GaudiKernel/StatusCode.h" 
#include "GaudiKernel/MsgStream.h" 
// Kernel 
#include "Kernel/CaloCellID.h"
// from CaloKernel Package 
#include "CaloKernel/CaloException.h" 

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

template <class CONTENT                          ,  /// type of content 
  class         RETTYPE   = CONTENT              ,  /// return type     
  class         CONTAINER = std::vector<CONTENT> ,  /// container type  
  class         FUNCTOR   = 
   std::unary_function<const LHCb::CaloCellID&,RETTYPE&> >  /// functor
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
  CaloCollection  ( Content def        = Content() ,  /* "default" value */
                    const unsigned num = 0         ,  /* initial size */    
		    IMessageSvc* messageService = 0         )
    : CONTAINER             ( num , def      )
    , m_cc_def              ( def            )
    , m_cc_messageService   ( messageService ) 
  { };
  /// (virtual) destructor 
  virtual ~CaloCollection() { clear(); };
  //
  public: 
  
  // CONTAINER
  // access to CONTAINER interfce:
  /// access to the content  itself using LHCb::CaloCellID as an index
  inline       Content& operator[]( Index id )       
  { 
    size_type indx = id.index();
    return ( (indx < size())? *(begin()+indx) : def() ); 
  };
  /// the "const" version of access to the content  using LHCb::CaloCellID
  inline const Content& operator[]( Index id ) const 
  { 
    size_type indx = id.index();
    return ( (indx < size())? *(begin()+indx) : def() ); 
  };
  /// checked access, need to be catched!
  virtual inline  Content&     at( Index id ) 
  {
    size_type indx = id.index(); 
    if( indx >= size() ) 
      { throw CaloException("CaloCollection::at() - out_of_range()");} 
    return *(begin()+indx); 
  }
  /// the "const" version of checked access, need to be catched!
  virtual inline const Content&      at( Index id ) const  
  { 
    size_type indx = id.index(); 
    if( indx >= size() ) 
      { throw CaloException("CaloCollection::at() - out_of_range()");} 
    return *(begin()+indx); 
  }
  /// FUNCTOR! 
  /// access to FUNCTOR facilities
  /// please, pay some attention that return type here CAN BE DIFFERENT from 
  /// the return type of operator [] ! 
  /// this trick could be  heavily used in Geometry business
  virtual       ReturnType& operator() (Index id)       { return (*this)[id];};
  virtual const ReturnType& operator() (Index id) const { return (*this)[id];};
  // printout 
  virtual  std::ostream& fillStream( std::ostream& os ) const { 
    os << " class CaloCollection: "
       << " size=" << size(); 
    unsigned int num = std::count(begin(), end(), def() );
    os << "number of  non-default elements="<< size()-num;
    return os << std::endl ; 
  };
  ///
  virtual  MsgStream&    fillStream( MsgStream&    os ) const {
    os << " class CaloCollection: "
       << " size=" << size(); 
    unsigned int num = std::count(begin(), end(), def() );
    os << "number of  non-default elements="<< size()-num;
    return os << endreq ; 
  };
  ///
  friend std::ostream& operator << ( std::ostream& os , 
                                     const CaloCollection& cc ) 
  { return cc.fillStream(os); };
  ///
  friend std::ostream& operator << ( std::ostream& os , 
                                     const CaloCollection* cc ) 
  { return ( ( 0 == cc ) ? 
             os << " CaloCollection* points to NULL!" : os << *cc); };
  ///
  friend MsgStream&    operator << ( MsgStream&    os , 
                                     const CaloCollection& cc ) 
  { return cc.fillStream(os); };
  ///
  friend MsgStream&    operator << ( MsgStream&    os , 
                                     const CaloCollection* cc ) 
  { return ( ( 0 == cc ) ? 
             os << " CaloCollection* points to NULL!" : os << *cc); };
  ///
  inline  StatusCode addEntry ( const Content& content , Index id ) 
  {
    size_type indx = id.index(); 
    while( size() <= indx ) { push_back( def() ); }
    *(begin()+indx) = content; 
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
// The End 
// ============================================================================
#endif  //  CALOKERNEL_CALOCOLLECTION_H
// ============================================================================





