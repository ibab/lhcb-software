// $Id: $
#ifndef MATHIEUMCFILTERVTXTOPOTRACKSTOOL_H 
#define MATHIEUMCFILTERVTXTOPOTRACKSTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "MCInterfaces/IMCDecayFinder.h"
#include "IncTopoVert/IFilterVtxTopoTracksTool.h"            // Interface
#include "MCInterfaces/IPrintMCDecayTreeTool.h"

#include "GaudiAlg/GaudiHistoTool.h"


#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"

//from DVci
#include "Event/MCParticle.h"
#include "Kernel/Track2MC.h"
#include "Relations/IRelationWeighted2D.h"


using namespace std;

/** @class MCFilterVtxTopoTracksTool MCFilterVtxTopoTracksTool.h
 *  This tool is a filter to selected tracks to be used in ZVTOP from the MC.
 *  @author Julien Cogan and Mathieu Perrin-Terrin
 *  @date   2012-11-08
 */
class MCFilterVtxTopoTracksTool : public GaudiHistoTool, virtual public IFilterVtxTopoTracksTool {
public: 
  /// Standard constructor
  MCFilterVtxTopoTracksTool( const std::string& type, 
                                    const std::string& name,
                                    const IInterface* parent);

  StatusCode initialize();
  struct TrackAncestor
  {
    int MotherPid;
    int MotherMCIndex;
    int DaughterMCIndex;
    double MotherZ;

  };
  
  std::vector<const LHCb::Track*> & filteredTracks(Tuples::Tuple* tuple = NULL);
  std::vector<const LHCb::Track*> & filteredTracks(std::vector<const LHCb::Track*> input_tracks, Tuples::Tuple* tuple = NULL){
    return filteredTracks(tuple);
    };
  void SetParam(double p){
  };

  virtual ~MCFilterVtxTopoTracksTool( ); ///< Destructor


protected:


private:
  
  ///find the decay provided with the decay description
  bool findMCDecay                   ();
  
  //decay descriptor for Bs->TauTau->6Pi and Bs->TauTau->Mu3Pi
  string                           m_DecayDescriptor;
  string                           m_DecayDescriptor_2;
  
  bool                             m_DoPlot;

  


  //containers for the index of MC particles in the TES
  vector<TrackAncestor>                      m_MC_Tracks;
  LHCb::MCParticles*               m_MCParts;
 
  //container for tracks
  vector<const LHCb::Track*>  m_tracks;

  //tool for getting the MCDecay
  IMCDecayFinder*                  m_MCDecayFinder;
  IPrintMCDecayTreeTool*           m_MCDecayPrinter;
  
};
#endif // MATHIEUMCFILTERVTXTOPOTRACKSTOOL_H
