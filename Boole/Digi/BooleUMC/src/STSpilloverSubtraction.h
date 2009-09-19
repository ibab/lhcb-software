#ifndef STSPILLOVERSUBTRACTION_H
#define STSPILLOVERSUBTRACTION_H 1

#include "Kernel/STAlgBase.h"
#include <string>

/** @class STSpilloverSubtraction STSpilloverSubtraction.h
 *
 *  Class for subtracting ST spillover in upgrade simulations 
 *
 *  @author M. Kucharczyk
 *  @date   06/08/2009
 */


class STSpilloverSubtraction :public ST::AlgBase {

public:
  
  // Constructors and destructor
  STSpilloverSubtraction(const std::string& name, ISvcLocator* pSvcLocator); 
  virtual ~STSpilloverSubtraction();  

  // IAlgorithm members
  virtual StatusCode initialize();
  virtual StatusCode execute();

private:

  std::string m_inputLocation;
  std::vector<std::string> m_spillNames;
  std::vector<std::string> m_spillPaths;

  double m_q_prev_cut;
  double m_q_spillover_frac;
  double m_q_saturation;

};

#endif // STSPILLOVERSUBTRACTION_H
