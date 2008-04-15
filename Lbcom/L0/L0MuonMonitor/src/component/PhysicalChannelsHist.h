// $Id: PhysicalChannelsHist.h,v 1.2 2008-04-15 09:47:37 jucogan Exp $
#ifndef COMPONENT_PHYSICALCHANNELSHIST_H 
#define COMPONENT_PHYSICALCHANNELSHIST_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoTool.h"
#include "MuonKernel/MuonSystemLayout.h"

static const InterfaceID IID_PhysicalChannelsHist ( "PhysicalChannelsHist", 1, 0 );

/** @class PhysicalChannelsHist PhysicalChannelsHist.h component/PhysicalChannelsHist.h
 *  
 *
 *  @author 
 *  @date   2008-04-07
 */
class PhysicalChannelsHist : public GaudiHistoTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_PhysicalChannelsHist; }

  /// Standard constructor
  PhysicalChannelsHist( const std::string& type, 
                        const std::string& name,
                        const IInterface* parent);

  virtual ~PhysicalChannelsHist( ); ///< Destructor

  void bookHistos();
  void bookHistos(int quarter, int region, int station);
  void fillHistos(const std::vector<LHCb::MuonTileID> &tiles, int ts=0);
  
protected:

private:
  AIDA::IHistogram1D * m_hist[4][3][5][4];
  AIDA::IHistogram2D * m_hist2D[4][3][5][4];
  AIDA::IHistogram2D * m_histBX[4][3][5][4];
  MuonSystemLayout  m_channel_layout[3];
  MuonSystemLayout  m_opt_link_layout;
  
  void setLayouts(); // Set the layouts in use

  bool m_2D; 
  bool m_BX; 

};
#endif // COMPONENT_PHYSICALCHANNELSHIST_H
