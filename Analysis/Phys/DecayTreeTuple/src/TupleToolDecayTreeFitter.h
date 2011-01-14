// $Id: $
#ifndef TUPLETOOLDECAYTREEFITTER_H 
#define TUPLETOOLDECAYTREEFITTER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "TupleToolBase.h"
#include "Kernel/IParticleTupleTool.h"
#include "Kernel/IOnOffline.h"
#include "DecayTreeFitter/Fitter.h"

// STL
#include <vector>
#include <string>

// pid
namespace LHCb{
  class ParticleID;
}


#include "Kernel/IEventTupleTool.h"            // Interface


class DVAlgorithm;
/** @class TupleToolDecayTreeFitter TupleToolDecayTreeFitter.h
 *  
 * Yasmine Amhis and Matt Needham 
 * 30-10-10
 */
class TupleToolDecayTreeFitter : public TupleToolBase, virtual public IParticleTupleTool {
public: 
  /// Standard constructor
  TupleToolDecayTreeFitter( const std::string& type, 
                            const std::string& name,
                            const IInterface* parent);

  virtual ~TupleToolDecayTreeFitter( ); ///< Destructor

 
  
  StatusCode initialize();


  virtual StatusCode fill( const LHCb::Particle*
			   , const LHCb::Particle*
                           , const std::string&
                           , Tuples::Tuple& );

  
  
 private:
  StatusCode filldaughters( const DecayTreeFitter::Fitter& fitter,
			    const LHCb::Particle* P,
			    const std::string& prefix,
			    const Tuples::Tuple& tuple )const;
  StatusCode fill(const DecayTreeFitter::Fitter& fitter, 
		  const LHCb::Particle* P,
		  const std::string& prefix, 
		  const Tuples::Tuple& tuple) const;
  
  
  //  const LHCb::Particle* findMother( const LHCb::Particle* top , const LHCb::Particle* P ) const ;
  
  std::string getName(int id) const;
  
  const LHCb::Vertex* originVertex( const  LHCb::Particle*
				   ,const LHCb::Particle* ) const;
  
  std::vector<unsigned int> m_constrainedPids;
  std::string m_pvLocation ; ///<  PV location to be used. If empty, take context-dependent default

  DVAlgorithm* m_dva;
 
  std::vector<std::string> m_daughters;
  std::vector<std::string> m_daughter;
  std::vector<LHCb::ParticleID> m_daughterPids;
  std::vector<LHCb::ParticleID> m_daughtersPids;

  std::vector<std::string> m_mother ; 
  std::vector<LHCb::ParticleID> m_motherPid; 

  bool m_constrainAllDaughters; 
  bool m_constrainOneDaughter;
  bool m_constrainToPV;
  bool m_refit;
  bool m_verbose; 
  std::string m_toolName;


 
 
  unsigned int m_pidJpsi; 
  unsigned int m_pidLambda; 
  unsigned int m_pidKS0; 
  unsigned int m_pidPhi;
  unsigned int m_pidKst0; 
 };
#endif // TUPLETOOLDECAYTREEFITTER_H
