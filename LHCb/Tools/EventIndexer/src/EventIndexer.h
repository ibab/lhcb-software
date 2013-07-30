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
    /// Stream name.
    std::string stream;
    /// Logical File Name (LFN)
    std::string lfn;
    std::string rawID;  // rawID
    std::string applicationName;
    std::string applicationVersion;
    std::string condDBTags_dict;
    /// Position of the event in the file.
    long long position;
    /// Event number
    long long eventNumber;
    /// Run number
    long long runNumber;
    unsigned long long gpsTime;
    int nPVs;               // Number of reconstructed Primary Vertices
    int nLongTracks;       // Number of reconstructed Long Tracks
    int nDownstreamTracks; // Number of reconstructed Downstream Tracks
    int nUpstreamTracks;   // Number of reconstructed Upstream Tracks
    int nVeloTracks;       // Number of reconstructed VELO Tracks
    int nTTracks;          // Number of reconstructed T-station Tracks
    int nBackTracks;       // Number of reconstructed VELO backwards Tracks
    int nTracks;           // Number of reconstructed Tracks
    int nRich1Hits;        // Number of hits in RICH1
    int nRich2Hits;        // Number of hits in RICH2
    int nVeloClusters;     // Number of VELO clusters
    int nITClusters;       // Number of IT clusters
    int nTTClusters;       // Number of TT clusters
    int nUTClusters;       // Number of UT clusters
    int nOTClusters;       // Number of OT clusters
    int nFTClusters;       // Number of FT clusters
    int nSPDhits;          // Number of SPD hits
    int nMuonCoordsS0;     // Number of Coords in Muon Station 0
    int nMuonCoordsS1;     // Number of Coords in Muon Station 1
    int nMuonCoordsS2;     // Number of Coords in Muon Station 2
    int nMuonCoordsS3;     // Number of Coords in Muon Station 3
    int nMuonCoordsS4;     // Number of Coords in Muon Station 4
    int nMuonTracks;        // Number of Muon Tracks

    std::string stripping_lines_dict;
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
