#include "GaussCherenkov/RichPmtQE.h"
#include "G4ios.hh"

// initialize the QE vectors with 20 bins are adjust the
//size after getting the correct size from the db.
RichPmtQE::RichPmtQE() : 
  m_curPmtNum(0), 
  m_curRichDetNum(0),
  m_numPmtQEbins(20),
  m_PmtQEff(std::vector<double>(20)),
  m_PhotEnergyPmtQE(std::vector<double>(20)),
  m_WaveEnerCnv(1243.125) { }

RichPmtQE::RichPmtQE(const int pmtnum, const int richdetnum) :
  m_curPmtNum(pmtnum), 
  m_curRichDetNum(richdetnum),
  m_numPmtQEbins(20), 
  m_PmtQEff(std::vector<double>(20)),
  m_PhotEnergyPmtQE(std::vector<double>(20)), 
  m_WaveEnerCnv(1243.125) {

  //  m_numPmtQWRIbins(20),
  //  m_PmtQWRI(std::vector<double>(20)),
  //  m_PhotEnergyPmtQWRI(std::vector<double>(20)), 
  // m_numPmtPCRIbins(20), 
  //  m_PmtPCRI(std::vector<double>(20)),
  // m_PhotEnergyPmtPCRI(std::vector<double>(20)), 
 
}
RichPmtQE::~RichPmtQE() { ; }

double RichPmtQE::getPmtPhotonWaveLengthFromBin(int curmbin)
{
  double photwa(0);
  const double photener = getPmtPhotEnergyFromBin(curmbin);

  if ( photener > 0.0 ) {
    photwa = m_WaveEnerCnv/photener ;
  }else {
    G4cout << " RichPmtQE: Inadmissible photonenergy=  " << photener
           << "  Bin num =  " << curmbin << G4endl;
    photwa=0.0 ;
  }
  return photwa;
}

std::vector<double>RichPmtQE::PhotWavelengthPmtQE()
{
  std::vector<double> photwVect(m_numPmtQEbins);
  for (int ii=0 ; ii<m_numPmtQEbins; ++ii ){
    photwVect[ii] = getPmtPhotonWaveLengthFromBin(ii);
  }
  return photwVect;
}

double RichPmtQE::getPmtPhotonEnergyFromWavelen(double photonwlen )
{
  double photenergy;
  if(photonwlen > 0.0 ) {
    photenergy=m_WaveEnerCnv/photonwlen;
  }
  else
  {
    G4cout << " RichPmtQE: Inadmissible photonwavelength=  " << photonwlen
           << G4endl;
    photenergy=0.0;
  }
  return photenergy;
}

double RichPmtQE::getPmtPhotonWavelenFromEnergy(double  photonenerg ){
  double photonwlength;
  if( photonenerg > 0.0 ) {
    photonwlength=m_WaveEnerCnv/photonenerg;
  }else {

    G4cout <<" RichPmtQE: Inadmissible photonenergy=  "<<photonenerg<<G4endl;
    photonwlength=0.0;

  }
  return  photonwlength;
}

double  RichPmtQE::getPmtQEffFromPhotWavelen(double photwavelen) 
{
  const double photonener = getPmtPhotonEnergyFromWavelen( photwavelen);
  return getPmtQEffFromPhotEnergy(photonener);
}

double RichPmtQE::getPmtQEffFromPhotEnergy(double  photonenergy ){
  //if the energy is between 2 bins, interpolate.
  // assumes that energies are stored in increasing order in the class.
  // this means in the db they should be in the increasing order.

  double hq1,hq2, e1, e2,aslope,aintc;
  double qeff=0.0;

  for (int ibinq=0 ; ibinq<m_numPmtQEbins-1 ; ibinq++ ){
    e1 = m_PhotEnergyPmtQE[ibinq];
    e2 = m_PhotEnergyPmtQE[ibinq+1];
    if( photonenergy >= e1 &&  photonenergy <= e2 ) {
      hq1 =  m_PmtQEff[ibinq];
      hq2 =  m_PmtQEff[ibinq+1];
      if(e1 != e2 ){
        aslope = (hq2-hq1)/(e2-e1);
      }else{
        G4cout <<"RichPmtQE: Two Identical QE energy bins for same pmt, check db  "
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
  // qeff = getPmtCorrectedQEFromPhotEnergy(photonenergy,qeff);
  
  return qeff;
}

//additions by RWL 10.11.06



//double RichPmtQE::getPmtPCRIFromPhotEnergy(double  photonenergy ){
  //if the energy is between 2 bins, interpolate.
  // assumes that energies are stored in increasing order in the class.
  // this means in the db they should be in the increasing order.

//  double hq1,hq2, e1, e2,aslope,aintc;
//  double pcri=0.0;

//  for (int ibinq=0 ; ibinq<m_numPmtPCRIbins-1 ; ibinq++ ){
//    e1 = m_PhotEnergyPmtPCRI[ibinq];
//    e2 = m_PhotEnergyPmtPCRI[ibinq+1];
//    if( photonenergy >= e1 &&  photonenergy <= e2 ) {
//      hq1 =  m_PmtPCRI[ibinq];
//      hq2 =  m_PmtPCRI[ibinq+1];
//      if(e1 != e2 ){
//        aslope = (hq2-hq1)/(e2-e1);
//      }else{
//        G4cout <<"RichPmtPCRI: Two Identical PCRI energy bins for same pmt, check db  "
//               <<e1<<G4endl;
//        aslope=0.0;
//      }
//      aintc =  hq1 - (aslope * e1 );
//      pcri= aintc + aslope * photonenergy  ;
//    }
//  }
//  return pcri;
//}

//double RichPmtQE::getPmtQWRIFromPhotEnergy(double  photonenergy ){
  //if the energy is between 2 bins, interpolate.
  // assumes that energies are stored in increasing order in the class.
  // this means in the db they should be in the increasing order.

//  double hq1,hq2, e1, e2,aslope,aintc;
//  double qwri=0.0;

//  for (int ibinq=0 ; ibinq<m_numPmtQWRIbins-1 ; ibinq++ ){
//    e1 = m_PhotEnergyPmtQWRI[ibinq];
//    e2 = m_PhotEnergyPmtQWRI[ibinq+1];
//    if( photonenergy >= e1 &&  photonenergy <= e2 ) {
//      hq1 =  m_PmtQWRI[ibinq];
//      hq2 =  m_PmtQWRI[ibinq+1];
//      if(e1 != e2 ){
//        aslope = (hq2-hq1)/(e2-e1);
//      }else{
//        G4cout <<"RichPmtQWRI: Two Identical PCRI energy bins for same pmt, check db  "
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
//double RichPmtQE::getPmtCorrectedQEFromPhotEnergy(double photonenergy, double originalQE )
//{
  //  double thisPCRI=getPmtPCRIFromPhotEnergy( photonenergy );
  // double thisQWRI=getPmtQWRIFromPhotEnergy( photonenergy );
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
//      if(RichPmtPhCathMatName == ((*theMaterialTable)[imata]->GetName())) {
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
//      if( RichPmtQWMatName == ((*theMaterialTable)[imatb]->GetName())) {
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
    // done in FillPmtQETablesAtInit of RichPmtProperties. SE 26-10-2007
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


void RichPmtQE::setCurrentPmtQEwl(int ipmtnum, 
                                  int irichdetnum,
                                  int inumQEbins,
                                  const std::vector<double> & qeffvect,
                                  const std::vector<double> & qeffwavelen ) 
{
  m_curPmtNum = ipmtnum;
  m_curRichDetNum = irichdetnum;
  setAnPmtQEwl(inumQEbins,qeffvect,qeffwavelen);
}

void RichPmtQE::setAnPmtQEwl(int inumQEbins,
                             const std::vector<double> & qeffvect,
                             const std::vector<double> & qeffwavelen) {

  m_numPmtQEbins= inumQEbins;
  if ( m_numPmtQEbins != (int) m_PmtQEff.size() ) m_PmtQEff.resize(m_numPmtQEbins);
  if ( m_numPmtQEbins != (int) m_PhotEnergyPmtQE.size() ) m_PhotEnergyPmtQE.resize(m_numPmtQEbins);
  m_PmtQEff= qeffvect;
  for (int ij=0; ij<m_numPmtQEbins; ++ij) {
    m_PhotEnergyPmtQE[ij]=getPmtPhotonEnergyFromWavelen(qeffwavelen[ij]);
  }

}

void RichPmtQE::setCurrentPmtQEen(int jpmtnum, int jrichdetnum,
                                  int jnumQEbins, 
                                  const std::vector<double> & jqeffvect,
                                  const std::vector<double> & qeffener) {
  //   RichPmtQElog << MSG::INFO
  //  <<"set pmt QEen: pmtnum detnum numbins "<<jpmtnum<<"  "<<jrichdetnum
  //    <<"   "<<jnumQEbins<<endreq;
  m_curPmtNum= jpmtnum;
  m_curRichDetNum=jrichdetnum;
  setAnPmtQEen(jnumQEbins,jqeffvect,qeffener);

}

void RichPmtQE::setAnPmtQEen(int jnumQEbins, 
                             const std::vector<double> & jqeffvect,
                             const std::vector<double> & qeffener) {

  //   RichPmtQElog << MSG::INFO
  //   <<"set pmt QEen: NumQEbins "<<jnumQEbins<<endreq;

  m_numPmtQEbins= jnumQEbins;
  if(m_numPmtQEbins != (int) m_PmtQEff.size())m_PmtQEff.resize(m_numPmtQEbins);
  if(m_numPmtQEbins != (int) m_PhotEnergyPmtQE.size())
    m_PhotEnergyPmtQE.resize(m_numPmtQEbins);

  m_PmtQEff = jqeffvect;
  m_PhotEnergyPmtQE =qeffener;

}


//added setting functions by RWL 10.11.06

//void RichPmtQE::setCurrentPmtPCRIen(int jpmtnum, int jrichdetnum,
//                                  int jnumPCRIbins, 
//                                  const std::vector<double> & aPCRIvect,
//                                  const std::vector<double> & aPCRIener) {
  //   RichPmtQElog << MSG::INFO
  //  <<"set pmt QEen: pmtnum detnum numbins "<<jpmtnum<<"  "<<jrichdetnum
  //    <<"   "<<jnumQEbins<<endreq;
//  m_curPmtNum= jpmtnum;
//  m_curRichDetNum=jrichdetnum;
//  setAnPmtPCRIen(jnumPCRIbins,aPCRIvect,aPCRIener);
//
//}

//void RichPmtQE::setAnPmtPCRIen(int jnumPCRIbins, 
//                             const std::vector<double> & aPCRIvect,
//                             const std::vector<double> & aPCRIener) {

  //   RichPmtQElog << MSG::INFO
  //   <<"set pmt QEen: NumQEbins "<<jnumQEbins<<endreq;

//  m_numPmtPCRIbins= jnumPCRIbins;
//  if(m_numPmtPCRIbins != (int) m_PmtPCRI.size())m_PmtPCRI.resize(m_numPmtPCRIbins);
//  if(m_numPmtPCRIbins != (int) m_PhotEnergyPmtPCRI.size())
//    m_PhotEnergyPmtPCRI.resize(m_numPmtPCRIbins);
//
//  m_PmtPCRI = aPCRIvect;
//  m_PhotEnergyPmtPCRI =aPCRIener;
//
//}

//

//void RichPmtQE::setCurrentPmtQWRIen(int jpmtnum, int jrichdetnum,
//                                  int jnumQWRIbins, 
//                                  const std::vector<double> & aQWRIvect,
//                                  const std::vector<double> & aQWRIener) {
  //   RichPmtQElog << MSG::INFO
  //  <<"set pmt QEen: pmtnum detnum numbins "<<jpmtnum<<"  "<<jrichdetnum
  //    <<"   "<<jnumQEbins<<endreq;
//  m_curPmtNum= jpmtnum;
//  m_curRichDetNum=jrichdetnum;
//  setAnPmtQWRIen(jnumQWRIbins,aQWRIvect,aQWRIener);
//
//}

//void RichPmtQE::setAnPmtQWRIen(int jnumQWRIbins, 
//                             const std::vector<double> & aQWRIvect,
//                             const std::vector<double> & aQWRIener) {

  //   RichPmtQElog << MSG::INFO
  //   <<"set pmtQEen: NumQEbins "<<jnumQEbins<<endreq;

//  m_numPmtQWRIbins= jnumQWRIbins;
//  if(m_numPmtQWRIbins != (int) m_PmtQWRI.size())m_PmtQWRI.resize(m_numPmtQWRIbins);
//  if(m_numPmtQWRIbins != (int) m_PhotEnergyPmtQWRI.size())
//    m_PhotEnergyPmtQWRI.resize(m_numPmtQWRIbins);
//
//  m_PmtQWRI = aQWRIvect;
//  m_PhotEnergyPmtQWRI =aQWRIener;
//
//}

//end of change RWL

