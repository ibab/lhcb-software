// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Rec/Brunel/src/BrunelSicb/BrunelFortran.cpp,v 1.1.1.1 2001-03-14 16:06:45 cattaneb Exp $
#define SICBCNV_SICBFORTRAN_CPP

struct GCBANK_DEF;
struct CBLINK_DEF;
struct CBLINN_DEF;
GCBANK_DEF* __GCBANK = 0;
CBLINK_DEF* __CBLINK = 0;
CBLINN_DEF* __CBLINN = 0;

// Include files
#include <string>
//#include <iostream>
#include <cstring>
#include <cstdio>
#include "GaudiKernel/System.h"
#include "SicbCnv/SicbFortran.h"
#ifdef WIN32
  #define CALL_TYPE __cdecl
  namespace win {
    #include <windows.h>
  };
#else
  #define CALL_TYPE
  #include "dlfcn.h"
#endif

//------------------------------------------------------------------------------
//
// Implementation of interfaces to some FORTRAN subroutines from SICB
//
// Author :        Markus Frank
//
//------------------------------------------------------------------------------
namespace SicbFortran   {
  class SicbFunctions;

  class SicbFunctions    {
  public:
    bool m_isValid;
// The following functions are used for pile up of SICB data
    void (CALL_TYPE *INILUMI       )(int& iRet, float& LumPeak, float& fillDur, float& DecTime, float& BunchCrossRate, float& TotalCrossSect );
    void (CALL_TYPE *SETNPILEUP    )( const int& );
    void (CALL_TYPE *REMERGE       )( void );
    void (CALL_TYPE *DROPDIVISION  )( void );
    void (CALL_TYPE *READEVENT     )( int&, const int&, const int& );
    long (CALL_TYPE *OPENINPUTFILE )( const std::string&, const int& );
    void (CALL_TYPE *CLOSEINPUTFILE)( const int& );
    bool (CALL_TYPE *CHECKEVTQLTY  )( int&, int&, int&);
    long (CALL_TYPE *UBIN          )( const std::string&, int*  );
    long (CALL_TYPE *UDINIT        )( void );
    long (CALL_TYPE *UDINITEX      )( const std::string& );
    long (CALL_TYPE *UDLAST        )( void );
    void (CALL_TYPE *UTZEBRA       )( const int& );
    void (CALL_TYPE *UBFIND        )( const std::string&, int* );
    void (CALL_TYPE *UBINDEX       )( const std::string&, int* iret);
    void (CALL_TYPE *UBIDNAME      )( const int&, char* );
    void (CALL_TYPE *UBDROP        )( const std::string& );
    void (CALL_TYPE *UBNOBJ        )( const std::string&, const int&, int& );
    void (CALL_TYPE *UBPUTA        )( const std::string&, int*, int*, int*, int* );
    void (CALL_TYPE *UBPUTR        )( int*, int*, int*, int* );
    void (CALL_TYPE *UTDGET        )( const std::string&, const std::string&, int&, int* );
    void (CALL_TYPE *GAUDIGUFLD    )( const float*, float* );
	void (CALL_TYPE *GETZEBRASIZE  )( int& );
    GCBANK_DEF& (CALL_TYPE *GET_GCBANK)();
    CBLINK_DEF& (CALL_TYPE *GET_CBLINK)();
    CBLINN_DEF& (CALL_TYPE *GET_CBLINN)();
    
    SicbFunctions();
    System::ImageHandle loadFunctionLibrary(const char* which);
    bool load(System::ImageHandle handle, const char* name, void** pFunction);
    bool isValid()    const;
    static SicbFunctions* instance();
  };
  void init() {
    SicbFunctions::instance();
  }
  GCBANK_DEF* getGCBANK()  {
    return &SicbFunctions::instance()->GET_GCBANK();
  }
  CBLINK_DEF* getCBLINK()  {
    return &SicbFunctions::instance()->GET_CBLINK();
  }
  CBLINN_DEF* getCBLINN()  {
    return &SicbFunctions::instance()->GET_CBLINN();
  }
// The following functions are used for pile up of SICB data
  // interface to INILUMI
  void IniLumi(int& iRet, float& LumPeak, float& fillDur, float& DecTime, float& BunchCrossRate, float& TotalCrossSect){
    SicbFunctions::instance()->INILUMI(iRet, LumPeak, fillDur, DecTime, BunchCrossRate, TotalCrossSect);
  }
  // interface to SETNPILEUP
  void SetNPileUp(const int& nPileUp){
    SicbFunctions::instance()->SETNPILEUP(nPileUp);
  }
  // interface to REMERGE
  void ReMerge( void ){
    SicbFunctions::instance()->REMERGE( );
  }
  // interface to DROPDIVISION
  void DropDivision( void ){
    SicbFunctions::instance()->DROPDIVISION();
  }
  // Interface to UTZEBRA
  void utzebra     ( const int& size )            {
    SicbFunctions::instance()->UTZEBRA(size);
  }
  // Interface to READEVENT
  void ReadEvent ( int& iret, const int& iPileUp, const int& lUnit) {
    SicbFunctions::instance()->READEVENT(iret, iPileUp, lUnit);
  }
  // Interface to OPENINPUTFILE
  long OpenInputFile ( const std::string& fName, const int& lUnit ) {
    return SicbFunctions::instance()->OPENINPUTFILE(fName, lUnit);
  }
  // Interface to CLOSEINPUTFILE
  void CloseInputFile      (const int& lUnit) {
    SicbFunctions::instance()->CLOSEINPUTFILE(lUnit);
  }
  // Interface to CHECKEVTQ
  bool CheckEvtQlty ( int& iret, int& ievt, int& irun ) {
    return SicbFunctions::instance()->CHECKEVTQLTY(iret, ievt, irun);
  }
  // Interface to UBIN
  long ubin        ( const std::string& name, int* iret ) {
    return  SicbFunctions::instance()->UBIN(name, iret);
  }
  // Interface to UDINIT
  long udinit      ( void )            {
    return SicbFunctions::instance()->UDINIT();
  }
  // Interface to UDINIT
  long udinitEx    ( const std::string& name )  {
    return SicbFunctions::instance()->UDINITEX(name);
  }
  // Interface to UDLAST
  long udlast      ( void ) {
    return SicbFunctions::instance()->UDLAST();
  }    
  // Interface to UBFIND
  void ubfind      ( const std::string& name, int* iret ) {
    SicbFunctions::instance()->UBFIND(name, iret);
  }
  // Interface to UBINDEX
  void ubindex     ( const std::string& name, int* iret ) {
    SicbFunctions::instance()->UBINDEX(name, iret);
  }
  // Interface to UBIDNAME
  void ubidname    ( const int& npar, char* name ) {
    SicbFunctions::instance()->UBIDNAME(npar, name);
  }
  // Interface to UBDROP
  void ubdrop      ( const std::string& name )    {
    SicbFunctions::instance()->UBDROP(name);
  }
  // Interface to UBNOBJ
  void ubnobj      ( const std::string& name, const int& ipart, int& iret) {
    SicbFunctions::instance()->UBNOBJ(name, ipart, iret);
  }
  // Interface to UBPUTA
  void ubputa      ( const std::string& name, int* a1, int* a2, int* a3, int* a4 ) {
    SicbFunctions::instance()->UBPUTA(name, a1, a2, a3, a4);
  }
  // Interface to UBPUTR
  void ubputr      ( int* a1, int* a2, int* a3, int* a4 )    {
    SicbFunctions::instance()->UBPUTR(a1, a2, a3, a4);
  }
  // Interface to UTDGET
  void utdget      ( const std::string& a1, const std::string& a2, int& a3, int* a4) {
    SicbFunctions::instance()->UTDGET (a1, a2, a3, a4 );
  }
    // Magnetic field
  void gaudiGufld  ( const float* a1, float* a2 ) {
    SicbFunctions::instance()->GAUDIGUFLD ( a1, a2 );
  }
    // Default size of ZEBRA common
  void getZebraSize  ( int& a1 ) {
    SicbFunctions::instance()->GETZEBRASIZE ( a1 );
  }


  /// Wrapper constructor
  SicbFunctions::SicbFunctions()    {
    m_isValid = true;
    System::ImageHandle mh = 0;
    //    std::string dllLibrary = System::exeName();
    std::string dllLibrary = "";
    mh = loadFunctionLibrary(dllLibrary.c_str());
    if ( !mh || !m_isValid )  {
      if ( mh ) System::unloadDynamicLib(mh);
      dllLibrary = "SicbCnvBase";
      m_isValid = true;
      mh = loadFunctionLibrary(dllLibrary.c_str());
      if ( !mh || !m_isValid )    {
        if ( mh ) System::unloadDynamicLib(mh);
        printf("\n\nCannot load SICB functions....handle=%p\n\n\n", mh);
        m_isValid = false;
      }
      else    {
        printf(">>>>> Resolved SICB functions from DLL %s\n",dllLibrary.c_str());
        printf(">>>>> No other fortran allowed!\n");
      }
    }
    if ( 0 == GET_GCBANK ) {
      printf("%s Error accessing wrapper to GCBANK\n", __FILE__);
    }
    else {
      __GCBANK = &GET_GCBANK();
    }
    if ( 0 == GET_CBLINK ) {
      printf("%s Error accessing wrapper to CBLINK\n", __FILE__);
    }
    else {
      __CBLINK = &GET_CBLINK();
    }
    if ( 0 == GET_CBLINN ) {
      printf("%s Error accessing wrapper to CBLINN\n", __FILE__);
    }
    else {
      __CBLINN = &GET_CBLINN();
    }      
  }
  System::ImageHandle SicbFunctions::loadFunctionLibrary(const char* lib)   {
    System::ImageHandle mh = 0;
    System::loadDynamicLib(lib, &mh );
    if ( 0 != mh )  {
      load( mh, "IniLumi",           (void**)&INILUMI );
      load( mh, "SetNPileUp",        (void**)&SETNPILEUP );
      load( mh, "ReMerge",           (void**)&REMERGE );
      load( mh, "DropDivision",            (void**)&DROPDIVISION );
      load( mh, "ReadEvent",         (void**)&READEVENT );
      load( mh, "OpenInputFile",     (void**)&OPENINPUTFILE );
      load( mh, "CloseInputFile",    (void**)&CLOSEINPUTFILE );
      load( mh, "CheckEvtQlty",      (void**)&CHECKEVTQLTY );
      load( mh, "ubin",              (void**)&UBIN );
      load( mh, "udinit",            (void**)&UDINIT );
      load( mh, "udinitEx",          (void**)&UDINITEX );
      load( mh, "udlast",            (void**)&UDLAST );
      load( mh, "udlast",            (void**)&UDLAST );
      load( mh, "utzebra",           (void**)&UTZEBRA );
      load( mh, "ubfind",            (void**)&UBFIND );
      load( mh, "ubindex",           (void**)&UBINDEX );
      load( mh, "ubidname",          (void**)&UBIDNAME );
      load( mh, "ubdrop",            (void**)&UBDROP );
      load( mh, "ubnobj",            (void**)&UBNOBJ );
      load( mh, "ubputa",            (void**)&UBPUTA );
      load( mh, "ubputr",            (void**)&UBPUTR );
      load( mh, "utdget",            (void**)&UTDGET );
      load( mh, "gaudiGufld",        (void**)&GAUDIGUFLD );
      load( mh, "getZebraSize",      (void**)&GETZEBRASIZE );
      load( mh, "getCBLINK",         (void**)&GET_CBLINK );
      load( mh, "getCBLINN",         (void**)&GET_CBLINN );
      load( mh, "getGCBANK",         (void**)&GET_GCBANK );
    }
    return mh;
  }
  bool SicbFunctions::isValid()    const   {
    return m_isValid;
  }
  SicbFunctions* SicbFunctions::instance()    {
    static SicbFunctions inst;
//    if ( 0 == inst )  {
//      inst = new SicbFunctions();
//    }
    return &inst;
  }
  bool SicbFunctions::load(System::ImageHandle handle, const char* name, void** pFunction)    {
    System::getProcedureByName(handle, name, (System::EntryPoint*)pFunction);
    if ( 0 == *pFunction )   {
      m_isValid = false;
//      printf("Loading SICB function %s was NOT successful!....\n", name);
      return false;
    }
    return true;
  }
}; // namespace
