// $Id: LokiSVTag.h,v 1.1 2011-06-24 10:10:10 aphan Exp $


//-----------------------------------------------------------------------------
//
// bjet identification based on muons
//
//-----------------------------------------------------------------------------

#ifndef LOKISVTAG_H
#define LOKISVTAG_H 1

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


// ============================================================================
// Declaration
namespace LoKi
{
  class SVTag
    : public virtual IJetTagTool
    , public GaudiTool
  {
    friend class ToolFactory<LoKi::SVTag>;  // friend factory

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

  public:

    // standard constructor
    SVTag
      (
       const std::string &type,
       const std::string &name,
       const IInterface *parent
       );

    // standard destructor
    ~SVTag();

  private:
    
    SVTag();                        // disable default constructor
    SVTag(const SVTag&);            // disable copy constructor
    SVTag& operator=(const SVTag&); // disable assignement operator
    
    const LHCb::Particle *m_Jet;  // jet
    IJetMaker::Input m_JetInputs; // jet inputs
    LHCb::RecVertex m_JetVertex;  // jet vertex

    IDistanceCalculator *m_distCalc; // distance calculator

    // Particle to mcparticle associator tool and type
    IParticle2MCAssociator* m_p2mcAssoc;
    std::string m_p2mcAssocType;

  private:

    // SeedFinder
    IJetMaker* m_seedFinder ; // combiner to be used
    std::string  m_seedFinderName ;

    std::string m_AlgMode; // algorithm mode - LengthAvg, LengthMax
    //                - SigAvg, SigMax (default)
    //                - Nvertices
  };
}

#endif
