// $Id: ClusterLHCbIDCnv.h,v 1.1.1.1 2005-04-01 16:18:50 erodrigu Exp $
#ifndef TRACKTOOLS_CLUSTERLHCBIDCNV_H 
#define TRACKTOOLS_CLUSTERLHCBIDCNV_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "Kernel/LHCbID.h"
#include "Event/OTTime.h"
#include "Event/ITCluster.h"
#include "Event/VeloCluster.h"


static const InterfaceID IID_ClusterLHCbIDCnv ( "ClusterLHCbIDCnv", 1, 0 );

/** @class ClusterLHCbIDCnv ClusterLHCbIDCnv.h
 *  Tool to make the conversions between a XxxCluster
 *  and the corresponding LHCbID
 *
 *  @author Eduardo Rodrigues
 *  @date   2005-03-31
 */
class ClusterLHCbIDCnv : public GaudiTool {
public:
const OTTime&      otTime( const LHCbID& id );
const ITCluster&   itCluster( const LHCbID& id );  
const VeloCluster& veloCluster( const LHCbID& id );

LHCbID lhcbid( const OTTime& otTime );
LHCbID lhcbid( const ITCluster& itCluster );
LHCbID lhcbid( const VeloCluster& veloCluster );

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ClusterLHCbIDCnv; }

  /// Standard constructor
  ClusterLHCbIDCnv( const std::string& type, 
                  const std::string& name,
                  const IInterface* parent);

  virtual ~ClusterLHCbIDCnv( ); ///< Destructor

protected:

private:

};
#endif // TRACKTOOLS_CLUSTERLHCBIDCNV_H
