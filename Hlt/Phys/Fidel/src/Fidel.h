// $Id: Fidel.h,v 1.8 2008-08-04 11:05:52 sfurcas Exp $
// $Id: Fidel.h,v 1.8 2008-08-04 11:05:52 sfurcas Exp $
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
  
  double     m_minPt;           ///< Minimum B candidate Pt
  double     m_minP;            ///< Minimum B candidate P
  double     m_minMass;         ///< Minimum B candidate range mass
  double     m_maxMass;         ///< Maximum B candidate range mass
  double     m_inputParticles;  ///< Max number particles in the loop
  double     m_maxPointing;     ///< Max pointing angle 
  double     m_maxChi2;         ///< Max Chi2 of B vertex
  bool       m_basicparticle;   ///< Enable particles couple in loop
  double     m_maxIpchi2;       ///< Maximum Chi2 Impact parameter
  bool       m_checkQ;          ///< Check quantum numbers-Charge 
  int        m_minQ;            ///< Minimum range charge
  int        m_maxQ;            ///< Maximum range charge
  bool       m_checkB;          ///< Check quantum numbers-Baryon number
  int        m_minB;            ///< Minimum range baryon number
  int        m_maxB;            ///< Maximum range baryon number
  bool       m_checkL;          ///< Check quantum number-Lepton number
  int        m_minL;            ///< Minimum range lepton number
  int        m_maxL;            ///< Maximum range lepton number
  bool       m_cc;              ///< Check quantum number for charge coniugate 
  int        m_pid;             ///< Default Pid to save B candidate
  double     m_minCts;          ///< Minimum Flight Significance
  bool       m_muonReq;         ///< Enable to simulate Hlt single muon
  double     m_minProb;         ///< Minimum Probability of Chi2 vertex
  double     m_minFsB1Res;      ///< Minimum Flight Significance between B and Res 1
  double     m_minFsB2Res;      ///< Minimum Flight Significance between B and Res 2
  double     m_minFsB1ch;       ///< Minimum Flight Significance between B and Charmed particle 1
  double     m_minFsB2ch;       ///< Minimum Flight Significance between B and Charmed particle 2
  double     m_minFsB1KS0;      ///< Minimum Flight Significance between B and KS0 particle 1
  double     m_minFsB2KS0;      ///< Minimum Flight Significance between B and KS0 particle 2
  double     m_minFs;           ///< Minimum Flight Significance between PV and Res
  double     m_minChFs;         ///< Minimum Flight Significance between PV and Charmed particle
  
  
  
};
#endif // FIDEL_H
