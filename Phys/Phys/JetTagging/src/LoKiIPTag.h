// $Id: LokiIPTag.h,v 1.1 2011-06-24 10:10:10 aphan Exp $


//-----------------------------------------------------------------------------
//
// bjet identification based on track impact parameters
//
//-----------------------------------------------------------------------------

#ifndef LOKIIPTAG_H
#define LOKIIPTAG_H 1

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


// ============================================================================
// Declaration
namespace LoKi
{
  class IPTag
    : public virtual IJetTagTool
    , public GaudiTool
  {
    friend class ToolFactory<LoKi::IPTag>;  // friend factory

    public:

      StatusCode initialize(); // standard initialise
      StatusCode finalize();   // standard finalise

      // Method that calculates the tag weight of a jet
      bool calculateJetProperty
      (
        const LHCb::Particle *jet,
	std::map <std::string,double> &jetWeight
      );

      // Method that sets the jet vertex
      StatusCode setJetVertex();

    protected:

      // standard constructor
      IPTag
      (
        const std::string &type,
        const std::string &name,
        const IInterface *parent
      );

       // standard destructor
      ~IPTag();

    private:

      IPTag();                          // disable default constructor
      IPTag(const IPTag&);            // disable copy constructor
      IPTag& operator=(const IPTag&); // disable assignement operator

      const LHCb::Particle *m_Jet;  // jet
      IJetMaker::Input m_JetInputs; // jet inputs
      LHCb::RecVertex m_JetVertex;  // jet vertex

      LHCb::Particles *m_CleanedDaugthers;    // daughter vector
      StatusCode removeV0();      // remove V0 daugthers from the jets daugthers
      double m_DauTrackCutChi;    // daughter cut on track chi squared probability
      double m_DauTrackCutGhost;  // daughter cut on ghost probability
      double m_DtrakMax;
      double m_DMK0;
      double m_DMLambda;
      double m_secCut;
      double m_sumCut;
      IDistanceCalculator *m_distCalc; // distance calculator


      std::string m_AlgMode; // algorithm mode - SumTracks, SecondTrack, ThirdTrack
  };
}

#endif
