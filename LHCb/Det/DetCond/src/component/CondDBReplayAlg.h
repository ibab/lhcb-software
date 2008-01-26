#ifndef CONDDBREPLAYALG_H_
#define CONDDBREPLAYALG_H_
// $Id: CondDBReplayAlg.h,v 1.1 2008-01-26 15:47:28 marcocle Exp $

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/Time.h"
#include <list>

class ICondDBReader;

/** @class CondDBReplayAlg CondDBReplayAlg.h
 *  
 *  Simple algorithm that reads a file in the format produced by CondDBLogger
 *  and re-play the request to the database with the same timing written in the
 *  log file.
 *
 *  @author Marco Clemencic <marco.clemencic@cern.ch>
 *  @date   2008-01-25
 */
class CondDBReplayAlg : public GaudiAlgorithm {
public: 
  /// Standard constructor
  CondDBReplayAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CondDBReplayAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  /// Path to the file containing the log.
  std::string m_logFileName;

  /// Name of the reader to use to replay the requests.
  std::string m_readerName;

  /// Pointer to the ICondDBReader service.
  ICondDBReader *m_reader;

  struct operation_t {
    Gaudi::Time time;
    std::string node;
    Gaudi::Time evttime;
    bool use_numeric_channel;
    cool::ChannelId channel;
    std::string chn_name;
  };
  typedef std::list<operation_t> list_t;
  
  /// List of operations to perform
  list_t m_operations;

};

#endif /*CONDDBREPLAYALG_H_*/
