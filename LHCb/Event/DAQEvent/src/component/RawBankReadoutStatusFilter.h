// $Id: RawBankReadoutStatusFilter.h,v 1.1 2007-12-05 13:59:18 odescham Exp $
#ifndef COMPONENT_RAWBANKREADOUTSTATUSFILTER_H 
#define COMPONENT_RAWBANKREADOUTSTATUSFILTER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "Event/RawBank.h"
#include "Event/RawBankReadoutStatus.h"

/** @class RawBankReadoutStatusFilter RawBankReadoutStatusFilter.h component/RawBankReadoutStatusFilter.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2007-12-05
 */
class RawBankReadoutStatusFilter : public GaudiAlgorithm {
public: 
  /// Standard constructor
  RawBankReadoutStatusFilter( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RawBankReadoutStatusFilter( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  int m_type;
  long m_mask;
};
#endif // COMPONENT_RAWBANKREADOUTSTATUSFILTER_H
