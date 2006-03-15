// $Id: FilterGoodDecay.h,v 1.2 2006-03-15 13:37:21 pkoppenb Exp $
#ifndef FILTERGOODDECAY_H 
#define FILTERGOODDECAY_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class FilterGoodDecay FilterGoodDecay.h
 *  
 *  Accepts event if defined decay is found
 *
 *  @author Patrick KOPPENBURG
 *  @date   2004-11-16
 */
class IMCDecayFinder;
class FilterGoodDecay : public GaudiAlgorithm {
public: 
  /// Standard constructor
  FilterGoodDecay( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~FilterGoodDecay( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  IMCDecayFinder* m_pMCDecFinder ;        ///<  MCDecayFinder

};
#endif // FILTERGOODDECAY_H
