// $Id: $
// Include files

// local
#include "RichG4ScintAnalysis.h"
#include "GaussRICH/RichG4SvcLocator.h"
#include "G4Track.hh"
#include "G4Step.hh"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IValidity.h"
#include "GaudiKernel/Time.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/Bootstrap.h"

#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichG4ScintAnalysis
//
// 2010-04-08 : Sajan EASO
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================



void RichG4ScintFillHisto (const G4double Energy, const G4int NumPhot, const G4Step aStep ){


    IHistogramSvc* CurrentHistoSvc=RichG4SvcLocator::RichG4HistoSvc();

    G4ThreeVector xb=  aStep.GetPreStepPoint()->GetPosition();
    G4ThreeVector xe = aStep.GetPostStepPoint()->GetPosition();
    G4cout<<"G4Cint analysis xb se "<<xb<<"  "<<xe<<G4endl;
    G4cout<<"G4Cint analysis energy numphot per step "<< Energy<<"  "
          <<NumPhot<<G4endl;
    

    G4double dx = xe.x() - xb.x();
    G4double dy = xe.y() - xb.y();
    G4double dz = xe.z() - xb.z();
    G4double distsq = (dx*dx + dy*dy +dz*dz);
    
    G4double distance = distsq > 0.0 ? pow( distsq ,0.5) : 0.0;
    G4cout<<" distance in step "<<distance <<G4endl;
    
    if(distance > 50.0 ) { // histo only for long steps of 50 mm or above.
      G4double EUnit = Energy*1000.0 / distance;  // 1000 is to convert from cm to meters
      G4double NumPh = NumPhot*1000.0/ distance;  // 1000 is to convert from cm to meters
      G4cout<<" Eunit numph "<<EUnit<<"  "<<NumPh<<G4endl;
      
      if( EUnit > 0.0) {


        
        SmartDataPtr<IHistogram1D>m_hEnergyLossInCF4 (CurrentHistoSvc,
                   "RICHG4HISTOSET1/901");
        if( m_hEnergyLossInCF4  ) m_hEnergyLossInCF4->fill(EUnit*1.0, 1.0);
      }
      if(NumPh>0.0) {


        SmartDataPtr<IHistogram1D>m_hNumScintPhotProdInCF4 (CurrentHistoSvc,
                   "RICHG4HISTOSET1/910");
      if(m_hNumScintPhotProdInCF4) m_hNumScintPhotProdInCF4->fill(NumPh*1.0, 1.0);
    
      
      }
      
    
    
      
      
    }
    


    
    

  
  G4cout<<" End of filling histo for CF4 "<<G4endl;
  
}


