// $Id: XmlOTDetectorCnv.cpp,v 1.2 2003-06-16 13:40:41 sponce Exp $

/// DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"
/// OTDet 
#include "OTDet/DeOTDetector.h"

typedef XmlUserDetElemCnv<DeOTDetector>       XmlOTDetectorCnv;
static CnvFactory<XmlOTDetectorCnv>           s_XmlOTDetectorCnvFactory ;
const ICnvFactory&  XmlOTDetectorCnvFactory = s_XmlOTDetectorCnvFactory ;
