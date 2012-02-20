// $Id: $
#ifndef EXERCISE3SOLUTION_DETUTORIALPIXELCONTAINER_H 
#define EXERCISE3SOLUTION_DETUTORIALPIXELCONTAINER_H 1

// Include files

#include <iostream>
#include <vector>
#include "DetDesc/DetectorElement.h"
#include "DetDesc/IGeometryInfo.h"
#include "GaudiKernel/Transform3DTypes.h"
#include "GaudiKernel/Plane3DTypes.h"
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/MsgStream.h"

class MsgStream        ;

static const CLID CLID_DETutorialPixelContainer= 12096;


/** @class DeTutorialPixelContainer DeTutorialPixelContainer.h Exercise3Solution/DeTutorialPixelContainer.h
 *  
 *
 *  @author Sajan Easo
 *  @date   2012-02-13
 */
class DeTutorialPixelContainer: public  DetectorElement  {
public: 
  /// Standard constructor
  DeTutorialPixelContainer(const std::string & name = ""  ); 

  virtual ~DeTutorialPixelContainer( ); ///< Destructor
  static const CLID& classID()  {  return CLID_DETutorialPixelContainer;}
  virtual const CLID& clID    () const  {return classID();}
  virtual StatusCode initialize();
  void FindGlobalPointFromPixelContainer (const double dx, const double dy, const double dz );
  
protected:

private:

 std::string m_TutorialDetPC;

};
#endif // EXERCISE3SOLUTION_DETUTORIALPIXELCONTAINER_H
