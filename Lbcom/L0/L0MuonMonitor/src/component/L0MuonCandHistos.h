// $Id: L0MuonCandHistos.h,v 1.3 2008-09-15 07:46:40 jucogan Exp $
#ifndef COMPONENT_L0MUONCANDHISTOS_H 
#define COMPONENT_L0MUONCANDHISTOS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoTool.h"
#include "Kernel/MuonLayout.h"

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

  void bookHistos(int nmax=8, bool shortname=true);
  void fillHistos(LHCb::L0MuonCandidates* cands, int ts=0, int bid=-1);
  void fillHistos(int sum, int bid=-1);

protected:

private:

  MuonLayout m_stationLayouts[3];

  AIDA::IHistogram1D * m_hpt;
  AIDA::IHistogram1D * m_hencodedpt;
  AIDA::IHistogram2D * m_hpos[3];
  AIDA::IHistogram2D * m_hnumber;
  AIDA::IHistogram1D * m_hsum;
  AIDA::IHistogram1D * m_hbid;
  AIDA::IHistogram1D * m_hevtbid;
  
};
#endif // COMPONENT_L0MUONCANDHISTOS_H
