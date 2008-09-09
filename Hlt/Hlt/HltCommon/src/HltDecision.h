// $Id: HltDecision.h,v 1.1 2008-09-09 11:00:44 graven Exp $
#ifndef HLTDECISION_H 
#define HLTDECISION_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "Kernel/IANNSvc.h"


/** @class HltDecision HltDecision.h
 *  
 *  functionality:
 *      Create a DecReport based on whether a given set of 
 *      algorithms passed.
 *
 *      the set of algorithms is divided into three groups:
 *      AND: all algorithms must have run and passed
 *      OR:  at least one must have run and passed
 *      NOT: none should have both 'run and pass'
 *
 *  @author Gerhard Raven
 *  @date   2008-09-09
 */
class HltDecision : public GaudiAlgorithm {
public: 
  HltDecision( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltDecision( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:
  bool accept() const;
  void vetoAlgsHandler( Property& );
  void requireAlgsHandler( Property& );
  void acceptAlgsHandler( Property& );
  StatusCode decodeAlgorithms( std::vector<std::string>& theNames,
                               std::vector<Algorithm*>& theAlgs );

  std::vector<Algorithm*> m_acceptAlgs;
  std::vector<Algorithm*> m_requireAlgs;
  std::vector<Algorithm*> m_vetoAlgs;

  std::vector<std::string> m_acceptNames;
  std::vector<std::string> m_requireNames;
  std::vector<std::string> m_vetoNames;
  std::string              m_decisionName;
  std::string              m_location;

  // assigned names and numbers service...
  mutable IANNSvc *m_annSvc;
  IANNSvc&     annSvc() const {
        if (m_annSvc == 0)  m_annSvc = svc<IANNSvc>("HltANNSvc");
        return *m_annSvc;
  }

};
#endif // HltDecision_H
