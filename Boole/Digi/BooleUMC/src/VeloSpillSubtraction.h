#ifndef VELOSPILLSUBTRACTION_H
#define VELOSPILLSUBTRACTION_H 1

#include "GaudiAlg/GaudiAlgorithm.h"
#include <string>

/** @class VeloSpillSubtraction VeloSpillSubtraction.h
 *
 *  Class for subtracting Velo spillover in upgrade simulations 
 *
 *  @author Tomasz Skwarnicki
 *  @date   02/09/2009
 */

class VeloSpillSubtraction :public GaudiAlgorithm {

public:
  
  // Constructors and destructor
  VeloSpillSubtraction(const std::string& name, ISvcLocator* pSvcLocator); 
  virtual ~VeloSpillSubtraction();  

  // IAlgorithm members
  virtual StatusCode initialize();
  virtual StatusCode execute();

private:

  std::string m_inputLocation;
  std::vector<std::string> m_spillNames;
  std::vector<std::string> m_spillPaths;

  double m_q_prev_cut;
  double m_q_cut;
  double m_q_spillover_frac;
  double m_q_saturation;

};

#endif // VELOSPILLSUBTRACTION_H
