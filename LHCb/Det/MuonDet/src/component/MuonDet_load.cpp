// $Id: MuonDet_load.cpp,v 1.1.1.1 2002-01-21 17:13:47 dhcroft Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
//
#define MUONDET_MUONDETLOAD_CPP 1 

//====================================================================
//  MuonDet_load.cpp
//--------------------------------------------------------------------
//
//  Package    : MuonDet
//
//  Description: Implementation of <Package>_load routine. This routine 
//               is needed for forcing the linker to load all the components
//               of the library.. 
//
//====================================================================

#include "GaudiKernel/ICnvFactory.h"
#include "GaudiKernel/ISvcFactory.h"
#include "GaudiKernel/IAlgFactory.h"

#include "GaudiKernel/ObjectFactory.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/ObjectList.h"

#define DLL_DECL_SERVICE(x)    extern const ISvcFactory& x##Factory; x##Factory.addRef();
#define DLL_DECL_CONVERTER(x)  extern const ICnvFactory& x##Factory; x##Factory.addRef();
#define DLL_DECL_ALGORITHM(x)  extern const IAlgFactory& x##Factory; x##Factory.addRef();


#include "MuonDet/DeMuonChamber.h"
_ImplementDataObjectFactory( DeMuonChamber ) ;

#include "MuonDet/DeMuonGasGap.h"
_ImplementDataObjectFactory( DeMuonGasGap ) ;

#include "MuonDet/DeMuonRegion.h"
_ImplementDataObjectFactory( DeMuonRegion ) ;

#include "MuonDet/MuonReadoutCond.h"
_ImplementDataObjectFactory( MuonReadoutCond ) ;

void MuonDet_load() 
{
  
  /// Converter factories 
  DLL_DECL_CONVERTER( XmlMuonRegionCnv );
  DLL_DECL_CONVERTER( XmlMuonReadoutCondCnv );
  
  /// Object factories 
  DLL_DECL_OBJECTFACTORY( DeMuonChamber ) ; 
  DLL_DECL_OBJECTFACTORY( DeMuonGasGap ) ; 
  DLL_DECL_OBJECTFACTORY( DeMuonRegion ) ; 
  DLL_DECL_OBJECTFACTORY( MuonReadoutCond ) ; 

};


extern "C" void MuonDet_loadRef()     {  MuonDet_load(); }
