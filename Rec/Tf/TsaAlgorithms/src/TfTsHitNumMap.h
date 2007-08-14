// $Id: TfTsHitNumMap.h,v 1.1.1.1 2007-08-14 13:50:47 jonrob Exp $
#ifndef TFTSHITNUMMAP_H 
#define TFTSHITNUMMAP_H 1

#include <map>
#include <vector>

/** @class TfTsHitNumMap TfTsHitNumMap.h
 *  
 *  Heinous class to provide a mapping between Tsa and Tf numbering schemes.
 *
 *  Ultimately this should go and Tsa use directly the Tf scheme.
 *
 *  @author Christopher Rob JONES
 *  @date   2007-07-05
 */

class TfTsHitNumMap 
{

public:

  typedef std::vector<int> TfRegions;
  typedef std::map< int, TfRegions > TsaToTfRegionMap;

public: 

  /// Standard constructor
  TfTsHitNumMap( ) 
  {
    // it
    m_itMap[0].push_back(0);
    m_itMap[0].push_back(1);
    m_itMap[1].push_back(2);
    m_itMap[2].push_back(3);
    m_itMap[3].push_back(4);
    m_itMap[4].push_back(5);
    // ot
    m_otMap[1].push_back(0);
    m_otMap[2].push_back(1);
  }

  ~TfTsHitNumMap( ) { }

  /// Get IT Tf regions(s) for a given Tsa region
  inline const TfRegions & tfITRegions( const int tsaRegion ) const
  {
    return m_itMap[tsaRegion];
  }

  /// Get OT Tf regions(s) for a given Tsa region
  inline const TfRegions & tfOTRegions( const int tsaRegion ) const
  {
    return m_otMap[tsaRegion];
  }

private:

  mutable TsaToTfRegionMap m_itMap;
  mutable TsaToTfRegionMap m_otMap;

};

#endif // TFTSHITNUMMAP_H
