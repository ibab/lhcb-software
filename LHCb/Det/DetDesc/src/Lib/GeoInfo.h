// $Id: GeoInfo.h,v 1.2 2003-04-25 08:52:24 sponce Exp $
/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $
/// ===========================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.1  2001/08/10 14:59:02  ibelyaev
/// modifications in IGeometryInfo and related classes
/// 
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
   */
  IGeometryInfo* 
  createGeometryInfo( IDetectorElement*  de              ,
                      const std::string& LogVol          , 
                      const std::string& Support         ,
                      const std::string& ReplicaNamePath );
  
  /** create regular geometry infor element 
   *  @exception GeometryInfoException null IDetectorElement pointer   
   *  @param de              pointer to detector element 
   *  @param LogVol          name of logical volume
   *  @param Support         name of support element 
   *  @param ReplicaPath     replica path 
   */
  IGeometryInfo* 
  createGeometryInfo( IDetectorElement*  de              ,
                      const std::string& LogVol          , 
                      const std::string& Support         ,
                      const ILVolume::ReplicaPath& ReplicaPath );

};

/// ===========================================================================
#endif ///< DETDESC_GEOINFO_H
/// ===========================================================================
