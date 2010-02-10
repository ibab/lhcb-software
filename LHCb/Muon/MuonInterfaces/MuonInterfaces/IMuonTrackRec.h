// $Id: IMuonTrackRec.h,v 1.1 2010-02-10 19:07:50 ggiacomo Exp $
#ifndef MUONNNET_IMUONTRACKREC_H 
#define MUONNNET_IMUONTRACKREC_H 1

// Include files
// from STL
#include <string>
#include <map>
#include <vector>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

class MuonHit;
class MuonTrack;
class MuonNeuron;

static const InterfaceID IID_IMuonTrackRec ( "IMuonTrackRec", 1, 0 );

/** @class IMuonTrackRec IMuonTrackRec.h MuonTrackrec/IMuonTrackRec.h
 *  
 *
 *  @author Giovanni Passaleva / Giacomo Graziani
 *  @date   2008-04-11
 */
class IMuonTrackRec : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IMuonTrackRec; }

  virtual const std::vector<MuonHit*>* trackhits() =0;
  virtual const std::vector<MuonTrack*>* tracks()  =0;
  virtual const std::vector<MuonNeuron*>* useneurons()  =0;
  virtual const std::vector<MuonNeuron*>* allneurons()  =0;
  virtual bool recOK() =0;
  virtual bool tooManyHits() =0;
  virtual bool clusteringOn() =0;
  virtual void setZref(double Zref) =0;
  virtual void setPhysicsTiming(bool PhysTiming) =0;
  virtual void setAssumeCosmics(bool AssumeCosmics) =0;
  virtual void setAssumePhysics(bool AssumePhysics) =0;  
  virtual void setSeedStation(int seedS) = 0;
  virtual void setSkipStation(int skipS) = 0;
  virtual StatusCode copyToLHCbTracks() = 0;
};
#endif // MUONNNET_IMUONTRACKREC_H
