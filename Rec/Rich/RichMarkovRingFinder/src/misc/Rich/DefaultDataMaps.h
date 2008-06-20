// $Id: DefaultDataMaps.h,v 1.1 2008-06-20 09:54:23 jonrob Exp $
#ifndef RICHMARKOVRINGFINDER_DEFAULTDATAMAPS_H
#define RICHMARKOVRINGFINDER_DEFAULTDATAMAPS_H 1

// STL
#include <map>

// Kernel
#include "Kernel/RichRadiatorType.h"

namespace Lester
{
  /** @class DefaultDataMaps DefaultDataMaps.h component/DefaultDataMaps.h
   *
   *  Creates default data maps
   *
   *  @author Chris Jones
   *  @date   2008-06-19
   */
  class DefaultDataMaps
  {

  public:

    /// Map type
    typedef std::map<double,double> Map;

  public:

    /// Constructor
    DefaultDataMaps();

    /// Returns the co point map for the given radiator
    const Map & coPointMap( const Rich::RadiatorType rad ) const
    {
      return m_coPointMap[rad];
    }

    /// Set the co point map for the given radiator
    void setCoPointMap( const Rich::RadiatorType rad,
                        const Map & map )
    {
      m_coPointMap[rad] = map;
    }

    /// Returns the ck theta map for the given radiator
    const Map & ckThetaMap( const Rich::RadiatorType rad ) const
    {
      return m_ckThetaMap[rad];
    }

    /// Set the ck theta map for the given radiator
    void setCkThetaMap( const Rich::RadiatorType rad,
                        const Map & map )
    {
      m_ckThetaMap[rad] = map;
    }

  private:

    std::vector<Map> m_coPointMap;
    std::vector<Map> m_ckThetaMap;

  };

}

#endif // RICHMARKOVRINGFINDER_DEFAULTDATAMAPS_H
