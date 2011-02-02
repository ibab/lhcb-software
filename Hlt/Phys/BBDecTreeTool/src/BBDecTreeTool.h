// $Id: $  -*- C++ -*-
#ifndef BBDecTreeTool_H
#define BBDecTreeTool_H
// ============================================================================
// Include files 
#include <string>
#include <vector>
#include <fstream>
#include "GaudiKernel/ToolFactory.h"
#include "GaudiAlg/GaudiTool.h"
#include "Event/RecVertex.h"
#include "Event/Particle.h"
#include "Kernel/IParticleFilter.h"
#include "Kernel/DVAlgorithm.h"
#include "Kernel/GetDVAlgorithm.h"
#include "Kernel/IDistanceCalculator.h"
#include "LoKi/ParticleCuts.h"
#include "LoKi/AParticleCuts.h"
#include "LoKi/ParticleContextCuts.h"
// ============================================================================
/** @class BBDecTreeTool
 *  This tool performs BBDT cuts for the HLT2 topological lines.
 *  
 *  The tool can be used with LoKi/Bender functors :
 *
 *  @code 
 *     xxx.Code = " ... & FILTER('BBDecTreeTool') "
 *  @endcode
 *
 *  @see LoKi::Cuts::FILTER 
 *  @see IParticleFilter
 *  @author Mike Williams (w/ help from Vanya of course!)
 *  @date 2011-01-19
 *  
 *                    $Revision:$
 *  Last modification $Date:$
 *                 by $Author:$
 * 
 */ 
class BBDecTreeTool : public extends1<GaudiTool,IParticleFilter>{

  friend class ToolFactory<BBDecTreeTool>; ///< friend factory 

  public:

  /** initialize tool */
  virtual StatusCode initialize();
  
  /** finalize tool */
  virtual StatusCode finalize() {return GaudiTool::finalize();}
  
  /** performs the filtering based on the BBDecTreeTool response
   *  @see IParticleFilter 
   */
  virtual bool operator()(const LHCb::Particle* p) const ;
  
protected:
  
  /** standard constructor 
   *  @param type the actual tool type (?)
   *  @param name the tool instance name 
   *  @param parent the tool parent 
   */
  BBDecTreeTool(const std::string& type, const std::string& name, 
		const IInterface* parent);  
  
  /// virtual & protected destructor 
  virtual ~BBDecTreeTool(){};
  
private:

  /// default constructor is disabled 
  BBDecTreeTool();
  
  /// copy constructor is disabled 
  BBDecTreeTool(const BBDecTreeTool&);
  
  /// assignemet operator is disabled 
  BBDecTreeTool& operator=(const BBDecTreeTool&);
  
  /// utility method for file read errors
  StatusCode readError(const std::string &msg) const;

  /// utility method to obtain index to m_values
  int getIndex(const std::vector<double> &vars) const;
  
  /// utility method to obtain split index for single variable
  int getVarIndex(int varIndex, double value) const; 
  
  /// is event in a "keep" region (factored out of the BBDT by hand)?
  bool keepRegion(const std::vector<double> &vars) const;
  
  // properties
  int m_nbody; ///< 2,3 or 4-body?
  double m_threshold; ///< response threshold (cut) value
  std::string m_paramFile; ///< parameter file (full path)
  
  // attributes
  int m_ntrees; ///< number of trees used in training
  std::vector<std::vector<double> > m_splits; ///< variable split points
  std::vector<unsigned short int> m_values; ///< response values 
  const LoKi::PhysTypes::Fun m_SUMPT; ///< SUMTREE(PT,'K+'==ABSID,0.0)
  const LoKi::PhysTypes::Fun m_MINPT; ///< MINTREE('K+'==ABSID,PT)
  const LoKi::Cuts::BPVIPCHI2 m_BPVIPCHI2; ///< BPVIPCHI2()
  const IDistanceCalculator* m_LoKiDistCalc; ///< LoKi::DistanceCalculator
  const DVAlgorithm* m_dva; ///< DVAlgorithm (to get BPV)
}; 
// ============================================================================
#endif /* BBDecTreeTool_H */
