// $Id: ICaloSplitTool.h,v 1.1 2002-03-19 17:31:26 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef CALOINTERFACES_ICALOSPLITTOOL_H 
#define CALOINTERFACES_ICALOSPLITTOOL_H 1
// Include files
// GaudiKernel
#include "GaudiKernel/IAlgTool.h"
// Local
#include "CaloInterfaces/ICaloSplitTool.h"
 
/** @class ICaloSplitTool ICaloSplitTool.h CaloInterfaces/ICaloSplitTool.h
 *
 *  An abstract interface for "cluster splitting tools", like 
 *     Bulos, or shower-shape fitting or iterative analysis.
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   19/03/2002
 */

class ICaloSplitTool: 
  public virtual IAlgTool
{
public:

  /// useful type definition for output container 
  typedef std::vector<CaloHypo*>  CaloHypos;
  
public:
  
  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() { return IID_ICaloSplitTool ; }
  
  /** The main processing method 
   *  @param  hypo   pointer to CaloHypo object to be processed
   *  @param  hypos  the result of the splitting procedure 
   *  @return status code 
   */  
  virtual StatusCode 
  process ( CaloHypo*  hypo  ,
            CaloHypos& hypos ) const = 0 ;
  
  /** destructor, virtual 
   */
  virtual ~ICaloSplitTool(){};

};

// ============================================================================
// The End 
// ============================================================================

#endif // CALOINTERFACES_ICALOSPLITTOOL_H
