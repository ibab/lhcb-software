// $Id: TsaSeed.h,v 1.7 2006-12-06 14:35:01 mneedham Exp $
#ifndef _TSASEED_H_
#define _TSASEED_H_

/** @class TsaSeed
 *
 *
 *  @author M.Needham
 *  @date   07/03/2002
 */

#include <string>
#include <vector>


#include "TsaBaseAlg.h"

class ITsaSeedStep;
class ITsaSeedAddHits;
class ITsaStubFind;
class ITsaStubLinker;
class ITsaStubExtender;

class TsaSeed: public TsaBaseAlg {

public:
  
  // Constructors and destructor
  TsaSeed(const std::string& name, 
                   ISvcLocator* pSvcLocator); 
  virtual ~TsaSeed();  

  // IAlgorithm members
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

private:

  std::string m_seedTrackLocation;
  std::string m_seedHitLocation;
  std::string m_seedStubLocation;

  std::string m_selectorType;
  double m_maxNumHits;
  bool m_calcLikelihood; 
  bool m_addHitsInITOverlap;

  std::vector<ITsaSeedStep*> m_xSearchStep;
  std::vector<ITsaSeedStep*> m_stereoStep;
  ITsaSeedStep* m_xSelection;
  ITsaSeedStep* m_finalSelection;
  ITsaSeedStep* m_likelihood;
  ITsaSeedAddHits* m_addHits; 
  ITsaStubFind* m_stubFind;
  ITsaStubLinker* m_stubLinker;
  ITsaStubExtender* m_extendStubs;
};

#endif // _TSASEED
