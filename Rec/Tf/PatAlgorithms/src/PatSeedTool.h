// $Id: PatSeedTool.h,v 1.1.1.1 2007-10-09 18:23:10 smenzeme Exp $
#ifndef PATSEEDTOOL_H
#define PATSEEDTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "PatSeedTrack.h"

#include "TfKernel/HitExtension.h"


static const InterfaceID IID_PatSeedTool ( "PatSeedTool", 1, 0 );


  /** @class PatSeedTool PatSeedTool.h
   *  Tool to fit the Pat Seeding tracks
   *
   *  @author Olivier Callot
   *  @date   2006-10-23 Initial version
   *  @date   2007-08-20 Update for a-team framework 
   */

  class PatSeedTool : public GaudiTool {
  public:

    // Return the interface ID
    static const InterfaceID& interfaceID() { return IID_PatSeedTool; }

    /// Standard constructor
    PatSeedTool( const std::string& type,
                 const std::string& name,
                 const IInterface* parent);

    virtual ~PatSeedTool( ); ///< Destructor

    bool fitTrack( PatSeedTrack& track, double maxChi2, int minPlanes, bool xOnly, bool forceDebug );

  protected:

    void fitXProjection( PatSeedTrack& track, bool forceDebug );

    void fitInitialXProjection( PatSeedTrack& track, bool forceDebug );

    void fitInitialStereoProjection( PatSeedTrack& track, bool forceDebug );

    void printTCoord( MsgStream& msg, PatSeedTrack& track, PatFwdHit* hit )  {
      double dist = track.distance( hit );
      double chi2 = dist*dist* hit->hit()->weight();
      msg << "  Hit st " << hit->hit()->station() << " lay " << hit->hit()->layer()
          << " region " << hit->hit()->region()
          << format( " code%3d z %7.1f distWire%7.2f drift%5.2f dist%8.3f rl%2d Chi2 %8.3f",
                     hit->planeCode(), hit->z(), hit->x() - track.xAtZ( hit->z() ),
                     hit->driftDistance(), dist, hit->rlAmb(), chi2 );
    }
  private:
  };

#endif // PATSEEDTOOL_H
