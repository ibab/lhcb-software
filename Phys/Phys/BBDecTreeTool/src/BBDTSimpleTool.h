// $Id: $  -*- C++ -*-
#ifndef BBDTSimpleTool_H
#define BBDTSimpleTool_H
// ============================================================================
// Include files 
#include <map>
#include <vector>
#include <string>
class StatusCode;
namespace Gaudi { 
  namespace Parsers {
    StatusCode parse(std::vector<std::map<std::string, 
		     std::pair<double,double> > >& result, 
		     const std::string& input);
  }
}
#include "GaudiKernel/ToolFactory.h"
#include "GaudiAlg/GaudiTool.h"
#include "BBDTVarHandler.h"
// ============================================================================
/** @class BBDTSimpleTool
 *  This tool performs "easy" BBDT cuts for the HLT2 topological lines.
 *  
 *  The tool can be used with LoKi/Bender functors :
 *
 *  @code #include <Kernel/GetIDVAlgorithm.h>
 *     xxx.Code = " ... & FILTER('BBDTSimpleTool') "
 *  @endcode
 *
 *  @see LoKi::Cuts::FILTER 
 *  @see IParticleFilter
 *  @author Mike Williams 
 *  @date 2011-02-15
 *  
 *                    $Revision:$
 *  Last modification $Date:$
 *                 by $Author:$
 * 
 */ 
class BBDTSimpleTool : public extends1<GaudiTool,IParticleFilter>{

  friend class ToolFactory<BBDTSimpleTool>; ///< friend factory 
  public:

  /** initialize tool */
  virtual StatusCode initialize();
  
  /** finalize tool */
  virtual StatusCode finalize();
  
  /** performs the filtering based on the BBDT "easy" cuts
   *  @see IParticleFilter 
   */
  virtual bool operator()(const LHCb::Particle* p) const ;
  
protected:
  
  /** standard constructor 
   *  @param type the actual tool type (?)
   *  @param name the tool instance name 
   *  @param parent the tool parent 
   */
  BBDTSimpleTool(const std::string& type, const std::string& name, 
		const IInterface* parent);  
  
  /// virtual & protected destructor 
  virtual ~BBDTSimpleTool(){};
  
private:

  /// default constructor is disabled 
  BBDTSimpleTool();
  
  /// copy constructor is disabled 
  BBDTSimpleTool(const BBDTSimpleTool&);
  
  /// assignemet operator is disabled 
  BBDTSimpleTool& operator=(const BBDTSimpleTool&);
  
  // properties
  /// Simple tree of cuts
  std::vector<std::map<std::string,std::pair<double,double> > > m_cuts; 
  
  // attributes
  mutable BBDTVarHandler *m_vars; ///< variables
}; 
// ============================================================================
#endif /* BBDTSimpleTool_H */

