import sys, PyCintex as Dict
Dict.loadDictionary('libUPIDict.so')
gbl = Dict.makeNamespace('')
std = Dict.makeNamespace('std')
intV = std.vector('int')

attach_terminal      = gbl.upic_attach_terminal
open_menu            = gbl.upic_open_menu
close_menu           = gbl.upic_close_menu
get_input            = gbl.upicc_get_input
get_input_with_index = gbl.upicc_get_input_with_index
quit                 = gbl.upic_quit

def write_message(t1, t2=''):                           gbl.upic_write_message(t1,t2)
def add_command(id, text, pf2_text=''):          return gbl.upic_add_command(id,text,pf2_text)
def add_comment(id, text, pf2_text=''):          return gbl.upic_add_comment(id,text,pf2_text)
def replace_command(mid, id, text, pf2_text=''): return gbl.upic_replace_command(mid,id,text,pf2_text)

attach_terminal()
open_menu(1,0,0,'menu 1','up title','down title')
add_command(1,'some command')
add_command(2,'Quit')
close_menu()

while(1):
    r=get_input()
    cmd = r[1]
    write_message(str(r)+' '+str(r[0])+' '+str(cmd)+' '+str(r[2]))
    if cmd == 2:
      quit();
      sys.exit(0)

