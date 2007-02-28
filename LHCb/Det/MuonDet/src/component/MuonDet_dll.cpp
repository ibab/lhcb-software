// $Id: MuonDet_dll.cpp,v 1.4 2007-02-28 18:33:18 marcocle Exp $

#include "GaudiKernel/LoadFactoryEntries.h"

LOAD_FACTORY_ENTRIES(MuonDet)

// Factories for converters for classes in MuonDetLib

#include "DetDescCnv/XmlUserConditionCnv.h"
#include "DetDescCnv/XmlUserDetElemCnv.h"


#include "MuonDet/MuonChamberGrid.h"
typedef XmlUserConditionCnv<MuonChamberGrid> XmlMuonGridCnv;
DECLARE_CONVERTER_FACTORY(XmlMuonGridCnv)

#include "MuonDet/DeMuonChamber.h"
typedef XmlUserDetElemCnv<DeMuonChamber> XmlDeMuonChamber;
DECLARE_CONVERTER_FACTORY(XmlDeMuonChamber)

#include "MuonDet/DeMuonGasGap.h"
typedef XmlUserDetElemCnv<DeMuonGasGap> XmlDeMuonGasGap;
DECLARE_CONVERTER_FACTORY(XmlDeMuonGasGap)

#include "MuonDet/DeMuonRegion.h"
typedef XmlUserDetElemCnv<DeMuonRegion> XmlDeMuonRegion;
DECLARE_CONVERTER_FACTORY(XmlDeMuonRegion)

#include "MuonDet/DeMuonDetector.h"
typedef XmlUserDetElemCnv<DeMuonDetector> XmlDeMuonDetector;
DECLARE_CONVERTER_FACTORY(XmlDeMuonDetector)
