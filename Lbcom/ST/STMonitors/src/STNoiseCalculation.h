#ifndef STNOISECALCULATION_H 
#define STNOISECALCULATION_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

namespace ST {
  class ISTNoiseCalculationTool;
}
/** @class STNoiseCalculation STNoiseCalculation.h
 *  
 *
 *  @author Mark Tobin
 *  @date   2009-10-01
 */
namespace ST {
  class STNoiseCalculation : public GaudiAlgorithm {
  public: 
    /// Standard constructor
    STNoiseCalculation( const std::string& name, ISvcLocator* pSvcLocator );
    
    virtual ~STNoiseCalculation( ); ///< Destructor
    
    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution
    
  protected:
    
  private:
    
    ISTNoiseCalculationTool* m_noiseTool; ///< Tool to calculate noise
    std::string m_noiseToolType; ///< Tool type (default is STNoiseCalculationTool)
    std::string m_noiseToolName; ///< Tool name (default is TTNoiseCalculationTool)

  };
}
#endif // STNOISECALCULATION_H
