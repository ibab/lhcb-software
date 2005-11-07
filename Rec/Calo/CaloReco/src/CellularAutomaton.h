// $Id:
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.5  2004/10/22 16:33:53  ibelyaev
//  update CellularAutomaton and remove obsolete algs
//
// ============================================================================
#ifndef CALOCA_CELLULARAUTOMATON_H
#define CALOCA_CELLULARAUTOMATON_H 1
// ============================================================================
// STD and STL 
// ============================================================================
#include <string>
#include <iostream>
// ============================================================================
/// GaudiKernel
// ============================================================================
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StreamBuffer.h"
// ============================================================================
/// CaloKernel 
// ============================================================================
#include "CaloKernel/CaloVector.h"
#include "CaloKernel/CaloAlgorithm.h"
// ============================================================================
/// CaloDet 
// ============================================================================
#include "CaloDet/DeCalorimeter.h"
// ============================================================================
/// local
// ============================================================================
#include "CelAutoTaggedCell.h"
// ============================================================================

/** @class CellularAutomaton CellularAutomaton.h 
 *
 *  Algorithm of clustering 
 *
 *  @author  Nicole Brun 
 *  @date    27/02/2001
 */
class CellularAutomaton : public CaloAlgorithm 
{
  /// friend factory for instantiation
  friend class AlgFactory<CellularAutomaton>;
public:
  
  /// container to tagged  cells with sequential access 
  typedef std::vector<CelAutoTaggedCell*> SeqVector;
  /// container to tagged  cells with direct (by CaloCellID key)  access 
  typedef CaloVector<CelAutoTaggedCell*>  DirVector ;
  
public:  
  
  /** standard  initialization  method 
   *  @see CaloAlgorithm 
   *  @see     Algorithm 
   *  @see    IAlgorithm 
   *  @return status code 
   */
  virtual StatusCode initialize () ;
  
  /** standard  execution  method 
   *  @see CaloAlgorithm 
   *  @see     Algorithm 
   *  @see    IAlgorithm 
   *  @return status code 
   */
  virtual StatusCode execute    () ;
  
protected:
  
  /** standard constructor
   *  @see Algorithm 
   *  @param name   name of the algorithm
   *  @param svcloc pointer to service locator 
   */
  CellularAutomaton
  ( const std::string& name   , 
    ISvcLocator*       svcloc ) ;

  /// destructor (virtual and protected)
  virtual ~CellularAutomaton()  ;

private:
  
  ///  default  constructor  is private 
  CellularAutomaton();
  ///  copy     constructor  is private 
  CellularAutomaton
  ( const CellularAutomaton& );
  ///  assignement operator  is private 
  CellularAutomaton& operator=
  ( const CellularAutomaton& );

protected:
  
  
  inline bool isLocMax
  ( const CaloDigit*     digit ,
    const DirVector&     hits  ,
    const DeCalorimeter* det   ) ;
  
  inline bool isLocMax
  ( const CelAutoTaggedCell* taggedCell ,
    const DirVector&         hitsDirect ,
    const DeCalorimeter*     detector   )
  {
    if ( 0 == taggedCell ) { return false ; }
    return isLocMax ( taggedCell->digit() , hitsDirect , detector ) ;
  }
  
  inline void appliRulesTagger
  ( CelAutoTaggedCell*   taggedCell,
    DirVector&           taggedCellsDirect,
    const DeCalorimeter* detector );
  
  inline void setEXYCluster
  ( CaloCluster*         cluster,
    const DeCalorimeter* detector );

private :
  
  bool m_sort     ;
  bool m_sortByET ;
  
};

// ============================================================================
// The End 
// ============================================================================
#endif  // CALOCA_CELLULARAUTOMATON_H
// ============================================================================










