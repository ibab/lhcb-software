#ifndef TUPLETOOLBS2KMUNU_H 
#define TUPLETOOLBS2KMUNU_H 1

// from Gaudi
#include "DecayTreeTupleBase/TupleToolBase.h"
#include "Kernel/IParticleTupleTool.h" 

#include "Kernel/IParticleDescendants.h"
#include <Kernel/IDVAlgorithm.h>
#include <Kernel/GetIDVAlgorithm.h>

#include "ITaggingUtilsChecker.h"
#include "Kernel/IVertexFit.h"
#include "Kernel/IP2VVPartAngleCalculator.h" 



class IDVAlgorithm;
class IDistanceCalculator;
class IPVReFitter;
class IBTaggingTool;
class IVertexFit;
// from local

/** @class IsoBDT_small IsoBDT_small.h
 *
 *
 *  Tuple designed for isolation study
 *  @ Basem KHANJI
 *  @ date   01/04/2013
 */

class TupleToolBu2phimunu : public TupleToolBase, virtual public IParticleTupleTool {

public:
  
  /// Standard constructor
  TupleToolBu2phimunu( const std::string& type, 
                       const std::string& name,
                       const IInterface* parent);
  
  virtual ~TupleToolBu2phimunu( ){}; ///< Destructor
  
  virtual StatusCode initialize();
  
  virtual StatusCode fill(   const LHCb::Particle*
                             , const LHCb::Particle*
                             , const std::string&
                             , Tuples::Tuple& );
  
  double calculateThetaL( const LHCb::Particle* particle );
  double calculateThetaK( const LHCb::Particle* particle );
  double calculatePhi( const LHCb::Particle* particle );
  double calculateTransThetaTr( const LHCb::Particle* particle );
  double calculateTransPhiTr( const LHCb::Particle* particle );
  double calculateTransThetaV( const LHCb::Particle* particle );
  
private:
  
  std::string m_ParticlePath;
  std::string aux_name ;
  //const IDistanceCalculator* m_dist;
  IDVAlgorithm* m_dva;
  const IDistanceCalculator* m_dist;
  IParticleDescendants* m_descend;
  ITaggingUtilsChecker* m_util;
  std::string  m_pvReFitterName;
  IPVReFitter* m_pvReFitter;
  IVertexFit *m_vtxfitter;
  
  
  const LHCb::Particle* findID(unsigned int id, LHCb::Particle::ConstVector& v, std::string s="");
  
  double get_MINIPCHI2(const LHCb::Particle* p);
  double get_IPCHI2wrtDcyVtx(const LHCb::Particle* B, const LHCb::Particle* P);
  double get_MIPDV(const LHCb::Particle* p);
  double get_D_wrt_DcyVtx(const LHCb::Particle* B, const LHCb::Particle* P);
  
  std::vector<std::string>  m_inNames_muon ;
  std::vector<std::string>  m_inNames_kaon ;
  std::vector<std::string>  m_inNames_kaon2 ;
  // New BDT tunning for B+
  std::vector<std::string>  m_inNames_muon_New ;
  std::vector<std::string>  m_inNames_kaon_New ;
  std::vector<std::string>  m_inNames_kaon2_New ;
  //
  std::string m_inputLocation;
  // New BDT tunning for B+ 
  std::vector<double>* m_input_muon;
  std::vector<double>* m_input_kaon;
  std::vector<double>* m_input_kaon2;
  std::vector<double>* m_input_muon_New;
  std::vector<double>* m_input_kaon_New;
  std::vector<double>* m_input_kaon2_New;
  //
  IClassifierReader* m_read_BDT_muontrack;
  IClassifierReader* m_read_BDT_kaontrack;
  IClassifierReader* m_read_BDT_kaon2track;
  // New BDT tunning for B+
  IClassifierReader* m_read_BDT_muontrack_New;
  IClassifierReader* m_read_BDT_kaontrack_New;
  IClassifierReader* m_read_BDT_kaon2track_New;
 
  //
  void BDTvariableNames_bb_bkg_training(std::vector<std::string>& inNames ) const;
  void BDTvariableNames_Bplus_training(std::vector<std::string>& inNames ) const;
  
  void InCone(Gaudi::XYZPoint o1,
              Gaudi::XYZVector p1,
              Gaudi::XYZPoint o2,
              Gaudi::XYZVector p2,
              Gaudi::XYZPoint& vtx, double&
              doca, double& angle) const;

  double pointer (Gaudi::XYZVector vtx,
                  Gaudi::XYZVector P_tr,
                  Gaudi::XYZVector P_mu) const;

  double arcosine(Gaudi::XYZVector p1,
                  Gaudi::XYZVector p2) const;

  void closest_point(Gaudi::XYZPoint o,
                     Gaudi::XYZVector p,
                     Gaudi::XYZPoint o_mu,
                     Gaudi::XYZVector p_mu, 
                     Gaudi::XYZPoint& close1,
                     Gaudi::XYZPoint& close2,
                     Gaudi::XYZPoint& vertex, 
                     bool& fail) const ;
    
};
//===========================================================================//
#endif // USER_ISOBDT_SMALL_H
