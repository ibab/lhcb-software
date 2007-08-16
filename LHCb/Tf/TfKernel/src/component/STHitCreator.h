// $Id: STHitCreator.h,v 1.2 2007-08-16 12:54:00 jonrob Exp $
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

    /// init
    virtual StatusCode initialize();

    /// incident service handle
    virtual void handle( const Incident& incident ) ;

    virtual STHitRange hits() const ;
    virtual STHitRange hits(int iStation, int iLayer) const ;
    virtual STHitRange hits(int iStation, int iLayer, int iRegion) const ;
    virtual STHitRange hits(int iStation, int iLayer, int iRegion, float xmin, float xmax) const  ;
    virtual STHitRange hits(int iStation, int iLayer, int iRegion, float xmin, float xmax, float ymin, float ymax ) const ;
    virtual const STRegion* region( int iStation, int iLayer, int iRegion ) const ;

  private:

    void loadHits() const ;

  private:
    mutable HitCreatorGeom::STDetector* m_detectordata ;
    std::string m_clusterLocation;
    std::string m_detectorLocation;
  } ;
};

#endif

