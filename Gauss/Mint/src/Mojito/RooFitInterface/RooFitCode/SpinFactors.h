// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:11 GMT
#ifndef SPINFACTOR_S
#define SPINFACTOR_S
#include "TObject.h"
#include "Rtypes.h"

#include <complex>
#include <iostream>

#include "RooRealProxy.h"
#include "SpinFactor.h"
#include "SpinFactorThreeBody.h"


class SF_DtoVP : public SpinFactorThreeBody {
  
public:
  SF_DtoVP(  const char* name
             , const char* title
             , RooAbsReal& _MV
             , RooAbsReal& _mAB
             , RooAbsReal& _mAC
             , RooAbsReal& _mBC
             , RooAbsReal& _mA
             , RooAbsReal& _mB
             , RooAbsReal& _mC
             , RooAbsReal& _mD
             )
    : SpinFactorThreeBody(name, title, _MV,_mAB, _mAC, _mBC,
                 _mA,_mB,_mC,_mD)
  {}

  SF_DtoVP(const SF_DtoVP& other
	 , const char* name=0)
    : SpinFactorThreeBody(other, name)
  {}

  virtual TObject* clone(const char* newname) const{
    return new SF_DtoVP(*this, newname);}
  virtual TObject* clone() const{
    return new SF_DtoVP(*this);}
  inline virtual ~SF_DtoVP(){}

  virtual Double_t evaluate() const 
  {
    return mAC - mBC + (mD*mD - mC*mC)*(mB*mB-mA*mA)/(MV*MV);
    //return mBC - mAC + (mD*mD - mC*mC)*(mA*mA-mB*mB)/(MV*MV);
  }
 protected:

  //ClassDef(SF_DtoVP, 0)
  
    };

class SF_DtoTP : public SpinFactorThreeBody {
  
public:
  SF_DtoTP(  const char* name
             , const char* title
             , RooAbsReal& _MV
             , RooAbsReal& _mAB
             , RooAbsReal& _mAC
             , RooAbsReal& _mBC
             , RooAbsReal& _mA
             , RooAbsReal& _mB
             , RooAbsReal& _mC
             , RooAbsReal& _mD
             )
    : SpinFactorThreeBody(name, title, _MV,_mAB, _mAC, _mBC,
                 _mA,_mB,_mC,_mD)
  {}

  SF_DtoTP(const SF_DtoTP& other
	 , const char* name=0)
    : SpinFactorThreeBody(other, name)
  {}

  virtual TObject* clone(const char* newname) const{
    return new SF_DtoTP(*this, newname);}
  virtual TObject* clone() const{
    return new SF_DtoTP(*this);}
  inline virtual ~SF_DtoTP(){}

  virtual Double_t evaluate() const 
  {
    Double_t term1 = mBC-mAC+(mD*mD-mC*mC)*(mA*mA-mB*mB)/(MV*MV);
    Double_t term2 = mAB-2.0*mD*mD-2.0*mC*mC+pow(mD*mD-mC*mC,2)/(MV*MV);
    Double_t term3 = mAB-2.0*mA*mA-2.0*mB*mB+pow(mA*mA-mB*mB,2)/(MV*MV);

    return pow(term1,2)-(1.0/3.0)*term2*term3;
    
  }
 protected:

  //ClassDef(SF_DtoTP, 0)
  
    };


class SF_DtoPP1_PtoVP2_VtoP3P4 : public SpinFactor{

 public:
  SF_DtoPP1_PtoVP2_VtoP3P4(const char* name
	     , const char* title
	     , RooAbsReal& _MV
	     , int _p1_i
	     , int _p2_i
	     , int _p3_i
	     , int _p4_i
	     , RooAbsReal& _t01
	     , RooAbsReal& _s12
	     , RooAbsReal& _s23
	     , RooAbsReal& _s34
	     , RooAbsReal& _t40
	     , RooAbsReal& _m0
	     , RooAbsReal& _m1
	     , RooAbsReal& _m2
	     , RooAbsReal& _m3
	     , RooAbsReal& _m4
	 )
    : SpinFactor( name, title
		  , _p1_i, _p2_i, _p3_i, _p4_i
		  , _t01, _s12, _s23, _s34, _t40
		  , _m0, _m1, _m2, _m3, _m4)
    , MV("MV", "MV", this, _MV)
    {}
  
  SF_DtoPP1_PtoVP2_VtoP3P4(const SF_DtoPP1_PtoVP2_VtoP3P4& other
	 , const char* name=0)
    : SpinFactor(other, name)
    , MV("MV", this, other.MV)
    {}

 public:
  virtual TObject* clone(const char* newname) const{
    return new SF_DtoPP1_PtoVP2_VtoP3P4(*this, newname);}
  virtual TObject* clone() const{
    return new SF_DtoPP1_PtoVP2_VtoP3P4(*this);}
  inline virtual ~SF_DtoPP1_PtoVP2_VtoP3P4(){}


  
  virtual Double_t evaluateSlow() const{
    if(! getMomenta()) return -9999;
    TLorentzVector pV = p3 + p4;
    TLorentzVector qV = p3 - p4;
 
    return p2.Dot(qV)
      -    p2.Dot(pV) * pV.Dot(qV) / (MV*MV)
      ;
  }
  virtual Double_t evaluate() const{
    getFastMomenta();
    /*
      p2qV = p2 (p3 - p4)
           = 23 -24

      p2pV = 23 + 24
      pVqV = 33 - 44

    */
    Double_t p2qV = p23 - p24;
    Double_t p2pV = p23 + p24;
    Double_t pVqV = p33 - p44;

    Double_t returnVal
      = p2qV
      - p2pV * pVqV / (MV*MV);

    if(db){
      if(fabs(returnVal - evaluateSlow()) > 1.e-10){
	std::cout << "compare: " << GetName()
		  << ": " << returnVal << ", " << evaluateSlow()
		  << std::endl;
      }
    }
    return returnVal;
  }

 protected:
  RooRealProxy MV;
  //ClassDef(SF_DtoPP1_PtoVP2_VtoP3P4, 0)
};

class SF_DtoAP1_AtoVP2_VtoP3P4 : public SpinFactor{

 public:
  SF_DtoAP1_AtoVP2_VtoP3P4(const char* name
	     , const char* title
	     , RooAbsReal& _MA
	     , RooAbsReal& _MV
	     , int _p1_i
	     , int _p2_i
	     , int _p3_i
	     , int _p4_i
	     , RooAbsReal& _t01
	     , RooAbsReal& _s12
	     , RooAbsReal& _s23
	     , RooAbsReal& _s34
	     , RooAbsReal& _t40
	     , RooAbsReal& _m0
	     , RooAbsReal& _m1
	     , RooAbsReal& _m2
	     , RooAbsReal& _m3
	     , RooAbsReal& _m4
	 )
    : SpinFactor( name, title
		  , _p1_i, _p2_i, _p3_i, _p4_i
		  , _t01, _s12, _s23, _s34, _t40
		  , _m0, _m1, _m2, _m3, _m4)
    , MA("MA", "MA", this, _MA)
    , MV("MV", "MV", this, _MV)
    {}
  
  SF_DtoAP1_AtoVP2_VtoP3P4(const SF_DtoAP1_AtoVP2_VtoP3P4& other
	 , const char* name=0)
    : SpinFactor(other, name)
    , MA("MA", this, other.MA)
    , MV("MV", this, other.MV)
    {}

 public:
  virtual TObject* clone(const char* newname) const{
    return new SF_DtoAP1_AtoVP2_VtoP3P4(*this, newname);}
  virtual TObject* clone() const{
    return new SF_DtoAP1_AtoVP2_VtoP3P4(*this);}
  inline virtual ~SF_DtoAP1_AtoVP2_VtoP3P4(){}
  
  virtual Double_t evaluateSlow() const{
    if(! getMomenta()) return -9999;
    TLorentzVector pV = p3 + p4;
    TLorentzVector qV = p3 - p4;
    TLorentzVector pA = p2 + p3 + p4;

    return p1.Dot(qV)
      -    p1.Dot(pA) * pA.Dot(qV) / (MA*MA)
      -    p1.Dot(pV) * pV.Dot(qV) / (MV*MV)
      +    p1.Dot(pA) * pA.Dot(pV) * pV.Dot(qV) / (MA*MA * MV*MV)
      ;
  }
  virtual Double_t evaluate() const{
    getFastMomenta();
    /*
      p1qV = p1(p3-p4) = p13 -p14

      p1pA = p1(p2 + p3 + p4) = p12 + p13 + p14
      pAqV = (2+3+4)(3-4)
           = 23 + 33 + 34 - 24 -34 -44
           = 23 + 33      - 24     -44
       
      p1pV = 1(3+4)
           = 13 + 14;
      pVqV = 33-44
      
      p1pA = above
      pApV = (2 + 3 + 4)(3+4)
           = 23 + 24 + 33 + 34 + 34 + 44
           = 23 + 24 + 33 + 2*34 + 44;
      pVqV = above

    */

    Double_t p1qV = p13 - p14;

    Double_t p1pA = p12 + p13 + p14;
    Double_t pAqV = p23 + p33 - p24 - p44;

    Double_t p1pV = p13 + p14;
    Double_t pVqV = p33 - p44;
    
    Double_t pApV = p23 + p24 + p33 + 2*p34 + p44;

    Double_t returnVal 
      = p1qV
      - p1pA * pAqV / (MA*MA)
      - p1pV * pVqV / (MV*MV)
      + p1pA * pApV *pVqV / (MA*MA * MV*MV)
      ;

    if(db){
      if(fabs(returnVal - evaluateSlow()) > 1.e-10){
	std::cout << "compare: " << GetName()
		  << ": " << returnVal << ", " << evaluateSlow()
		  << std::endl;
      }
    }
    return returnVal;
  }


 protected:
  RooRealProxy MA, MV;
  //ClassDef(SF_DtoAP1_AtoVP2_VtoP3P4, 0)
};

class SF_DtoAP1_AtoSP2_StoP3P4 : public SpinFactor{

 public:
  SF_DtoAP1_AtoSP2_StoP3P4(const char* name
	     , const char* title
	     , RooAbsReal& _MA
	     , int _p1_i
	     , int _p2_i
	     , int _p3_i
	     , int _p4_i
	     , RooAbsReal& _t01
	     , RooAbsReal& _s12
	     , RooAbsReal& _s23
	     , RooAbsReal& _s34
	     , RooAbsReal& _t40
	     , RooAbsReal& _m0
	     , RooAbsReal& _m1
	     , RooAbsReal& _m2
	     , RooAbsReal& _m3
	     , RooAbsReal& _m4
	 )
    : SpinFactor( name, title
		  , _p1_i, _p2_i, _p3_i, _p4_i
		  , _t01, _s12, _s23, _s34, _t40
		  , _m0, _m1, _m2, _m3, _m4)
    , MA("MA", "MA", this, _MA)
    {}
  
  SF_DtoAP1_AtoSP2_StoP3P4(const SF_DtoAP1_AtoSP2_StoP3P4& other
	 , const char* name=0)
    : SpinFactor(other, name)
    , MA("MA", this, other.MA)
    {}

 public:
  virtual TObject* clone(const char* newname) const{
    return new SF_DtoAP1_AtoSP2_StoP3P4(*this, newname);}
  virtual TObject* clone() const{
    return new SF_DtoAP1_AtoSP2_StoP3P4(*this);}
  inline virtual ~SF_DtoAP1_AtoSP2_StoP3P4(){}
  
  virtual Double_t evaluateSlow() const{
    if(! getMomenta()) return -9999;
    TLorentzVector pA = p2 + p3 + p4;
    TLorentzVector qA = (p3 + p4) - p2;
 
    return p1.Dot(qA)
      -    p1.Dot(pA) * pA.Dot(qA) / (MA*MA)
      ;
  }
  virtual Double_t evaluate() const{
    getFastMomenta();
    /*
      p1qA = p1*(p3 + p4 - p2)
           = 13 + 14 - 12

      p1pA = p1*(p3 + p4 + p2)
           = 13 + 14 + 12
      pAqA = (p3+p4 - p2)(p3+p4 + p2)
           = (p3+p4)^2 - p22
           = p33 + p44 + 2p34 - p22

    */

   Double_t p1qA = p13 + p14 - p12;
   Double_t p1pA = p13 + p14 + p12;
   Double_t pAqA = p33 + p44 + 2*p34 - p22;

   Double_t returnVal = p1qA
     -                  p1pA*pAqA / (MA*MA);

   if(db){
     if(fabs(returnVal - evaluateSlow()) > 1.e-10){
       
       std::cout << "compare: " << GetName()
		 << ": " << returnVal << ", " << evaluateSlow()
		 << std::endl;
     }
   }
   return -returnVal; // obacht! spin fuckup!
   
  }

 protected:
  RooRealProxy MA;
  //ClassDef(SF_DtoAP1_AtoSP2_StoP3P4, 0)
};


class SF_DtoV1V2_V1toP1P2_V2toP3P4_S : public SpinFactor{

 public:
  SF_DtoV1V2_V1toP1P2_V2toP3P4_S(const char* name
				 , const char* title
				 , RooAbsReal& _MV1
				 , RooAbsReal& _MV2
				 , int _p1_i
				 , int _p2_i
				 , int _p3_i
				 , int _p4_i
				 , RooAbsReal& _t01
				 , RooAbsReal& _s12
				 , RooAbsReal& _s23
				 , RooAbsReal& _s34
				 , RooAbsReal& _t40
				 , RooAbsReal& _m0
				 , RooAbsReal& _m1
				 , RooAbsReal& _m2
				 , RooAbsReal& _m3
				 , RooAbsReal& _m4
				 )
    : SpinFactor( name, title
		  , _p1_i, _p2_i, _p3_i, _p4_i
		  , _t01, _s12, _s23, _s34, _t40
		  , _m0, _m1, _m2, _m3, _m4)
    , MV1("MV1", "MV1", this, _MV1)
    , MV2("MV2", "MV2", this, _MV2)
    {}
  
  SF_DtoV1V2_V1toP1P2_V2toP3P4_S(const SF_DtoV1V2_V1toP1P2_V2toP3P4_S& other
	 , const char* name=0)
    : SpinFactor(other, name)
    , MV1("MV1", this, other.MV1)
    , MV2("MV2", this, other.MV2)
    {}

 public:
  virtual TObject* clone(const char* newname) const{
    return new SF_DtoV1V2_V1toP1P2_V2toP3P4_S(*this, newname);}
  virtual TObject* clone() const{
    return new SF_DtoV1V2_V1toP1P2_V2toP3P4_S(*this);}
  inline virtual ~SF_DtoV1V2_V1toP1P2_V2toP3P4_S(){}
  
  virtual Double_t evaluateSlow() const{
    if(! getMomenta()) return -9999;
    TLorentzVector pV1 = p1 + p2;
    TLorentzVector qV1 = p1 - p2;
    TLorentzVector pV2 = p3 + p4;
    TLorentzVector qV2 = p3 - p4;

    return qV1.Dot(qV2)
      -    qV1.Dot(pV1) * pV1.Dot(qV2) / (MV1*MV1)
      -    qV1.Dot(pV2) * pV2.Dot(qV2) / (MV2*MV2)
      +    qV1.Dot(pV1) * pV1.Dot(pV2) * pV2.Dot(qV2) / (MV1*MV1 * MV2*MV2)
      ;
  }
  virtual Double_t evaluate() const{
    getFastMomenta();
    /*
      qV1*qV2 = (p1 - p2)*(p3-p4)
              = p13 - p14 -p23 + p24

      qV1*pV1 = (p1 - p2)(p1 + p2)
              = p11 - p22
	      
      pV1*qV2 = (1 + 2)(3-4)
              = 13 -14 +23 -24

      qV1*pV2 = (1-2)(3+4)
              = 13 + 14 -23 -24

      pV2*qV2 = (3+4)(3-4)
              = 33 -44
 
      qV1*pV1 = 11-22

      pV1*pV2 = (1+2)(3+4)
                13 + 14 + 23 + 24

      pV2*qV2 = 33-44
    */

    Double_t qV1qV2 = p13 - p14 - p23 + p24;
    Double_t qV1pV1 = p11-p22;
    Double_t pV1qV2 = p13 - p14 + p23 - p24;
    Double_t qV1pV2 = p13 + p14 - p23 - p24;
    Double_t pV2qV2 = p33 - p44;
    Double_t pV1pV2 = p13 + p14 + p23 + p24;

    double returnVal = qV1qV2
      -    qV1pV1 * pV1qV2/(MV1*MV1)
      -    qV1pV2 * pV2qV2/(MV2*MV2)
      +    qV1pV1 * pV1pV2 * pV2qV2 / (MV1*MV1 * MV2*MV2)
      ;
    
    if(db){
      if(fabs(returnVal - evaluateSlow()) > 1.e-10){
	std::cout << "compare: " << GetName()
		  << ": " << returnVal << ", " << evaluateSlow()
		  << std::endl;
      }
    }
    return returnVal;
  }

 protected:
  RooRealProxy MV1, MV2;
  //ClassDef(SF_DtoV1V2_V1toP1P2_V2toP3P4_S, 0)
};

class SF_DtoVS_VtoP1P2_StoP3P4 : public SpinFactor{

 public:
  SF_DtoVS_VtoP1P2_StoP3P4(const char* name
			   , const char* title
			   , RooAbsReal& _MV // yep, it really doesn't depend on MS
			   , int _p1_i
			   , int _p2_i
			   , int _p3_i
			   , int _p4_i
			   , RooAbsReal& _t01
			   , RooAbsReal& _s12
			   , RooAbsReal& _s23
			   , RooAbsReal& _s34
			   , RooAbsReal& _t40
			   , RooAbsReal& _m0
			   , RooAbsReal& _m1
			   , RooAbsReal& _m2
			   , RooAbsReal& _m3
			   , RooAbsReal& _m4
			   )
    : SpinFactor( name, title
		  , _p1_i, _p2_i, _p3_i, _p4_i
		  , _t01, _s12, _s23, _s34, _t40
		  , _m0, _m1, _m2, _m3, _m4)
    , MV("MV", "MV", this, _MV)
    {}
  
  SF_DtoVS_VtoP1P2_StoP3P4(const SF_DtoVS_VtoP1P2_StoP3P4& other
	 , const char* name=0)
    : SpinFactor(other, name)
    , MV("MV", this, other.MV)
    {}

 public:
  virtual TObject* clone(const char* newname) const{
    return new SF_DtoVS_VtoP1P2_StoP3P4(*this, newname);}
  virtual TObject* clone() const{
    return new SF_DtoVS_VtoP1P2_StoP3P4(*this);}
  inline virtual ~SF_DtoVS_VtoP1P2_StoP3P4(){}
  
  virtual Double_t evaluateSlow() const{
    if(! getMomenta()) return -9999;
    TLorentzVector pS = p3 + p4;
    TLorentzVector pV = p1 + p2;
    TLorentzVector qV = p1 - p2;
 
    return pS.Dot(qV)
      -    pS.Dot(pV) * pV.Dot(qV) / (MV*MV)
      ;
  }
  virtual Double_t evaluate() const{
    getFastMomenta();
    /*
      ps.Dot(qV) = (p3 + p4) * (p1 - p2)
                 =  p3p1 + p4p1  - p3p2 - p4p2
      
      pS.Dot(pV) = (p3 + p4) * (p1 + p2)
                 = p3p1 + p4p1 + p3p2 + p4p2

      pV.Dot(qV) = (p1 + p2) ( p1 - p2)
                  = p1p1 - p1p2 + p1p2 - p2p2
                  = p1p1 - p2p2;
    
    */
    Double_t pS_dot_qV = p13 + p14 - p23 - p24;
    Double_t pS_dot_pV = p13 + p14 + p23 + p24;
    Double_t pV_dot_qV = p11 - p22;
    
    Double_t returnVal = pS_dot_qV 
      -                  pS_dot_pV * pV_dot_qV / (MV*MV);

    if(db){
      if(fabs(returnVal - evaluateSlow()) > 1.e-10){
	std::cout << "compare: " << GetName()
		  << ": " << returnVal << ", " << evaluateSlow()
		  << std::endl;
      }
    }
    return returnVal;
 
  }
 protected:
  RooRealProxy MV;
  //ClassDef(SF_DtoVS_VtoP1P2_StoP3P4, 0)
};


#endif
//
