// $Id: ICaloSubClusterTag.h,v 1.1 2002-04-02 11:08:12 ibelyaev Exp $
// ============================================================================
// CVS  tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef CALOINTERFACES_ICALOSUBCLUSTERTAG_H 
#define CALOINTERFACES_ICALOSUBCLUSTERTAG_H 1
// Include files
// GaudiKernel
#include "GaudiKernel/IInterface.h"
// CaloInterafces 
#include "CaloInterfaces/ICaloClusterTool.h"
#include "CaloInterfaces/IIDICaloSubClusterTag.h"

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
  static const InterfaceID& interfaceID()    { return IID_ICaloSubClusterTag ; }
  
  /** The main method 
   *  @param cluster pointer to ClaoCluster object to be selected/tagged 
   *  @return status code 
   */
  virtual StatusCode tag   ( CaloCluster* cluster ) const = 0 ;
  
  /** The main method 
   *  @param cluster pointer to ClaoCluster object to be untagged 
   *  @return status code 
   */
  virtual StatusCode untag ( CaloCluster* cluster ) const = 0 ;
  
protected:
  
  /// destructor (virtual and protected)
  virtual ~ICaloSubClusterTag(){};
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // CALOINTERFACES_ICALOSUBCLUSTERTAG_H
// ============================================================================
