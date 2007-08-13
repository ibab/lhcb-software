// $Id: OTHitCreator.h,v 1.1.1.1 2007-08-13 11:13:58 jonrob Exp $
#ifndef _OTHitCreator_H
#define _OTHitCreator_H

/** @class OTHitCreator OTDataSvc.h
 *
 *  Interface to data svc
 *
 *  @author M.Needham
 *  @date   30/04/2004
 */

////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// OTHitCreator header file
//
////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "TfKernel/IOTHitCreator.h"
#include "GaudiKernel/IIncidentListener.h"
#include "TfKernel/OTHit.h"
#include "OTRawBankDecoder.h"

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
    //class OTRegion ;
    //template <typename RegionType> class Detector ;
    //template <typename ModuleType, typename DetectorType> class RegionOfModules ;
  }

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

    /// init
    virtual StatusCode initialize();
    virtual StatusCode finalize();

    /// incident service handle
    virtual void handle( const Incident& incident ) ;

    virtual OTHitRange hits() const ;
    virtual OTHitRange hits(int iStation, int iLayer) const ;
    virtual OTHitRange hits(int iStation, int iLayer, int iRegion) const ;
    virtual OTHitRange hits(int iStation, int iLayer, int iRegion, float xmin, float xmax ) const ;
    virtual OTHitRange hits(int iStation, int iLayer, int iRegion, float xmin, float xmax, float ymin, float ymax ) const ;
    virtual const OTRegion* region( int iStation, int iLayer, int iRegion ) const ;

  public:
    double tmin() const { return m_tmin ; }
    double tmax() const { return m_tmax ; }
    bool   rejectOutOfTime() const { return m_rejectOutOfTime ; }
    const IOTRawBankDecoder* decoder() const { return &(*m_otdecoder) ; }
  private:
    ToolHandle<IOTRawBankDecoder> m_otdecoder ;
    bool m_rejectOutOfTime ;
    double m_tmin ;
    double m_tmax ;
    //mutable HitCreatorGeom::Detector<HitCreatorGeom::OTRegion>* m_detectordata ;
    //mutable HitCreatorGeom::Detector<HitCreatorGeom::RegionOfModules<HitCreatorGeom::OTModule> > *m_detectordata ;
    HitCreatorGeom::OTDetector *m_detectordata ;
  };
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

