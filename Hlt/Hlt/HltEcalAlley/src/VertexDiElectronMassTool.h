// $Id: VertexDiElectronMassTool.h,v 1.1 2009-02-17 16:24:03 witekma Exp $
#ifndef VERTEXDIELECTRONMASSTOOL_H 
#define VERTEXDIELECTRONMASSTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Event/Track.h"
#include "Event/RecVertex.h"
#include "TrackInterfaces/IFunctionTool.h"            // Interface
#include "Event/L0CaloCandidate.h"
#include "CaloInterfaces/ICaloClusterization.h"
#include "CaloDet/DeCalorimeter.h"

/** @class VertexDiElectronMassTool VertexDiElectronMassTool.h
 *  
 *
 *  @author Mariusz Witek
 *  @date   2008-07-05
 */
class VertexDiElectronMassTool : public GaudiTool, 
                        virtual public IRecVertexFunctionTool {
public: 
  /// Standard constructor
  VertexDiElectronMassTool( const std::string& type, 
                   const std::string& name,
                   const IInterface* parent);

  virtual ~VertexDiElectronMassTool( ); ///< Destructor

  StatusCode initialize();
  
  double function(const LHCb::RecVertex& vertex);

private:

  void getRadiation(const LHCb::Track& tra, LHCb::L0CaloCandidates* calos, Gaudi::XYZVector& phorad);

  void getRadiationFromClusters(const LHCb::Track& tra, Gaudi::XYZVector& phorad);


  double m_minEtCluster;

  const DeCalorimeter* m_detector;

  ICaloClusterization* m_tool;

};
#endif // VERTEXDIELECTRONMASSTOOL_H
