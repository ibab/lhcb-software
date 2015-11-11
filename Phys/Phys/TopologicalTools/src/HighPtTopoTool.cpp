// $Id: $  -*- C++ -*-                                                        
// ============================================================================
// Include files
#include <string>
#include <fstream>
#include <vector>
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/ITriggerTisTos.h"
#include "Kernel/IAccept.h"
#include "Event/HltObjectSummary.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Vector4DTypes.h"
#include "Event/Particle.h"

// =============================================================================

class HighPtTopoTool : public extends1<GaudiTool,IAccept> {

  friend class ToolFactory<HighPtTopoTool>;

private:

  HighPtTopoTool();
  HighPtTopoTool(const HighPtTopoTool&);
  HighPtTopoTool& operator=(const HighPtTopoTool&);

  float m_minpT;
  float m_conesize;
  float m_minconepT;
  std::string m_hltdecision;
  ITriggerTisTos *m_tistostool;


protected:

  HighPtTopoTool(const std::string& type,const std::string& name,
                 const IInterface* parent);
  ~HighPtTopoTool(){}

public:

  virtual StatusCode initialize();
  virtual StatusCode finalize() {return GaudiTool::finalize();}
  bool accept() const;
  void getHltObjP4 (const LHCb::HltObjectSummary *obj,Gaudi::LorentzVector &p4) const;
};
// =============================================================================

HighPtTopoTool::HighPtTopoTool(const std::string& type,const std::string& name,
                               const IInterface* parent)
  : base_class(type,name,parent), m_tistostool(0) {
  declareProperty("minpT"       , m_minpT = 20000.);
  declareProperty("conesize"    , m_conesize = -1);
  declareProperty("minconepT"   , m_minconepT = 0.);
  declareProperty("hltdecision" , m_hltdecision = "Hlt2Topo.*BBDTDecision");
}

StatusCode HighPtTopoTool::initialize(){
  StatusCode sc = GaudiTool::initialize();
  if(sc.isFailure()) return sc;
  m_tistostool = tool<ITriggerTisTos>("TriggerTisTos","TriggerTisTosTool",this);
  m_tistostool->setOfflineInput();
  //m_tistostool->setTriggerInput();
  return StatusCode::SUCCESS;
}

bool HighPtTopoTool::accept() const {
  std::vector<const LHCb::HltObjectSummary*> hltObjs
    = m_tistostool->hltObjectSummaries(m_hltdecision,2,2);
  unsigned int num = hltObjs.size();
  std::vector<std::vector<int> > lhcbIDs(num);
  Gaudi::LorentzVector P4;
  for(unsigned int i = 0; i < num; i++){
      getHltObjP4(hltObjs[i],P4);
      if(m_conesize<0 && P4.Pt()  > m_minpT) return true;
      if(m_conesize>0 && P4.Pt()  > m_minpT) {
          LHCb::Tracks* tracks = get<LHCb::Tracks>("Rec/Track/Best");//LHCb::TrackLocation::Default);
          double sumPx = 0.0; double sumPy = 0.0;
          double sumPx_out = 0.0; double sumPy_out = 0.0; double sumPt_out = 0.0;
          for( LHCb::Tracks::const_iterator it = tracks->begin(); it != tracks->end(); ++it){
              LHCb::Track* track = (*it);
              double trackpt = track->pt();
              if(trackpt<200) continue;
              if(track->ghostProbability()>0.4 || track->chi2PerDoF()>3) continue;
              Gaudi::XYZVector trackMomentum = track->momentum();
              double trackpx = trackMomentum.X();
              double trackpy = trackMomentum.Y();

              // -- Calculate the difference in Eta and Phi between the particle in question and a track
              double deltaPhi = fabs( P4.Phi() - track->phi());//trackMomentum.Phi() );
              if(deltaPhi > M_PI) deltaPhi  = 2*M_PI-deltaPhi;
              double deltaEta = P4.Eta() - track->pseudoRapidity();//trackMomentum.Eta();
              double deltaR = std::sqrt(deltaPhi * deltaPhi + deltaEta * deltaEta);

              // -- Add the tracks to the summation if deltaR is smaller than the cut value of deltaR
              if(deltaR < m_conesize && (track->type()==3)){
                  sumPx += trackpx;
                  sumPy += trackpy;
              } else if(deltaR > m_conesize && (track->type()==3)) {sumPx_out += trackpx; sumPy_out += trackpy; sumPt_out += trackpt;}
          }
          if(sqrt(sumPx*sumPx+sumPy*sumPy)  > m_minconepT && P4.Pt()  > m_minpT) return true;
      }
  }
  return false;
}
void HighPtTopoTool::getHltObjP4 (const LHCb::HltObjectSummary *obj,Gaudi::LorentzVector &p4) const{
  LHCb::HltObjectSummary::Info info = obj->numericalInfo();
  double p = 1/info["7#Particle.1/p"];
  double slopex = info["5#Particle.slopes.x"];
  double slopey = info["6#Particle.slopes.y"];
  double slopez = 1/sqrt(slopex*slopex + slopey*slopey + 1.0);
  double pz = slopez*p;
  double mm = info["1#Particle.measuredMass"];
  p4.SetPxPyPzE(slopex*pz,slopey*pz,pz,sqrt(p*p+mm*mm));
}
// =============================================================================
DECLARE_TOOL_FACTORY(HighPtTopoTool)
// =============================================================================
