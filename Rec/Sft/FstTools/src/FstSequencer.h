#ifndef FSTSEQUENCER_H 
#define FSTSEQUENCER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class FstSequencer FstSequencer.h
 *  Sequencer for the First Software Trigger.
 *
 *  @author Olivier Callot
 *  @date   2012-10-08
 */
class FstSequencer : public GaudiAlgorithm {
public: 
  /// Standard constructor
  FstSequencer( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~FstSequencer( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  void resetExecuted();         ///< Called before an event processing

protected:
  class AlgorithmEntry {
  public:
    /// Standard constructor
    AlgorithmEntry( Algorithm* alg ) {
      m_algorithm = alg;
      m_timer = 0;
    }

    virtual ~AlgorithmEntry( ) {}; ///< Destructor

    Algorithm* algorithm()        const  { return m_algorithm; }
    void       setTimer( int nb )        { m_timer = nb;       }
    int        timer()            const  { return m_timer;     }
  private:
    Algorithm*  m_algorithm;   ///< Algorithm pointer
    int         m_timer;       ///< Timer number fo rthis algorithm
  };

  /** Decode a vector of string. */
  StatusCode decodeNames(  );

private:
  /** Private copy, copy not allowed **/
  FstSequencer( const FstSequencer& a );

  /** Private  assignment operator: This is not allowed **/
  FstSequencer& operator=( const FstSequencer& a );

  StringArrayProperty   m_names;         ///< Input string, list of algorithms
  bool m_forcePassOK;
  std::vector<AlgorithmEntry> m_entries; ///< List of algorithms to process.
  bool m_isInitialized;                  ///< Indicate that we are ready
  bool m_measureTime;                    ///< Flag to measure time
  ISequencerTimerTool* m_timerTool;      ///< Pointer to the timer tool
  int  m_timer;                          ///< Timer number for the sequencer


  int m_nEvent;
  int m_nAccepted;
};
#endif // FSTSEQUENCER_H
