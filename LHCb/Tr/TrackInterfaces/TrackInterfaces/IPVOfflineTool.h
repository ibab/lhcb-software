// $Id: IPVOfflineTool.h,v 1.2 2007-12-04 12:01:51 witekma Exp $
#ifndef IPVOFFLINETOOL_H 
#define IPVOFFLINETOOL_H 1
// from STL
#include <string>
#include <vector>
#include "Event/RecVertex.h"
#include "Event/Track.h"
// from Gaudi
#include "GaudiKernel/AlgTool.h"
// Interface ID 
static const InterfaceID IID_IPVOfflineTool("IPVOfflineTool",1,0);

class IPVOfflineTool : virtual public IAlgTool {
 public:
  // Retrieve interface ID
  static const InterfaceID& interfaceID() {return IID_IPVOfflineTool;}


  /** Reconstructs single PV excluding tracks from a given vector.
   *
   *  The full reconstruction of single PV is performed for a given 
   *  seed point with the tracks from the default location
   *  excluding tracks in tracks2exclude vector.
   *  It is not a refit. Procedure can fail due to tracks excluded. 
   */
  virtual StatusCode reDoSinglePV(const Gaudi::XYZPoint xyzseed,
			          std::vector<const LHCb::Track*>& tracks2exclude,
			          LHCb::RecVertex& outvtx)=0;

  /** Reconstructs all PVs excluding tracks from a given vector and returns new PV.
   *
   *  The full reconstruction of all PVs is performed with the tracks
   *  from the default location excluding tracks in tracks2exclude vector.
   *  The returned outvtx matches invtx by comparing track contents.
   *  It is not a refit. Procedure can fail due to tracks excluded. 
   */
  virtual StatusCode reDoMultiPV(const LHCb::RecVertex& invtx, 
			         std::vector<const LHCb::Track*>& tracks2exclude, 
			         LHCb::RecVertex& outvtx)=0;  


  /// Reconstructs single PV with tracks specified in a vector.
  virtual StatusCode reconstructSinglePVFromTracks(const Gaudi::XYZPoint xyzseed,
			          std::vector<const LHCb::Track*>& tracks2use,
			          LHCb::RecVertex& outvtx)=0;

  /// Reconstructs all PVs with tracks specified in a vector. Return weights.
  virtual StatusCode reconstructMultiPVFromTracks(
			          std::vector<const LHCb::Track*>& tracks2use,
			          std::vector<LHCb::RecVertex>& outvtxVec)=0;
  
  /// Reconstructs all PVs with tracks specified in a vector. Return weights.
  virtual StatusCode reconstructMultiPVWithWeightsFromTracks(
			          std::vector<const LHCb::Track*>& tracks2use,
			          std::vector<LHCb::RecVertex>& outvtxVec,
                std::vector< std::vector<double> >& weightsvec)=0;

  /// Reconstructs all PVs with tracks from default location.
  virtual StatusCode reconstructMultiPV(std::vector<LHCb::RecVertex>& outvtxVec)=0; 

  /// Reconstructs all PVs with tracks from default location. Return weights.
  virtual StatusCode reconstructMultiPVWithWeights(std::vector<LHCb::RecVertex>& outvtxVec,
                                        std::vector< std::vector<double> >& weightsvec)=0; 

  /// Reconstructs single PV for a given seed with tracks from default location.
  virtual StatusCode reconstructSinglePV(const Gaudi::XYZPoint xyzseed,
			                 LHCb::RecVertex& outvtx)=0;


};
#endif // IPVOFFLINETOOL_H
