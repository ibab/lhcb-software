
/// DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"
/// STDet 
#include "STDet/DeSTDetector.h"

typedef XmlUserDetElemCnv<DeSTDetector>       XmlSTDetectorCnv;
static CnvFactory<XmlSTDetectorCnv>           s_XmlSTDetectorCnvFactory ;
const ICnvFactory&  XmlSTDetectorCnvFactory = s_XmlSTDetectorCnvFactory ;
