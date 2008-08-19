// $Id: Fidel.h,v 1.9 2008-08-19 11:47:35 pkoppenb Exp $
// $Id: Fidel.h,v 1.9 2008-08-19 11:47:35 pkoppenb Exp $
#ifndef FIDEL_H 
#define FIDEL_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DVAlgorithm.h"

//class IGeomDispCalculator ;

/** @class Fidel Fidel.h
 *  
 *  Algorithm selecting Xb -> 2 body, where the tow bodies can be any
 *  Final state particle or resonance. Quantum number conservations 
 *  are checked.
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
  
  double     m_inputParticles;  ///< Max number particles in the loop
  bool       m_basicparticle;   ///< Enable particles couple in loop
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
  double     m_minFsB1Res;      ///< Minimum Flight Significance between B and Res 1
  double     m_minFsB2Res;      ///< Minimum Flight Significance between B and Res 2
  double     m_minFsB1ch;       ///< Minimum Flight Significance between B and Charmed particle 1
  double     m_minFsB2ch;       ///< Minimum Flight Significance between B and Charmed particle 2
  double     m_minFsB1KS0;      ///< Minimum Flight Significance between B and KS0 particle 1
  double     m_minFsB2KS0;      ///< Minimum Flight Significance between B and KS0 particle 2
  double     m_minFs;           ///< Minimum Flight Significance between PV and Res
  double     m_minChFs;         ///< Minimum Flight Significance between PV and Charmed particle
  
  std::string m_combinationCut ; ///< the cut for the combination of daughters
  std::string  m_motherCut     ; ///< the cut to be applied to the constructed mother particle
  LoKi::Types::ACut m_acut     ; ///< the actual cut for combination of good daughters
  LoKi::Types::Cut  m_cut      ; ///< the actual 'final' cut for the mother particle
  std::string  m_factory       ; ///< the type/name of hybrid C++/Python factory
  
};
#endif // FIDEL_H
