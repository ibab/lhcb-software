// $Id: TsaSpacePointCreator.h,v 1.1.1.1 2006-07-24 14:56:45 mneedham Exp $
#ifndef _TSASPACEPOINTCREATOR_H_
#define _TSASPACEPOINTCREATOR_H_

/** @class TsaSpacePointCreator TsaSpacePointCreator.h Tsa/TsaSpacePointCreator
 *
 *  Class for spacepoints creation
 *
 *  @author M.Needham
 *  @date   07/03/2002
 */

#include "TsaBaseAlg.h"
#include <string>

#include "TsaKernel/SpacePoint.h"
class IITBoxProcessor;

class TsaSpacePointCreator: public TsaBaseAlg {

public:
  
  // Constructors and destructor
  TsaSpacePointCreator(const std::string& name, 
                   ISvcLocator* pSvcLocator); 
  virtual ~TsaSpacePointCreator();  

  // IAlgorithm members
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

private:

  typedef std::vector<IITBoxProcessor*> itBoxCont;
  std::string m_itProcessorName;
  itBoxCont m_boxs; 

  Tsa::SpacePointVector* m_tmpCont;
  int m_nProcessor;

};

#endif // _TSASPACEPOINTCREATOR











