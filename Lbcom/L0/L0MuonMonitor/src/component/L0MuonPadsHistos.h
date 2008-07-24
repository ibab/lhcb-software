// $Id: L0MuonPadsHistos.h,v 1.1 2008-07-24 09:36:53 jucogan Exp $
#ifndef COMPONENT_L0MUONPADSHISTOS_H 
#define COMPONENT_L0MUONPADSHISTOS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoTool.h"
#include "Kernel/MuonLayout.h"

#include "L0MuonKernel/MonUtilities.h"

static const InterfaceID IID_L0MuonPadsHistos ( "L0MuonPadsHistos", 1, 0 );

/** @class L0MuonPadsHistos L0MuonPadsHistos.h component/L0MuonPadsHistos.h
 *  
 *
 *  @author 
 *  @date   2008-04-02
 */
class L0MuonPadsHistos : public GaudiHistoTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_L0MuonPadsHistos; }

  /// Standard constructor
  L0MuonPadsHistos( const std::string& type, 
                     const std::string& name,
                     const IInterface* parent);

  virtual ~L0MuonPadsHistos( ); ///< Destructor

  void bookHistos(bool shortname=true);
  void bookHistos(int station, bool shortname=true);
  void fillHistos(const std::vector<LHCb::MuonTileID> &pads, int ts=0);
  

protected:

private:

  AIDA::IHistogram2D * m_hmap[L0Muon::MonUtilities::NStations];
  AIDA::IHistogram2D * m_hmultiBx[L0Muon::MonUtilities::NStations];
  AIDA::IHistogram2D * m_hmultiBx_region[L0Muon::MonUtilities::NStations][L0Muon::MonUtilities::NRegions];

  MuonLayout m_stationLayouts[L0Muon::MonUtilities::NStations];

};
#endif // COMPONENT_L0MUONPADSHISTOS_H
