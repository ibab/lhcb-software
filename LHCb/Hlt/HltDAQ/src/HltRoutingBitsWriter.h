// $Id: HltRoutingBitsWriter.h,v 1.5 2010-08-10 14:05:37 graven Exp $
#ifndef HLTCOMMON_HLTROUTINGBITSWRITER_H
#define HLTCOMMON_HLTROUTINGBITSWRITER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiKernel/IUpdateManagerSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "DetDesc/Condition.h"
#include <array>

#include "Kernel/IHltMonitorSvc.h"

#include "LoKi/OdinTypes.h"
#include "LoKi/L0Types.h"
#include "LoKi/HLTTypes.h"

/** @class HltRoutingBitsWriter HltRoutingBitsWriter.h
 *
 *
 *  @author Gerhard Raven
 *  @date   2008-07-29
 */
class HltRoutingBitsWriter : public GaudiHistoAlg, IIncidentListener {
public:
  /// Standard constructor
  HltRoutingBitsWriter( const std::string& name, ISvcLocator* pSvcLocator );

  ~HltRoutingBitsWriter( ) override = default; ///< Destructor

  StatusCode initialize() override;    ///< Algorithm initialization
  StatusCode execute   () override;    ///< Algorithm execution
private:
  StatusCode decode();

  enum { nBits = 3*sizeof(unsigned int)*8 };
  struct hlt_eval_t {
    LoKi::Types::HLT_Cut *predicate;
    StatEntity *counter;
    AIDA::IHistogram1D* hist;
    RateCounter* rate;
  } ;
  struct l0_eval_t {
    LoKi::Types::L0_Cut *predicate;
    StatEntity *counter;
    AIDA::IHistogram1D* hist;
    RateCounter* rate;
  } ;
  struct odin_eval_t {
    LoKi::Types::ODIN_Cut *predicate;
    StatEntity *counter;
    AIDA::IHistogram1D* hist;
    RateCounter* rate;
  } ;
  // 8 ODIN, 24 L0DU, 32 Hlt1, 32 Hlt2
  std::array<odin_eval_t,8> m_odin_evaluators;
  std::array<l0_eval_t, 24> m_l0_evaluators;
  std::array<hlt_eval_t,64> m_hlt_evaluators;
  void zeroEvaluators(bool skipDelete=false);

  Condition *m_runpars;
  IUpdateManagerSvc *m_updMgrSvc;
  IHltMonitorSvc* m_hltMonSvc;

  StatusCode i_updateConditions();
  void handle(const Incident&);

  std::string m_monSvc;
  std::string m_odin_location;
  std::string m_l0_location;
  std::string m_hlt_location[2];

  unsigned long long m_startOfRun;
  double m_binWidth; // in _minutes_!
  double m_timeSpan; // in _minutes_!

  bool updateRequired() const { return m_bits_updated || m_preambulo_updated; }

  std::map<unsigned int,std::string> m_bits;
  bool m_bits_updated;
  void updateBits ( Property& );

  std::vector<std::string> m_preambulo_ ;             // the preambulo itself
  std::string m_preambulo ;                           // the preambulo itself
  bool m_preambulo_updated;
  void updatePreambulo ( Property& );

  bool m_useCondDB;
  bool m_updateBank;
};
#endif // HLTROUTINGBITSWRITER_H
