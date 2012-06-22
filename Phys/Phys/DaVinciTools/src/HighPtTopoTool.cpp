// $Id: $  -*- C++ -*-                                                        
// ============================================================================
// Include files
#include <string>
#include <fstream>
#include <vector>
#include "GaudiKernel/ToolFactory.h"
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/ITriggerTisTos.h"
#include "Kernel/IAccept.h"
#include "Event/HltObjectSummary.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Vector4DTypes.h"

// =============================================================================

class HighPtTopoTool : public extends1<GaudiTool,IAccept> {

  friend class ToolFactory<HighPtTopoTool>;

private:

  HighPtTopoTool();
  HighPtTopoTool(const HighPtTopoTool&);
  HighPtTopoTool& operator=(const HighPtTopoTool&);

  float m_minpT;
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
  declareProperty("minpT", m_minpT = 20000.);
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
    = m_tistostool->hltObjectSummaries("Hlt2Topo.*BBDTDecision",2,2);
  unsigned int num = hltObjs.size();
  std::vector<std::vector<int> > lhcbIDs(num);
  Gaudi::LorentzVector P4;
  for(unsigned int i = 0; i < num; i++){
    getHltObjP4(hltObjs[i],P4);
    if(P4.Pt()  > m_minpT) return true;
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
