// $Id: HltLine.h,v 1.1 2008-10-02 13:12:49 graven Exp $
#ifndef HLTLINE_H
#define HLTLINE_H 1

// Include files
// from Gaudi
#include "Kernel/IANNSvc.h"
#include "GaudiAlg/GaudiAlgorithm.h"
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
class HltLine : public GaudiAlgorithm {
public:
  /// Standard constructor
  HltLine( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltLine( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization


  void resetExecuted();         ///< Called before an event processing

  class HltStage {
  public:
    HltStage( )
    : m_parent(0)
    , m_timerTool(0) 
    , m_algorithm(0)
    , m_reverse(false)
    , m_dirty(true)
    , m_initialized(false)
    { }
    ~HltStage() { if (m_algorithm) m_algorithm->release();}; ///< Destructor
    bool passed() const            { return algorithm()?algorithm()->filterPassed():true; }
    StatusCode execute(ISequencerTimerTool* = 0);
    StatusCode initialize(ISequencerTimerTool* = 0);
    std::string toString() const { return name(); }
    StatusCode  fromString(const std::string& name);
    const std::string& name()     const { return m_name; }
    void resetExecuted() const          { if (algorithm()) algorithm()->resetExecuted(); }
  private:
    void setTimer( int nb )        { m_timer = nb;       }
    Algorithm* algorithm()        const  { return m_algorithm; }
    bool       reverse()          const  { return m_reverse;   }
    int        timer()            const  { return m_timer;     }

    HltLine*    m_parent;
    ISequencerTimerTool* m_timerTool;      ///< Pointer to the timer tool
    Algorithm*  m_algorithm;   ///< Algorithm pointer
    std::string m_name;
    bool        m_reverse;     ///< Indicates that the flag has to be inverted
    bool        m_dirty;
    bool        m_initialized;
    int         m_timer;       ///< Timer number fo rthis algorithm
  };

private:
  //TODO: move into DecReport...
  enum stage { initial=          0,  // i.e. did not pass 'prescale
               prescaled =       1,  // i.e. did not pass 'seed'
               seeded =          2 , // i.e. did not pass 'filter'
               filtered =        3,  // i.e. did not pass 'postscale'
               postscaled =      4 ,
               nStages };
  const std::string& transition( const stage &s) const {
        static std::vector<std::string> s_map;
        if (s_map.empty()) {
            s_map.push_back("prescale");
            s_map.push_back("seed");
            s_map.push_back("filter");
            s_map.push_back("postscale");
        }
        return s_map[s];
  }



  /** Decode a vector of string. */
  StatusCode decodeNames(  );

  Algorithm* getSubAlgorithm(const std::string& name);
private:
  IANNSvc&     annSvc() const;

  /** Private copy, assignment operator. This is not allowed **/
  HltLine( const HltLine& a );
  HltLine& operator=( const HltLine& a );


  boost::array<HltStage,nStages> m_stages; ///< List of algorithms to process.
  ISequencerTimerTool* m_timerTool;      ///< Pointer to the timer tool
  IJobOptionsSvc* m_jos;                 ///< Pointer to job options service
  IAlgManager* m_algMgr;                 ///< Pointer to algorithm manager
  mutable IANNSvc *m_hltANNSvc;
  std::string m_outputContainerName;
  bool m_ignoreFilter;                   ///< True if one continues always.
  bool m_isInitialized;                  ///< Indicate that we are ready
  bool m_measureTime;                    ///< Flag to measure time
  bool m_returnOK;                       ///< Forces the sequencer to return a good status
  bool m_acceptOnError;                  ///< Forces accept if error
  int  m_timer;                          ///< Timer number for the sequencer
};
#endif // HltLine_H
