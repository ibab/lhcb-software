// $Id: LokiMuonTag.h,v 1.1 2011-06-24 10:10:10 aphan Exp $


//-----------------------------------------------------------------------------
//
// bjet identification based on muons
//
//-----------------------------------------------------------------------------

#ifndef LOKIELECTRONTAG_H
#define LOKIELECTRONTAG_H 1

// ============================================================================
// Includes

// Kernel
#include "Kernel/IJetMaker.h"
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
  class ElectronTag
    : public virtual IJetTagTool
    , public GaudiTool
  {
    friend class ToolFactory<LoKi::ElectronTag>;  // friend factory

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
      ElectronTag
      (
        const std::string &type,
        const std::string &name,
        const IInterface *parent
      );

       // standard destructor
      ~ElectronTag();

    private:

      ElectronTag();                          // disable default constructor
      ElectronTag(const ElectronTag&);            // disable copy constructor
      ElectronTag& operator=(const ElectronTag&); // disable assignement operator

      const LHCb::Particle *m_Jet;  // jet
      IJetMaker::Input m_JetInputs; // jet inputs
      LHCb::RecVertex m_JetVertex;  // jet vertex

      LHCb::Particles *m_Electrons;    // electron vector
      StatusCode getElectrons();       // get and select electrons
      double m_ElectronCutPTRel;       // electron cut on ptrel
      double m_ElectronCutDR;          // electron cut on dr
      double m_ElectronCutPT;          // electron cut on pt
      double m_ElectronTrackCutChi;    // electron cut on track chi squared probability
      double m_ElectronTrackCutIP;     // electron cut on track ip significance
      double m_ElectronTrackCutGhost;  // electron cut on ghost probability

      IDistanceCalculator *m_distCalc; // distance calculator

      std::string m_AlgMode; // algorithm mode - PtRel or IPSig

  };
}

#endif
