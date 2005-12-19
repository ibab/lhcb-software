// $Id: XmlITDetectorCnv.cpp,v 1.1 2005-12-19 15:18:25 mneedham Exp $

// DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"

// TTDet
#include "STDet/DeITDetector.h"

typedef XmlUserDetElemCnv<DeITDetector>       XmlITDetectorCnv;
static CnvFactory<XmlITDetectorCnv>           s_XmlITDetectorCnvFactory ;
const ICnvFactory&  XmlITDetectorCnvFactory = s_XmlITDetectorCnvFactory ;
