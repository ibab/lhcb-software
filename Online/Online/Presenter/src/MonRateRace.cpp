//Warning in <TColor::Number2Pixel>: color with index 5544 not defined

//  Error in <TClass::New>: cannot create object of class TH1
//  monRate: Number of processes publishing via this 
//   dimContent: 1
//  monRate: Counter number 2 dimContent: 2.80182
//  pure virtual method called
//  terminate called without an active exception
//  RTL:Handled signal: 6 [SIGABRT] Old action:(nil)


//  Program received signal SIGSEGV, Segmentation fault.
//  [Switching to Thread 0xb0e12b90 (LWP 13025)]
//  0xb6b68b9e in TH1::LabelsInflate (this=0x8e13248, ax=0xb6d284e8 "X") at hist/hist/src/TH1.cxx:4154
//  4154  hist/hist/src/TH1.cxx: No such file or directory.
//    in hist/hist/src/TH1.cxx
//  (gdb) bt
//  #0  0xb6b68b9e in TH1::LabelsInflate (this=0x8e13248, ax=0xb6d284e8 "X") at hist/hist/src/TH1.cxx:4154
//  #1  0xb6b7d69e in TH1D::SetBinContent (this=0x8e13248, bin=11, content=2.9673854663394628) at hist/hist/src/TH1.cxx:8641
//  #2  0x080b799c in DbRootHist::fillHistogram (this=0x8e12298) at ../src/DbRootHist.cpp:750
//  #3  0x080a265d in refreshHisto (dbHistoOnPage=0x8e12298) at ../src/ParallelWait.cpp:132
//  #4  0x080a4141 in boost::_bi::list1<boost::_bi::value<DbRootHist*> >::operator()<void (*)(DbRootHist*), boost::_bi::list0> (this=0x8e53d20, f=@0x8e53d1c, a=@0xb0e12323)
//      at /afs/cern.ch/sw/lcg/external/Boost/1.39.0_python2.5/slc4_ia32_gcc34/include/boost-1_39/boost/bind/bind.hpp:246
//  #5  0x080a4191 in boost::_bi::bind_t<void, void (*)(DbRootHist*), boost::_bi::list1<boost::_bi::value<DbRootHist*> > >::operator() (this=0x8e53d1c)
//      at /afs/cern.ch/sw/lcg/external/Boost/1.39.0_python2.5/slc4_ia32_gcc34/include/boost-1_39/boost/bind/bind_template.hpp:20
//  #6  0x080a41ba in boost::detail::thread_data<boost::_bi::bind_t<void, void (*)(DbRootHist*), boost::_bi::list1<boost::_bi::value<DbRootHist*> > > >::run (this=0x8e53c60)
//      at /afs/cern.ch/sw/lcg/external/Boost/1.39.0_python2.5/slc4_ia32_gcc34/include/boost-1_39/boost/thread/detail/thread.hpp:56
//  #7  0xb620a75d in thread_proxy () from /afs/cern.ch/sw/lcg/external/Boost/1.39.0_python2.5/slc4_ia32_gcc34/lib/libboost_thread-gcc34-mt-1_39.so.1.39.0
//  #8  0xb6f6e4ff in start_thread (arg=0xb0e12b90) at pthread_create.c:297
//  #9  0xb526249e in clone () at ../sysdeps/unix/sysv/linux/i386/clone.S:130


//  Program received signal SIGSEGV, Segmentation fault.
//  [Switching to Thread 0xb1686b90 (LWP 24800)]
//  0x00000000 in ?? ()
//  (gdb) bt
//  #0  0x00000000 in ?? ()
//  #1  0xb79467ac in TCint::UpdateListOfGlobals (this=0x8289210) at core/meta/src/TCint.cxx:644
//  #2  0xb78a2a65 in TROOT::GetListOfGlobals (this=0xb7d53b00, load=true) at core/base/src/TROOT.cxx:1084
//  #3  0xb79240ee in TDataMember (this=0x8cac180, info=0x8cac150, cl=0x8c9c0a0) at core/meta/src/TDataMember.cxx:405
//  #4  0xb79479e0 in TCint::CreateListOfDataMembers (this=0x8289210, cl=0x8c9c0a0) at core/meta/src/TCint.cxx:897
//  #5  0xb7915b2b in TClass::GetListOfDataMembers (this=0x8c9c0a0) at core/meta/src/TClass.cxx:2455
//  #6  0xb7914962 in TClass::GetDataMember (this=0x8c9c0a0, datamember=0xb7c29f8b "*fData") at core/meta/src/TClass.cxx:2230
//  #7  0xb790b793 in TBuildRealData::Inspect (this=0xb16855e0, cl=0x8c9c0a0, pname=0xb16855f0 "", mname=0xb7c29f8b "*fData", add=0x8c96358) at core/meta/src/TClass.cxx:381
//  #8  0xb7a32ae4 in TString::ShowMembers () from /afs/cern.ch/sw/lcg/app/releases/ROOT/5.22.00c/slc4_ia32_gcc34_dbg/root/lib/libCore.so
//  #9  0xb7a4d57f in G__G__Base2_89_0_156 () from /afs/cern.ch/sw/lcg/app/releases/ROOT/5.22.00c/slc4_ia32_gcc34_dbg/root/lib/libCore.so
//  #10 0xb70949b0 in Cint::G__CallFunc::Execute (this=0x8ca5b98, pobject=0x8c96354) at cint/cint/src/CallFunc.cxx:441
//  #11 0xb794f8e9 in Cint::G__CallFunc::Exec (this=0x8ca5b98, pobject=0x8c96354) at include/cint/CallFunc.h:95
//  #12 0xb794d0af in TCint::CallFunc_Exec (this=0x8289210, func=0x8ca5b98, address=0x8c96354) at core/meta/src/TCint.cxx:2203
//  #13 0xb7912697 in TClass::CallShowMembers (this=0x8c9c0a0, obj=0x8c96354, insp=@0xb16855e0, parent=0xb16855f0 "", isATObject=0) at core/meta/src/TClass.cxx:1465
//  #14 0xb7911ae5 in TClass::BuildRealData (this=0x8c9c0a0, pointer=0x8c96354, isTransient=false) at core/meta/src/TClass.cxx:1296
//  #15 0xb790bc64 in TBuildRealData::Inspect (this=0xb1685a50, cl=0x8aa1cd0, pname=0xb1685a60 "", mname=0xb7c29a59 "fName", add=0x8c96354) at core/meta/src/TClass.cxx:470
//  #16 0xb7a2e272 in TNamed::ShowMembers () from /afs/cern.ch/sw/lcg/app/releases/ROOT/5.22.00c/slc4_ia32_gcc34_dbg/root/lib/libCore.so
//  #17 0xb79122e0 in TClass::CallShowMembers (this=0x8aa1cd0, obj=0x8c96348, insp=@0xb1685a50, parent=0xb1685a60 "", isATObject=1) at core/meta/src/TClass.cxx:1423
//  #18 0xb7911ae5 in TClass::BuildRealData (this=0x8aa1cd0, pointer=0x0, isTransient=false) at core/meta/src/TClass.cxx:1296
//  #19 0xb6ebab9d in TStreamerInfo::Build (this=0x8c96248) at io/io/src/TStreamerInfo.cxx:207
//  #20 0xb7917dbc in TClass::GetStreamerInfo (this=0x8aa1cd0, version=1) at core/meta/src/TClass.cxx:3028
//  #21 0xb6ebaf0e in TStreamerInfo::Build (this=0x8c94a28) at io/io/src/TStreamerInfo.cxx:243
//  #22 0xb7917dbc in TClass::GetStreamerInfo (this=0x8c057d0, version=6) at core/meta/src/TClass.cxx:3028
//  #23 0xb6ebaf0e in TStreamerInfo::Build (this=0x8a8cda0) at io/io/src/TStreamerInfo.cxx:243
//  #24 0xb6e7a9f2 in TBufferFile::WriteClassBuffer (this=0x8a8c590, cl=0x8c05498, pointer=0x8c00da8) at io/io/src/TBufferFile.cxx:3453
//  #25 0xb6c932ef in TH1D::Streamer () from /afs/cern.ch/sw/lcg/app/releases/ROOT/5.22.00c/slc4_ia32_gcc34_dbg/root/lib/libHist.so
//  #26 0xb786508a in TDirectory::CloneObject (this=0xb7d53b00, obj=0x8c00da8, autoadd=true) at core/base/src/TDirectory.cxx:263
//  #27 0xb787d442 in TObject::Clone (this=0x8c00da8) at core/base/src/TObject.cxx:202
//  #28 0xb787c42d in TNamed::Clone (this=0x8c00da8, newname=0xb6d9a472 "") at core/base/src/TNamed.cxx:67
//  #29 0xb6bdbb92 in TH1::LabelsInflate (this=0x8c00da8, ax=0xb6d9b4e8 "X") at hist/hist/src/TH1.cxx:4153
//  #30 0xb6bf069e in TH1D::SetBinContent (this=0x8c00da8, bin=11, content=1.9830331682127715) at hist/hist/src/TH1.cxx:8641
//  #31 0x080b7238 in DbRootHist::fillHistogram (this=0x8bffe28) at ../src/DbRootHist.cpp:750
//  #32 0x080a2245 in refreshHisto (dbHistoOnPage=0x8bffe28) at ../src/ParallelWait.cpp:132
//  #33 0x080a3d29 in boost::_bi::list1<boost::_bi::value<DbRootHist*> >::operator()<void (*)(DbRootHist*), boost::_bi::list0> (this=0x8c2a7e8, f=@0x8c2a7e4, a=@0xb1686323)
//      at /afs/cern.ch/sw/lcg/external/Boost/1.39.0_python2.5/slc4_ia32_gcc34/include/boost-1_39/boost/bind/bind.hpp:246
//  #34 0x080a3d79 in boost::_bi::bind_t<void, void (*)(DbRootHist*), boost::_bi::list1<boost::_bi::value<DbRootHist*> > >::operator() (this=0x8c2a7e4)
//      at /afs/cern.ch/sw/lcg/external/Boost/1.39.0_python2.5/slc4_ia32_gcc34/include/boost-1_39/boost/bind/bind_template.hpp:20
//  #35 0x080a3da2 in boost::detail::thread_data<boost::_bi::bind_t<void, void (*)(DbRootHist*), boost::_bi::list1<boost::_bi::value<DbRootHist*> > > >::run (this=0x8c2a728)
//      at /afs/cern.ch/sw/lcg/external/Boost/1.39.0_python2.5/slc4_ia32_gcc34/include/boost-1_39/boost/thread/detail/thread.hpp:56
//  #36 0xb627d75d in thread_proxy () from /afs/cern.ch/sw/lcg/external/Boost/1.39.0_python2.5/slc4_ia32_gcc34/lib/libboost_thread-gcc34-mt-1_39.so.1.39.0

//  Program received signal SIGSEGV, Segmentation fault.
//  [Switching to Thread 0xb315b6d0 (LWP 32371)]
//  0xafcfd71f in TASImage::DrawHLine (this=0x8fb4a30, y=32000, x1=99, x2=896, col=4294901502, thick=1) at graf2d/asimage/src/TASImage.cxx:3664
//  3664  graf2d/asimage/src/TASImage.cxx: No such file or directory.
//    in graf2d/asimage/src/TASImage.cxx
//  (gdb) bt
//  #0  0xafcfd71f in TASImage::DrawHLine (this=0x8fb4a30, y=32000, x1=99, x2=896, col=4294901502, thick=1) at graf2d/asimage/src/TASImage.cxx:3664
//  #1  0xafcfe54f in TASImage::DrawBox (this=0x8fb4a30, x1=99, y1=32000, x2=896, y2=32000, col=0x8ec87d8 "#fefefe", thick=1, mode=1) at graf2d/asimage/src/TASImage.cxx:3912
//  #2  0xafd8a7f3 in TImageDump::DrawBox (this=0x8f88990, x1=0, y1=0, x2=20, y2=2.082058898471868) at graf2d/postscript/src/TImageDump.cxx:171
//  #3  0xb5168596 in TPad::PaintBox (this=0x8f2ee50, x1=0, y1=0, x2=20, y2=2.082058898471868, option=0xb50b1318 "") at graf2d/gpad/src/TPad.cxx:3121
//  #4  0xb4f93df8 in TBox::PaintBox (this=0x8f63920, x1=0, y1=0, x2=20, y2=2.082058898471868, option=0xb50b7638 "") at graf2d/graf/src/TBox.cxx:582
//  #5  0xb4ff81be in TWbox::PaintWbox (this=0x8f63920, x1=0, y1=0, x2=20, y2=2.082058898471868, color=10, bordersize=1, bordermode=1) at graf2d/graf/src/TWbox.cxx:138
//  #6  0xb4ff8163 in TWbox::Paint (this=0x8f63920) at graf2d/graf/src/TWbox.cxx:127
//  #7  0xb4fa43bf in TFrame::Paint (this=0x8f63920, option=0x815dd60 "") at graf2d/graf/src/TFrame.cxx:124
//  #8  0xb5166d45 in TPad::Paint (this=0x8f2ee50) at graf2d/gpad/src/TPad.cxx:2777
//  #9  0xb5143ddd in TCanvas::Paint (this=0x8f2ee50, option=0xb51b2aeb "") at graf2d/gpad/src/TCanvas.cxx:1350
//  #10 0xb516e626 in TPad::Print (this=0x8f2ee50, filenam=0x8f74838 "/tmp//Counternumber2.png", option=0xb51b2ec2 "png") at graf2d/gpad/src/TPad.cxx:4504
//  #11 0xb5170122 in TPad::SaveAs (this=0x8f2ee50, filename=0x8f73cfc "/tmp//Counternumber2.png") at graf2d/gpad/src/TPad.cxx:4933
//  #12 0x0811b1da in PresenterMainFrame::refreshPage (this=0xbff1aa78) at ../src/PresenterMainFrame.cpp:4619
//  #13 0x08093d5c in G__cintBootstrapDict_560_0_45 (result7=0x8f05124, funcname=0x0, libp=0x8f0517c, hash=0) at /afs/cern.ch/user/p/psomogyi/cmtuser/Online_v4r28/Online/Presenter/slc4_ia32_gcc34_dbg/dict/cintBootstrapDict.cpp:1369

#include "MonRateRace.h"
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/classification.hpp>

boost::mutex infoHandlerMutex;

MonRateRace::MonRateRace(const std::string & serviceName, pres::EffServiceType serviceType) :
  DimInfo(serviceName.c_str(), (double)std::numeric_limits<double>::min(), this),
  m_value(0.0),
  m_title(pres::s_eff_init),
  m_rateIsValid(false),
  m_serviceType(serviceType)
{
  m_infoHandlerMutex = new boost::mutex();
//  serviceName -> isRate  vs. isTCK // dim buffer content
// valid title test: last N callbacks give the same result? -> gate validRate too
}

void MonRateRace::infoHandler()
{ 
  boost::unique_lock<boost::mutex> infoHandlerLock(*m_infoHandlerMutex);
  if (infoHandlerLock) {
    if (pres::MonRate == m_serviceType) {   
      char* cnt_comment  = 0;
      cnt_comment = static_cast<char*>(getData());
      std::stringstream cntCommentStream;     
      for (int ptr = 2*sizeof(double); ptr < (int)(getSize()/sizeof(char)); ++ptr ){
        cntCommentStream << static_cast<char>(cnt_comment[ptr]);
          if ( boost::all( cntCommentStream.str().c_str(), boost::is_print()) ) {
          } else {
          } 

      }
      double* histoDimData = 0;
      histoDimData = static_cast<double*>(getData());
  
      if (std::numeric_limits<double>::min() == static_cast<double>(*histoDimData)) {
        m_value = 0.0;
        m_rateIsValid = false;
        m_title = pres::s_eff_init;
      } else {    
        m_value = static_cast<double>(histoDimData[0]);     
        if ((std::numeric_limits<double>::min() < m_value) &&
            (std::numeric_limits<double>::max() > m_value) ) { // 2.2e-308 to 1.8e308
          m_title = cntCommentStream.str();
          if ( boost::all( m_title.c_str(), boost::is_print()) ) {
            m_rateIsValid = true;
          } else {
            m_rateIsValid = false;
          }           
         } else { // something else...
           m_value = 0.0;
           m_title = pres::s_eff_init;
           m_rateIsValid = false;      
         }
      }
      cntCommentStream.str("");
    } else if (pres::TCKinfo == m_serviceType) {
      char* tckNickname  = 0;
      tckNickname = static_cast<char*>(getData());
      std::stringstream cntTckNickname;     
      for (int ptr = sizeof(int); ptr < (int)(getSize()/sizeof(char)); ++ptr ){
        cntTckNickname << static_cast<char>(tckNickname[ptr]);               
      }
      m_title = cntTckNickname.str();
      if ( boost::all( m_title.c_str(), boost::is_print()) ) {
        m_rateIsValid = true;
      } else {
        m_rateIsValid = false;
      }
      cntTckNickname.str("");
    }
  }
}
bool MonRateRace::isRateValid() {
  boost::unique_lock<boost::mutex> infoHandlerLock(*m_infoHandlerMutex);
  if (infoHandlerLock) {
   return m_rateIsValid;
  } else {
    return false;
  }
}
double MonRateRace::currentValue()
{
  double returnValue = 0.0;
  boost::unique_lock<boost::mutex> infoHandlerLock(*m_infoHandlerMutex);
  if (infoHandlerLock && m_rateIsValid) {
    returnValue = m_value;
    infoHandlerLock.unlock();    
  }
  return returnValue;
}
std::string MonRateRace::title()
{
  std::string returnTitle(pres::s_eff_init);
  boost::unique_lock<boost::mutex> infoHandlerLock(*m_infoHandlerMutex);
  if (infoHandlerLock && m_rateIsValid &&
      pres::s_eff_init != m_title) {
    returnTitle = m_title;
    infoHandlerLock.unlock();    
  }  
  return returnTitle;  
}

MonRateRace::~MonRateRace()
{
  if (m_infoHandlerMutex) { delete m_infoHandlerMutex; m_infoHandlerMutex = NULL;}  
}
