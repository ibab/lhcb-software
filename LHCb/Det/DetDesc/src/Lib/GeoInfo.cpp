// $Id: GeoInfo.cpp,v 1.6 2009-04-17 08:54:24 cattanem Exp $
/// ===========================================================================
/// DetDesc 
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/GeoInfo.h"
#include "DetDesc/GeometryInfoPlus.h"

// ============================================================================
/** @file GeoInfo.cpp
 *
 * Implementation of methods from namespace GeoInfo
 *
 * @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 * @date 10/08/2001 
 */
// ============================================================================

// ============================================================================
/** create "ghost" geometry info object
 *  @exception GeometryInfoException null IDetectorElement pointer   
 *  @param de pointer to detector element 
 *  @return pointer to IGeometryInfo object
 */
// ============================================================================
IGeometryInfo* 
GeoInfo::createGeometryInfo( IDetectorElement* de )
{ return new GeometryInfoPlus( de ); }

// ============================================================================
/** create "orphan" geometry info object
   *  @exception GeometryInfoException null IDetectorElement pointer   
   *  @param de pointer to detector element 
   *  @param LV name/address fo logical volume 
   *  @return pointer to IGeometryInfo object
   */
// ============================================================================
IGeometryInfo* 
GeoInfo::createGeometryInfo( IDetectorElement* de  ,
                             const std::string& LV )
{ return new GeometryInfoPlus( de , LV ); }

// ============================================================================
/** create regular geometry infor element 
 *  @exception GeometryInfoException null IDetectorElement pointer   
 *  @param de              pointer to detector element 
 *  @param LogVol          name of logical volume
 *  @param Support         name of support element 
 *  @param ReplicaNamePath replica path/address 
 *  @param alignmentPath   address of alignment condition
 */
// ============================================================================
IGeometryInfo* 
GeoInfo::createGeometryInfo( IDetectorElement*  de              ,
                             const std::string& LogVol          , 
                             const std::string& Support         ,
                             const std::string& ReplicaNamePath ,
                             const std::string& alignmentPath)
{ 
  return new GeometryInfoPlus( de,
                               LogVol,
                               Support,
                               ReplicaNamePath,
                               alignmentPath); 

}

// ============================================================================
/** create regular geometry infor element 
 *  @exception GeometryInfoException null IDetectorElement pointer   
 *  @param de              pointer to detector element 
 *  @param LogVol          name of logical volume
 *  @param Support         name of support element 
 *  @param ReplicaPath     replica path 
 *  @param alignmentPath   address of alignment condition
 */
// ============================================================================
IGeometryInfo* 
GeoInfo::createGeometryInfo( IDetectorElement*  de              ,
                             const std::string& LogVol          , 
                             const std::string& Support         ,
                             const ILVolume::ReplicaPath& ReplicaPath,
                             const std::string& alignmentPath)
{ 
  return new GeometryInfoPlus( de,
                               LogVol,
                               Support,
                               ReplicaPath,
                               alignmentPath);
  
}

// ============================================================================
