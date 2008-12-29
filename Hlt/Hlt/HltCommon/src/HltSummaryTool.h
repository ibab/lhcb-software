// $Id: HltSummaryTool.h,v 1.16 2008-12-29 12:56:09 graven Exp $
#ifndef HLTCOMMON_HLTSUMMARYTOOL_H 
#define HLTCOMMON_HLTSUMMARYTOOL_H 1

// Include files
// from Gaudi
#include "HltBase/HltBaseTool.h"
#include "Kernel/IHltSummaryTool.h"
#include "Event/HltSummary.h"
#include "Event/HltDecReports.h"
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

  void setSummary(const LHCb::HltSummary& ) {};
  
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

  // returns the list of selections replacing the wildchar "*"
  std::vector<std::string> selections(const std::string& name);
  
  // returns the number of candidates of a given selection
  size_t selectionNCandidates(const std::string& name);
  
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

  class FakeAlgorithm:virtual public IAlgorithm
  {    
  public:
    FakeAlgorithm(){};
    ~FakeAlgorithm(){};
    const std::string& version() const {return m_dummy;};
    StatusCode execute() {return StatusCode::SUCCESS;};
    bool isInitialized() const {return true;};
    bool isFinalized() const {return true;};
    bool isExecuted() const {return true;};
    StatusCode configure() {return StatusCode::SUCCESS;};
    StatusCode initialize() {return StatusCode::SUCCESS;};
    StatusCode start() {return StatusCode::SUCCESS;};
    StatusCode stop() {return StatusCode::SUCCESS;};
    StatusCode finalize() {return StatusCode::SUCCESS;};
    StatusCode terminate() {return StatusCode::SUCCESS;};
    StatusCode reinitialize() {return StatusCode::SUCCESS;};
    StatusCode restart() {return StatusCode::SUCCESS;};
    Gaudi::StateMachine::State FSMState() const {return Gaudi::StateMachine::State();};
    StatusCode sysInitialize() {return StatusCode::SUCCESS;};
    StatusCode sysStart() {return StatusCode::SUCCESS;};
    StatusCode sysReinitialize() {return StatusCode::SUCCESS;};
    StatusCode sysRestart() {return StatusCode::SUCCESS;};
    StatusCode sysExecute() {return StatusCode::SUCCESS;};
    StatusCode sysStop() {return StatusCode::SUCCESS;};
    StatusCode sysFinalize() {return StatusCode::SUCCESS;};
    StatusCode sysBeginRun( ) {return StatusCode::SUCCESS;};
    StatusCode sysEndRun( ) {return StatusCode::SUCCESS;};
    void resetExecuted( ) {};
    StatusCode beginRun() {return StatusCode::SUCCESS;};
    StatusCode endRun() {return StatusCode::SUCCESS;};
    void setExecuted(bool)    {};
    bool isEnabled() const    {return true;};
    bool filterPassed() const {return true;    };
    void setFilterPassed(bool){};    
    StatusCode queryInterface(const InterfaceID&, void**){return StatusCode::SUCCESS;};
    long unsigned int addRef(){return 0;};
    long unsigned int release(){return 0;};
    const std::string& name() const {return m_dummy;};
    
      
    std::string m_dummy;
  };
  
  


private:

  const LHCb::HltSummary& getSummary();

  void getDecReports();

  bool checkSelection(const std::string& name);
  
  std::string m_hltSummaryLocation;
  LHCb::HltSummary* m_summary;
  std::string m_hltDecReportsLocation;
  LHCb::HltDecReports* m_decReports;

  FakeAlgorithm m_fakeAlgorithm;
};
#endif // HLTCOMMON_HLTSUMMARYTOOL_H
