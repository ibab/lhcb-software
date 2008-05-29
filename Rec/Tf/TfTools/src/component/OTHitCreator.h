
//-----------------------------------------------------------------------------
/** @file OTHitCreator.h
 *
 *  Header file for class : Tf::OTHitCreator
 *
 *  $Id: OTHitCreator.h,v 1.4 2008-05-29 10:55:36 smenzeme Exp $
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

  class OTHitCreator: public GaudiTool,
                      virtual public IOTHitCreator,
                      virtual public IIncidentListener
  {

  public:

    /// constructer
    OTHitCreator(const std::string& type,
                 const std::string& name,
                 const IInterface* parent);

    // destructer
    virtual ~OTHitCreator() ;

    /// initialisation
    virtual StatusCode initialize();
    /// finalisation
    virtual StatusCode finalize();

    /// incident service handle
    virtual void handle( const Incident& incident ) ;

    // Load all the OT hits
    virtual OTHitRange hits() const ;

    // Load the hits for a given region of interest
    virtual OTHitRange hits(const TStationID iStation) const ;

    // Load the hits for a given region of interest
    virtual OTHitRange hits(const TStationID iStation,
                            const TLayerID iLayer) const ;

    // Load the hits for a given region of interest
    virtual OTHitRange hits(const TStationID iStation,
                            const TLayerID iLayer,
                            const OTRegionID iRegion) const ;

    // Load the hits for a given region of interest
    virtual OTHitRange hits(const TStationID iStation,
                            const TLayerID iLayer,
                            const OTRegionID iRegion,
                            const double xmin,
                            const double xmax) const ;

    // Load the hits for a given region of interest
    virtual OTHitRange hits(const TStationID iStation,
                            const TLayerID iLayer,
                            const OTRegionID iRegion,
                            const double xmin,
                            const double xmax,
                            const double ymin,
                            const double ymax) const ;

    // Retrieve the OTRegion for a certain region ID. The region
    virtual const OTRegion* region(const TStationID iStation,
                                   const TLayerID iLayer,
                                   const OTRegionID iRegion) const ;


    // Create a single OTHit from an lhcbid
    OTHit hit( const LHCb::OTChannelID id ) const ;    

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
    IOTRawBankDecoder* m_otdecoder ;
    bool m_rejectOutOfTime ;
    double m_tmin ;
    double m_tmax ;
    //mutable HitCreatorGeom::Detector<HitCreatorGeom::OTRegion>* m_detectordata ;
    //mutable HitCreatorGeom::Detector<HitCreatorGeom::RegionOfModules<HitCreatorGeom::OTModule> > *m_detectordata ;
    HitCreatorGeom::OTDetector *m_detectordata ;

    bool m_noDriftTimes;
    double m_forceDriftRadius;
    double m_forceResolution;
    OTDet::RtRelation *m_rtrel;
  };
}


#endif // TFKERNEL_OTHitCreator_H

