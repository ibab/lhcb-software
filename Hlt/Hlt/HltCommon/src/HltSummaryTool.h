// $Id: HltSummaryTool.h,v 1.10 2008-05-15 08:56:55 graven Exp $
#ifndef HLTCOMMON_HLTSUMMARYTOOL_H 
#define HLTCOMMON_HLTSUMMARYTOOL_H 1

// Include files
// from Gaudi
#include "HltBase/HltBaseTool.h"
#include "HltBase/IHltConfSummaryTool.h"
#include "Kernel/IHltSummaryTool.h"
#include "Event/HltSummary.h"
#include "GaudiKernel/IIncidentListener.h"

/** @class HltSummaryTool HltSummaryTool.h
 *  
 * functionality:
 *      simplify common operations to be asked to the hlt summary class
 *      please load the tool at the begin of your execute of your algorithm
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2007-02-08
 */
class HltSummaryTool : public HltBaseTool
                       , virtual public IIncidentListener
                       , virtual public IHltConfSummaryTool
                       , virtual public IHltSummaryTool
{
public:
  
  /// Standard constructor
  HltSummaryTool( const std::string& type, 
                  const std::string& name,
                  const IInterface* parent);

  virtual ~HltSummaryTool( ); ///< Destructor

  virtual StatusCode initialize();
  virtual void handle(const Incident&);



  StatusCode load() {getSummary();return StatusCode::SUCCESS;}  

  void setSummary(const LHCb::HltSummary& summary) {};
  

  const LHCb::HltSummary& summary();
  
  bool decision();
  
  bool decisionType(const std::string& name);
  
  bool hasSelection(const std::string& name);
  
  bool selectionDecision(const std::string& name);
  
  bool selectionDecisionType(const std::string& name, 
                             const std::string& type);
  
  std::vector<std::string> selections();

  size_t selectionNCandidates(const std::string& name);
  
  std::vector<std::string> selectionFilters(const std::string& name);

  std::string selectionType(const std::string& name);

  std::vector<std::string> selectionInputSelections(const std::string& name);

  std::vector<LHCb::Track*> selectionTracks(const std::string& name);
  
  std::vector<LHCb::RecVertex*> selectionVertices(const std::string& name);

  std::vector<const LHCb::Particle*> selectionParticles(const std::string& name);
  
  bool isInSelection(const std::string& name,
                     const LHCb::Track& track );





  std::vector<std::string> confKeys();

  int confInt(const std::string& name);

  double confDouble(const std::string& name);

  std::string confString(const std::string& name) ;

  std::vector<std::string> confStringVector(const std::string& name);  

private:

  const LHCb::HltSummary& getSummary();

  bool checkSelection(const std::string& name);
  
  std::string m_hltSummaryLocation;
  LHCb::HltSummary* m_summary;

  int id(const std::string& name) const {
     return annSvc().value("SelectionID",name)->second;
  }
  std::string name(int id) const {
     return annSvc().value("SelectionID",id)->first;
  }
  
};
#endif // HLTCOMMON_HLTSUMMARYTOOL_H
