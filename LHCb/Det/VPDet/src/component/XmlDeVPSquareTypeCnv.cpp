#include "GaudiKernel/CnvFactory.h"
#include "DetDescCnv/XmlUserDetElemCnv.h"
#include "VPDet/DeVPSquareType.h"

/// =======================================================================
/// Instantiation of a static factory class used by clients to create
/// instances of this service
/// =======================================================================
typedef XmlUserDetElemCnv<DeVPSquareType> XmlDeVPSquareTypeCnv;
DECLARE_CONVERTER_FACTORY(XmlDeVPSquareTypeCnv)
