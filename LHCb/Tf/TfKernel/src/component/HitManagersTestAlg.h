// $Id: HitManagersTestAlg.h,v 1.2 2007-08-21 14:24:31 jonrob Exp $
#ifndef COMPONENT_HITMANAGERSTESTALG_H
#define COMPONENT_HITMANAGERSTESTALG_H 1

// STD
#include <cmath>

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/RndmGenerators.h"

// TfKernel
#include "TfKernel/TStationHitManager.h"
#include "TfKernel/TTStationHitManager.h"
#include "TfKernel/HitExtension.h"
#include "TfKernel/LineHit.h"

namespace Tf
{

  /** @class HitManagersTestAlg HitManagersTestAlg.h component/HitManagersTestAlg.h
   *
   *  Simple little algorithm to test the Hit manager tools
   *
   *  @author Chris Jones
   *  @date   2007-08-21
   */
  class HitManagersTestAlg : public GaudiAlgorithm
  {

  public:

    /** @class TestHit HitManagersTestAlg.h component/HitManagersTestAlg.h
     *  Trivial test extension hit
     *  @author Chris Jones
     *  @date   2007-08-21
     */
    class TestHit : public HitExtension<Tf::LineHit>
    {
    public:
      TestHit() : HitExtension<Tf::LineHit>(NULL) { }
      TestHit( const Tf::OTHit & otHit ) : HitExtension<Tf::LineHit>(&otHit) { }
      TestHit( const Tf::STHit & stHit ) : HitExtension<Tf::LineHit>(&stHit) { }
      ~TestHit() { }
    public:
      /// Overload output to ostream
      friend inline std::ostream& operator << ( std::ostream& os, const TestHit & hit )
      { return os << &hit << " " << hit.hit()->x(); }
    };

  public:

    /// Standard constructor
    HitManagersTestAlg( const std::string& name, ISvcLocator* pSvcLocator );

    virtual ~HitManagersTestAlg( ); ///< Destructor

    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution

  private:

    /// Create a random T station ID
    inline TStationID tStationID() const
    {
      return TStationID((int)floor(m_rndm()*(float)Tf::RegionID::OTIndex::kNStations));
    }
    /// Create a random T layer ID
    inline TLayerID tLayerID() const
    {
      return TLayerID((int)floor(m_rndm()*(float)Tf::RegionID::OTIndex::kNLayers));
    }
    /// Create a random T region ID
    inline TRegionID tRegionID() const
    {
      return TRegionID((int)floor(m_rndm()*(float)(Tf::RegionID::OTIndex::kNRegions +
                                                   Tf::RegionID::ITIndex::kNRegions)));
    }

    /// Create a random TT station ID
    inline TTStationID ttStationID() const
    {
      return TTStationID((int)floor(m_rndm()*(float)Tf::RegionID::TTIndex::kNStations));
    }
    /// Create a random TT layer ID
    inline TTLayerID ttLayerID() const
    {
      return TTLayerID((int)floor(m_rndm()*(float)Tf::RegionID::TTIndex::kNLayers));
    }
    /// Create a random TT region ID
    inline TTRegionID ttRegionID() const
    {
      return TTRegionID((int)floor(m_rndm()*(float)Tf::RegionID::TTIndex::kNRegions));
    }

  private:


    mutable Rndm::Numbers m_rndm;

    // hit managers

    /// T hit manager
    typedef Tf::TStationHitManager<TestHit> THitMan;
    THitMan * m_tMan;

    /// TT hit manager
    typedef Tf::TTStationHitManager<TestHit> TTHitMan;
    TTHitMan * m_ttMan;

  };

}

#endif // COMPONENT_HITMANAGERSTESTALG_H
