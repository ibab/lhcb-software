#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IAccept.h"
#include "Event/L0CaloCandidate.h"

/** @class CaloGECFilter
 *
 *  Filters on sum of hits in ECAL + HCAL
 *
 *  @author Mike Williams
 *  @date   2014-01-24
 */

class CaloGECFilter : public extends1<GaudiTool,IAccept>  {

  friend class ToolFactory<CaloGECFilter>;

private:

  CaloGECFilter();
  CaloGECFilter(const CaloGECFilter&);
  CaloGECFilter& operator=(const CaloGECFilter&);

  float m_cutVal;

protected:

  CaloGECFilter(const std::string& type,const std::string& name,
		const IInterface* parent);
  ~CaloGECFilter(){}

public:

  virtual StatusCode initialize();
  bool accept() const;

};

CaloGECFilter::CaloGECFilter(const std::string& type,const std::string& name,
			     const IInterface* parent)
  : base_class(type,name,parent), m_cutVal(1000){
  declareProperty("MinMult", m_cutVal);
}

StatusCode CaloGECFilter::initialize(){
  StatusCode sc = GaudiTool::initialize();
  if(sc.isFailure()) return sc;
  return StatusCode::SUCCESS;
}
  
bool CaloGECFilter::accept() const {
  double multECAL=0,multHCAL=0;
  LHCb::L0CaloCandidates *cs = get<LHCb::L0CaloCandidates>("LLT/Calo");
  for (
       LHCb::L0CaloCandidates::iterator it = cs->begin();
       it != cs->end();
       ++it
       ) {
    LHCb::L0CaloCandidate *candidate = *it;
    if (candidate->type() == 6) {  // We have a multiplicity cut.
      // Personally, I'm not so very sure if using a double for such a
      // switch is a good idea.
      if (candidate->et() == 0.) {  // ECAL
	multECAL = candidate->etCode();
      } else if (candidate->et() == 1.) {  // HCAL
	multHCAL = candidate->etCode();
      } else { // This should not happen.
	fatal() << "Multiplicity is neither ECAL nor HCAL!" << endmsg;
      }
    }
  }
  return ((multECAL+multHCAL) < m_cutVal);
}

DECLARE_TOOL_FACTORY(CaloGECFilter)
