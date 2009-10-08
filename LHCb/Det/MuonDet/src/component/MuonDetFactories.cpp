// Factories for converters for classes in MuonDetLib
#include "DetDescCnv/XmlUserConditionCnv.h"
#include "DetDescCnv/XmlUserDetElemCnv.h"

#include "MuonDet/MuonChamberGrid.h"
typedef XmlUserConditionCnv<MuonChamberGrid> XmlMuonGridCnv;

#include "MuonDet/DeMuonChamber.h"
typedef XmlUserDetElemCnv<DeMuonChamber> XmlDeMuonChamber;


#include "MuonDet/DeMuonRegion.h"
typedef XmlUserDetElemCnv<DeMuonRegion> XmlDeMuonRegion;

#include "MuonDet/DeMuonDetector.h"
typedef XmlUserDetElemCnv<DeMuonDetector> XmlDeMuonDetector;

DECLARE_CONVERTER_FACTORY(XmlMuonGridCnv)
DECLARE_CONVERTER_FACTORY(XmlDeMuonChamber)
DECLARE_CONVERTER_FACTORY(XmlDeMuonRegion)
DECLARE_CONVERTER_FACTORY(XmlDeMuonDetector)
