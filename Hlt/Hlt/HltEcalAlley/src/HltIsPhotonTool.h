// $Id: HltIsPhotonTool.h,v 1.2 2008-12-03 12:30:54 graven Exp $
#ifndef HLTISPHOTONTOOL_H 
#define HLTISPHOTONTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Event/Track.h"
#include "TrackInterfaces/IFunctionTool.h"            // Interface
#include "Event/L0CaloCandidate.h"
#include "CaloInterfaces/ICaloClusterization.h"
#include "CaloDet/DeCalorimeter.h"
#include "TMVA_FisherArea0.C"
#include "TMVA_FisherArea1.C"
#include "TMVA_FisherArea2.C"
#include <memory>

/** @class HltIsPhotonTool HltIsPhotonTool.h
 *  
 *
 *  @author Mariusz Witek
 *  @date   2008-07-05
 */
class HltIsPhotonTool : public GaudiTool, 
                        virtual public ITrackFunctionTool {
public: 
  /// Standard constructor
  HltIsPhotonTool( const std::string& type, 
                   const std::string& name,
                   const IInterface* parent);

  virtual ~HltIsPhotonTool( ); ///< Destructor

  StatusCode initialize();
  StatusCode finalize();
  
  double function(const LHCb::Track& track);

private:
  HltIsPhotonTool( const HltIsPhotonTool& ) ;            // do not allow!
  HltIsPhotonTool& operator=( const HltIsPhotonTool& ) ; // do not allow!

  double m_minEtCluster;
  std::auto_ptr<IClassifierReader> m_reader0;  
  std::auto_ptr<IClassifierReader> m_reader1;  
  std::auto_ptr<IClassifierReader> m_reader2;  

  bool m_useFastDecoding; 
  const DeCalorimeter* m_detector;
  ICaloClusterization* m_tool;
  void caloClusterShapeVariables(const LHCb::CaloCluster *cluster,
				 double& fr2, double& fasym, double& fkappa, double& fr2r4);
          
  double photonDiscriminant(int icaloreg, double showershape, double showerasym, 
                            double showerkappa, double showertail,  double e);
  void  normalizeShowerShapeVariables(double& showershape, double& showerasym, 
				      double& showerkappa, double& showertail, double& showerenergy);
};
#endif // HLTISPHOTONTOOL_H
