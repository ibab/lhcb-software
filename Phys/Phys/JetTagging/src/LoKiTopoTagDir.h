// $Id: LokiSVTag.h,v 1.1 2011-06-24 10:10:10 aphan Exp $


//-----------------------------------------------------------------------------
//
// bjet identification based on muons
//
//-----------------------------------------------------------------------------

#ifndef LOKITOPOTAGDIR_H
#define LOKITOPOTAGDIR_H 1

// ============================================================================
// Includes

// Kernel
#include "Kernel/IJetMaker.h"
#include "Kernel/IDistanceCalculator.h"
#include "Kernel/IJetTagTool.h"
#include "Kernel/IJets2Jets.h"
#include "Kernel/ParticleID.h"
#include "Event/Particle.h"
#include "Kernel/IDVAlgorithm.h"
// Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IHistogramSvc.h"


class DVAlgorithm;

// ============================================================================
// Declaration
namespace LoKi
{
  class TopoTagDir
    : public virtual IJetTagTool
    , public GaudiTool
  {
    friend class ToolFactory<LoKi::TopoTagDir>;  // friend factory



  protected:

    // standard constructor
    TopoTagDir
      (const std::string& type,
       const std::string& name,
       const IInterface* parent)
      : GaudiTool(type, name, parent)
      , m_Jet(0)
      , m_JetVertex(0)
      , m_distCalc(0)
      , m_dva(0)
      , m_tinputs              ("/Event/Phys/PseudoTopoCands/Particles")
      , m_in2(0)
      // runs ntrack seeder by default
      {
	// Interface
	declareInterface<IJetTagTool>(this);
	// Algorithm related properties
	declareProperty("TestInputs", m_tinputs);
	declareProperty("In2", m_in2);
      }

    // standard destructor
    virtual  ~TopoTagDir(){};



  private:

    TopoTagDir();                        // disable default constructor
    TopoTagDir(const TopoTagDir&);            // disable copy constructor
    TopoTagDir& operator=(const TopoTagDir&); // disable assignement operator

  public:
    const LHCb::Particle *m_Jet;  // jet
    IJetMaker::Input m_JetInputs; // jet inputs
    LHCb::RecVertex m_JetVertex;  // jet vertex

    IDistanceCalculator *m_distCalc; // distance calculator

    IDVAlgorithm* m_dva;

  protected:

    std::string m_tinputs;
    int m_in2;

  private:

    void heavyVChi2Dof(const LHCb::Particle *p, double &chi2, int &dof);
    std::vector<const LHCb::Particle*> extractDaugthers(const LHCb::Particle *p);


  public:

    StatusCode initialize(); // standard initialise
    StatusCode finalize();   // standard finalise

    // Method that calculates the tag weight of a jet
    bool calculateJetProperty
      (
       const LHCb::Particle *jet,
       std::map < std::string,double > &jetWeight
       );


  };
}

#endif
