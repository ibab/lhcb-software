#include "GaussRICH/RichHpdQE.h"
#include "G4ios.hh"

// initialize the QE vectors with 20 bins are adjust the
//size after getting the correct size from the db.
RichHpdQE::RichHpdQE() : 
  m_curHpdNum(0), 
  m_curRichDetNum(0),
  m_numHpdQEbins(20),
  m_HpdQEff(std::vector<double>(20)),
  m_PhotEnergyHpdQE(std::vector<double>(20)),
  m_WaveEnerCnv(1243.125) { }

RichHpdQE::RichHpdQE(const int hpdnum, const int richdetnum) :
  m_curHpdNum(hpdnum), 
  m_curRichDetNum(richdetnum),
  m_numHpdQEbins(20), 
  m_HpdQEff(std::vector<double>(20)),
  m_PhotEnergyHpdQE(std::vector<double>(20)), 
  m_WaveEnerCnv(1243.125) {

  //  m_numHpdQWRIbins(20),
  //  m_HpdQWRI(std::vector<double>(20)),
  //  m_PhotEnergyHpdQWRI(std::vector<double>(20)), 
  // m_numHpdPCRIbins(20), 
  //  m_HpdPCRI(std::vector<double>(20)),
  // m_PhotEnergyHpdPCRI(std::vector<double>(20)), 
 
}
RichHpdQE::~RichHpdQE() { ; }

double RichHpdQE::getHpdPhotonWaveLengthFromBin(int curmbin)
{
  double photwa(0);
  const double photener = getHpdPhotEnergyFromBin(curmbin);

  if ( photener > 0.0 ) {
    photwa = m_WaveEnerCnv/photener ;
  }else {
    G4cout << " RichHpdQE: Inadmissible photonenergy=  " << photener
           << "  Bin num =  " << curmbin << G4endl;
    photwa=0.0 ;
  }
  return photwa;
}

std::vector<double>RichHpdQE::PhotWavelengthHpdQE()
{
  std::vector<double> photwVect(m_numHpdQEbins);
  for (int ii=0 ; ii<m_numHpdQEbins; ++ii ){
    photwVect[ii] = getHpdPhotonWaveLengthFromBin(ii);
  }
  return photwVect;
}

double RichHpdQE::getHpdPhotonEnergyFromWavelen(double photonwlen )
{
  double photenergy;
  if(photonwlen > 0.0 ) {
    photenergy=m_WaveEnerCnv/photonwlen;
  }
  else
  {
    G4cout << " RichHpdQE: Inadmissible photonwavelength=  " << photonwlen
           << G4endl;
    photenergy=0.0;
  }
  return photenergy;
}

double RichHpdQE::getHpdPhotonWavelenFromEnergy(double  photonenerg ){
  double photonwlength;
  if( photonenerg > 0.0 ) {
    photonwlength=m_WaveEnerCnv/photonenerg;
  }else {

    G4cout <<" RichHpdQE: Inadmissible photonenergy=  "<<photonenerg<<G4endl;
    photonwlength=0.0;

  }
  return  photonwlength;
}

double  RichHpdQE::getHpdQEffFromPhotWavelen(double photwavelen) 
{
  const double photonener = getHpdPhotonEnergyFromWavelen( photwavelen);
  return getHpdQEffFromPhotEnergy(photonener);
}

double RichHpdQE::getHpdQEffFromPhotEnergy(double  photonenergy ){
  //if the energy is between 2 bins, interpolate.
  // assumes that energies are stored in increasing order in the class.
  // this means in the db they should be in the increasing order.

  double hq1,hq2, e1, e2,aslope,aintc;
  double qeff=0.0;

  for (int ibinq=0 ; ibinq<m_numHpdQEbins-1 ; ibinq++ ){
    e1 = m_PhotEnergyHpdQE[ibinq];
    e2 = m_PhotEnergyHpdQE[ibinq+1];
    if( photonenergy >= e1 &&  photonenergy <= e2 ) {
      hq1 =  m_HpdQEff[ibinq];
      hq2 =  m_HpdQEff[ibinq+1];
      if(e1 != e2 ){
        aslope = (hq2-hq1)/(e2-e1);
      }else{
        G4cout <<"RichHpdQE: Two Identical QE energy bins for same hpd, check db  "
               <<e1<<G4endl;
        aslope=0.0;
      }
      aintc =  hq1 - (aslope * e1 );
      qeff= aintc + aslope * photonenergy  ;
    }
  }
  //RWL 11.10.06
  //correct QE for reflectivity at Air-QW, QW-PC and PC-Air surfaces
  // this correction is already applied when reading the QE table. Hence
  // it is commented out here to avoid duplicate corrections. SE 26-10-2007
  // this is to save cpu time.
  // qeff = getHpdCorrectedQEFromPhotEnergy(photonenergy,qeff);
  
  return qeff;
}

//additions by RWL 10.11.06



//double RichHpdQE::getHpdPCRIFromPhotEnergy(double  photonenergy ){
  //if the energy is between 2 bins, interpolate.
  // assumes that energies are stored in increasing order in the class.
  // this means in the db they should be in the increasing order.

//  double hq1,hq2, e1, e2,aslope,aintc;
//  double pcri=0.0;

//  for (int ibinq=0 ; ibinq<m_numHpdPCRIbins-1 ; ibinq++ ){
//    e1 = m_PhotEnergyHpdPCRI[ibinq];
//    e2 = m_PhotEnergyHpdPCRI[ibinq+1];
//    if( photonenergy >= e1 &&  photonenergy <= e2 ) {
//      hq1 =  m_HpdPCRI[ibinq];
//      hq2 =  m_HpdPCRI[ibinq+1];
//      if(e1 != e2 ){
//        aslope = (hq2-hq1)/(e2-e1);
//      }else{
//        G4cout <<"RichHpdPCRI: Two Identical PCRI energy bins for same hpd, check db  "
//               <<e1<<G4endl;
//        aslope=0.0;
//      }
//      aintc =  hq1 - (aslope * e1 );
//      pcri= aintc + aslope * photonenergy  ;
//    }
//  }
//  return pcri;
//}

//double RichHpdQE::getHpdQWRIFromPhotEnergy(double  photonenergy ){
  //if the energy is between 2 bins, interpolate.
  // assumes that energies are stored in increasing order in the class.
  // this means in the db they should be in the increasing order.

//  double hq1,hq2, e1, e2,aslope,aintc;
//  double qwri=0.0;

//  for (int ibinq=0 ; ibinq<m_numHpdQWRIbins-1 ; ibinq++ ){
//    e1 = m_PhotEnergyHpdQWRI[ibinq];
//    e2 = m_PhotEnergyHpdQWRI[ibinq+1];
//    if( photonenergy >= e1 &&  photonenergy <= e2 ) {
//      hq1 =  m_HpdQWRI[ibinq];
//      hq2 =  m_HpdQWRI[ibinq+1];
//      if(e1 != e2 ){
//        aslope = (hq2-hq1)/(e2-e1);
//      }else{
//        G4cout <<"RichHpdQWRI: Two Identical PCRI energy bins for same hpd, check db  "
//               <<e1<<G4endl;
//        aslope=0.0;
//      }
//      aintc =  hq1 - (aslope * e1 );
//      qwri= aintc + aslope * photonenergy  ;
//    }
//  }
//  return qwri;
//}
//
//double RichHpdQE::getHpdCorrectedQEFromPhotEnergy(double photonenergy, double originalQE )
//{
  //  double thisPCRI=getHpdPCRIFromPhotEnergy( photonenergy );
  // double thisQWRI=getHpdQWRIFromPhotEnergy( photonenergy );
  // the ref index is directly accessed through the G4 methods since
  // it is the property of the material bulk. G4 automatically does the interpolation
  // efficiently , of this property and hence no need to have our own direct access and interpolation.
  // Also this correction is applied to the QE before the event loop starts, to save cpu time.
  // SE 26-10-2007


//    double thisPCRI=1.0;
//    double thisQWRI=1.0;
//    static const G4MaterialTable* theMaterialTable = G4Material::GetMaterialTable();
//    G4int imata=0; G4bool matafound=false; 
//    G4int imatb=0; G4bool matbfound=false;
//    
//    while( (imata<  (G4int) theMaterialTable.size()) &&  (! matafound) ) {
//
//      if(RichHpdPhCathMatName == ((*theMaterialTable)[imata]->GetName())) {
//        G4Material* aMatPC = (*theMaterialTable)[imata];
//        G4MaterialPropertyVector* RindexPC=
//                      aMatPC->GetMaterialPropertiesTable()->GetProperty("RINDEX");
//          if(   RindexPC ) {
//            thisPCRI= RindexPC->GetProperty(photonenergy);
//          } 
//          imatafound=true;    
//      }      
//      imata++;      
//    }    
//
//    while( (imatb<  (G4int) theMaterialTable.size()) &&  (! matbfound) ) {
//      if( RichHpdQWMatName == ((*theMaterialTable)[imatb]->GetName())) {
//        G4Material* aMatQW = (*theMaterialTable)[imatb];
//        G4MaterialPropertyVector* RindexQW=
//                      aMatQW->GetMaterialPropertiesTable()->GetProperty("RINDEX");
//        if(   RindexQW ) {
//            thisQWRI= RindexQW->GetProperty(photonenergy);
//        } 
//
//        matbfound=true;        
//      }      
//      imatb++;
//      
//    }
//
    // the division by 100 from percent to real probability is 
    // done in FillHpdQETablesAtInit of RichHpdProperties. SE 26-10-2007
    //  double maxQEguess=1.0;
    // if(originalQE > 1.0) maxQEguess=100.0; //QE by now should be in real probablitity, this will catch the error if it isn't
  
    //originalQE = originalQE/maxQEguess;
  //  double thisQWrefl=0.0;
  // double thisPCrefl=0.0;
    // end of modif by SE

//  G4double thisQWrefl=(thisQWRI-1)/(thisQWRI+1);
//  thisQWrefl=thisQWrefl*thisQWrefl;
//  
//  G4double thisPCrefl=(thisPCRI-thisQWRI)/(thisPCRI+thisQWRI);
//  thisPCrefl=thisPCrefl*thisPCrefl;
  
  //double thisTrans=1.0;
  
//  G4double thisTrans= (1+thisQWrefl*thisPCrefl)*(1-thisQWrefl)*(1-thisPCrefl);

  //double thiscorrQE=1.0;
  
//  G4double thiscorrQE=(originalQE/(thisTrans)); //first order
//  thiscorrQE=(thiscorrQE/                      //second order
//                       (1+(1-thiscorrQE)*(thisPCRI-1)*(thisPCRI-1)
//      		                         /((thisPCRI+1)*(thisPCRI+1))));

//  return thiscorrQE;

//}

//end additions


void RichHpdQE::setCurrentHpdQEwl(int ihpdnum, 
                                  int irichdetnum,
                                  int inumQEbins,
                                  const std::vector<double> & qeffvect,
                                  const std::vector<double> & qeffwavelen ) 
{
  m_curHpdNum = ihpdnum;
  m_curRichDetNum = irichdetnum;
  setAnHpdQEwl(inumQEbins,qeffvect,qeffwavelen);
}

void RichHpdQE::setAnHpdQEwl(int inumQEbins,
                             const std::vector<double> & qeffvect,
                             const std::vector<double> & qeffwavelen) {

  m_numHpdQEbins= inumQEbins;
  if ( m_numHpdQEbins != (int) m_HpdQEff.size() ) m_HpdQEff.resize(m_numHpdQEbins);
  if ( m_numHpdQEbins != (int) m_PhotEnergyHpdQE.size() ) m_PhotEnergyHpdQE.resize(m_numHpdQEbins);
  m_HpdQEff= qeffvect;
  for (int ij=0; ij<m_numHpdQEbins; ++ij) {
    m_PhotEnergyHpdQE[ij]=getHpdPhotonEnergyFromWavelen(qeffwavelen[ij]);
  }

}

void RichHpdQE::setCurrentHpdQEen(int jhpdnum, int jrichdetnum,
                                  int jnumQEbins, 
                                  const std::vector<double> & jqeffvect,
                                  const std::vector<double> & qeffener) {
  //   RichHpdQElog << MSG::INFO
  //  <<"set hpd QEen: hpdnum detnum numbins "<<jhpdnum<<"  "<<jrichdetnum
  //    <<"   "<<jnumQEbins<<endreq;
  m_curHpdNum= jhpdnum;
  m_curRichDetNum=jrichdetnum;
  setAnHpdQEen(jnumQEbins,jqeffvect,qeffener);

}

void RichHpdQE::setAnHpdQEen(int jnumQEbins, 
                             const std::vector<double> & jqeffvect,
                             const std::vector<double> & qeffener) {

  //   RichHpdQElog << MSG::INFO
  //   <<"set hpd QEen: NumQEbins "<<jnumQEbins<<endreq;

  m_numHpdQEbins= jnumQEbins;
  if(m_numHpdQEbins != (int) m_HpdQEff.size())m_HpdQEff.resize(m_numHpdQEbins);
  if(m_numHpdQEbins != (int) m_PhotEnergyHpdQE.size())
    m_PhotEnergyHpdQE.resize(m_numHpdQEbins);

  m_HpdQEff = jqeffvect;
  m_PhotEnergyHpdQE =qeffener;

}


//added setting functions by RWL 10.11.06

//void RichHpdQE::setCurrentHpdPCRIen(int jhpdnum, int jrichdetnum,
//                                  int jnumPCRIbins, 
//                                  const std::vector<double> & aPCRIvect,
//                                  const std::vector<double> & aPCRIener) {
  //   RichHpdQElog << MSG::INFO
  //  <<"set hpd QEen: hpdnum detnum numbins "<<jhpdnum<<"  "<<jrichdetnum
  //    <<"   "<<jnumQEbins<<endreq;
//  m_curHpdNum= jhpdnum;
//  m_curRichDetNum=jrichdetnum;
//  setAnHpdPCRIen(jnumPCRIbins,aPCRIvect,aPCRIener);
//
//}

//void RichHpdQE::setAnHpdPCRIen(int jnumPCRIbins, 
//                             const std::vector<double> & aPCRIvect,
//                             const std::vector<double> & aPCRIener) {

  //   RichHpdQElog << MSG::INFO
  //   <<"set hpd QEen: NumQEbins "<<jnumQEbins<<endreq;

//  m_numHpdPCRIbins= jnumPCRIbins;
//  if(m_numHpdPCRIbins != (int) m_HpdPCRI.size())m_HpdPCRI.resize(m_numHpdPCRIbins);
//  if(m_numHpdPCRIbins != (int) m_PhotEnergyHpdPCRI.size())
//    m_PhotEnergyHpdPCRI.resize(m_numHpdPCRIbins);
//
//  m_HpdPCRI = aPCRIvect;
//  m_PhotEnergyHpdPCRI =aPCRIener;
//
//}

//

//void RichHpdQE::setCurrentHpdQWRIen(int jhpdnum, int jrichdetnum,
//                                  int jnumQWRIbins, 
//                                  const std::vector<double> & aQWRIvect,
//                                  const std::vector<double> & aQWRIener) {
  //   RichHpdQElog << MSG::INFO
  //  <<"set hpd QEen: hpdnum detnum numbins "<<jhpdnum<<"  "<<jrichdetnum
  //    <<"   "<<jnumQEbins<<endreq;
//  m_curHpdNum= jhpdnum;
//  m_curRichDetNum=jrichdetnum;
//  setAnHpdQWRIen(jnumQWRIbins,aQWRIvect,aQWRIener);
//
//}

//void RichHpdQE::setAnHpdQWRIen(int jnumQWRIbins, 
//                             const std::vector<double> & aQWRIvect,
//                             const std::vector<double> & aQWRIener) {

  //   RichHpdQElog << MSG::INFO
  //   <<"set hpd QEen: NumQEbins "<<jnumQEbins<<endreq;

//  m_numHpdQWRIbins= jnumQWRIbins;
//  if(m_numHpdQWRIbins != (int) m_HpdQWRI.size())m_HpdQWRI.resize(m_numHpdQWRIbins);
//  if(m_numHpdQWRIbins != (int) m_PhotEnergyHpdQWRI.size())
//    m_PhotEnergyHpdQWRI.resize(m_numHpdQWRIbins);
//
//  m_HpdQWRI = aQWRIvect;
//  m_PhotEnergyHpdQWRI =aQWRIener;
//
//}

//end of change RWL

