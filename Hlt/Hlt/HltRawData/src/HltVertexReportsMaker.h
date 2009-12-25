// $Id: HltVertexReportsMaker.h,v 1.6 2009-12-25 19:22:14 graven Exp $
#ifndef HLTVERTEXREPORTSMAKER_H 
#define HLTVERTEXREPORTSMAKER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "Event/HltVertexReports.h"
#include "Kernel/IANNSvc.h"
#include "Kernel/IOnOffline.h"
#include "HltBase/IHltRegister.h"
#include "HltBase/IHltData.h"
#include "HltBase/IHltInspector.h"

namespace LHCb { class HltVertexReports; };

/** @class HltvertexReportsMaker HltvertexReportsMaker.h
 *  
 *  @author Tomasz Skwarnicki
 *  @date   2008-08-05
 *
 *  Algorithm to translate HltSummary  into HltVertexReports
 *
 */
class HltVertexReportsMaker : public GaudiAlgorithm {
public:

  /// Standard constructor
  HltVertexReportsMaker( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltVertexReportsMaker( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:
  template <typename ITER>
  StatusCode HltVertexReportsMaker::saveCandidates(const std::string& selName, ITER begin, ITER end, LHCb::HltVertexReports* output) const ;



  // ----------------------- data members 

  /// location of output Hlt Summary
  std::string  m_outputHltVertexReportsLocation;

  /// location of output Hlt Summary
  std::vector<Gaudi::StringKey>  m_vertexSelections;

  /// HltANNSvc for making selection names to int selection ID
  IANNSvc* m_hltANNSvc;  
  Hlt::IData* m_hltSvc;
  Hlt::IRegister* m_regSvc;
  Hlt::IInspector* m_inspectionSvc;;

  IOnOffline* m_onOfflineTool;

  std::vector<const Hlt::Selection*> m_selections;
  std::vector<std::pair<std::string,std::string> >           m_tesSelections;

};

#endif // HLTVERTEXREPORTSMAKER_H
