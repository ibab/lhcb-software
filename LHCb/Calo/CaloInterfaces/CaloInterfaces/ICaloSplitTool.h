// $Id: ICaloSplitTool.h,v 1.3 2004-02-17 11:42:28 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2002/04/02 11:08:12  ibelyaev
//  new interfaces
//
// Revision 1.1  2002/03/19 17:31:26  ibelyaev
//  add new abstract interface for Cluster Splitting Tools
// 
// ============================================================================
#ifndef CALOINTERFACES_ICALOSPLITTOOL_H 
#define CALOINTERFACES_ICALOSPLITTOOL_H 1
// Include files
// STD & STL 
#include <functional>
// GaudiKernel
#include "GaudiKernel/IAlgTool.h"
// Local
#include "CaloInterfaces/IIDICaloSplitTool.h"

/** @class ICaloSplitTool ICaloSplitTool.h CaloInterfaces/ICaloSplitTool.h
 *
 *  An abstract interface for "cluster splitting tools", like 
 *     Bulos, or shower-shape fitting or iterative analysis.
 *
 *  @author Vanya Belyaev      Ivan.Belyaev@itep.ru
 *  @author Frederic Machefert machefer@in2p3.fr
 *  @date   19/03/2002
 */

class ICaloSplitTool: 
  public virtual IAlgTool ,
  public std::binary_function<CaloHypo*,std::vector<CaloHypo>&,StatusCode>
{
public:

  /// useful type definition for output container 
  typedef std::vector<CaloHypo*>  CaloHypos;
  
public:
  
  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() ;
  
  /** The main processing method 
   *  @param  hypo   pointer to CaloHypo object to be processed
   *  @param  hypos  the result of the splitting procedure 
   *  @return status code 
   */  
  virtual StatusCode 
  process ( CaloHypo*  hypo  ,
            CaloHypos& hypos ) const = 0 ;
  
  /** The main processing method (functor interface)
   *  @param  hypo   pointer to CaloHypo object to be processed
   *  @param  hypos  the result of the splitting procedure 
   *  @return status code 
   */  
  virtual StatusCode 
  operator() ( CaloHypo*  hypo  ,
               CaloHypos& hypos ) const = 0 ;
  
protected:
  
  /// destructor, virtual 
  virtual ~ICaloSplitTool();
  
};

// ============================================================================
// The End 
// ============================================================================

#endif // CALOINTERFACES_ICALOSPLITTOOL_H
