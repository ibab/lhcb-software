#ifndef TUPLETOOLAllTracks_H
#define TUPLETOOLAllTracks_H

// from Gaudi
#include "Kernel/IParticleTupleTool.h"
#include "TrackInterfaces/ITrackVertexer.h"
//from LHCb
#include "Event/Vertex.h"//includes Gaudi::LorentzVector
//from Analysis
#include "DecayTreeTupleBase/TupleToolBase.h"

class IDVAlgorithm;
class IDistanceCalculator;
class IVertexFit;
class IPVReFitter;
class IParticleDescendants;

using namespace LHCb;

/**

  @class  TupleToolAllTracks TupleToolAllTracks.h
  @brief  Isolation tool based on TupleToolVtxIsoln by Mitesh Patel, Patrick Koppenburg, using ideas from TupleToolSLTools
    The idea is to add particles, passing a selection that is set by configurable parameters,
    from given TES location(s) to a (configurable) target particle in the decay chain
    and provide information that can be used to isolate the specific decay,
    or to simply provide the information needed to study a new exclusive decay chain.

  In a pyton options-file, use e.g.:
  @code
    from Configurables import TupleToolAllTracks
    AllTracks=TupleToolAllTracks("AllTracks")
    locations = []
    loations.append('Phys/StdAllNoPIDsPions/Particles')
    locations.append("Phys/StdLoosePi02gg/Particles")
    locations.append("Phys/StdLooseAllPhotons/Particles")
    AllTracks.ANNPIDCut = 0.3
    AllTracks.GhostProb = 0.5
    AllTracks.Theta = 0.012
    AllTracks.DeltaPhi = 0.005
    AllTracks.NewVertexChi2 = 6
    AllTracks.MHi = 7000
    AllTracks.ImprovedVertex = 4
    AllTracks.PVIPchi2 = 25
    AllTracks.CorrectedMass = True
    AllTracks.InputParticles = locations
    AllTracks.Target = Lambda_c+ #has to be defined in decay descriptor
  @endcode
  then add the tool to the head(!) of your chain.

  For a preselection of the added particles, one can use e.g. a FilterDesktop:
  @code
    pionFilter = FilterDesktop("PionFilter")
    pionFilter.Code = "(P > 2.*GeV) & (P < 200.*GeV)" \
                      "& (PT < 10.*GeV)" \
                      "& (TRCHI2DOF < 3)" 
    SelStdNoPIDsPions = DataOnDemand(Location='Phys/StdAllNoPIDsPions/Particles')
    
    PifromB = Selection("SelPifromB",
                        Algorithm  = pionFilter,
                        RequiredSelections = [SelStdNoPIDsPions])
    SeqPifromB = SelectionSequence("SeqPifromB", TopSelection = PifromB)    
  @endcode
  and pass the outputlocation to the tool (if locations is your list of TES locations)
  @code
    locations.append(SeqPifromB.outputLocation())
  @endcode

  @note
    The calculation of the corrected mass and it's error has not been cross-checked with existing algorithms yet. Use at own risk!

  @date   2015-10-08
  @author Greg Ciezarek, Marco Fiore, Marian Stahl

**/

class TupleToolAllTracks : public TupleToolBase, virtual public IParticleTupleTool {
public:
  // Standard constructor
  TupleToolAllTracks( const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~TupleToolAllTracks( ){}
  virtual StatusCode initialize();
  StatusCode fill(const Particle* , const Particle* , const std::string&  , Tuples::Tuple& );

private:

  //Tools
  IDVAlgorithm         *m_dva;
  IDistanceCalculator  *m_dist;
  const IVertexFit     *m_pVertexFit;
  IPVReFitter          *m_pvReFitter;
  std::string           m_typeVertexFit;
  IParticleDescendants *m_particleDescendants;

  //functions
  bool                                         isTrackInDecay(const Particle *particle, const Particle *mother);
  std::pair<float,std::unique_ptr<RecVertex> > getminipchi   (const Particle *particle);
  std::vector<float>                           getcorrm      (const RecVertex *PV, const Vertex *SV, const Gaudi::LorentzVector &new_mother);

  //member variables
  std::vector<std::string> m_InputParticles;
  std::string              m_TargetParticle;
  float                    m_ANNPIDCut;
  float                    m_theta;
  float                    m_deltaphi;
  float                    m_GhostProb;
  float                    m_DLLPIDCut;
  float                    m_VertexChi2Cut;
  float                    m_ipchi2Cut;
  float                    m_PVipchi2Cut;
  float                    m_improvedVertex;
  float                    m_HiMassCut;
  unsigned long            m_mult;
  bool                     m_addpxpypz;
  bool                     m_addcorrm;
};
#endif // TUPLETOOLAllTracks_H
