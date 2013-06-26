#ifndef CaloDigitChannel_H
#define CaloDigitChannel_H 1
//
//Include files

//from STL

#include <cmath>
#include <string>
#include <vector>
//From Gaudi

#include "GaudiAlg/GaudiTupleAlg.h"

#include "CaloDet/DeCalorimeter.h"



/** @class CaloDigitChannel CaloDigitChannel.h
 *  
 *  Evaluate the Energy in the Calo Channel by Channel
 * 
 *  @author Yasmine AMHIS
 *  @date   2007-03-28
 */


class CaloDigitChannel: public GaudiTupleAlg{ 
public:
  CaloDigitChannel  ( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~CaloDigitChannel( ); ///< Destructor
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:
  DeCalorimeter* m_calo;
  
  
  std::string              m_nameOfDetector      ; // Detector short name
  std::string m_inputData;         ///< Input container
 

};
#endif //CaloDigitChannel
