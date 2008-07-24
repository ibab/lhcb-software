// $Id: L0MuonInfoHistos.h,v 1.1 2008-07-24 09:36:53 jucogan Exp $
#ifndef COMPONENT_L0MUONINFOHISTOS_H 
#define COMPONENT_L0MUONINFOHISTOS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoTool.h"

#include "Event/ODIN.h"
#include "Event/L0MuonInfo.h"

#include "L0MuonKernel/MonUtilities.h"

static const InterfaceID IID_L0MuonInfoHistos ( "L0MuonInfoHistos", 1, 0 );

/** @class L0MuonInfoHistos L0MuonInfoHistos.h component/L0MuonInfoHistos.h
 *  
 *
 *  @author Julien Cogan
 *  @date   2008-07-17
 */
class L0MuonInfoHistos : public GaudiHistoTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_L0MuonInfoHistos; }

  /// Standard constructor
  L0MuonInfoHistos( const std::string& type, 
                 const std::string& name,
                 const IInterface* parent);

  virtual ~L0MuonInfoHistos( ); ///< Destructor

  void bookHistos(bool shortname=true);
  void fillHistos();
  
  StatusCode getInfo();
  
  int errStatus(){return m_errStatus;}
  int ovfStatus(){return m_ovfStatus;}
  int bunchId(){return m_bunchId;}
  int evtNum(){return m_evtNum;}
  bool error(){return m_error;}

protected:

private:

  int m_errStatus;
  int m_ovfStatus;
  int m_bunchId;
  int m_evtNum;
  bool m_error;
  
  
  AIDA::IHistogram1D * m_herror;
  AIDA::IHistogram1D * m_hbid;
  AIDA::IHistogram1D * m_hbid2;
  AIDA::IHistogram1D * m_hevt;

};
#endif // COMPONENT_L0MUONINFOHISTOS_H
