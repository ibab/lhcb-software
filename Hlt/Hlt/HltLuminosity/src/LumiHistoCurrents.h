// $Id: LumiHistoCurrents.h,v 1.1 2009/05/05 
#ifndef LUMIHISTOCURRENTS_H 
#define LUMIHISTOCURRENTS_H 1

// Include files

//#include "HltBase/HltAlgorithm.h"
#include "HltBase/HltBaseAlg.h"
//#include "AIDA/IHistogram2D.h"
#include "TH2.h"

/** @class LumiHistoCurrents 
 *  
 *
 *  functionality: Fill Histos with currents from both beams for each bunch crossing type
 *
 *
 *  options:
 *  
 *
 *  @author Antonio Perez-Calero
 *  @date   2009-05-05
 */
class LumiHistoCurrents : public HltBaseAlg {
public:

  // Standard constructor
  LumiHistoCurrents( const std::string& name, ISvcLocator* pSvcLocator );

  // Standard destructor
  virtual ~LumiHistoCurrents( ); 


  // initialize algorithm
  virtual StatusCode initialize();


  // driver of the execute()
  virtual StatusCode execute();

  virtual StatusCode finalize();
  
private:
  
  
  std::vector<TH2F> m_HistoCurrents;


};

#endif // LUMIHISTOCURRENTS_H
