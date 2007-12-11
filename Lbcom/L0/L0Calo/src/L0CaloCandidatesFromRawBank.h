// $Id: L0CaloCandidatesFromRawBank.h,v 1.3 2007-12-11 18:26:50 robbep Exp $
#ifndef L0CALOCANDIDATESFROMRAWBANK_H 
#define L0CALOCANDIDATESFROMRAWBANK_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "CaloDet/DeCalorimeter.h"
#include "Event/RawEvent.h"
#include "Event/L0CaloCandidate.h"


static const InterfaceID IID_L0CaloCandidatesFromRawBank ( "L0CaloCandidatesFromRawBank", 2, 0 );

/** @class L0CaloCandidatesFromRawBank L0CaloCandidatesFromRawBank.h
 *  Tool to convert the Raw Bank L0CaloCandidate to TES containers
 *
 *  @author Olivier Callot
 *  @date   2006-11-06
 */
class L0CaloCandidatesFromRawBank : public GaudiTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_L0CaloCandidatesFromRawBank; }

  /// Standard constructor
  L0CaloCandidatesFromRawBank( const std::string& type, 
                               const std::string& name,
                               const IInterface* parent);

  virtual ~L0CaloCandidatesFromRawBank( ); ///< Destructor

  virtual StatusCode initialize(); ///< Initialisation

  void convertRawBankToTES( std::vector<std::vector<unsigned int> >& data,
                            std::string& nameFullInTES,
                            std::string& nameInTES,
			    const int version );

protected:

private:
  DeCalorimeter* m_ecal;         ///< ECAL detector element
  DeCalorimeter* m_hcal;         ///< HCAL detector element
  double         m_etScale;      ///< Conversion int -> Et.
  bool           m_doDebugDecoding ; ///< Store intermediate information for debug
};
#endif // L0CALOCANDIDATESFROMRAWBANK_H
