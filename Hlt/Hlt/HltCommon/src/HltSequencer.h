// $Id: HltSequencer.h,v 1.4 2007-01-11 14:07:14 hernando Exp $
#ifndef GAUDISEQUENCER_H 
#define GAUDISEQUENCER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"

// using namespace Gaudi;

// Forward declarations
class ISequencerTimerTool;

/** @class HltSequencer HltSequencer.h
 *  Sequencer for executing several algorithms, stopping when one is faulty.
 *
 *  @author Olivier Callot
 *  @date   2004-05-13
 */
class HltSequencer : public GaudiHistoAlg {
public:

  /// Standard constructor
  HltSequencer( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltSequencer( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  virtual StatusCode beginRun  ();    ///< Algorithm beginRun
  virtual StatusCode endRun    ();    ///< Algorithm endRun

  StatusCode resetExecuted();         ///< Called before an event processing

  /** for asynchronous changes in the list of algorithms */
  void membershipHandler( Property& theProp );

protected:

  class AlgorithmEntry {
  public: 
    /// Standard constructor
    AlgorithmEntry( Algorithm* alg ) {
      m_algorithm = alg;
      m_reverse = false;
      m_timer = 0;
    };

    virtual ~AlgorithmEntry( ) {}; ///< Destructor
    void setReverse( bool flag )            { m_reverse   = flag; };

    Algorithm* algorithm()        const  { return m_algorithm; };
    bool       reverse()          const  { return m_reverse;   };
    void       setTimer( int nb )        { m_timer = nb;       };
    int        timer()            const  { return m_timer;     };
  private:
    Algorithm*  m_algorithm;   ///< Algorithm pointer
    bool        m_reverse;     ///< Indicates that the flag has to be inverted
    int         m_timer;       ///< Timer number fo rthis algorithm
  };
  
  /** Decode a vector of string. */
  StatusCode decodeNames(  );
  
private:

  StringArrayProperty m_hisDescriptor;

  /** Private copy, copy not allowed **/
  HltSequencer( const HltSequencer& a );

  /** Private  assignment operator: This is not allowed **/
  HltSequencer& operator=( const HltSequencer& a );

  StringArrayProperty   m_names;         ///< Input string, list of algorithms
  AIDA::IHistogram1D* m_histoTime;
  AIDA::IHistogram1D* m_histoTime0;
  AIDA::IHistogram1D* m_histoRate;
  
  std::vector<AlgorithmEntry> m_entries; ///< List of algorithms to process.
  bool m_modeOR;                         ///< Indicates that the OR is wanted
  bool m_ignoreFilter;                   ///< True if one continues always.
  bool m_isInitialized;                  ///< Indicate that we are ready
  bool m_measureTime;                    ///< Flag to measure time
  bool m_returnOK;                       ///< Forces the sequencer to return a good status
  ISequencerTimerTool* m_timerTool;      ///< Pointer to the timer tool
  int  m_timer;                          ///< Timer number for the sequencer
};
#endif // GAUDISEQUENCER_H
