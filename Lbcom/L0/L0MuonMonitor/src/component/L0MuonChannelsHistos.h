// $Id: L0MuonChannelsHistos.h,v 1.2 2008-09-21 21:53:42 jucogan Exp $
#ifndef COMPONENT_L0MUONCHANNELSHISTOS_H 
#define COMPONENT_L0MUONCHANNELSHISTOS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoTool.h"
#include "MuonKernel/MuonSystemLayout.h"

#include "L0MuonKernel/MonUtilities.h"

static const InterfaceID IID_L0MuonChannelsHistos ( "L0MuonChannelsHistos", 1, 0 );

/** @class L0MuonChannelsHistos L0MuonChannelsHistos.h component/L0MuonChannelsHistos.h
 *  
 *
 *  @author 
 *  @date   2008-04-07
 */
class L0MuonChannelsHistos : public GaudiHistoTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_L0MuonChannelsHistos; }

  /// Standard constructor
  L0MuonChannelsHistos( const std::string& type, 
                        const std::string& name,
                        const IInterface* parent);

  virtual ~L0MuonChannelsHistos( ); ///< Destructor

  void bookHistos(bool shortname=true);
  void bookHistos(int quarter, int region, int station,bool shortname=true);
  void fillHistos(const std::vector<LHCb::MuonTileID> &tiles, int ts=0);
  void fillHistos(const std::vector<std::pair<LHCb::MuonTileID,int > > &tiles);
  void fillHistos(const std::vector<std::pair<LHCb::MuonTileID,std::pair<int,int> > > &tiles);
  void fillHistosDT(const std::vector<std::pair<LHCb::MuonTileID, int > > &tiles);
  
protected:

private:

  AIDA::IHistogram1D * m_hist[4][3][5][4];
  AIDA::IHistogram2D * m_hist2D[4][3][5][4];
  AIDA::IHistogram2D * m_histBX[4][3][5][4];
  AIDA::IHistogram2D * m_histDT[4][3][5][4];

  MuonSystemLayout  m_channel_layout[3];
  MuonSystemLayout  m_opt_link_layout;
  
  void setLayouts(); // Set the layouts in use

  bool m_2D; 
  bool m_BX; 
  bool m_DT; 

};
#endif // COMPONENT_L0MUONCHANNELSHISTOS_H
