/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $ 
/// ===========================================================================
/// DetDesc 
#include "DetDesc/IGeometryInfo.h"
/// Local 
#include "GeoInfo.h"
#include "GeometryInfo.h"

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
{ return new GeometryInfo( de ); }

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
{ return new GeometryInfo( de , LV ); }

// ============================================================================
/** create regular geometry infor element 
 *  @exception GeometryInfoException null IDetectorElement pointer   
 *  @param de              pointer to detector element 
 *  @param LogVol          name of logical volume
 *  @param Support         name of support element 
 *  @param ReplicaNamePath replica path/address 
 */
// ============================================================================
IGeometryInfo* 
GeoInfo::createGeometryInfo( IDetectorElement*  de              ,
                             const std::string& LogVol          , 
                             const std::string& Support         ,
                             const std::string& ReplicaNamePath )
{ return new GeometryInfo( de , LogVol , Support , ReplicaNamePath ); }

// ============================================================================
/** create regular geometry infor element 
 *  @exception GeometryInfoException null IDetectorElement pointer   
 *  @param de              pointer to detector element 
 *  @param LogVol          name of logical volume
 *  @param Support         name of support element 
 *  @param ReplicaPath     replica path 
 */
// ============================================================================
IGeometryInfo* 
GeoInfo::createGeometryInfo( IDetectorElement*  de              ,
                             const std::string& LogVol          , 
                             const std::string& Support         ,
                             const ILVolume::ReplicaPath& ReplicaPath )
{ return new GeometryInfo( de , LogVol , Support , ReplicaPath ); }

// ============================================================================
