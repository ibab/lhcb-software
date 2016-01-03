// DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"

// UTDet
#include "STDet/DeUTDetector.h"

typedef XmlUserDetElemCnv<DeUTDetector>       XmlUTDetectorCnv;
DECLARE_CONVERTER_FACTORY(XmlUTDetectorCnv)
