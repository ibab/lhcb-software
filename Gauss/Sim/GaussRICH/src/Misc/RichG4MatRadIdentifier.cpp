// $Id: RichG4MatRadIdentifier.cpp,v 1.1 2006-02-10 09:34:47 seaso Exp $
// Include files 



// local
#include "RichG4MatRadIdentifier.h"
#include "RichG4RadiatorMaterialIdValues.h" 
#include "RichG4GaussPathNames.h"
// from G4
#include "G4Material.hh"
#include <utility>
//-----------------------------------------------------------------------------
// Implementation file for class : RichG4MatRadIdentifier
//
// 2006-02-02 : Sajan EASO
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichG4MatRadIdentifier* RichG4MatRadIdentifier::m_RichG4MatRadIdentifierInstance=0;

RichG4MatRadIdentifier::RichG4MatRadIdentifier(  ) 
  : m_RichNumRadMat(0)
{
  

  InitializeRichG4MatRadIdentifier();
}
//=============================================================================
// Destructor
//=============================================================================
RichG4MatRadIdentifier::~RichG4MatRadIdentifier() {} 

//=============================================================================
RichG4MatRadIdentifier* RichG4MatRadIdentifier::RichG4MatRadIdentifierInstance() 
{
  
  if( m_RichG4MatRadIdentifierInstance == 0) {
    m_RichG4MatRadIdentifierInstance = new   RichG4MatRadIdentifier();   
  }
  
  return  m_RichG4MatRadIdentifierInstance;
  
}
void RichG4MatRadIdentifier::InitializeRichG4MatRadIdentifier() 
{

  G4int n_mat=0;
  m_RichG4MatIndexRadNum.clear();      
  static const G4MaterialTable* theMaterialTable =
    G4Material::GetMaterialTable();
  G4int numberOfMat=  theMaterialTable->size() ;
  for(G4int iMat=0;iMat<numberOfMat;++iMat) {
    //    G4cout<<"G4mat name list "<<(*theMaterialTable)[iMat]->GetName()<<G4endl;
    G4String curMatName =  (*theMaterialTable)[iMat]->GetName();
    G4int curMatIndex = (*theMaterialTable)[iMat]->GetIndex();
  
    if(curMatName == AgelMaterialName ) {
      n_mat++;
      m_RichG4MatIndexRadNum[curMatIndex] = Rich1AgelGenericCkvRadiatorNum;

    }else if(curMatName == C4F10MaterialName || curMatName == C3F8MaterialName ) {
      n_mat++;
      m_RichG4MatIndexRadNum[curMatIndex] =  Rich1C4F10CkvRadiatorNum;
    
    }else if(curMatName == CF4MaterialName) {
      n_mat++;
      m_RichG4MatIndexRadNum[curMatIndex] = Rich2CF4CkvRadiatorNum;
      
    }else if(curMatName ==   FilterGenericMaterialName ) {
      n_mat++;
      m_RichG4MatIndexRadNum[curMatIndex] = RichFilterGenericCkvRadiatorNum;
    }else if (curMatName == FilterD263MaterialName ) {
      n_mat++;
      m_RichG4MatIndexRadNum[curMatIndex] = RichFilterD263CkvRadiatorNum;
    }else if (curMatName == Rich1GasQWindowMaterialName ) {
      n_mat++;
      m_RichG4MatIndexRadNum[curMatIndex] = Rich1GasQWindowCkvRadiatorNum;
      
    }else if (curMatName == Rich2GasQWindowMaterialName ) {
      n_mat++;
      m_RichG4MatIndexRadNum[curMatIndex] = Rich2GasQWindowCkvRadiatorNum;
      
    }else if (curMatName == RichHpdQuartzWindowMaterialName ) {
      n_mat++;
      m_RichG4MatIndexRadNum[curMatIndex] = RichHpdQuartzWindowCkvRadiatorNum;
      
    }else if (curMatName == AgelTile0MaterialName ) {
      n_mat++;
      m_RichG4MatIndexRadNum[curMatIndex] = Rich1AgelTile0CkvRadiatorNum;
    }else if (curMatName == AgelTile1MaterialName ) {
      n_mat++;
      m_RichG4MatIndexRadNum[curMatIndex] = Rich1AgelTile1CkvRadiatorNum ;
    }else if (curMatName == AgelTile2MaterialName ) {
      n_mat++;

      m_RichG4MatIndexRadNum[curMatIndex] = Rich1AgelTile2CkvRadiatorNum ;

    }else if (curMatName == AgelTile3MaterialName ) {
      n_mat++;
      m_RichG4MatIndexRadNum[curMatIndex] = Rich1AgelTile3CkvRadiatorNum ;
    }else if (curMatName == AgelTile4MaterialName ) {
      n_mat++;
      m_RichG4MatIndexRadNum[curMatIndex] = Rich1AgelTile4CkvRadiatorNum ;
    }else if (curMatName == AgelTile5MaterialName ) {
      n_mat++;
      m_RichG4MatIndexRadNum[curMatIndex] = Rich1AgelTile5CkvRadiatorNum ;
     }else if (curMatName == AgelTile6MaterialName ) {
      n_mat++;
      m_RichG4MatIndexRadNum[curMatIndex] = Rich1AgelTile6CkvRadiatorNum ;
    }else if (curMatName == AgelTile7MaterialName ) {
      n_mat++;
      m_RichG4MatIndexRadNum[curMatIndex] = Rich1AgelTile7CkvRadiatorNum ;
     }else if (curMatName == AgelTile8MaterialName ) {
      n_mat++;
      m_RichG4MatIndexRadNum[curMatIndex] = Rich1AgelTile8CkvRadiatorNum ;
    }else if (curMatName == AgelTile9MaterialName ) {
      n_mat++;
      m_RichG4MatIndexRadNum[curMatIndex] = Rich1AgelTile9CkvRadiatorNum ;
    }else if (curMatName == AgelTile10MaterialName ) {
      n_mat++;
      m_RichG4MatIndexRadNum[curMatIndex] = Rich1AgelTile10CkvRadiatorNum ;
    }else if (curMatName == AgelTile11MaterialName ) {
      n_mat++;
      m_RichG4MatIndexRadNum[curMatIndex] = Rich1AgelTile11CkvRadiatorNum ;
    }else if (curMatName == AgelTile12MaterialName ) {
      n_mat++;
      m_RichG4MatIndexRadNum[curMatIndex] = Rich1AgelTile12CkvRadiatorNum ;
    }else if (curMatName == AgelTile13MaterialName ) {
      n_mat++;
      m_RichG4MatIndexRadNum[curMatIndex] = Rich1AgelTile13CkvRadiatorNum ;
    }else if (curMatName == AgelTile14MaterialName ) {
      n_mat++;
      m_RichG4MatIndexRadNum[curMatIndex] = Rich1AgelTile14CkvRadiatorNum ;
    }else if (curMatName == AgelTile15MaterialName ) {
      n_mat++;
      m_RichG4MatIndexRadNum[curMatIndex] = Rich1AgelTile15CkvRadiatorNum ;    
     }else if (curMatName == Rich1NitrogenMaterialName ) {
      n_mat++;
      m_RichG4MatIndexRadNum[curMatIndex] = Rich1NitrogenCkvRadiatorNum;
     }else if (curMatName == Rich2NitrogenMaterialName ) {
      n_mat++;
      m_RichG4MatIndexRadNum[curMatIndex] = Rich2NitrogenCkvRadiatorNum;      
     }
    
    
  }
  m_RichNumRadMat = n_mat;
  
}

void RichG4MatRadIdentifier::PrintRadMatIndex () 
{
   // test printing
  G4cout <<" Number of Rich rad materials "<< m_RichNumRadMat<<G4endl;
  
  typedef std::map<G4int,G4int>::const_iterator CI;
  for(CI p=m_RichG4MatIndexRadNum.begin(); p!= m_RichG4MatIndexRadNum.end(); ++p ) {
    G4cout<<"RichMatRadIdenfier "<< p->first<<"    "<<p->second<<G4endl;
    
  }
 
  
}
void RichG4MatRadIdentifier::PrintRadNumPhotStepLim() {
  // test printing
  G4cout <<" Number of Rich rad materials "<< m_RichNumRadMat<<G4endl;
  typedef std::map<G4int,G4int>::const_iterator CI;
  for (CI p= m_RichG4CkvMaxPhotPerStep.begin() ; p!=m_RichG4CkvMaxPhotPerStep.end(); ++p ) {
    G4cout<<"RichNumPhotStepLim "<<p->first<<"    "<<p->second<<G4endl;

  }  


}

G4int RichG4MatRadIdentifier::getRadiatorNumForG4MatIndex(G4int cIndex ) 
{
  // start with a dummy unphysical value and check for consistency.
  G4int aRn=-2;
  std::map<G4int,G4int>::iterator p = m_RichG4MatIndexRadNum.find(cIndex);
  if(p != m_RichG4MatIndexRadNum.end() ) aRn = p->second;

  // alternative way commented out.
  //    std::map<G4int,G4int>::const_iterator p = m_RichG4MatIndexRadNum.begin();
  //   G4bool foundRadMat = false;
  //  while ( ( p != m_RichG4MatIndexRadNum.end()) && (!(foundRadMat)) ) {
  //    G4cout<< " testaccess map "<<cIndex<<"  "<< p->first<<"  "<<p->second <<G4endl;
  //    
  //    if( (p->first) == cIndex ) { 
  //      aRn= p->second;
  //      foundRadMat =true;
  //    }
  //    ++p;
  //  }
  //  std::cout<<"RichG4MatRadIdentifier Index radnum "<<cIndex <<"  "<< aRn <<std::endl;
    return aRn;
}

void RichG4MatRadIdentifier::InitializeRichCkvMatMaxNumPhot(std::vector<G4int>  aMaxPhotLim  )
{
  G4int aAgelLim =  900;
  G4int aC4f10Lim = 40;  
  G4int aCf4Lim   =  40;  
  G4int aQuartzLim = 5000;
  
 
  if( (G4int) aMaxPhotLim.size() >= 4 ) {
    
    aAgelLim =   aMaxPhotLim[0];
    aC4f10Lim =  aMaxPhotLim[1];
    aCf4Lim   =  aMaxPhotLim[2];
    aQuartzLim = aMaxPhotLim[3];
  
  }

  m_RichG4CkvMaxPhotPerStep.clear();
  
  static const G4MaterialTable* theMaterialTable =
    G4Material::GetMaterialTable();
  G4int numberOfMat=  theMaterialTable->size() ;
  for(G4int iMat=0;iMat<numberOfMat;++iMat) {
    //    G4cout<<"G4mat name list "<<(*theMaterialTable)[iMat]->GetName()<<G4endl;
    G4String curMatName =  (*theMaterialTable)[iMat]->GetName();
    G4int curMatIndex = (*theMaterialTable)[iMat]->GetIndex();
    if(curMatName == AgelMaterialName ) {
      
      m_RichG4CkvMaxPhotPerStep[curMatIndex] =  aAgelLim;
    }  else if ( (curMatName == AgelTile0MaterialName) ||                 
                 (curMatName == AgelTile1MaterialName) ||  
                 (curMatName == AgelTile2MaterialName) ||  
                 (curMatName == AgelTile3MaterialName) ||  
                 (curMatName == AgelTile4MaterialName) ||  
                 (curMatName == AgelTile5MaterialName) ||  
                 (curMatName == AgelTile6MaterialName) ||  
                 (curMatName == AgelTile7MaterialName) ||  
                 (curMatName == AgelTile8MaterialName) ||  
                 (curMatName == AgelTile9MaterialName) ||  
                 (curMatName == AgelTile10MaterialName) ||  
                 (curMatName == AgelTile11MaterialName) ||  
                 (curMatName == AgelTile12MaterialName) ||  
                 (curMatName == AgelTile13MaterialName) ||  
                 (curMatName == AgelTile14MaterialName) ||  
                 (curMatName == AgelTile15MaterialName) ) {
             m_RichG4CkvMaxPhotPerStep[curMatIndex] =  aAgelLim;
    }else if ( curMatName == C4F10MaterialName || curMatName == C3F8MaterialName ) {
             m_RichG4CkvMaxPhotPerStep[curMatIndex] = aC4f10Lim;


    }else if ( curMatName ==  CF4MaterialName ) {
             m_RichG4CkvMaxPhotPerStep[curMatIndex] = aCf4Lim ;

    }else if (( curMatName == FilterGenericMaterialName ) ||
              ( curMatName == FilterD263MaterialName ) ||
              ( curMatName == Rich1GasQWindowMaterialName ) ||
              ( curMatName ==  Rich2GasQWindowMaterialName) ||
              ( curMatName == RichHpdQuartzWindowMaterialName) ) {
             m_RichG4CkvMaxPhotPerStep[curMatIndex] = aQuartzLim ;

    } else if ( ( curMatName ==  Rich1NitrogenMaterialName) ||
                ( curMatName ==  Rich2NitrogenMaterialName)) {
             m_RichG4CkvMaxPhotPerStep[curMatIndex] = aC4f10Lim;
    }

   
  }

}

G4int RichG4MatRadIdentifier::getRadiatorPhotPerStepLimit(G4int kIndex ) {
  G4int aLn =-2;
  std::map<G4int,G4int>::iterator p = m_RichG4CkvMaxPhotPerStep.find(kIndex);
  if(p != m_RichG4CkvMaxPhotPerStep.end() ) aLn = p->second;
  return aLn;
} 
