#ifndef SPINFACTORSSSS_FOURBODY_HH
#define SPINFACTORSSSS_FOURBODY_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:13 GMT

// These come from:
// D. Coffman et al: "Resonant substructure in Kbar pi pi pi decays of D mesons"
// Physical Review D, Vol 45, Number 7, page 2196
// The spin factors are in Table II, page 2201.

#include "Mint/DecayTree.h"
#include "Mint/SpinFactor.h"
#include "Mint/counted_ptr.h"
#include <iostream>


// ==========================================

class SF_DtoS1S2_S1toP1P2_S2toP3P4 : public SpinFactorTrivial{ 
  static DecayTree* _exampleDecay;
 public:
  SF_DtoS1S2_S1toP1P2_S2toP3P4(IDalitzEventAccess* , const DecayTree& )
    : SpinFactorTrivial(){}

  virtual ~SF_DtoS1S2_S1toP1P2_S2toP3P4(){}
  static const DecayTree& getExampleDecay();
  virtual const DecayTree& exampleDecay();
  virtual std::string name() const{
    return "SpinFactor4:SF_DtoS1S2_S1toP1P2_S2toP3P4-trivialSpinFactor";
  }
};

// ==========================================

class SF_DtoPP1P2_PtoP3P4 : public SpinFactorTrivial{ //Laurens wide Ks Spin
  static DecayTree* _exampleDecay;
 public:
  SF_DtoPP1P2_PtoP3P4(IDalitzEventAccess* , const DecayTree& )
    : SpinFactorTrivial(){}

  virtual ~SF_DtoPP1P2_PtoP3P4(){}
  static const DecayTree& getExampleDecay();
  virtual const DecayTree& exampleDecay();
  virtual std::string name() const{
    return "SpinFactor4:SF_DtoPP1P2_PtoP3P4-trivialSpinFactor";
  }
};

// ==========================================

class SF_DtoPP0_PtoVP1_VtoP2P3 : public SpinFactor{
 protected:
  static DecayTree* _exampleDecay;
  MINT::const_counted_ptr<AssociatedDecayTree> P, V;// *P1, *P2, *P3, *P4;

  virtual bool parseTree();
  
 public:
  virtual void printYourself(std::ostream& os=std::cout) const;
  SF_DtoPP0_PtoVP1_VtoP2P3(IDalitzEventAccess* events
			   , const DecayTree& theDecay) 
    : SpinFactor(events, theDecay, 4)
    , P(0), V(0)
    {
      if( ! parseTree()){
	throw "error in parseTree in constructor of SF_DtoPP0_PtoVP1_VtoP2P3";
      }
//      printYourself();
    }

  virtual ~SF_DtoPP0_PtoVP1_VtoP2P3(){}
  virtual double getVal();

  virtual std::string name() const{
    return "SpinFactor4:SF_DtoPP0_PtoVP1_VtoP2P3(" 
      + theDecay().oneLiner() + ")";
  }

  static const DecayTree& getExampleDecay();
  virtual const DecayTree& exampleDecay();

};

// ==========================================

class SF_DtoAP0_AtoVP1_VtoP2P3_BASE : public SpinFactor{
 protected:
  // static DecayTree* _exampleDecay;
  MINT::const_counted_ptr<AssociatedDecayTree> A, V;// *P1, *P2, *P3, *P4;

  virtual bool parseTree();

 public:
  virtual void printYourself(std::ostream& os=std::cout) const=0;
  SF_DtoAP0_AtoVP1_VtoP2P3_BASE(IDalitzEventAccess* events, const DecayTree& theDecay) 
    : SpinFactor(events, theDecay, 4)
    , A(0), V(0)
    {
      if( ! parseTree()){
	throw "error in parseTree in constructor of SF_DtoAP0_AtoVP1_VtoP2P3_BASE";
      }
      //printYourself();
    }

  virtual ~SF_DtoAP0_AtoVP1_VtoP2P3_BASE(){}
  virtual double getVal()=0;

  virtual const DecayTree& exampleDecay()=0;
  virtual std::string name() const{
    return "SpinFactor4:SF_DtoAP0_AtoVP1_VtoP2P3_BASE(" 
      + theDecay().oneLiner() + ")";
  }

};
// ==========================================

class SF_DtoAP0_AtoVP1_VtoP2P3 : public SF_DtoAP0_AtoVP1_VtoP2P3_BASE{
 protected:
  static DecayTree* _exampleDecay;
 public:
  virtual void printYourself(std::ostream& os=std::cout) const;
  SF_DtoAP0_AtoVP1_VtoP2P3(IDalitzEventAccess* events, const DecayTree& theDecay) 
    : SF_DtoAP0_AtoVP1_VtoP2P3_BASE(events, theDecay)
    {
      if( ! parseTree()){
	throw "error in parseTree in constructor of SF_DtoAP0_AtoVP1Dwave_VtoP2P3";
      }
//      printYourself();
    }

  virtual ~SF_DtoAP0_AtoVP1_VtoP2P3(){}
  virtual double getVal();

  static const DecayTree& getExampleDecay();
  virtual const DecayTree& exampleDecay();
  virtual std::string name() const{
    return "SpinFactor4:SF_DtoAP0_AtoVP1D_VtoP2P3(" 
      + theDecay().oneLiner() + ")";
  }

};
// ==========================================

class SF_DtoAP0_AtoVP1Dwave_VtoP2P3 : public SF_DtoAP0_AtoVP1_VtoP2P3_BASE{
 protected:
  static DecayTree* _exampleDecayD;
 public:
  virtual void printYourself(std::ostream& os=std::cout) const;
  SF_DtoAP0_AtoVP1Dwave_VtoP2P3(IDalitzEventAccess* events, const DecayTree& theDecay) 
    : SF_DtoAP0_AtoVP1_VtoP2P3_BASE(events, theDecay)
    {
      if( ! parseTree()){
	throw "error in parseTree in constructor of SF_DtoAP0_AtoVP1Dwave_VtoP2P3";
      }
//      printYourself();
    }

  virtual ~SF_DtoAP0_AtoVP1Dwave_VtoP2P3(){}
  virtual double getVal();

  static const DecayTree& getExampleDecay();
  virtual const DecayTree& exampleDecay();
  virtual std::string name() const{
    return "SpinFactor4:SF_DtoAP0_AtoVP1Dwave_VtoP2P3(" 
      + theDecay().oneLiner() + ")";
  }

};


// ==========================================

class SF_DtoAP0_AtoSP1_StoP2P3 : public SpinFactor{
 protected:
  static DecayTree* _exampleDecay;
  MINT::const_counted_ptr<AssociatedDecayTree> A, S;// *P1, *P2, *P3, *P4;

  virtual bool parseTree();

 public:
  virtual void printYourself(std::ostream& os=std::cout) const;
  SF_DtoAP0_AtoSP1_StoP2P3(IDalitzEventAccess* events, const DecayTree& theDecay) 
    : SpinFactor(events, theDecay, 4)
    , A(0), S(0)
    {
      if( ! parseTree()){
	throw "error in parseTree in constructor of SF_DtoAP0_AtoSP1_StoP2P3";
      }
//      printYourself();
    }

  virtual ~SF_DtoAP0_AtoSP1_StoP2P3(){}
  virtual double getVal();

  static const DecayTree& getExampleDecay();
  virtual const DecayTree& exampleDecay();
  virtual std::string name() const{
    return "SpinFactor4:SF_DtoAP0_AtoSP1_StoP2P3(" 
      + theDecay().oneLiner() + ")";
  }

};

// ==========================================

class SF_DtoV1V2_V1toP0P1_V1toP2P3_BASE : public SpinFactor{
 protected:
  //  static DecayTree* _exampleDecayS;
  MINT::const_counted_ptr<AssociatedDecayTree> V1, V2;// *P1, *P2, *P3, *P4;

  virtual bool parseTree();

 public:
  virtual void printYourself(std::ostream& os=std::cout) const=0;
 SF_DtoV1V2_V1toP0P1_V1toP2P3_BASE(IDalitzEventAccess* events
				   , const DecayTree& theDecay) 
   : SpinFactor(events, theDecay, 4)
    , V1(0), V2(0)
    {
      if( ! parseTree()){
	throw "error in parseTree in constructor of SF_DtoV1V2_V1toP0P1_V1toP2P3_S";
      }
      //printYourself();
    }
  
  virtual ~SF_DtoV1V2_V1toP0P1_V1toP2P3_BASE(){}
  virtual double getVal()=0;

  virtual const DecayTree& exampleDecay()=0;
  virtual std::string name() const{
    return "SpinFactor4:SF_DtoV1V2_V1toP0P1_V1toP2P3_BASE(" 
      + theDecay().oneLiner() + ")";
  }

};

// -----------------------------------------------

class SF_DtoV1V2_V1toP0P1_V1toP2P3_S : public SF_DtoV1V2_V1toP0P1_V1toP2P3_BASE{
  static DecayTree* _exampleDecayS;

 public:
  virtual void printYourself(std::ostream& os=std::cout) const;
 SF_DtoV1V2_V1toP0P1_V1toP2P3_S(IDalitzEventAccess* events
				, const DecayTree& theDecay)
   : SF_DtoV1V2_V1toP0P1_V1toP2P3_BASE(events, theDecay)
    {
//      printYourself();
    }

  virtual ~SF_DtoV1V2_V1toP0P1_V1toP2P3_S(){}
  virtual double getVal();

  static const DecayTree& getExampleDecay();
  virtual const DecayTree& exampleDecay();
  virtual std::string name() const{
    return "SpinFactor4:SF_DtoV1V2_V1toP0P1_V1toP2P3_S(" 
      + theDecay().oneLiner() + ")";
  }
};
// -----------------------------------------------

class SF_DtoV1V2_V1toP0P1_V1toP2P3_P : public SF_DtoV1V2_V1toP0P1_V1toP2P3_BASE{
  static DecayTree* _exampleDecayP;

 public:
  virtual void printYourself(std::ostream& os=std::cout) const;
  SF_DtoV1V2_V1toP0P1_V1toP2P3_P(IDalitzEventAccess* events
				 , const DecayTree& theDecay)
    : SF_DtoV1V2_V1toP0P1_V1toP2P3_BASE(events, theDecay)
    {
//      printYourself();
    }

  virtual ~SF_DtoV1V2_V1toP0P1_V1toP2P3_P(){}
  virtual double getVal();

  static const DecayTree& getExampleDecay();
  virtual const DecayTree& exampleDecay();
  virtual std::string name() const{
    return "SpinFactor4:SF_DtoV1V2_V1toP0P1_V1toP2P3_P(" 
      + theDecay().oneLiner() + ")";
  }
};
// -----------------------------------------------

class SF_DtoV1V2_V1toP0P1_V1toP2P3_D : public SF_DtoV1V2_V1toP0P1_V1toP2P3_BASE{
  static DecayTree* _exampleDecayD;
 public:
  virtual void printYourself(std::ostream& os=std::cout) const;
  SF_DtoV1V2_V1toP0P1_V1toP2P3_D(IDalitzEventAccess* events
				 , const DecayTree& theDecay)
    : SF_DtoV1V2_V1toP0P1_V1toP2P3_BASE(events, theDecay)
    {
//      printYourself();
    }

  virtual ~SF_DtoV1V2_V1toP0P1_V1toP2P3_D(){}
  virtual double getVal();

  static const DecayTree& getExampleDecay();
  virtual const DecayTree& exampleDecay();
  virtual std::string name() const{
    return "SpinFactor4:SF_DtoV1V2_V1toP0P1_V1toP2P3_D(" 
      + theDecay().oneLiner() + ")";
  }
};

// ==========================================

class SF_DtoV1V2_V1toP0P1_V1toP2P3_S_nonResV1 : public SpinFactor{
 protected:
  static DecayTree* _exampleDecay;
  MINT::const_counted_ptr<AssociatedDecayTree> V2;// *P1, *P2, *P3, *P4;

  virtual bool parseTree();

 public:
  virtual void printYourself(std::ostream& os=std::cout) const;
  SF_DtoV1V2_V1toP0P1_V1toP2P3_S_nonResV1(IDalitzEventAccess* events
					  , const DecayTree& theDecay) 
    : SpinFactor(events, theDecay, 4)
    , V2(0)
    {
      if( ! parseTree()){
	throw "error in parseTree in constructor of SF_DtoV1V2_V1toP0P1_V1toP2P3_S_nonResV1";
      }
//      printYourself();
    }

  virtual ~SF_DtoV1V2_V1toP0P1_V1toP2P3_S_nonResV1(){}
  virtual double getVal();

  static const DecayTree& getExampleDecay();
  virtual const DecayTree& exampleDecay();
  virtual std::string name() const{
    return "SpinFactor4:SF_DtoV1V2_V1toP0P1_V1toP2P3_S_nonResV1(" 
      + theDecay().oneLiner() + ")";
  }
};

// ==========================================

class SF_DtoVS_VtoP0P1_StoP2P3 : public SpinFactor{
 protected:
  static DecayTree* _exampleDecay;
  MINT::const_counted_ptr<AssociatedDecayTree> V, S;// *P1, *P2, *P3, *P4;

  virtual bool parseTree();

 public:
  virtual void printYourself(std::ostream& os=std::cout) const;
  SF_DtoVS_VtoP0P1_StoP2P3(IDalitzEventAccess* events, const DecayTree& theDecay) 
    : SpinFactor(events, theDecay, 4)
    , V(0), S(0)
    {
      if( ! parseTree()){
	throw "error in parseTree in constructor of SF_DtoVS_VtoP0P1_StoP2P3";
      }
//      printYourself();
    }

  virtual ~SF_DtoVS_VtoP0P1_StoP2P3(){}
  virtual double getVal();

  static const DecayTree& getExampleDecay();
  virtual const DecayTree& exampleDecay();
  virtual std::string name() const{
    return "SpinFactor4:SF_DtoVS_VtoP0P1_StoP2P3(" 
      + theDecay().oneLiner() + ")";
  }
};

// ==========================================

class SF_DtoVS_VtoP0P1_StoP2P3_nonResV : public SpinFactor{
 protected:
  static DecayTree* _exampleDecay;
  MINT::const_counted_ptr<AssociatedDecayTree> S;// *P1, *P2, *P3, *P4;

  virtual bool parseTree();

 public:
  virtual void printYourself(std::ostream& os=std::cout) const;
  SF_DtoVS_VtoP0P1_StoP2P3_nonResV(IDalitzEventAccess* events, const DecayTree& theDecay) 
    : SpinFactor(events, theDecay, 4)
    , S(0)
    {
      if( ! parseTree()){
	throw "error in parseTree in constructor of SF_DtoVS_VtoP0P1_StoP2P3_nonResV";
      }
//      printYourself();
    }

  virtual ~SF_DtoVS_VtoP0P1_StoP2P3_nonResV(){}
  virtual double getVal();

  static const DecayTree& getExampleDecay();
  virtual const DecayTree& exampleDecay();
  virtual std::string name() const{
    return "SpinFactor4:SF_DtoVS_VtoP0P1_StoP2P3_nonResV(" 
      + theDecay().oneLiner() + ")";
  }
};

// ==========================================

class SF_DtoVT_VtoP0P1_TtoP2P3_BASE : public SpinFactor{
 protected:
  // static DecayTree* _exampleDecay;
  MINT::const_counted_ptr<AssociatedDecayTree> V, T;// *P1, *P2, *P3, *P4;

  virtual bool parseTree();

 public:
  virtual void printYourself(std::ostream& os=std::cout) const=0;
 SF_DtoVT_VtoP0P1_TtoP2P3_BASE(IDalitzEventAccess* events
			    , const DecayTree& theDecay) 
   : SpinFactor(events, theDecay, 4)
    , V(0), T(0)
    {
      if( ! parseTree()){
	throw "error in parseTree in constructor of SF_DtoVT_VtoP0P1_TtoP2P3_P";
      }
      //printYourself();
    }

  virtual ~SF_DtoVT_VtoP0P1_TtoP2P3_BASE(){}
  virtual double getVal()=0;

  virtual const DecayTree& exampleDecay()=0;
  virtual std::string name() const{
    return "SpinFactor4:SF_DtoVT_VtoP0P1_TtoP2P3_BASE("
      + theDecay().oneLiner() + ")";
  }

};
// ---------------------------------
// -----------------------------------------------

class SF_DtoVT_VtoP0P1_TtoP2P3_P : public SF_DtoVT_VtoP0P1_TtoP2P3_BASE{
  static DecayTree* _exampleDecay;
 public:
  virtual void printYourself(std::ostream& os=std::cout) const;
  
 SF_DtoVT_VtoP0P1_TtoP2P3_P(IDalitzEventAccess* events
			    , const DecayTree& theDecay)
   : SF_DtoVT_VtoP0P1_TtoP2P3_BASE(events, theDecay)
    {
//      printYourself();
    }
  
  virtual ~SF_DtoVT_VtoP0P1_TtoP2P3_P(){}
  virtual double getVal();

  static const DecayTree& getExampleDecay();
  virtual const DecayTree& exampleDecay();
  virtual std::string name() const{
    return "SpinFactor4:SF_DtoVT_VtoP0P1_TtoP2P3_P(" 
      + theDecay().oneLiner() + ")";
  }
};
//------------------------------------------------
class SF_DtoVT_VtoP0P1_TtoP2P3_D : public SF_DtoVT_VtoP0P1_TtoP2P3_BASE{
  static DecayTree* _exampleDecayD;
 public:
  virtual void printYourself(std::ostream& os=std::cout) const;
  
 SF_DtoVT_VtoP0P1_TtoP2P3_D(IDalitzEventAccess* events
			    , const DecayTree& theDecay)
   : SF_DtoVT_VtoP0P1_TtoP2P3_BASE(events, theDecay)
    {
//      printYourself();
    }
  
  virtual ~SF_DtoVT_VtoP0P1_TtoP2P3_D(){}
  virtual double getVal();

  static const DecayTree& getExampleDecay();
  virtual const DecayTree& exampleDecay();
  virtual std::string name() const{
    return "SpinFactor4:SF_DtoVT_VtoP0P1_TtoP2P3_D(" 
      + theDecay().oneLiner() + ")";
  }
};
//------------------------------------------------

class SF_DtoTS_TtoP0P1_StoP2P3 : public SpinFactor{
 protected:
  static DecayTree* _exampleDecay;
  MINT::const_counted_ptr<AssociatedDecayTree> T, S;// *P1, *P2, *P3, *P4;

  virtual bool parseTree();

 public:
  virtual void printYourself(std::ostream& os=std::cout) const;
 SF_DtoTS_TtoP0P1_StoP2P3(IDalitzEventAccess* events
			    , const DecayTree& theDecay) 
   : SpinFactor(events, theDecay, 4)
    , T(0), S(0)
    {
      if( ! parseTree()){
	throw "error in parseTree in constructor of SF_DtoTS_TtoP0P1_StoP2P3";
      }
//      printYourself();
    }

  virtual ~SF_DtoTS_TtoP0P1_StoP2P3(){}
  virtual double getVal();
  
  static const DecayTree& getExampleDecay();
  virtual const DecayTree& exampleDecay();
  virtual std::string name() const{
    return "SpinFactor4:SF_DtoTS_TtoP0P1_StoP2P3("
      + theDecay().oneLiner() + ")";
  }

};
// ---------------------------------
//------------------------------------------------
class SF_DtoT1T2_T1toP0P1_T2toP2P3_BASE : public SpinFactor{
 protected:
  MINT::const_counted_ptr<AssociatedDecayTree> T[2];// *P1, *P2, *P3, *P4;
  virtual bool parseTree();

 public:
  virtual void printYourself(std::ostream& os=std::cout) const=0;
 SF_DtoT1T2_T1toP0P1_T2toP2P3_BASE(IDalitzEventAccess* events
			    , const DecayTree& theDecay) 
   : SpinFactor(events, theDecay, 4)
    {
      T[0]=T[1]=0;
      if( ! parseTree()){
	throw "error in parseTree in constructor of SF_DtoT1T2_T1toP0P1_T2toP2P3_BASE";
      }
//      printYourself();
    }

  virtual ~SF_DtoT1T2_T1toP0P1_T2toP2P3_BASE(){}
  virtual double getVal()=0;
  
  virtual const DecayTree& exampleDecay()=0;
  virtual std::string name() const{
    return "SpinFactor4:SF_DtoT1T2_T1toP0P1_T2toP2P3_BASE("
      + theDecay().oneLiner() + ")";
  }

};
//------------------------------------------------

class SF_DtoT1T2_T1toP0P1_T2toP2P3_S : public SF_DtoT1T2_T1toP0P1_T2toP2P3_BASE{
  // the simplest D->T1, T2, with L=0
 protected:
  static DecayTree* _exampleDecay;
  virtual bool parseTree();

 public:
  virtual void printYourself(std::ostream& os=std::cout) const;
 SF_DtoT1T2_T1toP0P1_T2toP2P3_S(IDalitzEventAccess* events
			    , const DecayTree& theDecay) 
   : SF_DtoT1T2_T1toP0P1_T2toP2P3_BASE(events, theDecay)
    {
      //printYourself();
    }

  virtual ~SF_DtoT1T2_T1toP0P1_T2toP2P3_S(){}
  virtual double getVal();
  
  static const DecayTree& getExampleDecay();
  virtual const DecayTree& exampleDecay();
  virtual std::string name() const{
    return "SpinFactor4:SF_DtoT1T2_T1toP0P1_T2toP2P3_S("
      + theDecay().oneLiner() + ")";
  }

};
//------------------------------------------------
class SF_DtoT1T2_T1toP0P1_T2toP2P3_P : public SF_DtoT1T2_T1toP0P1_T2toP2P3_BASE{
  // the simplest D->T1, T2, with L=0
 protected:
  static DecayTree* _exampleDecay;
  virtual bool parseTree();

 public:
  virtual void printYourself(std::ostream& os=std::cout) const;
 SF_DtoT1T2_T1toP0P1_T2toP2P3_P(IDalitzEventAccess* events
			    , const DecayTree& theDecay) 
   : SF_DtoT1T2_T1toP0P1_T2toP2P3_BASE(events, theDecay)
    {
      //printYourself();
    }

  virtual ~SF_DtoT1T2_T1toP0P1_T2toP2P3_P(){}
  virtual double getVal();
  
  static const DecayTree& getExampleDecay();
  virtual const DecayTree& exampleDecay();
  virtual std::string name() const{
    return "SpinFactor4:SF_DtoT1T2_T1toP0P1_T2toP2P3_P("
      + theDecay().oneLiner() + ")";
  }

};
//------------------------------------------------
class SF_DtoT1T2_T1toP0P1_T2toP2P3_D : public SF_DtoT1T2_T1toP0P1_T2toP2P3_BASE{
  // the simplest D->T1, T2, with L=0
 protected:
  static DecayTree* _exampleDecay;
  virtual bool parseTree();

 public:
  virtual void printYourself(std::ostream& os=std::cout) const;
 SF_DtoT1T2_T1toP0P1_T2toP2P3_D(IDalitzEventAccess* events
			    , const DecayTree& theDecay) 
   : SF_DtoT1T2_T1toP0P1_T2toP2P3_BASE(events, theDecay)
    {
      //printYourself();
    }

  virtual ~SF_DtoT1T2_T1toP0P1_T2toP2P3_D(){}
  virtual double getVal();
  
  static const DecayTree& getExampleDecay();
  virtual const DecayTree& exampleDecay();
  virtual std::string name() const{
    return "SpinFactor4:SF_DtoT1T2_T1toP0P1_T2toP2P3_D("
      + theDecay().oneLiner() + ")";
  }

};
// ---------------------------------
//===========================================
//------------------------------------------------

class SF_DtoTP0_TtoVP1_VtoP2P3 : public SpinFactor{
  // the simplest D->T1, T2, with L=0
 protected:
  static DecayTree* _exampleDecay;
  MINT::const_counted_ptr<AssociatedDecayTree> T, V;// *P1, *P2, *P3, *P4;
   virtual bool parseTree();

 public:
   virtual void printYourself(std::ostream& os=std::cout) const;
   SF_DtoTP0_TtoVP1_VtoP2P3(IDalitzEventAccess* events
				     , const DecayTree& theDecay) 
     : SpinFactor(events, theDecay, 4)
     , T(0), V(0)
     {
       if( ! parseTree()){
	 throw "error in parseTree in constructor of SF_DtoT1P0_T1toT2P1_T2toP2P3_BASE";
       }
       //      printYourself();
     }
   
   virtual ~SF_DtoTP0_TtoVP1_VtoP2P3(){}
   virtual double getVal();
   
   static const DecayTree& getExampleDecay();
   virtual const DecayTree& exampleDecay();
   virtual std::string name() const{
     return "SpinFactor4:SF_DtoTP0_TtoVP1_VtoP2P3("
       + theDecay().oneLiner() + ")";
   }
   
};
//------------------------------------------------

class SF_DtoT1P0_T1toT2P1_T2toP2P3 : public SpinFactor{
  // the simplest D->T1, T2, with L=0
 protected:
  static DecayTree* _exampleDecay;
  MINT::const_counted_ptr<AssociatedDecayTree> T1, T2;// *P1, *P2, *P3, *P4;
   virtual bool parseTree();

 public:
   virtual void printYourself(std::ostream& os=std::cout) const;
   SF_DtoT1P0_T1toT2P1_T2toP2P3(IDalitzEventAccess* events
				     , const DecayTree& theDecay) 
     : SpinFactor(events, theDecay, 4)
     , T1(0), T2(0)
     {
       if( ! parseTree()){
	 throw "error in parseTree in constructor of SF_DtoT1P0_T1toT2P1_T2toP2P3_BASE";
       }
       //      printYourself();
     }
   
   virtual ~SF_DtoT1P0_T1toT2P1_T2toP2P3(){}
   virtual double getVal();
   
   static const DecayTree& getExampleDecay();
   virtual const DecayTree& exampleDecay();
   virtual std::string name() const{
     return "SpinFactor4:SF_DtoT1P0_T1toT2P1_T2toP2P3("
       + theDecay().oneLiner() + ")";
   }
   
};
//===========================================
//------------------------------------------------

class SF_DtoPseudoTP0_PseudoTtoVP1_VtoP2P3 : public SpinFactor{
  // the simplest D->T1, T2, with L=0
 protected:
  static DecayTree* _exampleDecay;
  MINT::const_counted_ptr<AssociatedDecayTree> PT, V;// *P1, *P2, *P3, *P4;
   virtual bool parseTree();

 public:
   virtual void printYourself(std::ostream& os=std::cout) const;
   SF_DtoPseudoTP0_PseudoTtoVP1_VtoP2P3(IDalitzEventAccess* events
				     , const DecayTree& theDecay) 
     : SpinFactor(events, theDecay, 4)
     , PT(0), V(0)
     {
       if( ! parseTree()){
	 throw "error in parseTree in constructor of SF_DtoT1P0_T1toT2P1_T2toP2P3_BASE";
       }
       //      printYourself();
     }
   
   virtual ~SF_DtoPseudoTP0_PseudoTtoVP1_VtoP2P3(){}
   virtual double getVal();
   
   static const DecayTree& getExampleDecay();
   virtual const DecayTree& exampleDecay();
   virtual std::string name() const{
     return "SpinFactor4:SF_DtoPseudoTP0_PseudoTtoVP1_VtoP2P3("
       + theDecay().oneLiner() + ")";
   }
   
};
//------------------------------------------------

class SF_DtoPseudoTP0_PseudoTtoSP1_StoP2P3 : public SpinFactor{
  // the simplest D->T1, T2, with L=0
 protected:
  static DecayTree* _exampleDecay;
  MINT::const_counted_ptr<AssociatedDecayTree> PT, S;// *P1, *P2, *P3, *P4;
   virtual bool parseTree();

 public:
   virtual void printYourself(std::ostream& os=std::cout) const;
   SF_DtoPseudoTP0_PseudoTtoSP1_StoP2P3(IDalitzEventAccess* events
				     , const DecayTree& theDecay) 
     : SpinFactor(events, theDecay, 4)
     , PT(0), S(0)
     {
       if( ! parseTree()){
	 throw "error in parseTree in constructor of SF_DtoT1P0_T1toT2P1_T2toP2P3_BASE";
       }
       //      printYourself();
     }
   
   virtual ~SF_DtoPseudoTP0_PseudoTtoSP1_StoP2P3(){}
   virtual double getVal();
   
   static const DecayTree& getExampleDecay();
   virtual const DecayTree& exampleDecay();
   virtual std::string name() const{
     return "SpinFactor4:SF_DtoPseudoTP0_PseudoTtoSP1_StoP2P3("
       + theDecay().oneLiner() + ")";
   }
   
};
//------------------------------------------------

class SF_DtoPseudoTP0_PseudoTtoTP1_TtoP2P3 : public SpinFactor{
  // the simplest D->T1, T2, with L=0
 protected:
  static DecayTree* _exampleDecay;
  MINT::const_counted_ptr<AssociatedDecayTree> PT, T;// *P1, *P2, *P3, *P4;
   virtual bool parseTree();

 public:
   virtual void printYourself(std::ostream& os=std::cout) const;
   SF_DtoPseudoTP0_PseudoTtoTP1_TtoP2P3(IDalitzEventAccess* events
				     , const DecayTree& theDecay) 
     : SpinFactor(events, theDecay, 4)
     , PT(0), T(0)
     {
       if( ! parseTree()){
	 throw "error in parseTree in constructor of SF_DtoT1P0_T1toT2P1_T2toP2P3_BASE";
       }
       //      printYourself();
     }
   
   virtual ~SF_DtoPseudoTP0_PseudoTtoTP1_TtoP2P3(){}
   virtual double getVal();
   
   static const DecayTree& getExampleDecay();
   virtual const DecayTree& exampleDecay();
   virtual std::string name() const{
     return "SpinFactor4:SF_DtoPseudoTP0_PseudoTtoTP1_TtoP2P3("
       + theDecay().oneLiner() + ")";
   }
   
};
//===========================================
// ==========================================

class SF_DtoV1P0_V1toV2P1_V2toP2P3 : public SpinFactor{
  // author: Loic Esteve
protected:
 static DecayTree* _exampleDecay;
 MINT::const_counted_ptr<AssociatedDecayTree> V1, V2;// *P1, *P2, *P3, *P4;

 virtual bool parseTree();

public:
 virtual void printYourself(std::ostream& os=std::cout) const;
 SF_DtoV1P0_V1toV2P1_V2toP2P3(IDalitzEventAccess* events
				 , const DecayTree& theDecay) 
   : SpinFactor(events, theDecay, 4)
   , V1(0), V2(0)
   {
     if( ! parseTree()){
	throw "error in parseTree in constructor of SF_DtoV1P0_V1toV2P1_V2toP2P3";
     }
//     printYourself();
   }

 virtual ~SF_DtoV1P0_V1toV2P1_V2toP2P3(){}
 virtual double getVal();

 static const DecayTree& getExampleDecay();
 virtual const DecayTree& exampleDecay();
 virtual std::string name() const{
   return "SpinFactor4:SF_DtoV1P0_V1toV2P1_V2toP2P3(" 
     + theDecay().oneLiner() + ")";
 }

};


// ==========================================


// ==========================================

// -----------------------------------------------


/*
class SF_DtoAP1_AtoSP2_StoP3P4 : public SpinFactor{
 public:
  SF_DtoAP1_AtoSP2_StoP3P4(IDalitzEventAccess* events) 
    : SpinFactor(events)
    {}
}
class SF_DtoV1V2_V1toP1P2_V2toP3P4_S : public SpinFactor{
 public:
  SF_DtoV1V2_V1toP1P2_V2toP3P4_S(IDalitzEventAccess* events) 
    : SpinFactor(events)
    {}
}
class SF_DtoVS_VtoP1P2_StoP3P4 : public SpinFactor{
 public:
  SF_DtoVS_VtoP1P2_StoP3P4(IDalitzEventAccess* events) 
    : SpinFactor(events)
    {}
}

*/
#endif
//
