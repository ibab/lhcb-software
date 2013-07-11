#ifndef ROMON_CONSTANTS_H
#define ROMON_CONSTANTS_H
namespace ROMon {
  enum { 
    CMD_ADD = 100,
    CMD_CONNECT,
    CMD_CHECK = 102,
    CMD_CHECK_CTRL,
    CMD_DELETE,
    CMD_UPDATE,
    CMD_EXCLUDE,
    CMD_SETCURSOR,
    CMD_POSCURSOR,
    CMD_SHOW,
    CMD_SHOWSUBFARM,
    CMD_SHOWCTRL,
    CMD_SHOWDEFERRED,
    CMD_SHOWMBM,
    CMD_SHOWCPU,
    CMD_SHOWPROCS,
    CMD_SHOWBOOT,
    CMD_SHOWSTATS,
    CMD_SHOWBENCHMARK,
    CMD_SHOWTORRENT,
    CMD_SHOWREADOUT,
    CMD_SHOWDEFERREDRUNS,
    CMD_SHOWSYS,
    CMD_SHOWHELP,
    CMD_HANDLE_KEY,
    CMD_RUNSTATE,
    CMD_PARTITIONID,
    CMD_NOTIFY,
    CMD_SUMMARIZE,

    CMD_ADD_ALARM,
    CMD_SHOW_ALARM,
    CMD_SHOW_SUMMARY,

    CMD_CLEAR,
    CMD_ENABLE,
    CMD_DISABLE,

    CMD_LAST
  };

  enum Idents {
    BUILDER_TASK =                'M',
    SENDER_TASK  =                'D',
    MOORE_TASK   =                'G',
    OVLWR_TASK   =                'O',

    REC_RECEIVER_TASK =           'R',
    REC_SENDER_TASK =             'S',
    REC_TASK =                    'B',

    MEP_BUFFER =                  'M',
    RES_BUFFER =                  'R',
    EVT_BUFFER =                  'E',
    SND_BUFFER =                  'S',
    INPUT_BUFFER =                'I',
    OUTPUT_BUFFER =               'O',
    OVL_BUFFER =                  'O',

    TASK_FSM_STATE_DEAD =         'D',
    TASK_FSM_STATE_NOT_READY =    'N',
    TASK_FSM_STATE_READY =        'r',
    TASK_FSM_STATE_RUNNING =      'R',

    SUBFARM_HEIGHT =              68,
    SUBFARM_WIDTH =              155,
    SUBFARM_NODE_OFFSET =          8,
    SUBFARM_DISP_WIDTH =          48,
    SUBFARM_DISP_HEIGHT_NORMAL =   5,
    SUBFARM_DISP_HEIGHT_DENSE =    4,
    SUBFARM_DISP_TOP =             4,
    SUBFARM_DISP_LEFT =            3,
    NUM_UPDATE_DIFF =              5
  };

  static const float SLOTS_MIN =  float(0.1E0);
  static const float SPACE_MIN =  float(0.1E0);
}
#ifdef _WIN32
inline int strcasecmp(const char* a, const char* b) { return _stricmp(a,b); }
inline int strncasecmp(const char* a, const char* b, size_t n) { return _strnicmp(a,b,n); }
#ifndef vsnprintf
#define vsnprintf   _vsnprintf
#endif
#endif

#endif // ROMON_CONSTANTS_H
