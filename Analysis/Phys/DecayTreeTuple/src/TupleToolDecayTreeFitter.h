
/**@class TupleToolDecayTreeFitter 

    * History : this tool was written during the study of the b->JpsiX    lifetimes. This means that the tool is not generic. 
                But it happened to be very useful for debugging and understanding the fitting code  aka : DecayTreeFitter. 
    * Advice  : Most of the output of this code could be saved via the LoKi style lines.    


    * Usage   : One can plug this Tool in DecayTreeTuple. 
    * Configuration : 
    
        - particleToFit = ["Lambda_b0"] # name of the head of the tree.
	- refit = True # save a fit with no contraints. 
	- constrainToPV = True  # save a fit to compute the lifetime. 
	- constrainToOneDaughter = True  # save a fit where one of the daughters in constrained. 
	- daughterToConstrain = ["J/psi(1S)"] # chose the daughter to constrain. 
	- constrainToAllDaughters = True # save a fit where more than one daughter is constrained. 
        - daughtersToConstrain = ["J/psi(1S)","Lambda0"] #chose the daughters to constrain.
	-
    * Outputs: 
       - for each fit is saved: 
          - The status, the chi2, the number of degres of freedom of the fit. 
	  - The fitted mass, mass error for example : Lambda_b0_massConAllDaughters_M ...
	  - The fitted lifetime, and error : Lambda_b0_taufitctau (ctau is given in mm).


**/



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
