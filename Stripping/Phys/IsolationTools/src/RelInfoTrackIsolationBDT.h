#ifndef RELINFOTRACKISOVARIABLES_H 
#define RELINFOTRACKISOVARIABLES_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DaVinciAlgorithm.h"
#include "Kernel/GetIDVAlgorithm.h"
#include "Kernel/IRelatedInfoTool.h"
#include "Event/RelatedInfoMap.h"
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IDistanceCalculator.h"
//needed for TMVA
#include "MVADictTools/TMVATransform.h"
#include "Kernel/IParticleDictTool.h"
#include "Event/Particle.h"


/** @class RelInfoTrackIsolationBDT RelInfoTrackIsolationBDT.h
 *
 * \brief Calculate track isolation.
 *    Compare daughter
 *
 *    m_bdt1 : highest ranked bdt value
 *    m_bdt2 : second highest ranked bdt value
 *    m_bdt2 : third highest ranked bdt value
 *
 * Options:
 *
 *   None
 *
 *  Converted from ConeVariables by A. Shires 18/06/2014
 *
 *  @author Alex Shires
 *  @date   2014-06-19
 *
 */

class IDVAlgorithm;
class IDistanceCalculator;
class IParticleDictTool ;

class RelInfoTrackIsolationBDT : public GaudiTool, 
                                 virtual public IRelatedInfoTool 
{

public:

  /// Standard constructor
  RelInfoTrackIsolationBDT( const std::string& type,
                            const std::string& name,
                            const IInterface* parent );

  virtual StatusCode initialize();

  virtual StatusCode calculateRelatedInfo( const LHCb::Particle*,
                                           const LHCb::Particle*);

  virtual LHCb::RelatedInfoMap* getInfo(void);

  virtual ~RelInfoTrackIsolationBDT( ); ///< Destructor

private:

  int m_variables;
  LHCb::RelatedInfoMap m_map;
  std::vector<short int> m_keys;

  int m_trackType ;

  std::string m_ParticlePath ;

  double m_bdt1;
  double m_bdt2;
  double m_bdt3;

  std::string m_weightsName;
  /// TMVA transform
  //Reader
  //TMVA::Reader *m_Reader;
  std::string m_transformName ;
  //variables
  float var_log_doca ;
  float var_angle ;
  float var_fc ;
  float var_PVdist ;
  float var_SVdist ;
  float var_ipchisqany ;

  TMVATransform m_tmva ;
  TMVATransform::optmap m_optmap ;
  IParticleDictTool::DICT m_varmap ;
  IParticleDictTool::DICT m_out ;


  //save the vertice
  std::string m_PVInputLocation ;
  LHCb::RecVertex::Range m_vertices ;


  ///============================================================================
  /// Track isolation method
  ///============================================================================

  bool calcBDTValue( const LHCb::Particle * part
                     , const LHCb::Particle::Range particles
                     , const LHCb::VertexBase * PV
                     , const LHCb::VertexBase * SV
                     ) ;
  ///============================================================================
  /// Helper functions
  ///============================================================================
  double calcIPToAnyPV( const LHCb::Track * track ) ;
  double  enclosedAngle( Gaudi::XYZVector, Gaudi::XYZVector );
  void    getPerpFeet(  Gaudi::XYZPoint track_pos,
                        Gaudi::XYZVector track_p,
                        Gaudi::XYZPoint mu_pos,
                        Gaudi::XYZVector mu_p,
                        Gaudi::XYZPoint& perpFootTrack,
                        Gaudi::XYZPoint& perpFootMu,
                        Gaudi::XYZPoint& vertex, bool& fail);
  double calcFC(  Gaudi::XYZVector track_mom,
                  Gaudi::XYZVector mu_mom,
                  Gaudi::XYZPoint mu_track_vertex,
                  const LHCb::VertexBase* PV);
  double calcVertexDist(Gaudi::XYZPoint muTrack,
                        const LHCb::VertexBase* v);



};

#endif // CONEVARIABLES_H
