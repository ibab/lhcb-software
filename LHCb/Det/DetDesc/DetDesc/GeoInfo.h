// $Id: GeoInfo.h,v 1.1 2006-04-20 14:39:55 ranjard Exp $
/// ===========================================================================
#ifndef DETDESC_GEOINFO_H 
#define DETDESC_GEOINFO_H 1
/// STD/STL 
#include <string>
/// DetDesc 
#include "DetDesc/ILVolume.h"
/// forward declarations
class IDetectorElement;
class IGeometryInfo;

/** @namespace GeoInfo GeoInfo.h
 *  
 *  collection of "creational" methods for 
 *  creation of IGeometryInfo objects 
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   10/08/2001
 */

namespace GeoInfo 
{
  
  /** create "ghost" geometry info object
   *  @exception GeometryInfoException null IDetectorElement pointer   
   *  @param de pointer to detector element 
   *  @return pointer to IGeometryInfo object
   */
  IGeometryInfo* 
  createGeometryInfo( IDetectorElement* de );
  
  /** create "orphan" geometry info object
   *  @exception GeometryInfoException null IDetectorElement pointer   
   *  @param de pointer to detector element 
   *  @param LV name/address fo logical volume 
   *  @return pointer to IGeometryInfo object
   */
  IGeometryInfo* 
  createGeometryInfo( IDetectorElement* de  ,
                      const std::string& LV );

  /** create regular geometry infor element 
   *  @exception GeometryInfoException null IDetectorElement pointer   
   *  @param de              pointer to detector element 
   *  @param LogVol          name of logical volume
   *  @param Support         name of support element 
   *  @param ReplicaNamePath replica path/address 
   *  @param alignmentPath   address of alignment condition
   */
  IGeometryInfo* 
  createGeometryInfo( IDetectorElement*  de              ,
                      const std::string& LogVol          , 
                      const std::string& Support         ,
                      const std::string& ReplicaNamePath ,
                      const std::string& alignmentPath="");
  
  /** create regular geometry infor element 
   *  @exception GeometryInfoException null IDetectorElement pointer   
   *  @param de              pointer to detector element 
   *  @param LogVol          name of logical volume
   *  @param Support         name of support element 
   *  @param ReplicaPath     replica path 
   *  @param alignmentPath   address of alignment condition
   */
  IGeometryInfo* 
  createGeometryInfo( IDetectorElement*  de              ,
                      const std::string& LogVol          , 
                      const std::string& Support         ,
                      const ILVolume::ReplicaPath& ReplicaPath,
                      const std::string& alignmentPath="");



}

/// ===========================================================================
#endif ///< DETDESC_GEOINFO_H
/// ===========================================================================
