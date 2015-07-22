#ifndef SPINFACTORS_FOURBODY_FINALSTATE_VECTOR_HH
#define SPINFACTORS_FOURBODY_FINALSTATE_VECTOR_HH
// author: Philippe d'Argent (p.dargent@cern.ch)
// status:  Tu 6 May 2014 

#include "Mint/DalitzEventPattern.h"
#include "Mint/IDalitzEvent.h"
#include "Mint/DecayTree.h"
#include "Mint/SpinFactor.h"
#include "Mint/counted_ptr.h"
#include <iostream>

using namespace std;
// ==========================================
// Spin Factor 1-4

class SF_BtoAV0_AtoVP1_VtoP2P3_BASE : public SpinFactor{
protected:
    // static DecayTree* _exampleDecay;
    MINT::const_counted_ptr<AssociatedDecayTree> A, V;// *P1, *P2, *P3, *P4;
    
    virtual bool parseTree(const DalitzEventPattern& pat);
    
public:
    virtual void printYourself(std::ostream& os=std::cout) const=0;
    SF_BtoAV0_AtoVP1_VtoP2P3_BASE(const DecayTree& theDecay) 
    : SpinFactor(theDecay, 4)
    , A(0), V(0)
    {
        //if( ! parseTree(const DalitzEventPattern& pat)){
        //    throw "error in parseTree in constructor of SF_BtoAV0_AtoVP1_VtoP2P3_BASE";
        //}
        //printYourself();
    }
    
    virtual ~SF_BtoAV0_AtoVP1_VtoP2P3_BASE(){}
    virtual double getVal(IDalitzEvent& evt)=0;
    virtual std::complex<double> getNewVal(IDalitzEvent& evt)=0;
    
    virtual const DecayTree& exampleDecay()=0;
    virtual std::string name() const{
        return "SpinFactor4:SF_BtoAV0_AtoVP1_VtoP2P3_BASE(" 
        + theBareDecay().oneLiner() + ")";
    }
    
};
// ==========================================
// Spin Factor 1

class SF_BtoAV0_AtoVP1_VtoP2P3 : public SF_BtoAV0_AtoVP1_VtoP2P3_BASE{
  //int _pol;
protected:
  int _pol;
  static DecayTree* _exampleDecay;
  //static int _pol;  
 public:
  virtual void printYourself(std::ostream& os=std::cout) const;
  SF_BtoAV0_AtoVP1_VtoP2P3(const DecayTree& theDecay, const int pol) 
    : SF_BtoAV0_AtoVP1_VtoP2P3_BASE(theDecay), _pol(pol)
    {
    //  if( ! parseTree(const DalitzEventPattern& pat)){
	//throw "error in parseTree in constructor of SF_BtoAV0_AtoVP1_VtoP2P3";
    //  }
   // _pol=pol;  
//      printYourself();
    }

  virtual ~SF_BtoAV0_AtoVP1_VtoP2P3(){}
  virtual double getVal(IDalitzEvent& evt){return getNewVal(evt).real();}
  virtual std::complex<double> getNewVal(IDalitzEvent& evt);

  static const DecayTree& getExampleDecay();
  virtual const DecayTree& exampleDecay();
  virtual std::string name() const{
    return "SpinFactor4:SF_BtoAV0_AtoVP1_VtoP2P3(" 
      + theBareDecay().oneLiner() + ")";
  }
  int getPolarisation() {return _pol;}
    
    
};


// Spin Factor 2

class SF_BtoAV0_AtoVP1Dwave_VtoP2P3 : public SF_BtoAV0_AtoVP1_VtoP2P3_BASE{
protected:
    int _pol;
    static DecayTree* _exampleDecayDwave;
public:
    virtual void printYourself(std::ostream& os=std::cout) const;
    SF_BtoAV0_AtoVP1Dwave_VtoP2P3(const DecayTree& theDecay, const int pol) 
    : SF_BtoAV0_AtoVP1_VtoP2P3_BASE(theDecay), _pol(pol) {}
    
    virtual ~SF_BtoAV0_AtoVP1Dwave_VtoP2P3(){}
    virtual double getVal(IDalitzEvent& evt){return getNewVal(evt).real();}
    virtual std::complex<double> getNewVal(IDalitzEvent& evt);
    
    static const DecayTree& getExampleDecay();
    virtual const DecayTree& exampleDecay();
    virtual std::string name() const{
        return "SpinFactor4:SF_BtoAV0_AtoVP1Dwave_VtoP2P3(" 
        + theBareDecay().oneLiner() + ")";
    }
    int getPolarisation() {return _pol;}
};

// Spin Factor 3

class SF_BtoAV0_AtoVP1_VtoP2P3_P : public SF_BtoAV0_AtoVP1_VtoP2P3_BASE{
protected:
    int _pol;
    static DecayTree* _exampleDecayP;
public:
    virtual void printYourself(std::ostream& os=std::cout) const;
    SF_BtoAV0_AtoVP1_VtoP2P3_P( const DecayTree& theDecay, const int pol) 
    : SF_BtoAV0_AtoVP1_VtoP2P3_BASE(theDecay), _pol(pol){}
    
    virtual ~SF_BtoAV0_AtoVP1_VtoP2P3_P(){}
    virtual double getVal(IDalitzEvent& evt){return getNewVal(evt).real();}
    virtual std::complex<double> getNewVal(IDalitzEvent& evt);
    
    static const DecayTree& getExampleDecay();
    virtual const DecayTree& exampleDecay();
    virtual std::string name() const{
        return "SpinFactor4:SF_BtoAV0_AtoVP1_VtoP2P3_P(" 
        + theBareDecay().oneLiner() + ")";
    }
    int getPolarisation() {return _pol;}

};

// Spin Factor 4

class SF_BtoAV0_AtoVP1_VtoP2P3_D : public SF_BtoAV0_AtoVP1_VtoP2P3_BASE{
protected:
    int _pol;
    static DecayTree* _exampleDecayD;
public:
    virtual void printYourself(std::ostream& os=std::cout) const;
    SF_BtoAV0_AtoVP1_VtoP2P3_D( const DecayTree& theDecay, const int pol) 
    : SF_BtoAV0_AtoVP1_VtoP2P3_BASE(theDecay), _pol(pol){}
    
    virtual ~SF_BtoAV0_AtoVP1_VtoP2P3_D(){}
    virtual double getVal(IDalitzEvent& evt){return getNewVal(evt).real();}
    virtual std::complex<double> getNewVal(IDalitzEvent& evt);
    
    static const DecayTree& getExampleDecay();
    virtual const DecayTree& exampleDecay();
    virtual std::string name() const{
        return "SpinFactor4:SF_BtoAV0_AtoVP1_VtoP2P3_D(" 
        + theBareDecay().oneLiner() + ")";
    }
    int getPolarisation() {return _pol;}

};

// ==========================================
// Spin Factor 5

class SF_BtoAV0_AtoSP1_StoP2P3_BASE : public SpinFactor{
protected:
    // static DecayTree* _exampleDecay;
    MINT::const_counted_ptr<AssociatedDecayTree> S, A;// *P1, *P2, *P3, *P4;
    
    virtual bool parseTree(const DalitzEventPattern& pat);
    
public:
    virtual void printYourself(std::ostream& os=std::cout) const=0;
    SF_BtoAV0_AtoSP1_StoP2P3_BASE( const DecayTree& theDecay) 
    : SpinFactor(theDecay, 4), S(0), A(0){}
    
    virtual ~SF_BtoAV0_AtoSP1_StoP2P3_BASE(){}
    virtual double getVal(IDalitzEvent& evt)=0;
    virtual std::complex<double> getNewVal(IDalitzEvent& evt)=0;
    
    virtual const DecayTree& exampleDecay()=0;
    virtual std::string name() const{
        return "SpinFactor4:SF_BtoAV0_AtoSP1_StoP2P3_BASE(" 
        + theBareDecay().oneLiner() + ")";
    }
    
};

class SF_BtoAV0_AtoSP1_StoP2P3 : public SF_BtoAV0_AtoSP1_StoP2P3_BASE{
protected:
    int _pol;
    static DecayTree* _exampleDecay;
public:
    virtual void printYourself(std::ostream& os=std::cout) const;
    SF_BtoAV0_AtoSP1_StoP2P3( const DecayTree& theDecay,const int pol) 
    : SF_BtoAV0_AtoSP1_StoP2P3_BASE(theDecay), _pol(pol){}
    
    virtual ~SF_BtoAV0_AtoSP1_StoP2P3(){}
    virtual double getVal(IDalitzEvent& evt){return getNewVal(evt).real();}
    virtual std::complex<double> getNewVal(IDalitzEvent& evt);
    
    static const DecayTree& getExampleDecay();
    virtual const DecayTree& exampleDecay();
    virtual std::string name() const{
        return "SpinFactor4:SF_BtoAV0_AtoSP1_StoP2P3(" 
        + theBareDecay().oneLiner() + ")";
    }
    int getPolarisation() {return _pol;}
};



// ==========================================
// Spin Factor 6

class SF_BtoV1V0_V1toV2P1_V2toP2P3_BASE : public SpinFactor{
protected:
    // static DecayTree* _exampleDecay;
    MINT::const_counted_ptr<AssociatedDecayTree> V1, V2;// *P1, *P2, *P3, *P4;
    
    virtual bool parseTree(const DalitzEventPattern& pat);
    
public:
    virtual void printYourself(std::ostream& os=std::cout) const=0;
    SF_BtoV1V0_V1toV2P1_V2toP2P3_BASE( const DecayTree& theDecay) 
    : SpinFactor(theDecay, 4), V1(0), V2(0){}
    
    virtual ~SF_BtoV1V0_V1toV2P1_V2toP2P3_BASE(){}
    virtual double getVal(IDalitzEvent& evt)=0;
    virtual std::complex<double> getNewVal(IDalitzEvent& evt)=0;
    
    virtual const DecayTree& exampleDecay()=0;
    virtual std::string name() const{
        return "SpinFactor4:SF_BtoV1V0_V1toV2P1_V2toP2P3_BASE(" 
        + theBareDecay().oneLiner() + ")";
    }
    
};
// Spin Factor 6
class SF_BtoV1V0_V1toV2P1_V2toP2P3 : public SF_BtoV1V0_V1toV2P1_V2toP2P3_BASE{
protected:
    int _pol;
    static DecayTree* _exampleDecay;
public:
    virtual void printYourself(std::ostream& os=std::cout) const;
    SF_BtoV1V0_V1toV2P1_V2toP2P3( const DecayTree& theDecay,const int pol) 
    : SF_BtoV1V0_V1toV2P1_V2toP2P3_BASE(theDecay), _pol(pol){}
    
    virtual ~SF_BtoV1V0_V1toV2P1_V2toP2P3(){}
    virtual double getVal(IDalitzEvent& evt){return getNewVal(evt).real();}
    virtual std::complex<double> getNewVal(IDalitzEvent& evt);
    
    static const DecayTree& getExampleDecay();
    virtual const DecayTree& exampleDecay();
    virtual std::string name() const{
        return "SpinFactor4:SF_BtoV1V0_V1toV2P1_V2toP2P3(" 
        + theBareDecay().oneLiner() + ")";
    }
    int getPolarisation() {return _pol;}
};


//=========================================================
//SF 7-8

class SF_BtoPseudoTV0_PseudoTtoVP1_VtoP2P3_BASE : public SpinFactor{
protected:
    // static DecayTree* _exampleDecay;
    MINT::const_counted_ptr<AssociatedDecayTree> PT, V;// *P1, *P2, *P3, *P4;
    
    virtual bool parseTree(const DalitzEventPattern& pat);
    
public:
    virtual void printYourself(std::ostream& os=std::cout) const=0;
    SF_BtoPseudoTV0_PseudoTtoVP1_VtoP2P3_BASE( const DecayTree& theDecay) 
    : SpinFactor(theDecay, 4), PT(0), V(0){}
    
    virtual ~SF_BtoPseudoTV0_PseudoTtoVP1_VtoP2P3_BASE(){}
    virtual double getVal(IDalitzEvent& evt)=0;
    virtual std::complex<double> getNewVal(IDalitzEvent& evt)=0;
    
    virtual const DecayTree& exampleDecay()=0;
    virtual std::string name() const{
        return "SpinFactor4:SF_BtoPseudoTV0_PseudoTtoVP1_VtoP2P3_BASE(" 
        + theBareDecay().oneLiner() + ")";
    }
    
};

// SF 7
class SF_BtoPseudoTV0_PseudoTtoVP1_VtoP2P3_P : public SF_BtoPseudoTV0_PseudoTtoVP1_VtoP2P3_BASE{
protected:
    int _pol;
    static DecayTree* _exampleDecayP;
public:
    virtual void printYourself(std::ostream& os=std::cout) const;
    SF_BtoPseudoTV0_PseudoTtoVP1_VtoP2P3_P( const DecayTree& theDecay,const int pol) 
    : SF_BtoPseudoTV0_PseudoTtoVP1_VtoP2P3_BASE(theDecay), _pol(pol){}
    
    virtual ~SF_BtoPseudoTV0_PseudoTtoVP1_VtoP2P3_P(){}
    virtual double getVal(IDalitzEvent& evt){return getNewVal(evt).real();}
    virtual std::complex<double> getNewVal(IDalitzEvent& evt);
    
    static const DecayTree& getExampleDecay();
    virtual const DecayTree& exampleDecay();
    virtual std::string name() const{
        return "SpinFactor4:SF_BtoPseudoTV0_PseudoTtoVP1_VtoP2P3_P(" 
        + theBareDecay().oneLiner() + ")";
    }
    int getPolarisation() {return _pol;}

};

// SF 8
class SF_BtoPseudoTV0_PseudoTtoVP1_VtoP2P3_D : public SF_BtoPseudoTV0_PseudoTtoVP1_VtoP2P3_BASE{
protected:
    int _pol;
    static DecayTree* _exampleDecayD;
public:
    virtual void printYourself(std::ostream& os=std::cout) const;
    SF_BtoPseudoTV0_PseudoTtoVP1_VtoP2P3_D( const DecayTree& theDecay,const int pol) 
    : SF_BtoPseudoTV0_PseudoTtoVP1_VtoP2P3_BASE(theDecay), _pol(pol){}
    
    virtual ~SF_BtoPseudoTV0_PseudoTtoVP1_VtoP2P3_D(){}
    virtual double getVal(IDalitzEvent& evt){return getNewVal(evt).real();}
    virtual std::complex<double> getNewVal(IDalitzEvent& evt);
    
    static const DecayTree& getExampleDecay();
    virtual const DecayTree& exampleDecay();
    virtual std::string name() const{
        return "SpinFactor4:SF_BtoPseudoTV0_PseudoTtoVP1_VtoP2P3_D(" 
        + theBareDecay().oneLiner() + ")";
    }
    int getPolarisation() {return _pol;}

};

//=========================================================
//SF 9

class SF_BtoPseudoTV0_PseudoTtoSP1_StoP2P3_BASE : public SpinFactor{
protected:
    // static DecayTree* _exampleDecay;
    MINT::const_counted_ptr<AssociatedDecayTree> PT, S;// *P1, *P2, *P3, *P4;
    
    virtual bool parseTree(const DalitzEventPattern& pat);
    
public:
    virtual void printYourself(std::ostream& os=std::cout) const=0;
    SF_BtoPseudoTV0_PseudoTtoSP1_StoP2P3_BASE( const DecayTree& theDecay) 
    : SpinFactor(theDecay, 4) , PT(0), S(0){}
    
    virtual ~SF_BtoPseudoTV0_PseudoTtoSP1_StoP2P3_BASE(){}
    virtual double getVal(IDalitzEvent& evt)=0;
    virtual std::complex<double> getNewVal(IDalitzEvent& evt)=0;
    
    virtual const DecayTree& exampleDecay()=0;
    virtual std::string name() const{
        return "SpinFactor4:SF_BtoPseudoTV0_PseudoTtoSP1_StoP2P3_BASE(" 
        + theBareDecay().oneLiner() + ")";
    }
    
};

class SF_BtoPseudoTV0_PseudoTtoSP1_StoP2P3_P : public SF_BtoPseudoTV0_PseudoTtoSP1_StoP2P3_BASE{
protected:
    int _pol;
    static DecayTree* _exampleDecayP;
public:
    virtual void printYourself(std::ostream& os=std::cout) const;
    SF_BtoPseudoTV0_PseudoTtoSP1_StoP2P3_P( const DecayTree& theDecay,const int pol) 
    : SF_BtoPseudoTV0_PseudoTtoSP1_StoP2P3_BASE(theDecay), _pol(pol){}
    
    virtual ~SF_BtoPseudoTV0_PseudoTtoSP1_StoP2P3_P(){}
    virtual double getVal(IDalitzEvent& evt){return getNewVal(evt).real();}
    virtual std::complex<double> getNewVal(IDalitzEvent& evt);
    
    static const DecayTree& getExampleDecay();
    virtual const DecayTree& exampleDecay();
    virtual std::string name() const{
        return "SpinFactor4:SF_BtoPseudoTV0_PseudoTtoSP1_StoP2P3_P(" 
        + theBareDecay().oneLiner() + ")";
    }
    int getPolarisation() {return _pol;}

};

//----------------------------------------
//SF 10

class SF_BtoPseudoTV0_PseudoTtoTP1_TtoP2P3_BASE : public SpinFactor{
protected:
    // static DecayTree* _exampleDecay;
    MINT::const_counted_ptr<AssociatedDecayTree> PT, T;// *P1, *P2, *P3, *P4;
    
    virtual bool parseTree(const DalitzEventPattern& pat);
    
public:
    virtual void printYourself(std::ostream& os=std::cout) const=0;
    SF_BtoPseudoTV0_PseudoTtoTP1_TtoP2P3_BASE( const DecayTree& theDecay) 
    : SpinFactor(theDecay, 4), PT(0), T(0) {}
    
    virtual ~SF_BtoPseudoTV0_PseudoTtoTP1_TtoP2P3_BASE(){}
    virtual double getVal(IDalitzEvent& evt)=0;
    virtual std::complex<double> getNewVal(IDalitzEvent& evt)=0;
    
    virtual const DecayTree& exampleDecay()=0;
    virtual std::string name() const{
        return "SpinFactor4:SF_BtoPseudoTV0_PseudoTtoTP1_TtoP2P3_BASE(" 
        + theBareDecay().oneLiner() + ")";
    }
    
};

class SF_BtoPseudoTV0_PseudoTtoTP1_TtoP2P3_P : public SF_BtoPseudoTV0_PseudoTtoTP1_TtoP2P3_BASE{
protected:
    int _pol;
    static DecayTree* _exampleDecayP;
public:
    virtual void printYourself(std::ostream& os=std::cout) const;
    SF_BtoPseudoTV0_PseudoTtoTP1_TtoP2P3_P( const DecayTree& theDecay,const int pol) 
    : SF_BtoPseudoTV0_PseudoTtoTP1_TtoP2P3_BASE(theDecay), _pol(pol){}
    
    virtual ~SF_BtoPseudoTV0_PseudoTtoTP1_TtoP2P3_P(){}
    virtual double getVal(IDalitzEvent& evt){return getNewVal(evt).real();}
    virtual std::complex<double> getNewVal(IDalitzEvent& evt);
    
    static const DecayTree& getExampleDecay();
    virtual const DecayTree& exampleDecay();
    virtual std::string name() const{
        return "SpinFactor4:SF_BtoPseudoTV0_PseudoTtoTP1_TtoP2P3_P(" 
        + theBareDecay().oneLiner() + ")";
    }
    int getPolarisation() {return _pol;}
};

//----------------------------------------
//SF 11

class SF_BtoTV0_TtoVP1_VtoP2P3_BASE : public SpinFactor{
protected:
    // static DecayTree* _exampleDecay;
    MINT::const_counted_ptr<AssociatedDecayTree> T, V;// *P1, *P2, *P3, *P4;
    
    virtual bool parseTree(const DalitzEventPattern& pat);
    
public:
    virtual void printYourself(std::ostream& os=std::cout) const=0;
    SF_BtoTV0_TtoVP1_VtoP2P3_BASE( const DecayTree& theDecay) 
    : SpinFactor(theDecay, 4), T(0), V(0){}
    
    virtual ~SF_BtoTV0_TtoVP1_VtoP2P3_BASE(){}
    virtual double getVal(IDalitzEvent& evt)=0;
    virtual std::complex<double> getNewVal(IDalitzEvent& evt)=0;
    
    virtual const DecayTree& exampleDecay()=0;
    virtual std::string name() const{
        return "SpinFactor4:SF_BtoTV0_TtoVP1_VtoP2P3_BASE(" 
        + theBareDecay().oneLiner() + ")";
    }
    
};

class SF_BtoTV0_TtoVP1_VtoP2P3_P : public SF_BtoTV0_TtoVP1_VtoP2P3_BASE{
protected:
    int _pol;
    static DecayTree* _exampleDecayP;
public:
    virtual void printYourself(std::ostream& os=std::cout) const;
    SF_BtoTV0_TtoVP1_VtoP2P3_P( const DecayTree& theDecay,const int pol) 
    : SF_BtoTV0_TtoVP1_VtoP2P3_BASE(theDecay), _pol(pol){}
    
    virtual ~SF_BtoTV0_TtoVP1_VtoP2P3_P(){}
    virtual double getVal(IDalitzEvent& evt){return getNewVal(evt).real();}
    virtual std::complex<double> getNewVal(IDalitzEvent& evt);
    
    static const DecayTree& getExampleDecay();
    virtual const DecayTree& exampleDecay();
    virtual std::string name() const{
        return "SpinFactor4:SF_SF_BtoTV0_TtoVP1_VtoP2P3_P(" 
        + theBareDecay().oneLiner() + ")";
    }
    int getPolarisation() {return _pol;}
};


// ==========================================
// Spin Factor 12/13

class SF_BtoVP1_VtoV0S_StoP2P3_BASE : public SpinFactor{
protected:
    // static DecayTree* _exampleDecay;
    MINT::const_counted_ptr<AssociatedDecayTree> S, V;// *P1, *P2, *P3, *P4;
    
    virtual bool parseTree(const DalitzEventPattern& pat);
    
public:
    virtual void printYourself(std::ostream& os=std::cout) const=0;
    SF_BtoVP1_VtoV0S_StoP2P3_BASE( const DecayTree& theDecay) 
    : SpinFactor(theDecay, 4), S(0), V(0){}
    
    virtual ~SF_BtoVP1_VtoV0S_StoP2P3_BASE(){}
    virtual double getVal(IDalitzEvent& evt)=0;
    virtual std::complex<double> getNewVal(IDalitzEvent& evt)=0;
    
    virtual const DecayTree& exampleDecay()=0;
    virtual std::string name() const{
        return "SpinFactor4:SF_BtoVP1_VtoV0S_StoP2P3_BASE(" 
        + theBareDecay().oneLiner() + ")";
    }
    
};

// Spin Factor 12/13
class SF_BtoVP1_VtoV0S_StoP2P3 : public SF_BtoVP1_VtoV0S_StoP2P3_BASE{
protected:
    int _pol;
    bool _is3bodyDecay;
    static DecayTree* _exampleDecay;
public:
    virtual void printYourself(std::ostream& os=std::cout) const;
    SF_BtoVP1_VtoV0S_StoP2P3( const DecayTree& theDecay,const int pol, const bool is3body) 
    : SF_BtoVP1_VtoV0S_StoP2P3_BASE(theDecay), _pol(pol), _is3bodyDecay(is3body){}
    
    virtual ~SF_BtoVP1_VtoV0S_StoP2P3(){}
    virtual double getVal(IDalitzEvent& evt){return getNewVal(evt).real();}
    virtual std::complex<double> getNewVal(IDalitzEvent& evt);
    
    static const DecayTree& getExampleDecay();
    virtual const DecayTree& exampleDecay();
    virtual std::string name() const{
        return "SpinFactor4:SF_BtoVP1_VtoV0S_StoP2P3(" 
        + theBareDecay().oneLiner() + ")";
    }
    int getPolarisation() {return _pol;}
    bool is3bodyDecay() {return _is3bodyDecay;}
};

// ==========================================
// Spin Factor 14

class SF_BtoV1P1_V1toV2V0_V2toP2P3_BASE : public SpinFactor{
protected:
    // static DecayTree* _exampleDecay;
    MINT::const_counted_ptr<AssociatedDecayTree> V1, V2;// *P1, *P2, *P3, *P4;
    
    virtual bool parseTree(const DalitzEventPattern& pat);
    
public:
    virtual void printYourself(std::ostream& os=std::cout) const=0;
    SF_BtoV1P1_V1toV2V0_V2toP2P3_BASE( const DecayTree& theDecay) 
    : SpinFactor(theDecay, 4), V1(0), V2(0){}
    
    virtual ~SF_BtoV1P1_V1toV2V0_V2toP2P3_BASE(){}
    virtual double getVal(IDalitzEvent& evt)=0;
    virtual std::complex<double> getNewVal(IDalitzEvent& evt)=0;
    
    virtual const DecayTree& exampleDecay()=0;
    virtual std::string name() const{
        return "SpinFactor4:SF_BtoV1P1_V1toV2V0_V2toP2P3_BASE(" 
        + theBareDecay().oneLiner() + ")";
    }
    
};

// Spin Factor 14
class SF_BtoV1P1_V1toV2V0_V2toP2P3 : public SF_BtoV1P1_V1toV2V0_V2toP2P3_BASE{
protected:
    int _pol;
    int _LS_s;
    static DecayTree* _exampleDecay;
public:
    virtual void printYourself(std::ostream& os=std::cout) const;
    SF_BtoV1P1_V1toV2V0_V2toP2P3( const DecayTree& theDecay,const int pol, const int LS_s=0) 
    : SF_BtoV1P1_V1toV2V0_V2toP2P3_BASE(theDecay), _pol(pol), _LS_s(LS_s){}
    
    virtual ~SF_BtoV1P1_V1toV2V0_V2toP2P3(){}
    virtual double getVal(IDalitzEvent& evt){return getNewVal(evt).real();}
    virtual std::complex<double> getNewVal(IDalitzEvent& evt);
    
    static const DecayTree& getExampleDecay();
    virtual const DecayTree& exampleDecay();
    virtual std::string name() const{
        return "SpinFactor4:SF_BtoV1P1_V1toV2V0_V2toP2P3(" 
        + theBareDecay().oneLiner() + ")";
    }
    int getPolarisation() {return _pol;}
    int getLS_S() {return _LS_s;}
};



// ==========================================
// Spin Factor 15/16

class SF_BtoAP1_AtoVV0_VtoP2P3_BASE : public SpinFactor{
protected:
    // static DecayTree* _exampleDecay;
    MINT::const_counted_ptr<AssociatedDecayTree> A, V;// *P1, *P2, *P3, *P4;
    
    virtual bool parseTree(const DalitzEventPattern& pat);
    
public:
    virtual void printYourself(std::ostream& os=std::cout) const=0;
    SF_BtoAP1_AtoVV0_VtoP2P3_BASE( const DecayTree& theDecay) 
    : SpinFactor(theDecay, 4), A(0), V(0){}
    
    virtual ~SF_BtoAP1_AtoVV0_VtoP2P3_BASE(){}
    virtual double getVal(IDalitzEvent& evt)=0;
    virtual std::complex<double> getNewVal(IDalitzEvent& evt)=0;
    
    virtual const DecayTree& exampleDecay()=0;
    virtual std::string name() const{
        return "SpinFactor4:SF_BtoAP1_AtoVV0_VtoP2P3_BASE(" 
        + theBareDecay().oneLiner() + ")";
    }
    
};

// Spin Factor 15/16
class SF_BtoAP1_AtoVV0_VtoP2P3 : public SF_BtoAP1_AtoVV0_VtoP2P3_BASE{
protected:
    int _pol;
    bool _is3bodyDecay;
    static DecayTree* _exampleDecay;
public:
    virtual void printYourself(std::ostream& os=std::cout) const;
    SF_BtoAP1_AtoVV0_VtoP2P3( const DecayTree& theDecay,const int pol, const bool is3body) 
    : SF_BtoAP1_AtoVV0_VtoP2P3_BASE(theDecay), _pol(pol), _is3bodyDecay(is3body){}
    
    virtual ~SF_BtoAP1_AtoVV0_VtoP2P3(){}
    virtual double getVal(IDalitzEvent& evt){return getNewVal(evt).real();}
    virtual std::complex<double> getNewVal(IDalitzEvent& evt);
    
    static const DecayTree& getExampleDecay();
    virtual const DecayTree& exampleDecay();
    virtual std::string name() const{
        return "SpinFactor4:SF_BtoAP1_AtoVV0_VtoP2P3(" 
        + theBareDecay().oneLiner() + ")";
    }
    int getPolarisation() {return _pol;}
    bool is3bodyDecay() {return _is3bodyDecay;}
};



// ==========================================
// Spin Factor 17
class SF_BtoV1V2_V1toP1V0_V2toP2P3_BASE : public SpinFactor{
protected:
    // static DecayTree* _exampleDecay;
    MINT::const_counted_ptr<AssociatedDecayTree> V1, V2;// *P1, *P2, *P3, *P4;
    
    virtual bool parseTree(const DalitzEventPattern& pat);
    
public:
    virtual void printYourself(std::ostream& os=std::cout) const=0;
    SF_BtoV1V2_V1toP1V0_V2toP2P3_BASE( const DecayTree& theDecay) 
    : SpinFactor(theDecay, 4), V1(0), V2(0){}
    
    virtual ~SF_BtoV1V2_V1toP1V0_V2toP2P3_BASE(){}
    virtual double getVal(IDalitzEvent& evt)=0;
    virtual std::complex<double> getNewVal(IDalitzEvent& evt)=0;
    
    virtual const DecayTree& exampleDecay()=0;
    virtual std::string name() const{
        return "SpinFactor4:SF_BtoV1V2_V1toP1V0_V2toP2P3_BASE(" 
        + theBareDecay().oneLiner() + ")";
    }
    
};
// Spin Factor 17
class SF_BtoV1V2_V1toP1V0_V2toP2P3 : public SF_BtoV1V2_V1toP1V0_V2toP2P3_BASE{
protected:
    int _pol;
    static DecayTree* _exampleDecay;
public:
    virtual void printYourself(std::ostream& os=std::cout) const;
    SF_BtoV1V2_V1toP1V0_V2toP2P3( const DecayTree& theDecay,const int pol) 
    : SF_BtoV1V2_V1toP1V0_V2toP2P3_BASE(theDecay), _pol(pol){}
    
    virtual ~SF_BtoV1V2_V1toP1V0_V2toP2P3(){}
    virtual double getVal(IDalitzEvent& evt){return getNewVal(evt).real();}
    virtual std::complex<double> getNewVal(IDalitzEvent& evt);
    
    static const DecayTree& getExampleDecay();
    virtual const DecayTree& exampleDecay();
    virtual std::string name() const{
        return "SpinFactor4:SF_BtoV1V2_V1toP1V0_V2toP2P3(" 
        + theBareDecay().oneLiner() + ")";
    }
    int getPolarisation() {return _pol;}
};

// ==========================================
// Spin Factor 18
class SF_BtoAV_AtoP1V0_VtoP2P3_BASE : public SpinFactor{
protected:
    // static DecayTree* _exampleDecay;
    MINT::const_counted_ptr<AssociatedDecayTree> A, V;// *P1, *P2, *P3, *P4;
    
    virtual bool parseTree(const DalitzEventPattern& pat);
    
public:
    virtual void printYourself(std::ostream& os=std::cout) const=0;
    SF_BtoAV_AtoP1V0_VtoP2P3_BASE( const DecayTree& theDecay) 
    : SpinFactor(theDecay, 4), A(0), V(0){}
    
    virtual ~SF_BtoAV_AtoP1V0_VtoP2P3_BASE(){}
    virtual double getVal(IDalitzEvent& evt)=0;
    virtual std::complex<double> getNewVal(IDalitzEvent& evt)=0;
    
    virtual const DecayTree& exampleDecay()=0;
    virtual std::string name() const{
        return "SpinFactor4:SF_BtoAV_AtoP1V0_VtoP2P3_BASE(" 
        + theBareDecay().oneLiner() + ")";
    }
    
};

// Spin Factor 18
class SF_BtoAV_AtoP1V0_VtoP2P3 : public SF_BtoAV_AtoP1V0_VtoP2P3_BASE{
protected:
    int _pol;
    static DecayTree* _exampleDecay;
public:
    virtual void printYourself(std::ostream& os=std::cout) const;
    SF_BtoAV_AtoP1V0_VtoP2P3( const DecayTree& theDecay,const int pol) 
    : SF_BtoAV_AtoP1V0_VtoP2P3_BASE(theDecay), _pol(pol){}
    
    virtual ~SF_BtoAV_AtoP1V0_VtoP2P3(){}
    virtual double getVal(IDalitzEvent& evt){return getNewVal(evt).real();}
    virtual std::complex<double> getNewVal(IDalitzEvent& evt);
    
    static const DecayTree& getExampleDecay();
    virtual const DecayTree& exampleDecay();
    virtual std::string name() const{
        return "SpinFactor4:SF_BtoAV_AtoP1V0_VtoP2P3(" 
        + theBareDecay().oneLiner() + ")";
    }
    int getPolarisation() {return _pol;}
};

// ==========================================
// Spin Factor 19
class SF_BtoAS_AtoP1V0_StoP2P3_BASE : public SpinFactor{
protected:
    // static DecayTree* _exampleDecay;
    MINT::const_counted_ptr<AssociatedDecayTree> A, S;// *P1, *P2, *P3, *P4;
    
    virtual bool parseTree(const DalitzEventPattern& pat);
    
public:
    virtual void printYourself(std::ostream& os=std::cout) const=0;
    SF_BtoAS_AtoP1V0_StoP2P3_BASE( const DecayTree& theDecay) 
    : SpinFactor(theDecay, 4), A(0), S(0){}
    
    virtual ~SF_BtoAS_AtoP1V0_StoP2P3_BASE(){}
    virtual double getVal(IDalitzEvent& evt)=0;
    virtual std::complex<double> getNewVal(IDalitzEvent& evt)=0;
    
    virtual const DecayTree& exampleDecay()=0;
    virtual std::string name() const{
        return "SpinFactor4:SF_BtoAS_AtoP1V0_StoP2P3_BASE(" 
        + theBareDecay().oneLiner() + ")";
    }
    
};

// Spin Factor 19
class SF_BtoAS_AtoP1V0_StoP2P3 : public SF_BtoAS_AtoP1V0_StoP2P3_BASE{
protected:
    int _pol;
    static DecayTree* _exampleDecay;
public:
    virtual void printYourself(std::ostream& os=std::cout) const;
    SF_BtoAS_AtoP1V0_StoP2P3( const DecayTree& theDecay,const int pol) 
    : SF_BtoAS_AtoP1V0_StoP2P3_BASE(theDecay), _pol(pol){}
    
    virtual ~SF_BtoAS_AtoP1V0_StoP2P3(){}
    virtual double getVal(IDalitzEvent& evt){return getNewVal(evt).real();}
    virtual std::complex<double> getNewVal(IDalitzEvent& evt);
    
    static const DecayTree& getExampleDecay();
    virtual const DecayTree& exampleDecay();
    virtual std::string name() const{
        return "SpinFactor4:SF_BtoAS_AtoP1V0_StoP2P3(" 
        + theBareDecay().oneLiner() + ")";
    }
    int getPolarisation() {return _pol;}
};

// ==========================================
// Spin Factor 20
class SF_BtoPS_PtoP1V0_StoP2P3_BASE : public SpinFactor{
protected:
    // static DecayTree* _exampleDecay;
    MINT::const_counted_ptr<AssociatedDecayTree> P, S;// *P1, *P2, *P3, *P4;
    
    virtual bool parseTree(const DalitzEventPattern& pat);
    
public:
    virtual void printYourself(std::ostream& os=std::cout) const=0;
    SF_BtoPS_PtoP1V0_StoP2P3_BASE( const DecayTree& theDecay) 
    : SpinFactor(theDecay, 4), P(0), S(0){}
    
    virtual ~SF_BtoPS_PtoP1V0_StoP2P3_BASE(){}
    virtual double getVal(IDalitzEvent& evt)=0;
    virtual std::complex<double> getNewVal(IDalitzEvent& evt)=0;
    
    virtual const DecayTree& exampleDecay()=0;
    virtual std::string name() const{
        return "SpinFactor4:SF_BtoPS_PtoP1V0_StoP2P3_BASE(" 
        + theBareDecay().oneLiner() + ")";
    }
    
};

// Spin Factor 20
class SF_BtoPS_PtoP1V0_StoP2P3 : public SF_BtoPS_PtoP1V0_StoP2P3_BASE{
protected:
    int _pol;
    static DecayTree* _exampleDecay;
public:
    virtual void printYourself(std::ostream& os=std::cout) const;
    SF_BtoPS_PtoP1V0_StoP2P3( const DecayTree& theDecay,const int pol) 
    : SF_BtoPS_PtoP1V0_StoP2P3_BASE(theDecay), _pol(pol){}
    
    virtual ~SF_BtoPS_PtoP1V0_StoP2P3(){}
    virtual double getVal(IDalitzEvent& evt){return getNewVal(evt).real();}
    virtual std::complex<double> getNewVal(IDalitzEvent& evt);
    
    static const DecayTree& getExampleDecay();
    virtual const DecayTree& exampleDecay();
    virtual std::string name() const{
        return "SpinFactor4:SF_BtoPS_PtoP1V0_StoP2P3(" 
        + theBareDecay().oneLiner() + ")";
    }
    int getPolarisation() {return _pol;}
};


// ==========================================
// Spin Factor 21
class SF_BtoPV_PtoP1V0_VtoP2P3_BASE : public SpinFactor{
protected:
    // static DecayTree* _exampleDecay;
    MINT::const_counted_ptr<AssociatedDecayTree> P, V;// *P1, *P2, *P3, *P4;
    
    virtual bool parseTree(const DalitzEventPattern& pat);
    
public:
    virtual void printYourself(std::ostream& os=std::cout) const=0;
    SF_BtoPV_PtoP1V0_VtoP2P3_BASE( const DecayTree& theDecay) 
    : SpinFactor(theDecay, 4), P(0), V(0){}
    
    virtual ~SF_BtoPV_PtoP1V0_VtoP2P3_BASE(){}
    virtual double getVal(IDalitzEvent& evt)=0;
    virtual std::complex<double> getNewVal(IDalitzEvent& evt)=0;
    
    virtual const DecayTree& exampleDecay()=0;
    virtual std::string name() const{
        return "SpinFactor4:SF_BtoPV_PtoP1V0_VtoP2P3_BASE(" 
        + theBareDecay().oneLiner() + ")";
    }
    
};

class SF_BtoPV_PtoP1V0_VtoP2P3 : public SF_BtoPV_PtoP1V0_VtoP2P3_BASE{
protected:
    int _pol;
    static DecayTree* _exampleDecay;
public:
    virtual void printYourself(std::ostream& os=std::cout) const;
    SF_BtoPV_PtoP1V0_VtoP2P3( const DecayTree& theDecay,const int pol) 
    : SF_BtoPV_PtoP1V0_VtoP2P3_BASE(theDecay), _pol(pol){}
    
    virtual ~SF_BtoPV_PtoP1V0_VtoP2P3(){}
    virtual double getVal(IDalitzEvent& evt){return getNewVal(evt).real();}
    virtual std::complex<double> getNewVal(IDalitzEvent& evt);
    
    static const DecayTree& getExampleDecay();
    virtual const DecayTree& exampleDecay();
    virtual std::string name() const{
        return "SpinFactor4:SF_BtoPV_PtoP1V0_VtoP2P3(" 
        + theBareDecay().oneLiner() + ")";
    }
    int getPolarisation() {return _pol;}
};

// ==========================================
// Spin Factor 22
class SF_BtoPV0_PtoS_StoP2P3_BASE : public SpinFactor{
protected:
    // static DecayTree* _exampleDecay;
    MINT::const_counted_ptr<AssociatedDecayTree> P, S;// *P1, *P2, *P3, *P4;
    
    virtual bool parseTree(const DalitzEventPattern& pat);
    
public:
    virtual void printYourself(std::ostream& os=std::cout) const=0;
    SF_BtoPV0_PtoS_StoP2P3_BASE( const DecayTree& theDecay) 
    : SpinFactor(theDecay, 4), P(0), S(0){}
    
    virtual ~SF_BtoPV0_PtoS_StoP2P3_BASE(){}
    virtual double getVal(IDalitzEvent& evt)=0;
    virtual std::complex<double> getNewVal(IDalitzEvent& evt)=0;
    
    virtual const DecayTree& exampleDecay()=0;
    virtual std::string name() const{
        return "SpinFactor4:SF_BtoPV0_PtoS_StoP2P3_BASE(" 
        + theBareDecay().oneLiner() + ")";
    }
    
};

class SF_BtoPV0_PtoS_StoP2P3 : public SF_BtoPV0_PtoS_StoP2P3_BASE{
protected:
    int _pol;
    static DecayTree* _exampleDecay;
public:
    virtual void printYourself(std::ostream& os=std::cout) const;
    SF_BtoPV0_PtoS_StoP2P3( const DecayTree& theDecay,const int pol) 
    : SF_BtoPV0_PtoS_StoP2P3_BASE(theDecay), _pol(pol){}
    
    virtual ~SF_BtoPV0_PtoS_StoP2P3(){}
    virtual double getVal(IDalitzEvent& evt){return getNewVal(evt).real();}
    virtual std::complex<double> getNewVal(IDalitzEvent& evt);
    
    static const DecayTree& getExampleDecay();
    virtual const DecayTree& exampleDecay();
    virtual std::string name() const{
        return "SpinFactor4:SF_BtoPV0_PtoS_StoP2P3(" 
        + theBareDecay().oneLiner() + ")";
    }
    int getPolarisation() {return _pol;}
};

// ==========================================
// Spin Factor 23

class SF_BtoVP1_VtoAP2_AtoV0P3_BASE : public SpinFactor{
protected:
    // static DecayTree* _exampleDecay;
    MINT::const_counted_ptr<AssociatedDecayTree> V, A;// *P1, *P2, *P3, *P4;
    
    virtual bool parseTree(const DalitzEventPattern& pat);
    
public:
    virtual void printYourself(std::ostream& os=std::cout) const=0;
    SF_BtoVP1_VtoAP2_AtoV0P3_BASE(const DecayTree& theDecay) 
    : SpinFactor(theDecay, 4)
    , A(0), V(0)
    {
        //if( ! parseTree(const DalitzEventPattern& pat)){
        //    throw "error in parseTree in constructor of SF_BtoAV0_AtoVP1_VtoP2P3_BASE";
        //}
        //printYourself();
    }
    
    virtual ~SF_BtoVP1_VtoAP2_AtoV0P3_BASE(){}
    virtual double getVal(IDalitzEvent& evt)=0;
    virtual std::complex<double> getNewVal(IDalitzEvent& evt)=0;
    
    virtual const DecayTree& exampleDecay()=0;
    virtual std::string name() const{
        return "SpinFactor4:SF_BtoVP1_VtoAP2_AtoV0P3_BASE(" 
        + theBareDecay().oneLiner() + ")";
    }
    
};


class SF_BtoVP1_VtoAP2_AtoV0P3 : public SF_BtoVP1_VtoAP2_AtoV0P3_BASE{
    //int _pol;
protected:
    const int _pol;
    static DecayTree* _exampleDecay;
    //static int _pol;  
public:
    virtual void printYourself(std::ostream& os=std::cout) const;
    SF_BtoVP1_VtoAP2_AtoV0P3(const DecayTree& theDecay, const int pol) 
    : SF_BtoVP1_VtoAP2_AtoV0P3_BASE(theDecay), _pol(pol)
    {
        //  if( ! parseTree(const DalitzEventPattern& pat)){
        //throw "error in parseTree in constructor of SF_BtoAV0_AtoVP1_VtoP2P3";
        //  }
        // _pol=pol;  
        //      printYourself();
    }
    
    virtual ~SF_BtoVP1_VtoAP2_AtoV0P3(){}
    virtual double getVal(IDalitzEvent& evt){return getNewVal(evt).real();}
    virtual std::complex<double> getNewVal(IDalitzEvent& evt);
    
    static const DecayTree& getExampleDecay();
    virtual const DecayTree& exampleDecay();
    virtual std::string name() const{
        return "SpinFactor4:SF_BtoVP1_VtoAP2_AtoV0P3(" 
        + theBareDecay().oneLiner() + ")";
    }
    const int getPolarisation() {return _pol;}
    
    
};

// ==========================================
// Spin Factor 24

class SF_BtoVP1_VtoSV0_StoP2P3_BASE : public SpinFactor{
protected:
    // static DecayTree* _exampleDecay;
    MINT::const_counted_ptr<AssociatedDecayTree> V, S;// *P1, *P2, *P3, *P4;
    
    virtual bool parseTree(const DalitzEventPattern& pat);
    
public:
    virtual void printYourself(std::ostream& os=std::cout) const=0;
    SF_BtoVP1_VtoSV0_StoP2P3_BASE(const DecayTree& theDecay) 
    : SpinFactor(theDecay, 4)
    , V(0), S(0)
    {
        //if( ! parseTree(const DalitzEventPattern& pat)){
        //    throw "error in parseTree in constructor of SF_BtoAV0_AtoVP1_VtoP2P3_BASE";
        //}
        //printYourself();
    }
    
    virtual ~SF_BtoVP1_VtoSV0_StoP2P3_BASE(){}
    virtual double getVal(IDalitzEvent& evt)=0;
    virtual std::complex<double> getNewVal(IDalitzEvent& evt)=0;
    
    virtual const DecayTree& exampleDecay()=0;
    virtual std::string name() const{
        return "SpinFactor4:SF_BtoVP1_VtoSV0_StoP2P3_BASE(" 
        + theBareDecay().oneLiner() + ")";
    }
    
};


class SF_BtoVP1_VtoSV0_StoP2P3 : public SF_BtoVP1_VtoSV0_StoP2P3_BASE{
    //int _pol;
protected:
    const int _pol;
    static DecayTree* _exampleDecay;
    //static int _pol;  
public:
    virtual void printYourself(std::ostream& os=std::cout) const;
    SF_BtoVP1_VtoSV0_StoP2P3(const DecayTree& theDecay, const int pol) 
    : SF_BtoVP1_VtoSV0_StoP2P3_BASE(theDecay), _pol(pol)
    {
        //  if( ! parseTree(const DalitzEventPattern& pat)){
        //throw "error in parseTree in constructor of SF_BtoAV0_AtoVP1_VtoP2P3";
        //  }
        // _pol=pol;  
        //      printYourself();
    }
    
    virtual ~SF_BtoVP1_VtoSV0_StoP2P3(){}
    virtual double getVal(IDalitzEvent& evt){return getNewVal(evt).real();}
    virtual std::complex<double> getNewVal(IDalitzEvent& evt);
    
    static const DecayTree& getExampleDecay();
    virtual const DecayTree& exampleDecay();
    virtual std::string name() const{
        return "SpinFactor4:SF_BtoVP1_VtoSV0_StoP2P3(" 
        + theBareDecay().oneLiner() + ")";
    }
    const int getPolarisation() {return _pol;}
    
    
};


// ==========================================
// Spin Factor 25

class SF_BtoPV0_PtoP1V1_V1toP2P3_BASE : public SpinFactor{
protected:
    // static DecayTree* _exampleDecay;
    MINT::const_counted_ptr<AssociatedDecayTree> V, P;// *P1, *P2, *P3, *P4;
    
    virtual bool parseTree(const DalitzEventPattern& pat);
    
public:
    virtual void printYourself(std::ostream& os=std::cout) const=0;
    SF_BtoPV0_PtoP1V1_V1toP2P3_BASE(const DecayTree& theDecay) 
    : SpinFactor(theDecay, 4)
    , V(0), P(0)
    {
        //if( ! parseTree(const DalitzEventPattern& pat)){
        //    throw "error in parseTree in constructor of SF_BtoAV0_AtoVP1_VtoP2P3_BASE";
        //}
        //printYourself();
    }
    
    virtual ~SF_BtoPV0_PtoP1V1_V1toP2P3_BASE(){}
    virtual double getVal(IDalitzEvent& evt)=0;
    virtual std::complex<double> getNewVal(IDalitzEvent& evt)=0;
    
    virtual const DecayTree& exampleDecay()=0;
    virtual std::string name() const{
        return "SpinFactor4:SF_BtoPV0_PtoP1V1_V1toP2P3_BASE(" 
        + theBareDecay().oneLiner() + ")";
    }
    
};


class SF_BtoPV0_PtoP1V1_V1toP2P3 : public SF_BtoPV0_PtoP1V1_V1toP2P3_BASE{
    //int _pol;
protected:
    const int _pol;
    static DecayTree* _exampleDecay;
    //static int _pol;  
public:
    virtual void printYourself(std::ostream& os=std::cout) const;
    SF_BtoPV0_PtoP1V1_V1toP2P3(const DecayTree& theDecay, const int pol) 
    : SF_BtoPV0_PtoP1V1_V1toP2P3_BASE(theDecay), _pol(pol)
    {
        //  if( ! parseTree(const DalitzEventPattern& pat)){
        //throw "error in parseTree in constructor of SF_BtoAV0_AtoVP1_VtoP2P3";
        //  }
        // _pol=pol;  
        //      printYourself();
    }
    
    virtual ~SF_BtoPV0_PtoP1V1_V1toP2P3(){}
    virtual double getVal(IDalitzEvent& evt){return getNewVal(evt).real();}
    virtual std::complex<double> getNewVal(IDalitzEvent& evt);
    
    static const DecayTree& getExampleDecay();
    virtual const DecayTree& exampleDecay();
    virtual std::string name() const{
        return "SpinFactor4:SF_BtoPV0_PtoP1V1_V1toP2P3(" 
        + theBareDecay().oneLiner() + ")";
    }
    const int getPolarisation() {return _pol;}
    
    
};


#endif
//
