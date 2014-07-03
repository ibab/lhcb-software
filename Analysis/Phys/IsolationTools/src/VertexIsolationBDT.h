#ifndef VERTEXISOLATIONBDT_H
#define VERTEXISOLATIONBDT_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
//#include "Kernel/DVAlgorithm.h"
#include "Kernel/IExtraInfoTool.h"
#include "GaudiAlg/GaudiTool.h"
#include "CaloUtils/CaloParticle.h"

#include "Kernel/IDVAlgorithm.h"
#include <Kernel/GetIDVAlgorithm.h>
#include "Kernel/IVertexFit.h"
#include "Kernel/IPVReFitter.h"
#include "TMVA/Reader.h"
class IDVAlgorithm;
class IDistanceCalculator;
class IVertexFit;
class IVertexReFitter;

namespace LHCb {
  class Particle;
  class Vertex;
}
//class IVertexFit;
//class LHCb::CaloParticle ;

/** @class VertexIsolationBDT VertexIsolationBDT.h
 *
 * \brief Calculate vertex isolation using a BDT.
 *    Take vertexed particle and add other tracks of the
 *    event, one by one, building a new vertex.
 *
 * Variables:
 *
 * Options:
 *    - InputParticles: containers for extra particles used for building extra vertices
 *
 *  @author Alexander Shires (from Greg Ciezerak.) (ashires@cern.ch)
 *  @date   27/06/2015
 *
 */

class VertexIsolationBDT : public GaudiTool, virtual public IExtraInfoTool
{

public:

  /// Standard constructor
  VertexIsolationBDT( const std::string& type,
                   const std::string& name,
                   const IInterface* parent );

  virtual StatusCode initialize () ;

  virtual StatusCode calculateExtraInfo( const LHCb::Particle*,
                                         const LHCb::Particle*);

  virtual int getFirstIndex(void);

  virtual int getNumberOfParameters(void);

  virtual int getInfo(int index, double & value, std::string & name);

  virtual ~VertexIsolationBDT( ); ///< Destructor

private:

  /// Check if a pure CALO Particle
  inline bool isPureNeutralCalo(const LHCb::Particle* P)const
  {
    LHCb::CaloParticle caloP(  (LHCb::Particle*) P );
    return caloP.isPureNeutralCalo();
  }  // Helpers

private:

  double m_bdt1 ;
  double m_bdt2 ;
  double m_bdt3 ;
  LHCb::Particle * m_part1 ;
  LHCb::Particle * m_part2 ;
  LHCb::Particle * m_part3 ;

  /// Find all the signal particles to vertex
  void findDaughters2Vertex( const LHCb::Particle* );
  LHCb::Particle::ConstVector m_particlesToVertex ;

  // Config and vertexing
  std::vector<std::string>   m_inputParticles ;
  IDVAlgorithm*              m_dva;
  const IDistanceCalculator* m_dist;
  const IVertexFit*          m_pVertexFit;
  const IPVReFitter*         m_pPVReFitter;
  std::string m_weightsName; 
  TMVA::Reader *m_Reader;

  //variables
  int m_var_type ;
  float m_var_minipchi2 ;
  float m_var_opening ;
  float m_var_trackipchi2 ;
  float m_var_newfdchi2 ;
  float m_var_deltafd ;
  float m_var_trackpt ;

 ;

public:

  // Structure of isolation vars
/*  struct IsolationResult
  {
    // Constructor with defaults
    IsolationResult() :  bdtval(0), pe(0), px(0), py(0), pz(0), 
                         pidk(0), probnnk(0), probnnmu(0), probnnpi(0),
                         probghost(0), bestParticle(NULL) {}
    // Members
    double bdtval, pe, px, py, pz ;
    double pidk, probnnk, pronnk, probnnmu, probnnp, probghost;
    LHCb::Particle *bestParticle ;
  };*/

  // Isolation calculation
  bool getIsolation( const LHCb::Particle *part 
          , LHCb::Particle::ConstVector&
          , const LHCb::Vertex* 
          , const LHCb::RecVertex::Range
          ) ;

  //variable calculations
  double getminipchi(const LHCb::Particle*
          , const LHCb::RecVertex::Range
          ) const ;
  double getfdchi2(const LHCb::Track*
          , LHCb::Vertex
          , const LHCb::RecVertex::Range 
          ) const ;
  double getopening(const LHCb::Track*,const  LHCb::Particle*) const ;  

};

#endif // VERTEXISOLATIONBDT_H
