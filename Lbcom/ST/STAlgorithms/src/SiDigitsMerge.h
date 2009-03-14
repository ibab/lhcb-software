// $Id: SiDigitsMerge.h,v 1.1 2009-03-14 09:18:49 mneedham Exp $
#ifndef SIDIGITSMERGE_H
#define SIDIGITSMERGE_H 1


/** @class SiDigitsMerge
 *
 *  simple class for adding digits
 *
 *  @author M.Needham
 *  @date   27/02/2009
 */

#include "SiClusterTraits.h"

template <class TYPE>
class SiDigitsMerge :public SiClusterTraits<TYPE>::BASEALG {

public:
  
  /// Constructor
  SiDigitsMerge( const std::string& name, ISvcLocator* pSvcLocator); 

  /// destructor
  virtual ~SiDigitsMerge();  

  /// initialize
  virtual StatusCode initialize();

  /// IAlgorithm members
  virtual StatusCode execute();

private:

  void updateCharge(typename SiClusterTraits<TYPE>::DIGIT* digit1, 
                    typename SiClusterTraits<TYPE>::DIGIT* digit2);

 
  unsigned int m_nBits;
  double m_minValue;
  double m_maxValue;

  std::vector<std::string> m_spillsVector;
  std::vector<std::string> m_spillsPath;
  std::string m_inputLocation;
  std::string m_outputLocation;


};

#endif // SiDigitsMerge
