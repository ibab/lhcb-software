// $Id: RichG4Counters.h,v 1.1 2003-06-19 08:17:52 seaso Exp $
#ifndef RICHANALYSIS_RICHG4COUNTERS_H 
#define RICHANALYSIS_RICHG4COUNTERS_H 1

// Include files

/** @class RichG4Counters RichG4Counters.h RichAnalysis/RichG4Counters.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2003-06-07
 */
class RichG4Counters {
public:

  virtual ~RichG4Counters( ); ///< Destructor
  static  RichG4Counters* getInstance();
  void InitRichEventCounters();

  
protected:

private:
  /// Standard constructor
  RichG4Counters( ); 
  static  RichG4Counters*  RichG4CountersInstance;
  
  int m_NumPhotProdRich1Gas;
  int m_NumPhotGasOnRich1Mirror1;
  int m_NumPhotGasOnRich1Mirror2;
  int m_NumPhotGasOnRich1GasQW;
  int m_NumPhotGasOnRich1HpdQW;
  int m_NumPhotGasRich1BeforeQE;
  int m_NumPhotGasRich1AfterQE;
  int m_NumPhotGasRich1SiDet;

  int m_NumPhotProdRich1Agel;
  int m_NumPhotAgelAtAgelDownstrZ;
  int m_NumPhotAgelOnRich1Mirror1;
  int m_NumPhotAgelOnRich1Mirror2;
  int m_NumPhotAgelOnRich1GasQW;
  int m_NumPhotAgelOnRich1HpdQW;
  int m_NumPhotAgelRich1BeforeQE;
  int m_NumPhotAgelRich1AfterQE;
  int m_NumPhotAgelRich1SiDet;

  
  int m_NumHitTotRich1Gas;
  int m_NumHitTotRich1Agel;
  int m_NumHitSaturatedRich1Gas;
  int m_NumHitSaturatedRich1Agel;
  
  int m_NumHitPartGunPrimaryPartRich1Gas;
  int m_NumHitPartGunPrimaryPartRich1Agel;
  

  int m_NumHitTotRich1All;
  
public:
#include "RichG4Counters.icpp"

};
#endif // RICHANALYSIS_RICHG4COUNTERS_H
