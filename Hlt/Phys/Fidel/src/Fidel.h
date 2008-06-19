// $Id: Fidel.h,v 1.4 2008-06-19 17:37:44 sfurcas Exp $
// $Id: Fidel.h,v 1.4 2008-06-19 17:37:44 sfurcas Exp $
#ifndef FIDEL_H 
#define FIDEL_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DVAlgorithm.h"

//#include "Kernel/Particle2MCLinker.h"

class IRelatedPV;



/** @class Fidel Fidel.h
 *  
 *
 *  @author Sara Furcas
 *  @date   2007-11-12
 */
class Fidel : public DVAlgorithm {
public: 
  /// Standard constructor
  Fidel( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~Fidel( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
    
private:
  
  double     m_minPt;
  double     m_minP;
  double     m_minMass;
  double     m_maxMass;
  double     m_inputParticles;
  double     m_maxPointing;
  double     m_maxChi2;
  bool       m_basicparticle;
  double     m_maxIps;
  bool       m_checkQ;
  int        m_minQ; 
  int        m_maxQ;
  bool       m_checkB;
  int        m_minB; 
  int        m_maxB;
  bool       m_checkL;
  int        m_minL;
  int        m_maxL;
  bool       m_cc;  
  int        m_pid;
  double     m_minCts;  
  bool       m_muonReq;
  double     m_minProb;
  double     m_minFsB1;
  double     m_minFsB2;
  
  
  IRelatedPV*        m_relatedPV;
  
  
  
};
#endif // FIDEL_H
