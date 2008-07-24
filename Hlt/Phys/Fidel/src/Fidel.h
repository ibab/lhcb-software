// $Id: Fidel.h,v 1.7 2008-07-24 10:11:34 sfurcas Exp $
// $Id: Fidel.h,v 1.7 2008-07-24 10:11:34 sfurcas Exp $
#ifndef FIDEL_H 
#define FIDEL_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DVAlgorithm.h"

//class IGeomDispCalculator ;

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
  double     m_maxIpchi2;
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
  double     m_minFsB1Res;
  double     m_minFsB2Res;  
  double     m_minFsB1ch;
  double     m_minFsB2ch;
  double     m_minFsB1KS0;
  double     m_minFsB2KS0;
  double     m_minFs;  
  double     m_minChFs;
  

  //IGeomDispCalculator* m_geomDispCalculator ; /// @todo replace
  
  
  
};
#endif // FIDEL_H
