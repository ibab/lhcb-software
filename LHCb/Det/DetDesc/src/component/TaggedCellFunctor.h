// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/component/TaggedCellFunctor.h,v 1.1 2001-02-03 19:52:28 ibelyaev Exp $
#ifndef CALORECONS_TAGGEDCELLFUNCTOR_H
#define CALORECONS_TAGGEDCELLFUNCTOR_H

// Include files
#include <functional>
#include <vector>
#include <iostream>
#include "Clustering/CelAutoTaggedCell.h"
#include "CaloEvent/CaloCellID.h"

// Forward declarations

//------------------------------------------------------------------
//
// Namespace  : CaloTaggedCellFunctor
//
// Description: Contain functors for use with STL algorithms
//              for one CaloTaggedCell*
//
// Author     : Nicole Brun
// Date       : 12 october 2000
// Changes    :
//
//------------------------------------------------------------------


namespace TaggedCellFunctor {
  class isEdge;
  class isClustered;
  class isClusteredOrEdge;
  class isSeed;
  class isWithSeed;
};

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

  isWithSeed( const CaloCellID& seed ) :
    m_seed ( seed ) {};

  // Destructor

  virtual ~isWithSeed(){};

  // Function

  inline bool operator() ( CelAutoTaggedCell* taggedCell ) const {
    return ( ( 0 == taggedCell ) ? false : taggedCell->isWithSeed( m_seed )  );
  } 
  
  private:
 
  CaloCellID m_seed; 

};

#endif // CALORECONS_TAGGEDCELLFUNCTOR_H

