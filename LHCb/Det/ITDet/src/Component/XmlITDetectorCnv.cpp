
/// DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"
/// ITDet 
#include "ITDet/DeITDetector.h"

typedef XmlUserDetElemCnv<DeITDetector>       XmlITDetectorCnv;
static CnvFactory<XmlITDetectorCnv>           s_XmlITDetectorCnvFactory ;
const ICnvFactory&  XmlITDetectorCnvFactory = s_XmlITDetectorCnvFactory ;
