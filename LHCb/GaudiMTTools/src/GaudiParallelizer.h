#ifndef LIB_GAUDIPARALLELIZER_H
#define LIB_GAUDIPARALLELIZER_H 1
// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

//Threadpool library Boost
#include "threadpool/boost/threadpool.hpp"

// Forward declarations
class ISequencerTimerTool;

/** @class GaudiParallelizer GaudiParallelizer.h
  *
  *
  * @author Illya Shapoval
  * @date 09/12/2011
  */
class GaudiParallelizer: public GaudiAlgorithm {
public:
  /// Standard constructor
  GaudiParallelizer(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~GaudiParallelizer(); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

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
      }

      virtual ~AlgorithmEntry( ) {}; ///< Destructor
      void setReverse( bool flag )         { m_reverse   = flag; }

      Algorithm* algorithm()        const  { return m_algorithm; }
      bool       reverse()          const  { return m_reverse;   }
      void       setTimer( int nb )        { m_timer = nb;       }
      int        timer()            const  { return m_timer;     }

      /// Thread task executor method to wrap an algorithm execution in
      void       run( GaudiParallelizer& prlzr ) {
        if ( prlzr.m_measureTime ) prlzr.m_timerTool->start( timer() );
        m_returncode = m_algorithm->sysExecute();
        if ( prlzr.m_measureTime ) prlzr.m_timerTool->stop( timer() );
        algorithm()->setExecuted( true );
      }

      StatusCode  m_returncode;  ///< StatusCode of an algorithm execution received from a thread

    private:
      Algorithm*  m_algorithm;   ///< Algorithm pointer
      bool        m_reverse;     ///< Indicates that the flag has to be inverted
      int         m_timer;       ///< Timer number for this algorithm
    };

  /** Decode a vector of string. */
  StatusCode decodeNames(  );

private:
  StringArrayProperty   m_names;         ///< Input string, list of algorithms
  std::vector<AlgorithmEntry> m_entries; ///< List of algorithms to process.
  bool m_modeOR;                         ///< Indicates that the OR is wanted instead of AND

  bool m_measureTime;                    ///< Flag to measure time
  bool m_returnOK;                       ///< Forces the sequencer to return a good status
  ISequencerTimerTool* m_timerTool;      ///< Pointer to the timer tool
  int  m_timer;                          ///< Timer number for the sequencer

  boost::threadpool::pool m_thrd_pool;   ///< Thread and task pool
  unsigned short m_nthreads;             ///< Number of threads in the thread pool
};

#endif // LIB_GAUDIPARALLELIZER_H
