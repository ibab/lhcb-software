// $Id: XmlOTDetectorCnv.cpp,v 1.1.1.1 2002-05-26 22:36:33 jvantilb Exp $

/// DetDesc
#include "DetDesc/XmlUserDetElemCnv.h"
/// OTDet 
#include "OTDet/DeOTDetector.h"

typedef XmlUserDetElemCnv<DeOTDetector>       XmlOTDetectorCnv;
static CnvFactory<XmlOTDetectorCnv>           s_XmlOTDetectorCnvFactory ;
const ICnvFactory&  XmlOTDetectorCnvFactory = s_XmlOTDetectorCnvFactory ;
