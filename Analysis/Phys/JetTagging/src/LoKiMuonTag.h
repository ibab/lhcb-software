// $Id: LokiMuonTag.h,v 1.1 2011-06-24 10:10:10 aphan Exp $


//-----------------------------------------------------------------------------
//
// bjet identification based on muons
//
//-----------------------------------------------------------------------------

#ifndef LOKIMUONTAG_H
#define LOKIMUONTAG_H 1

// ============================================================================
// Includes

// Kernel
#include "Kernel/IJetMaker.h"
#include "Kernel/IParticle2MCAssociator.h"
#include "Kernel/IDistanceCalculator.h"
#include "Kernel/IJetTagTool.h"
#include "Kernel/IJets2Jets.h"
#include "Kernel/ParticleID.h"

// Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IHistogramSvc.h"

// LoKi
#include "LoKi/ILoKiSvc.h"

// ============================================================================
// Declaration
namespace LoKi
{
  class MuonTag
    : public virtual IJetTagTool
    , public GaudiTool
  {
    friend class ToolFactory<LoKi::MuonTag>;  // friend factory

    public:

      StatusCode initialize(); // standard initialise
      StatusCode finalize();   // standard finalise

      // Method that calculates the tag weight of a jet
      bool calculateJetProperty
      (
        const LHCb::Particle *jet,
        std::map <std::string,double > &jetWeight
      );

      // Method that sets the jet vertex
      StatusCode setJetVertex();

    protected:

      // standard constructor
      MuonTag
      (
        const std::string &type,
        const std::string &name,
        const IInterface *parent
      );

       // standard destructor
      ~MuonTag();

    private:

      MuonTag();                          // disable default constructor
      MuonTag(const MuonTag&);            // disable copy constructor
      MuonTag& operator=(const MuonTag&); // disable assignement operator

      const LHCb::Particle *m_Jet;  // jet
      IJetMaker::Input m_JetInputs; // jet inputs
      LHCb::RecVertex m_JetVertex;  // jet vertex

      LHCb::Particles *m_Muons;    // muon vector
      StatusCode getMuons();       // get and select muons
      double m_MuonCutPTRel;       // muon cut on ptrel
      double m_MuonCutDR;          // muon cut on dr
      double m_MuonCutPT;          // muon cut on pt
      double m_MuonTrackCutChi;    // muon cut on track chi squared probability
      double m_MuonTrackCutIPSig;  // muon cut on track ip significance
      double m_MuonTrackCutGhost;  // muon cut on ghost probability

      IDistanceCalculator *m_distCalc; // distance calculator

      std::string m_AlgMode; // algorithm mode - PtRel or IPSig

      // Particle to mcparticle associator tool and type
      IParticle2MCAssociator* m_p2mcAssoc;
      std::string m_p2mcAssocType;
  };
}

#endif
