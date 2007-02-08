// $Id: HltSummaryTool.h,v 1.1 2007-02-08 17:38:06 hernando Exp $
#ifndef HLTCOMMON_HLTSUMMARYTOOL_H 
#define HLTCOMMON_HLTSUMMARYTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "HltBase/IHltSummaryTool.h"         
#include "HltBase/HltUtils.h"         


/** @class HltSummaryTool HltSummaryTool.h
 *  
 * functionality:
 *      simplify common operations to be asked to the hlt summary class
 *      please load the tool at the begin of your execute of your algorithm
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2007-02-08
 */
class HltSummaryTool : public GaudiTool, virtual public IHltSummaryTool {
public:
  
  /// Standard constructor
  HltSummaryTool( const std::string& type, 
                  const std::string& name,
                  const IInterface* parent);

  virtual ~HltSummaryTool( ); ///< Destructor
  
  StatusCode load() {
    m_summary = get<LHCb::HltSummary>(m_summaryLocation); 
    if (!m_summary) return StatusCode::FAILURE;
    return StatusCode::SUCCESS;
  }
  
  void setSummary(const LHCb::HltSummary& summary) 
  {m_summary = (LHCb::HltSummary*) (&summary);}
  
  bool isInSelection(const LHCb::Track& track, int id);

protected:

  std::string m_summaryLocation;
  LHCb::HltSummary* m_summary;
  
};
#endif // HLTCOMMON_HLTSUMMARYTOOL_H
