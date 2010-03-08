// $Id: ICaloHypo2Calo.h,v 1.4 2010-03-08 01:51:02 odescham Exp $
#ifndef ICALOHYPO2CALO_H 
#define ICALOHYPO2CALO_H 1

// Include files
// from STL
#include <string>

#include "Kernel/CaloCellID.h"
#include "Event/CaloDigit.h"
#include "Event/CaloCluster.h"
#include "Event/CaloHypo.h"
// from Gaudi
#include "GaudiKernel/IAlgTool.h"
// forward declaration

static const InterfaceID IID_ICaloHypo2Calo ( "ICaloHypo2Calo", 1, 2 );

/** @class ICaloHypo2Calo ICaloHypo2Calo.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2008-09-09
 */


class ICaloHypo2Calo : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ICaloHypo2Calo; }

  virtual StatusCode initialize()=0;
  virtual void setCalos(std::string from, std::string to)=0;
   virtual const std::vector<LHCb::CaloCellID>& cellIDs(LHCb::CaloHypo  fromHypo, std::string toCalo)=0;
  virtual const std::vector<LHCb::CaloCellID>& cellIDs(LHCb::CaloCluster  fromCluster, std::string toCalo)=0;
  virtual const std::vector<LHCb::CaloCellID>& cellIDs()=0;

  virtual const std::vector<LHCb::CaloDigit*>& digits(LHCb::CaloCluster  fromCluster, std::string toCalo)=0;
  virtual const std::vector<LHCb::CaloDigit*>& digits(LHCb::CaloHypo  fromHypo, std::string toCalo)=0;
  virtual const std::vector<LHCb::CaloDigit*>& digits()=0;

  virtual double energy(LHCb::CaloCluster  fromCluster, std::string toCalo)=0;
  virtual double energy(LHCb::CaloHypo  fromHypo, std::string toCalo)=0;
  virtual double energy()=0;

  virtual int multiplicity(LHCb::CaloCluster  fromCluster, std::string toCalo)=0;
  virtual int multiplicity(LHCb::CaloHypo   fromHypo     , std::string toCalo)=0;
  virtual int multiplicity()=0;
  virtual StatusCode _setProperty(const std::string&, const std::string&)=0;

protected:

private:

};
#endif // ICALOHYPO2CALO_H
