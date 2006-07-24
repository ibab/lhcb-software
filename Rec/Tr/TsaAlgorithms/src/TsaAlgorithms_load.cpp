// $Id: TsaAlgorithms_load.cpp,v 1.1.1.1 2006-07-24 14:56:45 mneedham Exp $

// GaudiKernel 

/** @file Tsa_load.cpp 
 * 
 *  Mandatory file to IMPLEMENT and DECLARE all factories for 
 *  all generic relation tables and associator tools 
 *
 *  @author M.Needham
 *  @date   25/04/2004 
 */

// GaudiKernel
#include "GaudiKernel/DeclareFactoryEntries.h"


DECLARE_FACTORY_ENTRIES( TsaAlgorithms ) {

  DECLARE_ALGORITHM( TsaSTClusterCreator );
  DECLARE_ALGORITHM( TsaOTClusterCreator );
  DECLARE_ALGORITHM( TsaInitialization );
  DECLARE_ALGORITHM( TsaSpacePointCreator );
  DECLARE_ALGORITHM( TsaToTrackCnv );
  DECLARE_ALGORITHM( TsaSeed );
  DECLARE_ALGORITHM( TsaSeedTrackCnv );

  DECLARE_TOOL( ITDataSvc );
  DECLARE_TOOL( OTDataSvc );
  DECLARE_TOOL( ITFaultsCalculator );
  DECLARE_TOOL( OTExpectedHits );
  DECLARE_TOOL( ITExpectedHits );
  DECLARE_TOOL( TsaFollow );  

 

};
