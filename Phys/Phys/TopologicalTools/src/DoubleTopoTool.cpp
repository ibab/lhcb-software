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

// =============================================================================

void getHltObjP4(const LHCb::HltObjectSummary *obj,Gaudi::LorentzVector &p4){
  LHCb::HltObjectSummary::Info info = obj->numericalInfo();
  double p = 1/info["7#Particle.1/p"];
  double slopex = info["5#Particle.slopes.x"];
  double slopey = info["6#Particle.slopes.y"];
  double slopez = 1/sqrt(slopex*slopex + slopey*slopey + 1.0);
  double pz = slopez*p;
  double mm = info["1#Particle.measuredMass"];
  p4.SetPxPyPzE(slopex*pz,slopey*pz,pz,sqrt(p*p+mm*mm));
}

double getHltObjsAngle(const LHCb::HltObjectSummary *obj1,
                       const LHCb::HltObjectSummary *obj2){
  Gaudi::LorentzVector p41,p42;
  getHltObjP4(obj1,p41);
  getHltObjP4(obj2,p42);
  //return p41.Vect().Angle(p42.Vect());
  return fabs(acos(p41.Vect().Unit().Dot(p42.Vect().Unit())));
}

double getHltObjsDPhi(const LHCb::HltObjectSummary *obj1,
		      const LHCb::HltObjectSummary *obj2){
  Gaudi::LorentzVector p41,p42;
  getHltObjP4(obj1,p41);
  getHltObjP4(obj2,p42);
  //return fabs(p41.DeltaPhi(p42));
  double dphi = fabs(p41.Vect().Phi() - p42.Vect().Phi());
  double pi = 3.14159;
  while(dphi >= pi) dphi -= 2*pi;
  return fabs(dphi);
}


double getHltObjsMass(const LHCb::HltObjectSummary *obj1,
                      const LHCb::HltObjectSummary *obj2){
  Gaudi::LorentzVector p41,p42;
  getHltObjP4(obj1,p41);
  getHltObjP4(obj2,p42);
  return (p41+p42).M();
}

bool addLHCbIDs(SmartRefVector< LHCb::HltObjectSummary >::const_iterator iter,
                std::vector<int> &ids){
  unsigned int len = iter->target()->lhcbIDs().size();
  if(len > 0){
    for(unsigned int i = 0; i < len; i++){
      ids.push_back(iter->target()->lhcbIDs()[i].lhcbID());
    }
    return true;
  }
  return false;
}

void getLHCbIDs(const LHCb::HltObjectSummary *obj, std::vector<int> &ids){
  const SmartRefVector< LHCb::HltObjectSummary > &sub1 = obj->substructure();
  SmartRefVector< LHCb::HltObjectSummary >::const_iterator iter1,iter2,iter3,
    iter4;
  int n = 0;
  for(iter1=sub1.begin();iter1!=sub1.end();++iter1){
    const SmartRefVector< LHCb::HltObjectSummary > &sub2
      = iter1->target()->substructure();
    for(iter2=sub2.begin(); iter2!=sub2.end();++iter2){
      n++; //std::cout << "a";
      if(!addLHCbIDs(iter2,ids)){
        n--; //std::cout << "b";
        const SmartRefVector< LHCb::HltObjectSummary > &sub3
          = iter2->target()->substructure();
        for(iter3=sub3.begin(); iter3!=sub3.end();++iter3){
          n++; //std::cout << "c";
          if(!addLHCbIDs(iter3,ids)){
            n--; //std::cout << "d";
            const SmartRefVector< LHCb::HltObjectSummary > &sub4
              = iter3->target()->substructure();
            for(iter4=sub4.begin(); iter4!=sub4.end();++iter4){
              n++; //std::cout << "e";
              addLHCbIDs(iter4,ids);
            }
          }
        }
      }
    }
  }
  /*
    std::cout << std::endl;
    int len = ids.size();
    std::cout << "LHCb IDs: (" << n << ":" << len << ") [ ";
    //for(int i = 0; i < len; i++) std::cout << ids[i] << " ";
    std::cout << "]" << std::endl;
  */
}

bool doHltObjsOverlap(const LHCb::HltObjectSummary *obj1,
                      const LHCb::HltObjectSummary *obj2){
  std::vector<int> ids1, ids2;
  getLHCbIDs(obj1,ids1);
  getLHCbIDs(obj2,ids2);
  unsigned int l1 = ids1.size(), l2 = ids2.size();
  for(unsigned int i = 0; i < l1; i++){
    for(unsigned int j = 0; j < l2; j++){
      if(ids1[i] == ids2[j]) return true;
    }
  }
  return false;
}

bool doLHCbIDsOverlap(const std::vector<int> &ids1,
                      const std::vector<int> &ids2){
  unsigned int l1 = ids1.size(), l2 = ids2.size();
  for(unsigned int i = 0; i < l1; i++){
    for(unsigned int j = 0; j < l2; j++){
      if(ids1[i] == ids2[j]) return true;
    }
  }
  return false;
}
// =============================================================================

class DoubleTopoTool : public extends1<GaudiTool,IAccept> {

  friend class ToolFactory<DoubleTopoTool>;

private:

  DoubleTopoTool();
  DoubleTopoTool(const DoubleTopoTool&);
  DoubleTopoTool& operator=(const DoubleTopoTool&);

  float m_minAngle;
  float m_minDPhi;
  float m_minMass;
  std::string m_hltdecision;
  ITriggerTisTos *m_tistostool;


protected:

  DoubleTopoTool(const std::string& type,const std::string& name,
                 const IInterface* parent);
  ~DoubleTopoTool(){}

public:

  virtual StatusCode initialize();
  bool accept() const;

};
// =============================================================================

DoubleTopoTool::DoubleTopoTool(const std::string& type,const std::string& name,
                               const IInterface* parent)
  : base_class(type,name,parent), m_tistostool(0) {
  declareProperty("minAngle"    , m_minAngle = 2/57.);
  declareProperty("minDPhi"     , m_minDPhi = 0/57.);
  declareProperty("minMass"     , m_minMass = 20000.0);
  declareProperty("hltdecision" , m_hltdecision = "Hlt2Topo.*BBDTDecision");
}

StatusCode DoubleTopoTool::initialize(){
  StatusCode sc = GaudiTool::initialize();
  if(sc.isFailure()) return sc;
  m_tistostool = tool<ITriggerTisTos>("TriggerTisTos","TriggerTisTosTool",this);
  m_tistostool->setOfflineInput();
  //m_tistostool->setTriggerInput();
  return StatusCode::SUCCESS;
}

bool DoubleTopoTool::accept() const {
  std::vector<const LHCb::HltObjectSummary*> hltObjs
    = m_tistostool->hltObjectSummaries(m_hltdecision,2,2);
  unsigned int num = hltObjs.size();
  //std::cout << "num HltObjs =" << num << std::endl;
  //std::vector<const LHCb::HltObjectSummary*>::const_iterator iter1,iter2;
  std::vector<std::vector<int> > lhcbIDs(num);
  for(unsigned int i = 0; i < num; i++){
    if(lhcbIDs[i].size() == 0) getLHCbIDs(hltObjs[i],lhcbIDs[i]);
    for(unsigned int j = i+1; j < num; j++){
      if(lhcbIDs[j].size() == 0) getLHCbIDs(hltObjs[j],lhcbIDs[j]);
      if(!doLHCbIDsOverlap(lhcbIDs[i],lhcbIDs[j])){
        if(getHltObjsAngle(hltObjs[i],hltObjs[j]) > m_minAngle &&
           getHltObjsMass(hltObjs[i],hltObjs[j]) > m_minMass &&
	   getHltObjsDPhi(hltObjs[i],hltObjs[j]) > m_minDPhi) return true;
        //if(getHltObjsMass(hltObjs[i],hltObjs[j]) > m_minMass) return true;
      }
    }
  }
  /*
    for(iter1 = hltObjs.begin(); iter1 != hltObjs.end(); iter1++){
    for(iter2 = iter1+1; iter2 != hltObjs.end(); iter2++){
    if(!doHltObjsOverlap(*iter1,*iter2)){
    if(getHltObjsAngle(*iter1,*iter2) > m_minAngle) {
    return true;
    }
    }
    }
    }
  */
  return false;
}
// =============================================================================
DECLARE_TOOL_FACTORY(DoubleTopoTool)
// =============================================================================
