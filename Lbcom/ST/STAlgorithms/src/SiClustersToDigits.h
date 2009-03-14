// $Id: SiClustersToDigits.h,v 1.1 2009-03-14 09:18:49 mneedham Exp $
#ifndef SICLUSTERSTODIGITS_H
#define SICLUSTERSTODIGITS_H 1


/** @class SiClustersToDigits
 *
 *  simple class for stripping digits from clusters
 *
 *  @author M.Needham
 *  @date   27/02/2009
 */

#include "SiClusterTraits.h"

template <class TYPE>
class SiClustersToDigits :public SiClusterTraits<TYPE>::BASEALG {

public:
  
  /// Constructor
  SiClustersToDigits( const std::string& name, ISvcLocator* pSvcLocator); 

  /// destructor
  virtual ~SiClustersToDigits();  

  /// IAlgorithm members
  virtual StatusCode execute();

private:

  typename SiClusterTraits<TYPE>::DIGIT* createDigit(const unsigned int value);

  void addNeighbours(TYPE* cluster, 
                     typename SiClusterTraits<TYPE>::DIGIT::Container* digitCont); 

  bool m_addNeighbourInfo;
  std::string m_inputLocation;
  std::string m_outputLocation;


};

#endif // SiClustersToDigits
