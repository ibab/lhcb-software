// $Id: ICalo2Calo.h,v 1.4 2009-04-16 16:08:43 odescham Exp $
#ifndef ICALO2CALO_H 
#define ICALO2CALO_H 1

// Include files
// from STL
#include <string>

#include "Kernel/CaloCellID.h"
#include "Event/CaloDigit.h"
#include "Event/CaloCluster.h"
// from Gaudi
#include "GaudiKernel/IAlgTool.h"
// forward declaration

static const InterfaceID IID_ICalo2Calo ( "ICalo2Calo", 1, 2 );

/** @class ICalo2Calo ICalo2Calo.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2007-05-29
 */


class ICalo2Calo : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ICalo2Calo; }

  virtual StatusCode initialize()=0;
  virtual void setCalos(std::string from, std::string to)=0;
  virtual const std::vector<LHCb::CaloCellID>& cellIDs(LHCb::CaloCluster fromCluster, std::string toCalo)=0;
  virtual const std::vector<LHCb::CaloCellID>& cellIDs(LHCb::CaloCellID  fromId     , std::string toCalo, bool init=true)=0;
  virtual const std::vector<LHCb::CaloCellID>& cellIDs()=0;
  virtual const std::vector<LHCb::CaloDigit*>& digits(LHCb::CaloCellID   fromId     , std::string toCalo)=0;
  virtual const std::vector<LHCb::CaloDigit*>& digits(LHCb::CaloCluster  fromCluster, std::string toCalo)=0;
  virtual const std::vector<LHCb::CaloDigit*>& digits()=0;
  virtual double energy(LHCb::CaloCellID   fromId     , std::string toCalo)=0;
  virtual double energy(LHCb::CaloCluster  fromCluster, std::string toCalo)=0;
  virtual double energy()=0;
  virtual int multiplicity(LHCb::CaloCellID   fromId     , std::string toCalo)=0;
  virtual int multiplicity(LHCb::CaloCluster  fromCluster, std::string toCalo)=0;
  virtual int multiplicity()=0;


  virtual bool isLocalMax(LHCb::CaloDigit digit)=0;

protected:

private:

};
#endif // ICALO2CALO_H
