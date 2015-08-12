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
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IParticleFilter.h"
#include "Kernel/IParticleDictTool.h"
// ============================================================================
/** @class BBDTSimpleTool
 *  This tool performs "easy" BBDT cuts for the HLT2 topological lines.
 *  It accepts particles that pass any of a set of collections of cuts.
 *
 *  Its properties:
 *    - <c>"Cuts":</c>  the sets of cuts.
 *    - <c>"ParticleDictTool":</c>  name of the class to use for variable
 *      functor handling.  It must inherit from <c>IParticleDictTool</c>.
 *      The default value is <c>"LoKi::Hybrid::DictOfFunctors"</c>.
 *
 *  A set of cuts is a map associating to variable nicknames a pair of real
 *  values that define the <c>(min, max)</c> of the accepted range of the
 *  variable.  If <c>max &lt; min</c>, then only the lower limit,
 *  <c>var &gt; min</c>, is applied.  There is no equivalent functionality that
 *  would provide only an upper limit.
 *
 *  The <c>Cuts</c> property of the tool contains a vector of such sets of
 *  cuts.  If an input particle passes all of the cuts of at least one of the
 *  sets of cuts, then the particle is accepted by the filter.
 * 
 *  The tool uses a private <c>LoKi::Hybrid::DictOfFunctors</c> for evaluating
 *  the values of the input variables.  It must be configured separately with
 *  a mapping of the variable nicknames as they appear in <c>Cuts</c> to
 *  the strings defining the corresponding LoKi functors for evaluating the
 *  variables.  The tool can be used with LoKi/Bender functors.  An example
 *  configuration in the context of a Hlt2 line follows:
 *  @code 
 *      from Configurables import BBDTSimpleTool
 *      from Configurables import LoKi__Hybrid__DictOfFunctors
 *      from Configurables import FilterDesktop
 *      from HltLine.HltLine import Hlt2Member
 *      from HltLine.HltLine import Hlt1Tool
 *
 *      varHandler = Hlt1Tool( type = LoKi__Hybrid__DictOfFunctors, name = "VarHandler",
 *                        Variables = {   "M"          :  "MM/MeV"
 *                              , "DOCA"       :  "DOCAMAX_('',False)/mm"
 *                              , "FDCHI2"     :  "BPVVDCHI2"
 *                              , "PTSUM"      :  "SUMTREE(PT,ISBASIC,0.0)/MeV"
 *                              }
 *
 *
 *      simpletool = Hlt1Tool(type=BBDTSimpleTool,name='TrgSimple',
 *                     Cuts=[ {  'M' : (2500,7000)      ## First set of cuts
 *                              ,'DOCA' : (-1,0.2)
 *                              ,'FDCHI2' : (1000,-1)
 *                              ,'PTSUM':(7000,-1)}
 *                            , {  'M' : (3000,7000)    ## Second set of cuts
 *                                ,'DOCA' : (-1,0.2)
 *                                ,'FDCHI2' : (1000,-1)
 *                                ,'PTSUM' : (8000,-1)}
 *                          ]
 *                     ParticleDictTool='LoKi::Hybrid::DictOfFunctors/'+varHandler.Name,
 *                     tools=[varHandler])
 *
 *      cuts = "FILTER('BBDTSimpleTool/TrgSimple')"
 *      filter = Hlt2Member(FilterDesktop, 'FilterBDT', Inputs=...,
 *                          Code=cuts,tools=[simpletool])
 *  @endcode
 *
 *  @see LoKi::Cuts::FILTER 
 *  @see IParticleFilter
 *  @see IParticleDictTool
 *  @see LoKi::Hybrid::DictOfFunctors
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
  std::string m_dictool_name; ///< the typename of the DictOfFunctors tool

  // attributes
  IParticleDictTool *m_hybrid_dicttool;
}; 
// ============================================================================
#endif /* BBDTSimpleTool_H */

