// $Id: L0MuonErrorHistos.h,v 1.2 2008-09-05 09:07:09 jucogan Exp $
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
  void bookHistos_gen(bool shortname=true);
  void bookHistos_quarter(int qua, bool shortname=true);
  void bookHistos_board(int qua, bool shortname=true);
  void bookHistos_board(int qua, int reg, bool shortname=true);
  void fillHistos();
  

protected:

private:


  
  void _fillHistos(std::string location,int board_type);

  AIDA::IHistogram1D * m_hgen[L0Muon::MonUtilities::NErrors];
  AIDA::IHistogram1D * m_hquarter[L0Muon::MonUtilities::NErrors][4];
  AIDA::IHistogram1D * m_hboard[L0Muon::MonUtilities::NErrors][4][12];

};


#endif // COMPONENT_L0MUONERRORHISTOS_H
