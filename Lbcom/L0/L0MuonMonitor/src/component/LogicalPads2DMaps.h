// $Id: LogicalPads2DMaps.h,v 1.2 2008-05-06 12:19:27 jucogan Exp $
#ifndef COMPONENT_LOGICALPADS2DMAPS_H 
#define COMPONENT_LOGICALPADS2DMAPS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoTool.h"
#include "Kernel/MuonLayout.h"

static const InterfaceID IID_LogicalPads2DMaps ( "LogicalPads2DMaps", 1, 0 );

/** @class LogicalPads2DMaps LogicalPads2DMaps.h component/LogicalPads2DMaps.h
 *  
 *
 *  @author 
 *  @date   2008-04-02
 */
class LogicalPads2DMaps : public GaudiHistoTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_LogicalPads2DMaps; }

  /// Standard constructor
  LogicalPads2DMaps( const std::string& type, 
                     const std::string& name,
                     const IInterface* parent);

  virtual ~LogicalPads2DMaps( ); ///< Destructor

  void bookHistos();
  void bookHistos(int station);
  void fillHistos(const std::vector<LHCb::MuonTileID> &pads, int ts=0);
  
  enum stations{M1=0,M2,M3,M4,M5,NStations};
  

protected:

private:

  AIDA::IHistogram2D * m_hmap[5];
  AIDA::IHistogram2D * m_hmultiBx[5];
  AIDA::IHistogram2D * m_hmultiBx_region[5][4];

  MuonLayout m_stationLayouts[5];

};
#endif // COMPONENT_LOGICALPADS2DMAPS_H
