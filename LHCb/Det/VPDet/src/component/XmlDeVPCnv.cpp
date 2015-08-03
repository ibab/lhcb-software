#include "DetDescCnv/XmlUserDetElemCnv.h"
#include "VPDet/DeVP.h"

/// =======================================================================
/// Instantiation of a static factory class used by clients to create
/// instances of this service
/// =======================================================================
typedef XmlUserDetElemCnv<DeVP> XmlDeVPCnv;
DECLARE_CONVERTER_FACTORY(XmlDeVPCnv)
