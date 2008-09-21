// $Id: L0MuonErrorHistos.h,v 1.4 2008-09-21 21:46:23 jucogan Exp $
#ifndef COMPONENT_L0MUONERRORHISTOS_H 
#define COMPONENT_L0MUONERRORHISTOS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoTool.h"

#include "Kernel/MuonTileID.h"
#include "Event/L0MuonError.h"

#include "L0MuonKernel/MonUtilities.h"

static const InterfaceID IID_L0MuonErrorHistos ( "L0MuonErrorHistos", 1, 0 );

/** @class L0MuonErrorHistos L0MuonErrorHistos.h component/L0MuonErrorHistos.h
 *  
 *
 *  @author Julien Cogan
 *  @date   2008-07-17
 */
class L0MuonErrorHistos : public GaudiHistoTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_L0MuonErrorHistos; }

  /// Standard constructor
  L0MuonErrorHistos( const std::string& type, 
                   const std::string& name,
                   const IInterface* parent);

  virtual ~L0MuonErrorHistos( ); ///< Destructor

  void bookHistos(bool shortname=true);
  void fillHistos();
  void bookHistos_multi(bool shortname=true);
  void bookHistos_gen(bool shortname=true);
  void bookHistos_quarter(int qua, bool shortname=true);
  void bookHistos_board(int qua, bool shortname=true);
  void bookHistos_board(int qua, int reg, bool shortname=true);
  

protected:

private:


  int m_nmulti_opt[4];
  int m_nmulti_ser[4];
  int m_nmulti_par[4];
  
  void _fillHistos(std::string location,int board_type);

  AIDA::IHistogram1D * m_hgen[L0Muon::MonUtilities::NErrors];
  AIDA::IHistogram1D * m_hquarter[L0Muon::MonUtilities::NErrors][4];
  AIDA::IHistogram1D * m_hboard[L0Muon::MonUtilities::NErrors][4][12];
  AIDA::IHistogram1D * m_hpu[4][12][4];
  AIDA::IHistogram1D * m_hmulti_opt[4];
  AIDA::IHistogram1D * m_hmulti_ser[4];
  AIDA::IHistogram1D * m_hmulti_par[4];

};


#endif // COMPONENT_L0MUONERRORHISTOS_H
