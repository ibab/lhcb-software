//-----------------------------------------------------------------------------
//
// b-jet classification using the NeuroBayes NN implementation
//
//-----------------------------------------------------------------------------

#ifndef LOKINNBTAG_H
#define LOKINNBTAG_H 1

// Kernel
#include "Kernel/IDistanceCalculator.h"
#include "Kernel/IJetTagTool.h"

// Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ToolFactory.h"



// Track Interfaces
#include "TrackInterfaces/IPVOfflineTool.h"

// Event
#include "Event/RecVertex.h"
#include "Event/ODIN.h"
#include "Event/Particle.h"

#ifdef __GNUC__
#include "NeuroBayesExpert.hh"
#endif

namespace LoKi
{
  class NNBTag : public virtual IJetTagTool, public GaudiTool
  {
    friend class ToolFactory<LoKi::NNBTag>;  // friend factory

  public:

    StatusCode initialize(); // standard initialise
    StatusCode finalize();   // standard finalise

    // Calculate jet properties, including NN classification
    bool calculateJetProperty
    (
     const LHCb::Particle *jet,
     std::map <std::string,double > &jetProperties
     );

  protected:

    // standard constructor
    NNBTag
    (
     const std::string &type,
     const std::string &name,
     const IInterface *parent
     );

    // standard destructor
    ~NNBTag();

    // compare particle pointers by track p_T
    //
    struct ParticlePtrComparePt {
      bool operator() (const LHCb::Particle* lhs, const LHCb::Particle* rhs)
      {
        // we want to sort by decreasing p_T, swap lhs and rhs:
        return ( rhs->pt() < lhs->pt() );
      }
    };

    /// safely retrieve ODIN bank
    LHCb::ODIN* getOdinBank();

    /// clear all data
    void clearState();

    /// get all primary vertices, refit if requested
    void getPrimaries();

    /// extract all charged particles & tracks from jet, refit if requested.
    void extractTracks();

    /// calculate IP and IP significance
    void calculateIP(const LHCb::Particle* p,
                     const Gaudi::XYZPoint& v,
                     double& ip,
                     double& ipsig);

    /// calculate per track variables e.g., ip and NN b-track probability
    void calculateTrackVariables();

    /// calculate per jet variables
    void calculateJetVariables();

    /// apply NeuroBayes b-track network
    float applyTrackNet();

    /// apply NeuroBayes b-jet network
    float applyJetNet();

  private:

    NNBTag();                         // disable default constructor
    NNBTag(const NNBTag&);            // disable copy constructor
    NNBTag& operator=(const NNBTag&); // disable assignement operator

  protected:

    IDistanceCalculator* m_distCalc; // distance calculator
    IPVOfflineTool* m_pvTool;
    IJetTagTool* m_ipJetTagger;

    // per jet cache
    //
    const LHCb::Particle* m_jet;  // jet
    unsigned int m_ntrk_bprob; ///< number of tracks we can assign a b-track probability to
    std::vector<const LHCb::Particle*> m_jet_trk_parts;
    std::vector<const LHCb::Particle*> m_jet_all_parts;
    std::vector<LHCb::Track*> m_jet_tracks;
    std::vector<double> m_trk_ip;
    std::vector<double> m_trk_ipsig;
    std::vector<double> m_trk_bprob;
    std::vector<const LHCb::Track*> m_pvTracks;
    LHCb::RecVertex::Range m_PVs;
    std::vector<LHCb::RecVertex> m_refittedPVs;
    std::vector<const LHCb::RecVertex*> m_refittedPVsPtrs;
    std::map<std::string,double> m_jetWeights;
    double m_sum_trk_netout;

#ifdef __GNUC__
    Expert* m_bTrackExpertForOdd;  ///< NeuroBayes b-track expert for odd events
    Expert* m_bTrackExpertForEven; ///< NeuroBayes b-track expert for even events
    Expert* m_bJetExpertForOdd;    ///< NeuroBayes b-jet expert for odd events
    Expert* m_bJetExpertForEven;   ///< NeuroBayes b-jet expert for even events
#endif

    // NeuroBayes configuration
    //
    static const unsigned int m_n_nb_track_vars = 5;
    static const unsigned int m_n_nb_jet_vars = 9;

    float m_nb_track_vars[m_n_nb_track_vars];
    float m_nb_jet_vars[m_n_nb_jet_vars];

    bool m_oddEvent;

    // tool configuration
    //
    double m_trackNetMinIP;
    double m_trackNetMaxIP;
    double m_trackNetMinIPSig;
    double m_trackNetMinPt;
    bool m_refitTracksAndPVs;
    std::string m_nbExpertiseDirectory;
    std::string m_nbExpertiseVersion;

  };
}

#endif // LOKINNBTAG
