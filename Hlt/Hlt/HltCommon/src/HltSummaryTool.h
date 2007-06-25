// $Id: HltSummaryTool.h,v 1.2 2007-06-25 20:50:25 hernando Exp $
#ifndef HLTCOMMON_HLTSUMMARYTOOL_H 
#define HLTCOMMON_HLTSUMMARYTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "HltBase/IHltSummaryTool.h"
#include "HltBase/HltTypes.h"
#include "Event/HltSummary.h"

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

  StatusCode initialize();

  const LHCb::HltSummary& summary();
  
  bool decision();
  
  bool decisionType(const std::string& name);
  
  bool hasSelection(const std::string& name);
  
  bool selectionDecision(const std::string& name);
  
  bool selectionDecisionType(const std::string& name, 
                             const std::string& type);
  
  std::vector<std::string> selections();
  
  std::vector<std::string> selectionFilters(const std::string& name);
  
  std::vector<std::string> selectionAncestors(const std::string& name);
  
  std::vector<LHCb::Track*> selectionTracks(const std::string& name);
  
  std::vector<LHCb::RecVertex*> selectionVertices(const std::string& name);
  
  bool isInSelection(const std::string& name,
                     const LHCb::Track& track );

  std::vector<std::string> confKeys();

  std::string confString(const std::string& name) ;

  std::vector<std::string> confStringVector(const std::string& name);  

protected:

  void getSummary();
  
protected:

  IDataProviderSvc* m_hltSvc;
  Hlt::Configuration* m_conf;

  std::string m_summaryLocation;
  LHCb::HltSummary* m_summary;
  
};
#endif // HLTCOMMON_HLTSUMMARYTOOL_H
