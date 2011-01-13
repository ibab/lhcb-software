// $Id: ICaloSplitTool.h,v 1.8 2008-09-22 00:50:30 odescham Exp $
// ============================================================================
#ifndef CALOINTERFACES_ICALOSPLITTOOL_H 
#define CALOINTERFACES_ICALOSPLITTOOL_H 1
// Include files
// STD & STL 
#include <functional>
// GaudiKernel
#include "GaudiKernel/IAlgTool.h"
#include "Event/CaloHypo.h"
/** @class ICaloSplitTool ICaloSplitTool.h CaloInterfaces/ICaloSplitTool.h
 *
 *  An abstract interface for "cluster splitting tools", like 
 *     Bulos, or shower-shape fitting or iterative analysis.
 *
 *  @author Vanya Belyaev      Ivan.Belyaev@itep.ru
 *  @author Frederic Machefert machefer@in2p3.fr
 *  @date   19/03/2002
 */


namespace LHCb{
  class     CaloHypo     ;     
}
static const InterfaceID IID_ICaloSplitTool( "ICaloSplitTool" , 2 , 0 );

class ICaloSplitTool: 
  public virtual IAlgTool ,
  public std::binary_function<LHCb::CaloHypo*,LHCb::CaloHypos&,StatusCode>
{
public:

  // OD ALREADY DEFINED AS A KEYEDCONTAINER IN EVENT
  // useful type definition for output container  
  //namespace LHCb  {
  //  typedef std::vector<LHCb::CaloHypo*>  CaloHypos;
  //}
  
public:
  
  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID()  { return IID_ICaloSplitTool;};
  
  /** The main processing method 
   *  @param  hypo   pointer to CaloHypo object to be processed
   *  @param  hypos  the result of the splitting procedure 
   *  @return status code 
   */  
  virtual StatusCode 
  process ( LHCb::CaloHypo*  hypo  ,
            LHCb::CaloHypos& hypos ) const = 0 ;
  
  /** The main processing method (functor interface)
   *  @param  hypo   pointer to CaloHypo object to be processed
   *  @param  hypos  the result of the splitting procedure 
   *  @return status code 
   */  
  virtual StatusCode 
  operator() ( LHCb::CaloHypo*  hypo  ,
               LHCb::CaloHypos& hypos ) const = 0 ;
  
};

#endif // CALOINTERFACES_ICALOSPLITTOOL_H
