// $Id: CaloCosmicsTrackTool.h,v 1.1 2008-05-22 22:47:59 odescham Exp $
#ifndef CALOCOSMICSTRACKTOOL_H 
#define CALOCOSMICSTRACKTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTupleTool.h"
// from LHCb
#include "CaloInterfaces/ICaloCosmicsTrackTool.h"            // Interface
#include "Kernel/IEventTimeDecoder.h"



/** @class CaloCosmicsTrackTool CaloCosmicsTrackTool.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2008-05-17
 */
class CaloCosmicsTrackTool : public GaudiTupleTool, virtual public ICaloCosmicsTrackTool {
public: 
  /// Standard constructor
  CaloCosmicsTrackTool( const std::string& type, 
                        const std::string& name,
                        const IInterface* parent);

  virtual ~CaloCosmicsTrackTool( ); ///< Destructor

  virtual StatusCode initialize();    
  virtual StatusCode finalize();    
  //
  virtual StatusCode processing();    
  StatusCode tupling(unsigned int unit);  
  const double phi(){return m_phi;}
  const double phiVariance(){return m_sPhi;}
  const double theta(){return m_theta;}
  const double thetaVariance(){return m_sTheta;}
  const Gaudi::XYZVector slopes(){return m_slopes;}
  const Gaudi::XYZPoint  referencePoint(){ return m_refPt;}
  const Gaudi::SymMatrix3x3 slopesCovariance(){return m_slopesCov;}
  const Gaudi::SymMatrix3x3 referencePointCovariance(){ return m_refPtCov;}
  const bool   forward(){return (m_dir == 1) ? true : false;}
  const bool   tracked(){return m_tracked;}
  const double time(){return m_time;}
  const double timeVariance(){return m_stime;}
  const bool   timed(){return  m_timed;}
  ICaloCosmicsTool* ecal(){return m_eCosmics;}
  ICaloCosmicsTool* hcal(){return m_hCosmics;}
  const LHCb::Track& track(){return m_track;}
  StatusCode propagate(Gaudi::Plane3D plane);
  StatusCode propagate(double z );

protected:
  
private:
  StatusCode matching();
  StatusCode fit3D();
  double dist2D(Gaudi::XYZPoint,Gaudi::XYZPoint);
  StatusCode buildTrack();
  
  //
  ICaloCosmicsTool* m_eCosmics;
  ICaloCosmicsTool* m_hCosmics;
  IEventTimeDecoder* m_odin;
  // 
  long m_run;
  long m_evt;
  long m_bx;
  double m_tx;
  double m_ty;
  Gaudi::XYZVector m_slopes;
  Gaudi::SymMatrix3x3 m_slopesCov;
  double m_stx;
  double m_sty;
  double m_theta;
  double m_sTheta;
  double m_phibis;
  double m_phi;
  double m_sPhi;
  double m_chi2;
  bool m_intern;
  bool m_extern;
  double m_time;
  double m_stime;  
  Gaudi::XYZPoint m_refPoint[2];
  Gaudi::SymMatrix3x3 m_refPointCov[2];
  Gaudi::XYZPoint m_refPt;
  Gaudi::SymMatrix3x3 m_refPtCov;
  int m_ref;

  int m_dir;
  bool m_tracked;
  bool m_timed;
  LHCb::Track m_track;
  // properties
  std::string m_cosmics;
  double m_chi2max;
  double m_chi2min;
  double m_fac;
  double m_tuple;
  std::string m_timer;
};  
#endif // CALOCOSMICSTRACKTOOL_H
