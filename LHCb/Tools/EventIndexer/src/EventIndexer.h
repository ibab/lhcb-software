#ifndef SRC_EVENTINDEXER_H
#define SRC_EVENTINDEXER_H 1

#include <string>

/// Class for the data to store in the tree
class IndexerData {
public:
  /// Constructor.
  IndexerData(): position(0), eventNumber(0), runNumber(0) {}
  /// Version of the Stripping.
  std::string stripping;
  /// Logical File Name (LFN)
  std::string lfn;
  /// Position of the event in the file.
  long long position;
  /// Event number
  long long eventNumber;
  /// Run number
  long long runNumber;
};

// hide the complex part of the header to Cint
#ifndef __CINT__

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIncidentListener.h"

#include "TFile.h"
#include "TTree.h"

/** @class EventIndexer EventIndexer.h src/EventIndexer.h
  *
  * Algorithm to generate a ROOT TTree used fill and index of the events.
  *
  * @author Marco Clemencic
  * @date 13/03/2013
  */
class EventIndexer: public extends1<GaudiAlgorithm, IIncidentListener> {
public:
  /// Standard constructor
  EventIndexer(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~EventIndexer(); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  virtual void handle(const Incident& incident);

protected:
private:

  /// Output file pointer.
  TFile* m_file;

  /// Output TTree.
  TTree* m_tree;

  /// Output file name property.
  std::string m_outputFileName;

  /// Stripping version (property).
  std::string m_stripping;

  IndexerData m_data;

  SmartIF<IIncidentSvc> m_incSvc;
};

#endif // __CINT__

#endif // SRC_EVENTINDEXER_H
