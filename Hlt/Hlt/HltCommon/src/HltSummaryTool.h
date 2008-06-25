// $Id: HltSummaryTool.h,v 1.12 2008-06-25 20:06:33 graven Exp $
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
  
  // returns the HLT summary
  const LHCb::HltSummary& summary();
  
  /// returns the HLT decision
  bool decision();
  
  /// do not use
  bool decisionType(const std::string& name);
  
  /// returns true if a given HLT selection is in the summary
  bool hasSelection(const std::string& name);
  
  /// returns the decision of a given HLT selecion
  bool selectionDecision(const std::string& name);
  
  // do not use
  bool selectionDecisionType(const std::string& name, 
                             const std::string& type);
  
  // returns the list of selection stored in the summmary
  std::vector<std::string> selections();

  // returns the number of candidates of a given selection
  size_t selectionNCandidates(const std::string& name);
  
  // returns a list of string that code the filter/cuts applied in a selection
  std::vector<std::string> selectionFilters(const std::string& name);

  // returns the type of candidates of a given selection, i.e Track, Vertex, Particle
  std::string selectionType(const std::string& name);

  // returns a list of input selections required to produce this selection
  std::vector<std::string> selectionInputSelections(const std::string& name);

  // returns a list of track candidates for this selection
  std::vector<LHCb::Track*> selectionTracks(const std::string& name);
  
  // returns a list of vertices candidates for this selection
  std::vector<LHCb::RecVertex*> selectionVertices(const std::string& name);

  // returns a list of particles candidates for this selection
  std::vector<const LHCb::Particle*> selectionParticles(const std::string& name);
  
  // do not use
  bool isInSelection(const std::string& name,
                     const LHCb::Track& track );

  // returns all the keys stored in the HLT configuration dictionary
  std::vector<std::string> confKeys();

  // returns the int value asociated to a HLT configuration key
  int confInt(const std::string& name);

  // returns the double value asociated to a HLT configuration key
  double confDouble(const std::string& name);

  // returns the string value asociated to a HLT configuration key
  std::string confString(const std::string& name) ;

  // returns a list of strings asociated to a HLT configuration key
  std::vector<std::string> confStringVector(const std::string& name);  

private:

  const LHCb::HltSummary& getSummary();

  bool checkSelection(const std::string& name);
  
  std::string m_hltSummaryLocation;
  LHCb::HltSummary* m_summary;

};
#endif // HLTCOMMON_HLTSUMMARYTOOL_H
