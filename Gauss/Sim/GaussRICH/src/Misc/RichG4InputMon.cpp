// $Id: RichG4InputMon.cpp,v 1.2 2006-02-10 09:36:04 seaso Exp $
// Include files 



// local
#include "GaussRICH/RichG4InputMon.h"
#include "GaussRICH/RichG4AnalysisConstGauss.h"
#include "GaussRICH/RichG4GaussPathNames.h"
#include "G4Material.hh"
#include <vector>
 

//-----------------------------------------------------------------------------
// Implementation file for class : RichG4InputMon
//
// 2005-08-19 : Sajan EASO
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichG4InputMon::RichG4InputMon( ):m_FirstMonInstance(true)
 { }
//=============================================================================
// Destructor
//=============================================================================
RichG4InputMon::~RichG4InputMon() {}; 
void RichG4InputMon::MonitorRich1GasRadiatorRefIndex() 
{
  if(m_FirstMonInstance) {
    
     const G4MaterialTable* theMaterialTable =
                         G4Material::GetMaterialTable();
     
     G4int numberOfMat= theMaterialTable->size() ;
     for(G4int iMat=0;iMat<numberOfMat;++iMat) {
//   G4cout<<"G4mat name list "<<(*theMaterialTable)[iMat]->GetName()<<G4endl;
       if( ( C4F10MaterialName == (*theMaterialTable)[iMat]->GetName()) || 
            ( CF4MaterialName == (*theMaterialTable)[iMat]->GetName()) || 
            ( C3F8MaterialName == (*theMaterialTable)[iMat]->GetName()) || 
            ( R2RadGasMaterialName == (*theMaterialTable)[iMat]->GetName() ) ||
            ( R1RadGasMaterialName  == (*theMaterialTable)[iMat]->GetName()) ){
         G4cout<<"Current Material Name for refindex verification "<<(*theMaterialTable)[iMat]->GetName()
               <<G4endl;
         
         G4MaterialPropertiesTable* aMatPropTable = 
           (*theMaterialTable)[iMat]->  GetMaterialPropertiesTable();
         aMatPropTable->DumpTable();
         G4cout<<"End of  refindex verification for Material "<<(*theMaterialTable)[iMat]->GetName()
               <<G4endl;
 
      }
       
       
     }
     
    
     
  }
  
  
}

//=============================================================================
