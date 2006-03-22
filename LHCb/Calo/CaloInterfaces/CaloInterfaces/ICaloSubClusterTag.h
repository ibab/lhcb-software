// $Id: ICaloSubClusterTag.h,v 1.5 2006-03-22 18:21:51 odescham Exp $
// ============================================================================
// CVS  tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2005/11/07 12:08:25  odescham
// v5r0 - Adapt to the new Track Event Model
//
// Revision 1.3  2004/02/17 11:51:54  ibelyaev
//  move IID* from CaloInterfaces to src directory
//
// Revision 1.2  2004/02/17 11:42:28  ibelyaev
//  fix in Likel(i,y)hoods
//
// Revision 1.1  2002/04/02 11:08:12  ibelyaev
//  new interfaces
// 
// ============================================================================
#ifndef CALOINTERFACES_ICALOSUBCLUSTERTAG_H 
#define CALOINTERFACES_ICALOSUBCLUSTERTAG_H 1
// Include files
// GaudiKernel
#include "GaudiKernel/IInterface.h"
// CaloInterafces 
#include "CaloInterfaces/ICaloClusterTool.h"

/** @class ICaloSubClusterTag ICaloSubClusterTag.h
 *  
 *
 *  @author Ivan Belyaev
 *  @date   01/04/2002
 */
class ICaloSubClusterTag :
  public virtual ICaloClusterTool 
{
  
public:
  
  /** static interface identification
   *  @see IInterface 
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() ;
  
  /** The main method 
   *  @param cluster pointer to ClaoCluster object to be selected/tagged 
   *  @return status code 
   */
  virtual StatusCode tag   ( LHCb::CaloCluster* cluster ) const = 0 ;
  
  /** The main method 
   *  @param cluster pointer to ClaoCluster object to be untagged 
   *  @return status code 
   */
  virtual StatusCode untag ( LHCb::CaloCluster* cluster ) const = 0 ;
  
protected:
  
  /// destructor (virtual and protected)
  virtual ~ICaloSubClusterTag();
  
};

// ============================================================================
#endif // CALOINTERFACES_ICALOSUBCLUSTERTAG_H
// ============================================================================
