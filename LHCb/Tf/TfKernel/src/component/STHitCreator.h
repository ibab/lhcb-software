// $Id: STHitCreator.h,v 1.3 2007-08-19 16:54:41 jonrob Exp $
#ifndef _ITHitCreator_H
#define _ITHitCreator_H

#include <string>

#include "GaudiAlg/GaudiTool.h"
//#include "GaudiKernel/ToolHandle.h"

#include "TfKernel/ISTHitCreator.h"
#include "GaudiKernel/IIncidentListener.h"
#include "TfKernel/STHit.h"

namespace Tf

{

  // forward declaration of the class that is holding the hit data
  namespace HitCreatorGeom {
    class STDetector ;
  }

  /** @class STHitCreator TfKernel/STHitCreator.h
   *
   *  Interface to data svc
   *
   *  @author M.Needham
   *  @date   30/04/2004
   */

  class STHitCreator: public GaudiTool,
                      virtual public ISTHitCreator,
                      virtual public IIncidentListener

  {

  public:

    /// constructor
    STHitCreator(const std::string& type,
                 const std::string& name,
                 const IInterface* parent);

    /// destructor
    virtual ~STHitCreator();

    /// initialize
    virtual StatusCode initialize();

    /// finalize
    virtual StatusCode finalize();

    /// incident service handle
    virtual void handle( const Incident& incident ) ;

    // Load all the IT hits
    virtual STHitRange itHits() const ;

    // Load the ST hits for a given region of interest
    virtual STHitRange hits(const TStationID iStation,
                            const TLayerID iLayer) const ;

    // Load the ST hits for a given region of interest
    virtual STHitRange hits(const TStationID iStation,
                            const TLayerID iLayer,
                            const ITRegionID iRegion) const ;

    // Load the ST hits for a given region of interest
    virtual STHitRange hits(const TStationID iStation,
                            const TLayerID iLayer,
                            const ITRegionID iRegion,
                            const float xmin,
                            const float xmax) const ;

    // Load the hits for a given region of interest
    virtual STHitRange hits(const TStationID iStation,
                            const TLayerID iLayer,
                            const ITRegionID iRegion,
                            const float xmin,
                            const float xmax,
                            const float ymin,
                            const float ymax) const ;

    // Retrieve the STRegion for a certain region ID. The region
    virtual const STRegion* region(const TStationID iStation,
                                   const TLayerID iLayer,
                                   const ITRegionID  iRegion) const ;

   // Load all the TT hits
    virtual STHitRange ttHits() const ;

   // Load the ST hits for a given region of interest
    virtual STHitRange hits(const TTStationID iStation,
                            const TTLayerID iLayer) const ;

    // Load the ST hits for a given region of interest
    virtual STHitRange hits(const TTStationID iStation,
                            const TTLayerID iLayer,
                            const TTRegionID iRegion) const ;

    // Load the ST hits for a given region of interest
    virtual STHitRange hits(const TTStationID iStation,
                            const TTLayerID iLayer,
                            const TTRegionID iRegion,
                            const float xmin,
                            const float xmax) const ;

    // Load the hits for a given region of interest
    virtual STHitRange hits(const TTStationID iStation,
                            const TTLayerID iLayer,
                            const TTRegionID iRegion,
                            const float xmin,
                            const float xmax,
                            const float ymin,
                            const float ymax) const ;

    // Retrieve the STRegion for a certain region ID. The region
    virtual const STRegion* region(const TTStationID iStation,
                                   const TTLayerID iLayer,
                                   const TTRegionID  iRegion) const ;

  private:

    /// Load the hits
    void loadHits() const ;

    // access on demand the IT Detector Data object
    inline HitCreatorGeom::STDetector* itDetData() const 
    {
      return m_itdetectordata ;
    }

    // access on demand the TT Detector Data object
    inline HitCreatorGeom::STDetector* ttDetData() const 
    {
      return m_ttdetectordata ;
    }

  private:

    mutable HitCreatorGeom::STDetector* m_itdetectordata ;
    mutable HitCreatorGeom::STDetector* m_ttdetectordata ;

    std::string m_itclusterLocation;
    std::string m_itdetectorLocation;

    std::string m_ttclusterLocation;
    std::string m_ttdetectorLocation;

  } ;
};

#endif

