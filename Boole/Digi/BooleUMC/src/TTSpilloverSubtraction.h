// $Id: TTSpilloverSubtraction.h,v 1.1 2010-06-20 07:20:05 tskwarni Exp $
#ifndef TTSPILLOVERSUBTRACTION_H
#define TTSPILLOVERSUBTRACTION_H 1

#include "Kernel/STAlgBase.h"
#include <string>

/** @class TTSpilloverSubtraction TTSpilloverSubtraction.h
 *
 *  Class for subtracting ST spillover in upgrade simulations 
 *
 *  @author Adam Webber
 *  @date   06/07/2010
 */


class TTSpilloverSubtraction :public ST::AlgBase {

public:
  
  // Constructors and destructor
  TTSpilloverSubtraction(const std::string& name, ISvcLocator* pSvcLocator); 
  virtual ~TTSpilloverSubtraction();  

  // IAlgorithm members
  virtual StatusCode initialize();
  virtual StatusCode execute();

protected:

  bool IsWithin(const int* a, const int a_size, const int test);
   

private:

  std::string m_inputLocation;
  std::vector<std::string> m_spillNames;
  std::vector<std::string> m_spillPaths;

  double m_q_prev_cut;
  std::vector<double> m_q_spillover_frac;
  double m_q_saturation;

};

#endif // TTSPILLOVERSUBTRACTION_H
