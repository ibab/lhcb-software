// $Id: ICaloSplitTool.h,v 1.7 2007-03-29 16:55:34 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.6  2006/03/22 18:21:51  odescham
// Migration to new Event Model
//
// Revision 1.5  2005/11/07 12:08:25  odescham
// v5r0 - Adapt to the new Track Event Model
//
// Revision 1.4  2004/02/17 11:51:54  ibelyaev
//  move IID* from CaloInterfaces to src directory
//
// Revision 1.3  2004/02/17 11:42:28  ibelyaev
//  fix in Likel(i,y)hoods
//
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
};
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
