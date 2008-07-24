// $Id: L0MuonCandHistos.h,v 1.1 2008-07-24 09:36:53 jucogan Exp $
#ifndef COMPONENT_L0MUONCANDHISTOS_H 
#define COMPONENT_L0MUONCANDHISTOS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoTool.h"

#include "Event/L0MuonCandidate.h"

static const InterfaceID IID_L0MuonCandHistos ( "L0MuonCandHistos", 1, 0 );

/** @class L0MuonCandHistos L0MuonCandHistos.h component/L0MuonCandHistos.h
 *  
 *
 *  @author Julien Cogan
 *  @date   2008-07-23
 */
class L0MuonCandHistos : public GaudiHistoTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_L0MuonCandHistos; }

  /// Standard constructor
  L0MuonCandHistos( const std::string& type, 
                    const std::string& name,
                    const IInterface* parent);

  virtual ~L0MuonCandHistos( ); ///< Destructor

  void bookHistos(bool shortname=true);
  void fillHistos(LHCb::L0MuonCandidates* cands, int ts=0);

protected:

private:

  AIDA::IHistogram1D * m_hpt;
  AIDA::IHistogram2D * m_hseedmap;
  AIDA::IHistogram2D * m_hcandinM1map;
  AIDA::IHistogram2D * m_hcandinM2map;
  AIDA::IHistogram2D * m_hnbcands_in_ts;

};
#endif // COMPONENT_L0MUONCANDHISTOS_H
