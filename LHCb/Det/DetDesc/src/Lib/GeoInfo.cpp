// $Id: GeoInfo.cpp,v 1.3 2005-06-03 10:19:44 jpalac Exp $
/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.2  2003/04/25 08:52:24  sponce
/// fixes after valgrind tests, application of code conventions
///
/// Revision 1.1  2001/08/10 14:59:02  ibelyaev
/// modifications in IGeometryInfo and related classes
/// 
/// ===========================================================================
/// DetDesc 
#include "DetDesc/IGeometryInfo.h"
/// Local 
#include "GeoInfo.h"
#include "GeometryInfo.h"
#include "GeometryInfoPlus.h"

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
