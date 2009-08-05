// $Id: TaggedCellFunctor.h,v 1.5 2009-08-05 17:38:30 ibelyaev Exp $
// ============================================================================
#ifndef CALOCA_TAGGEDCELLFUNCTOR_H
#define CALOCA_TAGGEDCELLFUNCTOR_H 1 
// ============================================================================
// Include files 
// ============================================================================
// STL
// ============================================================================
#include <functional>
#include <vector>
#include <iostream>
// ============================================================================
// local 
// ============================================================================
#include "CelAutoTaggedCell.h"
// ============================================================================
/** @namespace TaggedCellFunctor TaggedCellFunctor.h CaloCA/TaggedCellFunctor.h
 *
 * Algorithm of clustering  
 * Contain functors for use with STL algorithms
 * for one CaloTaggedCell*
 *
 *  @author  Nicole Brun 
 *  @date    27/02/2001
 */
// ============================================================================
namespace TaggedCellFunctor 
{
  class isEdge;
  class isClustered;
  class isClusteredOrEdge;
  class isSeed;
  class isWithSeed;
  class setStatus;
}
// ============================================================================
class TaggedCellFunctor::isEdge :
public std::unary_function<const CelAutoTaggedCell*, bool> {

  public:
 
  // Constructor

  isEdge(){};

  // Destructor

  virtual ~isEdge(){};

  // Function

  inline bool operator() ( const CelAutoTaggedCell* taggedCell ) const {
    return ( ( 0 == taggedCell ) ? false : taggedCell->isEdge() );
  } 
  
  private:

};

class TaggedCellFunctor::isClustered :
public std::unary_function<const CelAutoTaggedCell*, bool> {

  public:
 
  // Constructor

  isClustered(){};

  // Destructor

  virtual ~isClustered(){};

  // Function

  inline bool operator() ( const CelAutoTaggedCell* taggedCell ) const {
    return ( ( 0 == taggedCell ) ? false : taggedCell->isClustered() );
  } 
  
  private:

};

class TaggedCellFunctor::isClusteredOrEdge :
public std::unary_function<const CelAutoTaggedCell*, bool> {

  public:
 
  // Constructor

  isClusteredOrEdge(){};

  // Destructor

  virtual ~isClusteredOrEdge(){};

  // Function

  inline bool operator() ( const CelAutoTaggedCell* taggedCell ) const {
    return ( ( 0 == taggedCell ) ? false : taggedCell->isClustered() || taggedCell->isEdge() );
  }

  private:

};


class TaggedCellFunctor::isSeed :
public std::unary_function<const CelAutoTaggedCell*, bool> {

  public:
 
  // Constructor

  isSeed(){};

  // Destructor

  virtual ~isSeed(){};

  // Function

  inline bool operator() ( const CelAutoTaggedCell* taggedCell ) const {
    return ( ( 0 == taggedCell ) ? false : taggedCell->isSeed() );
  } 
  
  private:

};

class TaggedCellFunctor::isWithSeed :
public std::unary_function<CelAutoTaggedCell*, bool> {

  public:
 
  // Constructor

  isWithSeed( const LHCb::CaloCellID& seed ) :
    m_seed ( seed ) {};

  // Destructor

  virtual ~isWithSeed(){};

  // Function

  inline bool operator() ( CelAutoTaggedCell* taggedCell ) const {
    return ( ( 0 == taggedCell ) ? false : taggedCell->isWithSeed( m_seed )  );
  } 
  
  private:
 
  LHCb::CaloCellID m_seed; 

};

class TaggedCellFunctor::setStatus :
public std::unary_function<CelAutoTaggedCell*, void > {

  public:
 
  // Constructor

  setStatus(){};

  // Destructor

  virtual ~setStatus(){};

  // Function

  inline void operator() ( CelAutoTaggedCell* taggedCell ) const {
    taggedCell->setStatus();
  } 
  
};

#endif // CALOCA_TAGGEDCELLFUNCTOR_H
















