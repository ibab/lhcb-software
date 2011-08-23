// $Id: $  -*- C++ -*-
#ifndef BBDecTreeTool_H
#define BBDecTreeTool_H
// ============================================================================
// Include files 
#include <string>
#include <fstream>
#include "GaudiKernel/ToolFactory.h"
#include "GaudiAlg/GaudiTool.h"
#include "BBDTVarHandler.h"
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
  int getIndex() const;
  
  /// utility method to obtain split index for single variable
  int getVarIndex(int varIndex, double value) const; 
  
  // properties
  double m_threshold; ///< response threshold (cut) value
  std::string m_paramFile; ///< parameter file (full path)
  int m_key; ///< ANNSvc key to write to
  
  // attributes
  int m_ntrees; ///< number of trees used in training  
  std::vector<std::vector<double> > m_splits; ///< variable split points
  std::vector<unsigned short int> m_values; ///< response values 
  std::vector<std::string> m_pids; ///< PID names for PT variabls
  const IDistanceCalculator* m_dist; ///< LoKi::DistanceCalculator
  const DVAlgorithm* m_dva; ///< DVAlgorithm (to get BPV)
  mutable BBDTVarHandler m_vars; ///< variables
}; 
// ============================================================================
#endif /* BBDecTreeTool_H */
