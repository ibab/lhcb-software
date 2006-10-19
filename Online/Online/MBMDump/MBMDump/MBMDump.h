#ifndef MBMDUMP_H
#define MBMDUMP_H 1
#include <vector>
#include <utility>
#include "RTL/Bits.h"
#include "MBM/bmdef.h"

namespace LHCb {
  class RawBank;
  class MEPEvent;
  class MEPFragment;
  class MEPMultiFragment;
}

namespace MBMDump  {
  struct MainMenu;
  struct Requirement;
  struct PrintMenu;
  struct DisplayMenu;
  struct MEPWindow;
  struct MEPBankWindow;
  struct MEPFragmentWindow;
  struct MEPMultiFragmentWindow;

  enum { MAX_WORDS_PER_LINE = 8 };
  struct Format{
    int column_one_flag;
    int words_per_line;
    int ascii_flag;
    const char *fmt[MAX_WORDS_PER_LINE];
  };

  struct DataBlock{
    int *start;
    int length;
    int number;
    unsigned int name[4];
    int header;
  };

  class BaseMenu  {
  private:
    int m_ID;
    BaseMenu* m_parent;
  public:
    BaseMenu();
    virtual ~BaseMenu();
    int id() const { return m_ID; }
    void setParent(BaseMenu* par)  { m_parent = par;  }
    BaseMenu& parent()  const;
    virtual int run();
    virtual void handleMenu(int cmd_id) = 0;
    static int dispatch(int menu_id, int cmd_id, ...);
  };

  struct Requirement : public BaseMenu  {
    typedef Bits::BitMask<4> Mask;
    /// Command labels for the different menu items
    enum { C_EVT = 1, 
           C_TMASK0, C_TMASK1, C_TMASK2, C_TMASK3, 
           C_VETO0,  C_VETO1,  C_VETO2,  C_VETO3, 
           C_RTYPE, 
           C_MODE, 
           C_ADD, 
           C_PTO
    }; 
    bool m_reqActive;
    BMID m_bmID;
    int  m_evType;
    Mask m_trMask;
    Mask m_veto;
    int  m_reqTyp;
    int  m_reqMode;
    char m_rtype_c[4];
    char m_mode_c[8];

    Requirement();
    void buildMenu(int pg, int menu_id, int cmd_id);
    void add();
    void remove();
    virtual void handleMenu(int cmd_id);
  };

  /**
    *
    *
    */
  struct PrintMenu : public BaseMenu  {
    enum { WINDOW=0, C_DATA=1 };
    /// Command labels for the different menu items
    enum { C_PR =1, 
           C_NL,
           C_FN,
           C_AP
    };
    char m_fileName[255];
    int  print_flag;      /* 0 => window, 1 => current data */
    int  m_mxLines;
    char m_prtFlag[11];
    char m_fileMode[2];
    PrintMenu();
    void buildMenu(BaseMenu* parent, int cmd_id);
    virtual void handleMenu(int cmd_id);
  };

  /**
    *
    *
    */
  struct DisplayWindow : public BaseMenu  {
    DisplayWindow();
    void buildMenu(BaseMenu* parent, int cmd_id);
    virtual void handleMenu(int cmd_id);
  };

  /**
    *
    *
    */
  struct MEPBankWindow : public BaseMenu  {
    /// Command labels for the different menu items
    enum { C_DISMISS = 9999999,
           C_COM1=1,
           C_COM2,
           C_COM3,
           C_DATA};
    int m_parentCmd;
    const Format& m_fmt;
    MEPBankWindow(BaseMenu* par,int cmd_id, const Format& f, const LHCb::RawBank* b);
    virtual ~MEPBankWindow();
    virtual void handleMenu(int cmd_id);
  };

  /**
    *
    *
    */
  struct MEPFragmentWindow : public BaseMenu  {
    /// Command labels for the different menu items
    enum { C_DISMISS   = 9999999,
           C_COM1=1,
           C_COM2,
           C_COM3,
           C_COM4,
           C_COM5,
           C_COM6,
           C_COM7,
           C_BANKS
    };
    int m_parentCmd;
    const Format& m_fmt;
    LHCb::MEPFragment* m_frag;
    MEPBankWindow* m_bankWindow;
    MEPFragmentWindow(BaseMenu* par,int cmd_id, const Format& f, LHCb::MEPFragment* frag);
    virtual ~MEPFragmentWindow();
    virtual void handleMenu(int cmd_id);
  };

  /**
    *
    *
    */
  struct MEPBankListWindow : public BaseMenu  {
  public:
    typedef std::vector<std::pair<unsigned int,LHCb::RawBank*> > Banks;
  private:
    enum { C_DISMISS   = 9999999,
           C_COM1=1,
           C_COM2,
           C_COM3,
           C_COM4,
           C_COM5,
           C_COM6,
           C_COM7,
           C_BANKS
    };
    int m_parentCmd;
    const Format& m_fmt;
    Banks m_banks;
    MEPBankWindow* m_bankWindow;
  public:
    MEPBankListWindow(BaseMenu* par,int cmd_id, const Format& f, Banks& banks);
    virtual ~MEPBankListWindow();
    virtual void handleMenu(int cmd_id);
  };

  /**
    *
    *
    */
  struct MEPMultiFragmentWindow : public BaseMenu  {
    /// Command labels for the different menu items
    enum { C_DISMISS   = 9999999,
           C_SHOWBANKS = 9999998,
           C_COM1=1,
           C_COM2,
           C_COM3,
           C_COM4,
           C_COM5,
           C_COM6,
           C_COM7,
           C_COM8,
           C_COM9,
           C_COM10,
           C_FRAGS
    };
    int m_parentCmd;
    const Format& m_fmt;
    LHCb::MEPMultiFragment* m_frag;
    MEPFragmentWindow* m_fragWindow;
    MEPBankListWindow* m_banksWindow;
    MEPMultiFragmentWindow(BaseMenu* par,int cmd_id, const Format& f, LHCb::MEPMultiFragment* mf);
    virtual ~MEPMultiFragmentWindow();
    virtual void handleMenu(int cmd_id);
  };

  /**
    *
    *
    */
  struct MEPWindow : public BaseMenu  {
    /// Command labels for the different menu items
    enum { C_DISMISS   = 9999999,
           C_SHOWBANKS = 9999998,
           C_COM1=1,
           C_COM2,
           C_COM3,
           C_COM4,
           C_COM5,
           C_COM6,
           C_COM7,
           C_COM8,
           C_MULTIFRAGS
           };
    int m_parentCmd;
    const Format& m_fmt;
    MEPBankListWindow* m_banksWindow;
    MEPMultiFragmentWindow* m_multiFrags;
    MEPWindow(DisplayMenu* par, int cmd_id, const Format& f);
    virtual ~MEPWindow();
    static int backSpaceCallBack (int menu,int cmd,int par,void* param);
    virtual void handleMenu(int cmd_id);
  };

  /**
    *
    *
    */
  struct FormatMenu : public BaseMenu {
  private:
    /// Command labels for the different menu items
    enum {C_STD = 1, 
          C_COL1,
          C_NW,
          C_FMT
    };
    char   m_col1Value[9];
    char   m_fmtString[MAX_WORDS_PER_LINE];
    Format m_fmt;
  public:
    FormatMenu();
    const Format& fmt() const        { return m_fmt; }
    void  setFormat(const Format& f)  { m_fmt = f;    }
    void  buildMenu(BaseMenu* parent,int cmd_id);
    void  handleMenu(int cmd_id);
  };

  /**
    *
    *
    */
  struct DisplayMenu : public BaseMenu  {
  private:
    /// Command labels for the different menu items
    enum {C_FMT = 1, 
          C_FMT2,
          C_GET,
          C_DMP,
          C_BL, 
          C_HC,
          C_STD,
          C_HDR,
          C_TB,
          C_TOP,
          C_UP,
          C_DN,
          C_PU,
          C_PD,
          C_CMW,
          C_DHC,
          C_GO, 
          C_CHECK,
          C_RESET,
          C_DISPLAY,
          C_WRITE,
          C_WW,
          C_WD,
          C_MEP
    };
    DataBlock          m_evtData;
    DataBlock          m_bankData;
    DataBlock          m_currData;
    Format             m_currFmt;
    char               m_dispViewPort[7];
    int                m_dispReqOffset;
    MainMenu*          m_main;
    DisplayWindow      m_window;
    MEPWindow*         m_mepWindow;
    FormatMenu         m_fmtDataMenu;
    FormatMenu         m_fmtBankMenu;
    PrintMenu          m_prtMenu;
    MEPBankListWindow* m_banksWindow;
    int shiftLinesUp  (int first_word,int n_lines);
    int shiftLinesDown(int first_word,int n_lines);
    int jumpToLine    (int req_offset,int n_lines);
    int format_line   (int first_word, char *c);
  public:
    DisplayMenu();
    void buildMenu(BaseMenu* parent, int menu_id, int cmd_id);
    virtual void handleMenu(int cmd_id);
    const DataBlock& evtData() const  { return m_evtData; }
  };

  struct MainMenu : public BaseMenu  {
  private:
    /// Command labels for the different menu items
    enum {C_PROC = 1, 
          C_PART,
          C_BUF,
          C_RQS,
          C_INC_EXC,
          C_CMD,
          C_DEBUG,
          C_EXIT
    };
    BMID  m_bmID;
    int   m_partID;
    char  m_name[12];
    char  m_buffName[12];
    int*  m_memory;
    Requirement m_req[8];
    DisplayMenu m_dispMenu;
  public:
    MainMenu();
    void buildMenu();
    virtual void handleMenu(int cmd_id);
    int include();
    int exclude();
    int getEvent(struct DataBlock *event);
  };

  const char* procName();
  void rconv(char c[], int word);
  static const int N_LINES = 23;
  static const int LINE_LENGTH = 255;
  static const char *FMT_HEX08 = " %08X ";
  static const char *FMT_DEC10 = "%10d";
  template<class T> T* drop(T*& ref)  {
    if ( ref != 0 ) delete ref;
    return ref = 0;
  }
  template<class T, class Q> T* replace(T*& ref, Q* rep)  {
    if ( ref != 0 ) delete ref;
    return ref = rep;
  }
};
#endif //  MBMDUMP_H
