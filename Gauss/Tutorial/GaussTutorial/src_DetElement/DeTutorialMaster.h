// $Id: $
#ifndef DETUTORIALMASTER_H 
#define DETUTORIALMASTER_H 1

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

/** @class DeTutorialMaster DeTutorialMaster.h
 *  
 *
 *  @author Sajan Easo
 *  @date   2012-02-13
 */

static const CLID CLID_DETutorialMaster= 12095;



class DeTutorialMaster: public DetectorElement {
public: 
  /// Standard constructor
  DeTutorialMaster(const std::string & name = "" ); 

  virtual ~DeTutorialMaster( ); ///< Destructor

  static const CLID& classID()  {  return CLID_DETutorialMaster;}
  virtual const CLID& clID    () const  {return classID();}
  

 virtual StatusCode initialize();
 void FindGlobalPoint (const double dx, const double dy, const double dz );
  
protected:

private:

  std::string m_TutorialDet;
  

};
#endif // DETUTORIALMASTER_H
