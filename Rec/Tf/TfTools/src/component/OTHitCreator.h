
//-----------------------------------------------------------------------------
/** @file OTHitCreator.h
 *
 *  Header file for class : Tf::OTHitCreator
 *
 *  $Id: OTHitCreator.h,v 1.7 2010-01-27 23:25:06 wouter Exp $
 *
 *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
 *  @date   2007-06-01
 */
//-----------------------------------------------------------------------------

#ifndef TFKERNEL_OTHitCreator_H
#define TFKERNEL_OTHitCreator_H 1

#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "TfKernel/IOTHitCreator.h"
#include "GaudiKernel/IIncidentListener.h"
#include "TfKernel/OTHit.h"
#include "OTDAQ/IOTRawBankDecoder.h"

namespace LHCb{
  class OTChannelID;
}

class DeOTDetector;

namespace Tf
{

  // forward declaration of the class that is holding the hit data
  namespace HitCreatorGeom {
    class OTModule ;
    class OTDetector ;
  }

  /** @class OTHitCreator OTHitCreator.h
   *
   *  Implementation of Tf::IOTHitCreator.
   *
   *  Creates the Tf::OTHit objects for the Tf tracking framework
   *
   *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   *  @date   2007-06-01
   */

  class OTHitCreator final: public GaudiTool,
                            virtual public IOTHitCreator,
                            virtual public IIncidentListener 
                      
  {

  public:

    /// constructer
    OTHitCreator(const std::string& type,
                 const std::string& name,
                 const IInterface* parent);

    // destructer
    ~OTHitCreator() override = default;

    /// initialisation
    StatusCode initialize() override;
    /// finalisation
    StatusCode finalize() override;

    /// incident service handle
    void handle( const Incident& incident ) override;
 private:
    /// update manager handle
    StatusCode updateGeometry() ;
 public:
    // RestUsed flag for all OT hits
    void resetUsedFlagOfHits() const override;

    // Load all the OT hits
    OTHitRange hits() const override;

    // Load the hits for a given region of interest
    OTHitRange hits(const TStationID iStation) const override;

    // Load the hits for a given region of interest
    OTHitRange hits(const TStationID iStation,
                    const TLayerID iLayer) const override;

    // Load the hits for a given region of interest
    OTHitRange hits(const TStationID iStation,
                    const TLayerID iLayer,
                    const OTRegionID iRegion) const override;

    // Load the hits for a given region of interest
    OTHitRange hits(const TStationID iStation,
                    const TLayerID iLayer,
                    const OTRegionID iRegion,
                    const double xmin,
                    const double xmax) const override ;

    // Load the hits for a given region of interest
    OTHitRange hitsLocalXRange(const TStationID iStation,
		       const TLayerID iLayer,
		       const OTRegionID iRegion,
		       const double xmin,
		       const double xmax) const override;

    // Load the hits for a given region of interest
    OTHitRange hits(const TStationID iStation,
                    const TLayerID iLayer,
                    const OTRegionID iRegion,
                    const double xmin,
                    const double xmax,
                    const double ymin,
                    const double ymax) const override;

    // Retrieve the OTRegion for a certain region ID. The region
    const OTRegion* region(const TStationID iStation,
                           const TLayerID iLayer,
                           const OTRegionID iRegion) const override;


    // Create a single OTHit from an lhcbid
    OTHit hit( const LHCb::OTChannelID id ) const override;

  public:
    double tmin() const { return m_tmin ; }
    double tmax() const { return m_tmax ; }
    bool   rejectOutOfTime() const { return m_rejectOutOfTime ; }
    const IOTRawBankDecoder* decoder() const { return &(*m_otdecoder) ; }
    const HitCreatorGeom::OTModule* module( const LHCb::OTChannelID id ) const ;
    // return a pointer to a custom rt relation if drift times are not to be
    // used and null otherwise
    const OTDet::RtRelation* getRtRelation() const;

  private:
    ToolHandle<IOTRawBankDecoder> m_otdecoder ;
    bool m_rejectOutOfTime ;
    double m_tmin ;
    double m_tmax ;
    //mutable HitCreatorGeom::Detector<HitCreatorGeom::OTRegion>* m_detectordata ;
    //mutable HitCreatorGeom::Detector<HitCreatorGeom::RegionOfModules<HitCreatorGeom::OTModule> > *m_detectordata ;
    const DeOTDetector* m_otdetector ;
    std::unique_ptr<HitCreatorGeom::OTDetector> m_detectordata ;

    bool m_noDriftTimes;
    double m_forceDriftRadius;
    double m_forceResolution;
    std::unique_ptr<OTDet::RtRelation> m_rtrel;
  };
}


#endif // TFKERNEL_OTHitCreator_H

