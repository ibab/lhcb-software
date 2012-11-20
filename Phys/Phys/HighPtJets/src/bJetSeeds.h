#ifndef USER_bJetSeeds_H 
#define USER_bJetSeeds_H 1
// from Kernel
#include <Kernel/GetIDVAlgorithm.h>
#include <Kernel/IDistanceCalculator.h>
// from Event
#include "Event/Track.h"
#include "Event/RecVertex.h"
//from Kernel
#include "Kernel/DaVinciAlgorithm.h"

/** @class bJetSeeds bJetSeeds.h 
 *  
 *  @author Marcin Kucharczyk
 *  @date   2011-11-26
 */

using namespace LHCb;

typedef Gaudi::XYZVector EVector;
typedef Gaudi::XYZPoint EPoint;

class bJetSeeds : public DaVinciAlgorithm {

public: 
  /// Standard constructor
  bJetSeeds( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~bJetSeeds( ); ///< Destructor
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:

  // Functions
  std::vector<Vertex> build3PartSV(const Particle::ConstVector&);
  StatusCode partIPwrtPV(const LHCb::Particle* prt,
                         const LHCb::VertexBase* vBase,
                         double& ip, 
                         double& ipErr);
  int filterSV3Chi2(std::vector<Vertex> sds,
                    std::vector<Vertex>* purgedSeeds);

private:

  // Variables
  std::string m_inputPVsName;
  LHCb::RecVertices* m_inputPVs;
  IVertexFit* fitterSV;
  std::string m_VrtxFitterName;
  unsigned int m_nrPVs;
  unsigned int m_nrSeeds;
  double m_trkChi2;
  double m_ptrMom;
  double m_prtPt;
  double m_prtIPS;
  double m_dZSVPV;
  double m_sumMomSV;
  double m_vtxChi2;
  const IDistanceCalculator* m_DOCA;
  
};
//========================================================================//
#endif // USER_bJetSeeds_H
