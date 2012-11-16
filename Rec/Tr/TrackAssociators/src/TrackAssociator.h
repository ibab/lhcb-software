#ifndef TRACKASSOCIATOR_H 
#define TRACKASSOCIATOR_H 1

// Include files

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// from Event/LinkerEvent
#include "Linker/LinkedTo.h"

// Forward declarations

namespace LHCb {
  class MCParticle;
  class MuonCoord;
}

/** @class TrackAssociator TrackAssociator.h
 *  
 *  This algorithm computes the link between a Track and a MCParticle.
 *  The requirement is a match of both the Velo and the Seed part of the 
 *  Track. If there are not enough coordinates, the match is assumed so that
 *  a Velo only or a Seed only are matched properly.
 *  The required fraction of hits is a jobOption 'FractionOK', default 0.70.
 *
 *  Adapted to the new Track Event Model using Linkers
 *  @author Edwin Bos
 *  @date   2005-11-14
 *
 *  Based on the Tr/TrFitAssociators package by :
 *  @author Olivier Callot
 *  @date   2002-07-01
 */

class TrackAssociator : public GaudiAlgorithm {
public:

  // Standard constructor
  TrackAssociator( const std::string& name, ISvcLocator* pSvcLocator );

  // Destructor
  virtual ~TrackAssociator();

  // Algorithm initialization
  virtual StatusCode initialize();

  // Algorithm execution
  virtual StatusCode execute();

private:

  // For counting # and type of Measurements associated to a MCParticle
  void countMCPart( const LHCb::MCParticle* part,
                    double incVelo,
                    double incTT1,
                    double incSeed,
		    double incMuon );

  bool m_debugLevel;

  // jobOptions
  std::string m_tracksInContainer;  //< Name of the input Tracks container
  std::string m_linkerOutTable;     //< Name of the output Linker table
  double      m_fractionOK;         //< minimal good matching fraction
  bool        m_decideUsingMuons;   //< use muon hits in link decision

  // Vector containing the MCParticles which
  // have a Measurement of any type associated to them
  std::vector< const LHCb::MCParticle* > m_parts;

  // Number of Velo Measurements assigned to the MCParticle which
  // has the same vector index in m_parts
  std::vector< double > m_nVelo;

  // Number of TT Measurements assigned to the MCParticle which
  // has the same vector index in m_parts
  std::vector< double > m_nTT1;

  // Number of IT+OT Measurements assigned to the MCParticle which
  // has the same vector index in m_parts
  std::vector< double > m_nSeed;

  // Number of Muon Measurements assigned to the MCParticle which
  // has the same vector index in m_parts
  std::vector< double > m_nMuon;

  double m_nTotVelo;   // Total number of Velo hits
  double m_nTotTT1;    // Total number of TT hits
  double m_nTotSeed;   // Total number of IT+OT hits
  double m_nTotMuon;   // Total number of Muon hits

  typedef LinkedTo<LHCb::MCParticle,LHCb::MuonCoord> MuonLink;
  mutable MuonLink m_muonLink;
};
#endif // TRACKASSOCIATOR_H
