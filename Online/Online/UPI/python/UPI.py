import sys, platform, PyCintex as Dict

if platform.system()=='Linux':
  Dict.loadDictionary('libOnlineKernelDict.so')
  Dict.loadDictionary('libUPIDict.so')
else:
  Dict.loadDictionary('OnlineKernelDict.dll')
  Dict.loadDictionary('UPIDict.dll')
gbl = Dict.makeNamespace('')
pvss = Dict.makeNamespace('PVSS')

EventType            = 2

# graphics attributes
NORMAL              = 0x00
BOLD                = 0x01
UNDERLINE           = 0x02
INVERSE             = 0x04
FLASH               = 0x08
ATTRIBUTES          = 0x0F
ASCII               = 0x00
GRAPHIC             = 0x10
FONT_SUPP           = 0x20
FONT_USER           = 0x30
FONTS               = 0x30
MODIFIED            = 0x40

Interactor           = pvss.PyInteractor
resetANSI            = gbl.scrc_resetANSI
BaseSensor           = gbl.Sensor
Sensor               = gbl.UpiSensor
IocSensor            = gbl.IocSensor
TimeSensor           = gbl.TimeSensor
attach_terminal      = gbl.upic_attach_terminal
quit                 = gbl.upic_quit
set_cursor           = gbl.upic_set_cursor
write_rendered_message =      gbl.upic_write_rendered_message
def write_message(t1, t2=''): gbl.upic_write_message(t1,t2)

#window operations
open_window          = gbl.upic_open_window
open_old_window      = gbl.upic_open_old_window
set_message_window   = gbl.upic_set_message_window
get_message_window   = gbl.upicc_get_message_window

# Menu operations
open_menu            = gbl.upic_open_menu
close_menu           = gbl.upic_close_menu
delete_menu          = gbl.upic_delete_menu
erase_menu           = gbl.upic_erase_menu
hide_menu            = gbl.upic_hide_menu
open_detached_menu   = gbl.upic_open_detached_menu
open_pulldown_menu   = gbl.upic_open_pulldown_menu

delete_comment       = gbl.upic_delete_comment
delete_command       = gbl.upic_delete_command
enable_command       = gbl.upic_enable_command
disable_command      = gbl.upic_disable_command
refresh_screen       = gbl.upic_refresh_screen
begin_update         = gbl.upic_begin_update
end_update           = gbl.upic_end_update
change_titles        = gbl.upic_change_titles

get_input            = gbl.upicc_get_input
get_input_with_index = gbl.upicc_get_input_with_index


def add_command(id, text, pf2_text=''):             return gbl.upic_add_command(id,text,pf2_text)
def add_comment(id, text, pf2_text=''):             return gbl.upic_add_comment(id,text,pf2_text)
def add_param_line(id, text, pf2_text=''):          return gbl.upic_add_param_line(id,text,pf2_text)
def replace_comment(mid, id, text, pf2_text=''):    return gbl.upic_replace_comment(mid,id,text,pf2_text)
def replace_command(mid, id, text, pf2_text=''):    return gbl.upic_replace_command(mid,id,text,pf2_text)
def replace_param_line(mid, id, text, pf2_text=''): return gbl.upic_replace_param_line(mid,id,text,pf2_text)
def insert_command(mid,pos,id,text,pf2_text=''):    return gbl.upic_insert_command(mid,pos,id,text,pf2_text)
def insert_comment(mid,pos,id,text,pf2_text=''):    return gbl.upic_insert_comment(mid,pos,id,text,pf2_text)
def insert_param_line(mid,pos,id,text,pf2_text=''): return gbl.upic_insert_param_line(mid,pos,id,text,pf2_text)

def sensor():
  return Sensor.instance()


if __name__ == "__main__":
  attach_terminal()
  open_menu(1,0,0,'menu 1','up title','down title')
  add_command(1,'some command')
  add_command(2,'Quit')
  close_menu()
  
  while(1):
    r=get_input()
    cmd = r.r1
    write_message(str(r)+' '+str(r.r0)+' '+str(cmd)+' '+str(r.r2))
    if cmd == 2:
      quit();
      sys.exit(0)

