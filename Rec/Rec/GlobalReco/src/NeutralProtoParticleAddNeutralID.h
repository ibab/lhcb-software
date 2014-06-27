#ifndef NEUTRALPROTOPARTICLEADDNEUTRALID_H 
#define NEUTRALPROTOPARTICLEADDNEUTRALID_H 1

// Include files 
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "CaloInterfaces/IGammaPi0SeparationTool.h"
#include "CaloInterfaces/INeutralIDTool.h"
#include "Event/ProtoParticle.h"

/** @class NeutralProtoParticleAddNeutralID NeutralProtoParticleAddNeutralID.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2014-05-27
 */

class NeutralProtoParticleAddNeutralID : public GaudiAlgorithm {
public: 
  /// Standard constructor
  NeutralProtoParticleAddNeutralID( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~NeutralProtoParticleAddNeutralID( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  bool addInput(int index,double* data,LHCb::ProtoParticle* proto,LHCb::ProtoParticle::additionalInfo flag,std::string func="none");
private:
  std::string m_input;
  bool m_isNotE;
  bool m_isNotH;
  bool m_isPhoton;
  double m_isNotE_Pt;
  double m_isNotH_Pt;
  double m_isPhoton_Pt;
  IGammaPi0SeparationTool* m_gammaPi0;
  INeutralIDTool* m_neutralID;
};
#endif // NEUTRALPROTOPARTICLEADDNEUTRALID_H
