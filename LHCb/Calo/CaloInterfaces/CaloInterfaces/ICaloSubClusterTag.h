#ifndef CALOINTERFACES_ICALOSUBCLUSTERTAG_H 
#define CALOINTERFACES_ICALOSUBCLUSTERTAG_H 1
// Include files
// GaudiKernel
#include "GaudiKernel/IAlgTool.h"
// CaloInterafces 
#include "CaloInterfaces/ICaloClusterTool.h"

/** @class ICaloSubClusterTag ICaloSubClusterTag.h
 *  
 *
 *  @author Ivan Belyaev
 *  @date   01/04/2002
 */
static const InterfaceID IID_ICaloSubClusterTag( "ICaloSubClusterTag" , 1 , 0 );

class ICaloSubClusterTag :
  public virtual ICaloClusterTool 
{
  
public:
  
  /** static interface identification
   *  @see IInterface 
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID()  { return IID_ICaloSubClusterTag;};
  
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
  
  /// virtual destructor
  virtual ~ICaloSubClusterTag() {}
};

// ============================================================================
#endif // CALOINTERFACES_ICALOSUBCLUSTERTAG_H
// ============================================================================
