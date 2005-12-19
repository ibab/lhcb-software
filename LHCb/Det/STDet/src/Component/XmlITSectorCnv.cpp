// $Id: XmlITSectorCnv.cpp,v 1.1 2005-12-19 15:18:25 mneedham Exp $

// DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"

// TTDet
#include "STDet/DeITSector.h"

typedef XmlUserDetElemCnv<DeITSector >        XmlITSectorCnv;
static CnvFactory<XmlITSectorCnv>           s_XmlITSectorCnvFactory ;
const ICnvFactory&  XmlITSectorCnvFactory = s_XmlITSectorCnvFactory ;
