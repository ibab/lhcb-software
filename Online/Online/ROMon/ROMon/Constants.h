#ifndef ROMON_CONSTANTS_H
#define ROMON_CONSTANTS_H
namespace ROMon {
  enum { 
    CMD_ADD = 100,
    CMD_CONNECT,
    CMD_CHECK = 102,
    CMD_DELETE,
    CMD_UPDATE,
    CMD_SETCURSOR,
    CMD_POSCURSOR,
    CMD_SHOW,
    CMD_SHOWSUBFARM,
    CMD_SHOWCTRL,
    CMD_SHOWMBM,
    CMD_SHOWCPU,
    CMD_SHOWPROCS,
    CMD_SHOWHELP,
    CMD_HANDLE_KEY,
    CMD_RUNSTATE,
    CMD_LAST
  };

  enum Idents {
    BUILDER_TASK =                'M',
    SENDER_TASK  =                'D',
    MOORE_TASK   =                'G',

    REC_RECEIVER_TASK =           'R',
    REC_SENDER_TASK =             'S',
    REC_TASK =                    'B',

    MEP_BUFFER =                  'M',
    RES_BUFFER =                  'R',
    EVT_BUFFER =                  'E',
    SND_BUFFER =                  'S',
    INPUT_BUFFER =                'I',
    OUTPUT_BUFFER =               'O',

    SUBFARM_HEIGHT =              65,
    SUBFARM_WIDTH =              132,
    SUBFARM_NODE_OFFSET =          8,
    SUBFARM_DISP_WIDTH =          48,
    SUBFARM_DISP_HEIGHT_NORMAL =   5,
    SUBFARM_DISP_HEIGHT_DENSE =    4,
    SUBFARM_DISP_TOP =             4,
    SUBFARM_DISP_LEFT =            3,
    NUM_UPDATE_DIFF =              5
  };

  static const float SLOTS_MIN =  .1;
  static const float SPACE_MIN =  .1;
}

#endif // ROMON_CONSTANTS_H
