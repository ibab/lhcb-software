// $Id: ITrack2Calo.h,v 1.1 2007-06-25 20:35:01 odescham Exp $
#ifndef ITRACK2CALO_H 
#define ITRACK2CALO_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
// from LHCb
#include "Event/Track.h"
#include "Event/State.h"
#include "Event/CaloCluster.h"
#include "Event/CaloHypo.h"
#include "Event/CaloPosition.h"
#include "CaloDet/DeCalorimeter.h"
#include "Kernel/CaloCellID.h"

static const InterfaceID IID_ITrack2Calo ( "ITrack2Calo", 1, 0 );

/** @class ITrack2Calo ITrack2Calo.h Kernel/ITrack2Calo.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2007-06-25
 */
class ITrack2Calo : virtual public IAlgTool {
friend class CaloCellID;
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ITrack2Calo; }

  virtual StatusCode         initialize() = 0;
  virtual bool  match(const  LHCb::Track* track,
                      std::string det = DeCalorimeterLocation::Ecal,
                      CaloPlane::Plane plane = CaloPlane::ShowerMax, 
                      double delta = 0.,
                      LHCb::ParticleID pid = LHCb::ParticleID(211)
                      )= 0;


  virtual LHCb::State caloState() = 0;
  virtual LHCb::CaloCellID caloCellID() = 0;
  virtual bool isValid() = 0;
  //
  virtual LHCb::State closestState(LHCb::CaloHypo*    hypo   ,LHCb::ParticleID pid = LHCb::ParticleID(211))=0;
  virtual LHCb::State closestState(LHCb::CaloCluster* cluster,LHCb::ParticleID pid = LHCb::ParticleID(211))=0;
  virtual LHCb::State closestState(LHCb::CaloPosition calopos,LHCb::ParticleID pid = LHCb::ParticleID(211))=0;
  virtual LHCb::State closestState(LHCb::CaloCellID   cellID ,LHCb::ParticleID pid = LHCb::ParticleID(211))=0;
protected:

private:

};
#endif // ITRACK2CALO_H
