// $Id:
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/05/23 11:09:14  ibelyaev
//  see $CALOALGSROOT/doc/release.notes
//
// Revision 1.7  2002/04/04 20:11:41  ibelyaev
//  update for New Calorimeter Event Model (v10r0)
//
// Revision 1.6  2001/11/25 15:31:53  ibelyaev
//  update for newer CaloKernel package
//
// Revision 1.5  2001/11/02 19:17:22  ibelyaev
//  update for newer packages
//
// Revision 1.4  2001/10/28 19:25:01  ibelyaev
// version match and change base class for CellularAutomaton
//
// Revision 1.3  2001/06/23 17:01:41  ibelyaev
// update due to modifications in CaloCluster class
// ============================================================================
#ifndef CALOCA_CELLULARAUTOMATON_H
#define CALOCA_CELLULARAUTOMATON_H 1
// STD and STL 
#include <string>
#include <iostream>
/// GaudiKernel
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StreamBuffer.h"
/// CaloKernel 
#include "CaloKernel/CaloVector.h"
#include "CaloKernel/CaloAlgorithm.h"
/// CaloDet 
#include "CaloDet/DeCalorimeter.h"
/// local
#include "CelAutoTaggedCell.h"

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
  friend AlgFactory<CellularAutomaton>;
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
  
  /** standard  finalization  method 
   *  @see CaloAlgorithm 
   *  @see     Algorithm 
   *  @see    IAlgorithm 
   *  @return status code 
   */
  virtual StatusCode finalize   () ;

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

private:
  ///
  
  bool isLocMax
  ( CelAutoTaggedCell*   taggedCell,
    CaloDigits&          hitsDirect,
    const DeCalorimeter* detector );
  
  void appliRulesTagger
  ( CelAutoTaggedCell*&  taggedCell,
    DirVector&           taggedCellsDirect,
    const DeCalorimeter* detector );
  
  void setEXYCluster
  ( CaloCluster*         cluster,
    const DeCalorimeter* detector );
  
};

// ============================================================================
// The End 
// ============================================================================
#endif  // CALOCA_CELLULARAUTOMATON_H
// ============================================================================










