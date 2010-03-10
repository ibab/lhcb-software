// $Id: IsolationTwoBody.h,v 1.1 2010-03-10 13:21:43 giampi Exp $
#ifndef ISOLATIONTWOBODY_H 
#define ISOLATIONTWOBODY_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IIsolation.h"            // Interface
#include "Kernel/IDistanceCalculator.h"
//#include "IIsolation.h"            // Interface

using namespace LHCb;

/** @class IsolationTwoBody IsolationTwoBody.h
 *  
 *
 *  @author Giampiero Mancinelli
 *  @date   2010-02-04
 */
class IsolationTwoBody  : public GaudiTool, public virtual IIsolation {
  

public: 
  /// Standard constructor
  IsolationTwoBody(const std::string& type,
                  const std::string& name,
                  const IInterface* parent ); 
  
  StatusCode initialize();
  
  /// 
  virtual StatusCode getIso(LHCb::Particle::ConstVector muons, 
                            int* iso, int isotype,
                            const LHCb::VertexBase* PV, 
                            const LHCb::VertexBase* SV) const ;
  
  virtual ~IsolationTwoBody( ); ///< Destructor
  
  StatusCode IsHltGood(Gaudi::XYZPoint o,
                       Gaudi::XYZVector p,
                       Gaudi::XYZPoint o_mu,
                       Gaudi::XYZVector 
                       p_mu, Gaudi::XYZPoint PV, bool& 
                       hltgood, double& fc) const;
  StatusCode InCone(Gaudi::XYZPoint o1, 
                    Gaudi::XYZVector p1,
                    Gaudi::XYZPoint o2, 
                    Gaudi::XYZVector p2,
                    Gaudi::XYZPoint& vtx, double& 
                    doca, double& angle) const;
  
  double pointer (Gaudi::XYZVector vtx, 
                  Gaudi::XYZVector p, 
                  Gaudi::XYZVector p_mu) const;
  
  double arcosine(Gaudi::XYZVector p1,
                  Gaudi::XYZVector p2) const;
  
  
  StatusCode closest_point(Gaudi::XYZPoint o,
                           Gaudi::XYZVector p,
                           Gaudi::XYZPoint o_mu,
                           Gaudi::XYZVector
                           p_mu, Gaudi::XYZPoint& close1, 
                           Gaudi::XYZPoint& close2, 
                           Gaudi::XYZPoint& vertex, bool& 
                           fail) const;
  
private:
  
      //      if (angle <0.27 && fc<0.60 && doca<0.13 && ips>3.0 && svdis>-0.15
      //          && svdis<30. && pvdis>0.5 && pvdis<40. && track->type()==3) {
  IDistanceCalculator*  m_Geom;
  std::string m_ParticlePath;
  double m_angle;
  double m_fc;
  double m_doca;
  double m_ips;
  double m_svdis;
  double m_svdis_h;
  double m_pvdis;
  double m_pvdis_h;
  int m_tracktype;
  std::string  m_PVInputLocation;
  double ratio( double p1, double p2) const;
  
  
};
#endif // ISOLATIONTWOBODY_H
