import sys, PyCintex as Dict
SCR        = Dict.makeNamespace('SCR')
gbl        = Dict.makeNamespace('')
Context    = SCR.PasteboardContext
Display    = SCR.Display
Window     = SCR.Window
Pasteboard = SCR.Pasteboard
#
#
ASCII      = 0x00
NORMAL     = 0x00
BOLD       = 0x01
UNDERLINE  = 0x02
INVERSE    = 0x04
FLASH      = 0x08
ATTRIBUTES = 0x0F
GRAPHIC    = 0x10
FONT_SUPP  = 0x20
FONT_USER  = 0x30
MODIFIED   = 0x40
ON         = 1
OFF        = 0
SEQUENTIAL_WINDOW   = 0
DETACHED_WINDOW     = 1
PULLDOWN_WINDOW     = 2
#
#
#
CTRL_A     = 0x101
CTRL_D     = 0x104
CTRL_X     = 0x118
CTRL_Y     = 0x119
CTRL_Z     = 0x11A
DELETE     = 0x099
#
#
#
MOVE_UP    = 0x11C
MOVE_DOWN  = 0x11D
MOVE_LEFT  = 0x11E
MOVE_RIGHT = 0x11F

#
#
#
resetANSI                  = gbl.scrc_resetANSI
setANSI                    = gbl.scrc_setANSI
create_pasteboard          = gbl.scrcc_create_pasteboard
create_display             = gbl.scrcc_create_display
paste_display              = gbl.scrc_paste_display
begin_pasteboard_update    = gbl.scrc_begin_pasteboard_update
end_pasteboard_update      = gbl.scrc_end_pasteboard_update
delete_pasteboard          = gbl.scrc_delete_pasteboard
init_screen                = gbl.scrc_init_screen
init_windows               = gbl.scrc_init_windows

fflush                     = gbl.scrc_fflush
put_chars                  = gbl.scrc_put_chars
set_cursor                 = gbl.scrc_set_cursor
set_cursor_abs             = gbl.scrc_set_cursor_abs
wait                       = gbl.scrc_wait
change_display             = gbl.scrc_change_display  # change display size
paste_display              = gbl.scrc_paste_display
set_border                 = gbl.scrc_set_border
get_display_attr           = gbl.scrc_get_display_attr
repaint_screen             = gbl.scrc_repaint_screen
restore_screen             = gbl.scrc_restore_screen
save_screen                = gbl.scrc_save_screen
clear_screen               = gbl.scrc_clear_screen
#
insert_line                = gbl.scrc_insert_line
delete_line                = gbl.scrc_delete_line
open_window                = gbl.scrc_open_window
move_window_to             = gbl.scrc_move_window_to
move_window                = gbl.scrc_move_window
change_window              = gbl.scrc_change_window
get_window_position        = gbl.scrc_get_window_position
get_window_size            = gbl.scrc_get_window_size
#drag_result                = gbl.scrc_drag_result
#scroll_result              = gbl.scrc_scroll_result
put_display_on_window      = gbl.scrc_put_display_on_window
remove_display_from_window = gbl.scrc_remove_display_from_window
delete_window              = gbl.scrc_delete_window
wtc_wait                   = gbl.wtc_wait
IocSensor                  = gbl.IocSensor
#
#
#
for i in xrange(10): print open_window
#sys.exit(0)

ctxt=create_pasteboard('Hello device')
pb=ctxt.pb
#print insert_line
#print ctxt.rows,ctxt.cols
init_screen(pb,ctxt.rows,ctxt.cols)

init_windows(pb,ctxt.rows+1000,ctxt.cols)

def end_update(pb):
  end_pasteboard_update(pb)
  fflush(pb)


def create(x,y,typ,x1,y1):
  win=open_window(SEQUENTIAL_WINDOW)
  print win
  #time.sleep(1)
  disp=create_display(x,y,typ,1,'  disp 0   ')
  begin_pasteboard_update(pb)
  paste_display(disp,pb,x1,y1)
  end_update(pb)

  begin_pasteboard_update(pb)
  put_chars (disp, "normal",       NORMAL,    1, 1, 1)
  put_chars (disp, "underline",    UNDERLINE, 2, 1, 1)
  put_chars (disp, "inverse",      INVERSE,   3, 1, 1)
  put_chars (disp, "bold",         BOLD,      4, 1, 1)
  put_chars (disp, "bold|inverse", BOLD|INVERSE,5, 1, 1)
  put_chars (disp, "graphic : ",   NORMAL,    6, 1, 0)
  put_chars (disp, "abcdefghijkl", GRAPHIC,   6, 11,0)
  put_chars (disp, "dec_sup : ",   NORMAL,    7, 1, 0)
  put_chars (disp, "abcdefghijkl", BOLD,      7, 11,0)
  put_chars (disp, "user    : ",   NORMAL,    8, 1, 0)
  put_chars (disp, "abcdefghijkl", FONT_USER, 8, 11,0)
  end_update(pb)
  set_cursor (disp, 1, 1);
  return (win,disp)

import time
d0=create(10,30,INVERSE,2,2)
disps=[]
for j in xrange(10):
  disps.append(create(50,50,NORMAL,5,24+j*30))
disp2=None

def move():
  i=8
  j=0
  while(1):
    time.sleep(1)
    begin_pasteboard_update(pb)
    j = j + 1
    put_chars(disp,'new line %d  %s'%(j,time.strftime("%Y-%m-%d %H:%M:%S")),NORMAL,10,10,1)
    put_chars(disp2,time.strftime("%Y-%m-%d %H:%M:%S"),NORMAL,10,1,1)
    paste_display(disp2,pb,j+20,j+20)
    end_update(pb)


#insert_line(disp,'new line',NORMAL,1,8,MOVE_DOWN)
#end_pasteboard_update(pb)
#fflush(pb)
#set_cursor (disp, 1, 1);
#w = open_window(DETACHED_WINDOW)
disp2=create(20,20,NORMAL,25,34)
#move()
pos = 0
while(1):
    #try:
    key = wait(disp2[1])
    #key = read_keyboard(disp[1],1)
    #print '\n\nGot key: %d %X'%(key,key)
    if key == MOVE_RIGHT:
      pos = pos+1
      if pos>=len(disps): pos = len(disps)-1
      set_cursor_abs (pb, 1, pos*30+1);
    elif key == MOVE_LEFT:
      pos = pos-1
      if pos<0: pos = 0
      set_cursor_abs (pb, 1, pos*30-1);
    #except:
    #pass
resetANSI()


