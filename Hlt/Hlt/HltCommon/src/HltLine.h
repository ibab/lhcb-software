// $Id: HltLine.h,v 1.11 2009-04-18 11:51:19 graven Exp $
#ifndef HLTLINE_H
#define HLTLINE_H 1

// Include files
// from Gaudi
#include "Kernel/IANNSvc.h"
#include "HltBase/IHltDataSvc.h"
#include "GaudiAlg/GaudiHistoAlg.h"
#include "boost/array.hpp"

// Forward declarations
class ISequencerTimerTool;
class Algorithm;
class IJobOptionsSvc;
class IAlgManager;

/** @class HltLine HltLine.h
 *  Sequencer for Hlt processing, adapted from GaudiSequencer
 *
 *  @author Gerhard Raven
 *  @date   2008-09-13
 */
class HltLine : public GaudiHistoAlg{
public:
  /// Standard constructor
  HltLine( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltLine( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution


  void resetExecuted();         ///< Called before an event processing


  class HltStage {
  public:
    HltStage(HltLine& parent, const std::string& name)
    : m_parent(parent)
    , m_timerTool(0) 
    , m_algorithm(0)
    , m_property(name, std::string())
    , m_reverse(false)
    , m_dirty(true)
    , m_initialized(false)
    {  m_property.declareUpdateHandler(&HltStage::updateHandler,this); }

    HltStage( HltStage& rhs )
    : m_parent(rhs.m_parent)
    , m_timerTool(rhs.m_timerTool) 
    , m_algorithm(0)
    , m_property( rhs.m_property )
    , m_reverse(false)
    , m_dirty(true)
    , m_initialized(false)
    { m_property.declareUpdateHandler(&HltStage::updateHandler,this); }

    ~HltStage() { if (m_algorithm) m_algorithm->release();}; ///< Destructor
    bool passed() const            { return algorithm()?algorithm()->filterPassed():true; }
    StatusCode execute(ISequencerTimerTool* = 0);
    StatusCode initialize(ISequencerTimerTool* = 0);
    std::string toString() const { return name(); }
    StatusCode  fromString(const std::string& name);
    const std::string& name()     const { return m_property.value(); }
    void resetExecuted() const          { if (algorithm()) algorithm()->resetExecuted(); }
    StringProperty& property()          { return m_property; }
  private:
    void setTimer( int nb )              { m_timer = nb;       }
    Algorithm* algorithm()        const  { return m_algorithm; }
    bool       reverse()          const  { return m_reverse;   }
    int        timer()            const  { return m_timer;     }
    void updateHandler(Property& prop);

    HltLine&    m_parent;
    ISequencerTimerTool* m_timerTool;      ///< Pointer to the timer tool
    Algorithm*  m_algorithm;   ///< Algorithm pointer
    StringProperty  m_property;
    bool        m_reverse;     ///< Indicates that the flag has to be inverted
    bool        m_dirty;
    bool        m_initialized;
    int         m_timer;       ///< Timer number fo rthis algorithm
  };

private:
  //TODO: move into DecReport...
  enum stage { initial=          0,  // i.e. did not pass 'prescale
               prescaled =       1,  // i.e. did not pass 'seed'
               odin      =          2 , // i.e. did not pass 'filter0'
               l0du      =       3,  // i.e. did not pass 'filter1'
               hlt       =       4,  // i.e. did not pass 'filter2'
               filter1ed =       5,  // i.e. did not pass 'filter3'
               filter2ed =       6,  // i.e. did not pass 'postscale'
               postscaled =      7 ,
               nStages = postscaled };
  const std::string& transition( const stage &s) const {
        static std::vector<std::string> s_map;
        if (s_map.empty()) {
            s_map.push_back("Prescale");
            s_map.push_back("ODIN");
            s_map.push_back("L0DU");
            s_map.push_back("HLT");
            s_map.push_back("Filter0");
            s_map.push_back("Filter1");
            s_map.push_back("Postscale");
        }
        return s_map[s];
  };
  typedef std::vector<std::pair<const Algorithm*,unsigned> > SubAlgos;
  SubAlgos retrieveSubAlgorithms() const;

  /** Decode a vector of string. */
  StatusCode decodeNames(  );

  Algorithm* getSubAlgorithm(const std::string& name);
  IANNSvc&     annSvc() const;
  IHltDataSvc& dataSvc() const;

  /** Private copy, assignment operator. This is not allowed **/
  HltLine( const HltLine& a );
  HltLine& operator=( const HltLine& a );


  boost::array<HltStage*,nStages> m_stages; ///< List of algorithms to process.
  SubAlgos          m_subAlgo;        ///< list of subalgorithms and their sub-count
  ISequencerTimerTool* m_timerTool;      ///< Pointer to the timer tool
  IJobOptionsSvc* m_jos;                 ///< Pointer to job options service
  IAlgManager* m_algMgr;                 ///< Pointer to algorithm manager
  AIDA::IHistogram1D* m_stageHisto;
  AIDA::IHistogram1D* m_errorHisto;
  AIDA::IHistogram1D *m_timeHisto;
  AIDA::IHistogram1D *m_stepHisto;
  mutable IANNSvc *m_hltANNSvc;
  mutable IHltDataSvc *m_hltDataSvc;
  Hlt::Selection* m_selection;
  std::string m_outputContainerName;
  std::string m_decision;
  bool m_ignoreFilter;                   ///< True if one continues always.
  bool m_isInitialized;                  ///< Indicate that we are ready
  bool m_measureTime;                    ///< Flag to measure time
  bool m_returnOK;                       ///< Forces the sequencer to return a good status
  bool m_acceptOnError;                  ///< Forces accept if error
  int  m_timer;                          ///< Timer number for the sequencer
  double m_errorRate;                    ///< TODO: why double?
  double m_acceptRate;                   ///< TODO: why double?
};
#endif // HltLine_H
