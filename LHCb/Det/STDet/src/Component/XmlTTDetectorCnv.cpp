// $Id: XmlTTDetectorCnv.cpp,v 1.1 2005-12-19 15:18:25 mneedham Exp $

// DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"

// TTDet
#include "STDet/DeTTDetector.h"

typedef XmlUserDetElemCnv<DeTTDetector>       XmlTTDetectorCnv;
static CnvFactory<XmlTTDetectorCnv>           s_XmlTTDetectorCnvFactory ;
const ICnvFactory&  XmlTTDetectorCnvFactory = s_XmlTTDetectorCnvFactory ;
