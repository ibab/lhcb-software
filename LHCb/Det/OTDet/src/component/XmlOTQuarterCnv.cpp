// $Id: XmlOTQuarterCnv.cpp,v 1.1 2003-12-04 10:22:09 jnardull Exp $

/// DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"
/// OTDet 
#include "OTDet/DeOTQuarter.h"

typedef XmlUserDetElemCnv<DeOTQuarter>       XmlOTQuarterCnv;
static CnvFactory<XmlOTQuarterCnv>           s_XmlOTQuarterCnvFactory ;
const ICnvFactory&  XmlOTQuarterCnvFactory = s_XmlOTQuarterCnvFactory ;
