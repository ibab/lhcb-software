// $Id: L0MuonOptLinksHistos.h,v 1.1 2008-09-21 21:45:17 jucogan Exp $
#ifndef COMPONENT_L0MUONOPTLINKSHISTOS_H 
#define COMPONENT_L0MUONOPTLINKSHISTOS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoTool.h"
#include "MuonKernel/MuonSystemLayout.h"

#include "L0MuonKernel/MonUtilities.h"

static const InterfaceID IID_L0MuonOptLinksHistos ( "L0MuonOptLinksHistos", 1, 0 );

/** @class L0MuonOptLinksHistos L0MuonOptLinksHistos.h component/L0MuonOptLinksHistos.h
 *  
 *
 *  @author 
 *  @date   2008-04-07
 */
class L0MuonOptLinksHistos : public GaudiHistoTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_L0MuonOptLinksHistos; }

  /// Standard constructor
  L0MuonOptLinksHistos( const std::string& type, 
                        const std::string& name,
                        const IInterface* parent);

  virtual ~L0MuonOptLinksHistos( ); ///< Destructor

  void bookHistos(bool shortname=true);
  void bookMultiHistos(bool shortname=true);
  void bookHistos(int quarter, int region, int station,bool shortname=true);
  void fillHistos(const std::vector<LHCb::MuonTileID> & tiles);
  void fillHistos(const std::map<std::pair<LHCb::MuonTileID,int>,bool> & tiles);
  
protected:

private:

  AIDA::IHistogram1D * m_hevt;
  AIDA::IHistogram1D * m_hmultitot;
  AIDA::IHistogram1D * m_hmulti[4];
  AIDA::IHistogram1D * m_hist[4][5][4];
  AIDA::IHistogram2D * m_hist2D[4][5][4];

  MuonSystemLayout  m_optlink_layout;

  void setLayouts(); // Set the layouts in use

};
#endif // COMPONENT_L0MUONOPTLINKSHISTOS_H
