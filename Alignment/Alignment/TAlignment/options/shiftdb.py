import sys, os, string
from shiftdb_defines import *
import shiftdb_lib as lib
#from shiftdb_GUI import *
from mod_python import psp, apache, Session, util
from shiftdb_debugger import *


SHIFTDB_SCRIPTNAME='http://127.0.0.1/shiftdb/shiftdb.py'

# ------------------------------------------------------------------------------

def _startPage(req):
    req.content_type="text/html"
    psp.PSP(req, filename = 'templates/template_online_start_page.html').run()

# ------------------------------------------------------------------------------

def _endPage(req):
    psp.PSP(req, filename = 'templates/template_online_end_page.html').run()

# ------------------------------------------------------------------------------

def _printMenu(req):
    psp.PSP(req, filename= 'templates/template_top_menu.html').run({'arguments':[\
        ('Create User',SHIFTDB_SCRIPTNAME + '/createUser'),\
        ('Manage Users',SHIFTDB_SCRIPTNAME + '/manageUsers'),\
        ('Create Shift',SHIFTDB_SCRIPTNAME + '/createShift'),\
        ('Manage Shifts',SHIFTDB_SCRIPTNAME + '/manageShifts'),\
        ('Create Piquet',SHIFTDB_SCRIPTNAME + '/createPiquet'),\
        ('Manage Piquets',SHIFTDB_SCRIPTNAME + '/managePiquets')]})

# ------------------------------------------------------------------------------
def _restoreSession(req):
    '''Tries to restore a new session. If unsuccessful redirects to the login
       page
    '''
    if not hasattr(req,'session'): req.session=Session.Session(req)
    try:
        req.session['hits'] += 1
        if req.session['user_ip'] != req.get_remote_host():raise Exception("")
        # initialise session parameters....           
        # 
        # print session parameters: req.write(str(req.session.items()))
    except:
        # could not restore session so we redirect to a login page
        util.redirect(req, SHIFTDB_SCRIPTNAME+'/login')
    return req.session

# ------------------------------------------------------------------------------

def login(req, **args):

    if args.has_key('username') and args.has_key('password'):
        # try to authenticate the user
        # if yes redirect util.redirect('<target>')
        # else continue and print a message
        if authenticate(args['username'],args['password'],args['rights']):
            
            session=Session.Session(req)
            session['hits'] = 1
            session['username']=args['username']
            session['user_ip']=req.get_remote_host()
            #need rights too!
            session.save()
            util.redirect(req,SHIFTDB_SCRIPTNAME) 
    _startPage(req)
    #req.write(str(args))
    template = psp.PSP(req, filename = 'templates/template_online_login.html')
    template.run({ 'target'   : req.parsed_uri[apache.URI_PATH],
                   'username' : '',
                   'password' : '',
                })
    _endPage(req)

#------------------------------------------------------------------------------

def index(req, **args):
    session = _restoreSession(req)
    # print some initial menus
    _startPage(req)
    _printMenu(req)
    _endPage(req)

#------------------------------------------------------------------------------

def authenticate(username, password, admin=False):
    return SUCCESS

#-------------------------------------------------------------------------------

def manageUsers(req,**args):
    _startPage(req)
    _printMenu(req)
    if args.has_key('do_action'):
        #here we do extended view, modify or delete
        if args['do_action'] == 'Delete':
            if lib.doDeleteUser(args['id']):
                req.write('User %s successfully deleted!' % (args['username']))
            else:
                req.write('Error when trying to delete user %s ...' % (args['username']))
            _endPage(req)
            return
        elif args['do_action'] in ['View','Modify']:
            #if not args.has_key['id']:
            #    _endPage(req)
            #    return
            usr_list = lib.doGetUsers(('*',),('ID=%s' % (args['id']),))
            template=psp.PSP(req, filename='templates/template_manage_user.html')
            if args['do_action'] == 'View':
                action_target = (req.parsed_uri[apache.URI_PATH],'OK')
            elif args['do_action'] == 'Modify':
                i = req.parsed_uri[apache.URI_PATH].rfind('/')
                action_target = (req.parsed_uri[apache.URI_PATH][0:i] + '/createUser','Modify')
            else:
                _endPage(req)
                return
            template_args={'userAdd_URI' : action_target,
                           'userInfo' : [('','hidden','user_id',usr_list[0][0]), 
                                         ('Name:','text','name',usr_list[0][1],'20','20'),
                                         ('First Name:','text','first_name',usr_list[0][2],'20','20'),
                                         ('UserName:','text','username',usr_list[0][3],'15','15'),
                                         ('Affiliation:','textbox','affiliation',usr_list[0][4],'100','15','6'),
                                         ('CERN tel. no.:','text','cern_tel',usr_list[0][5],'12','12'),
                                         ('CERN GSM no.:','text','cern_gsm',usr_list[0][6],'12','12'),
                                         ('Known languages:','text','known_langs',usr_list[0][7],'50','25')]}
        else:
            req.write('Bad action!')
            return
    else:
        #show truncated users list
        base_cols = ('ID','NAME','FIRST_NAME','USERNAME')
        usr_list = lib.doGetUsers(base_cols)
        template=psp.PSP(req,filename='templates/template_list_visualizer.html')
        tbl_header=[('ID','hidden'),('Name','text'),('First Name','text'),('Username','text')]
        col_items=[]
        for j in range(len(usr_list)):
            for k in range(len(base_cols)):
                col_items.append((base_cols[k].lower(),usr_list[j][k]))
        template_args={'theader':tbl_header,
                       'actions':(req.parsed_uri[apache.URI_PATH],'View','Modify','Delete'),
                       'titems':col_items
                      }
    template.run(template_args)
    _endPage(req)
    

def manageShifts(req,**args):
    _startPage(req)
    _printMenu(req)
    pass
    _endPage(req)


def managePiquets(req,**args):
    _startPage(req)
    _printMenu(req)
    pass
    _endPage(req)

#-------------------------------------------------------------------------------

def createUser(req,**args):
    _startPage(req)
    _printMenu(req)
#    req.write(str(args))
    if not args.has_key('confirm'):    
        template=psp.PSP(req, filename='templates/template_manage_user.html')
        template.run({ 'userAdd_URI' : (req.parsed_uri[apache.URI_PATH],'Create'),
                       'userInfo' : [('Name:','text','name','','20','20'),
                                     ('First Name:','text','first_name','','20','20'),
                                     ('UserName:','text','username','','15','15'),
                                     ('Affiliation:','textbox','affiliation','','100','15','6'),
                                     ('CERN tel. no.:','text','cern_tel','','12','12'),
                                     ('CERN GSM no.:','text','cern_gsm','','12','12'),
                                     ('Known languages:','text','known_langs','','50','25')]
                       })
    else:
        if args['confirm'] == 'Create':
            msg = lib.doCreateUser(args)
            req.write(msg)
        elif args['confirm'] == 'Modify':
            if lib.doUpdateUser(args) == SUCCESS:
                util.redirect(req,SHIFTDB_SCRIPTNAME + '/manageUsers')
            else:
                req.write('<center>Error when trying to modify %s [%s]...</center>' % (args['username'],args['user_id']))

        else:
            req.write ('<center>Bad function call...</center>')
    _endPage(req)


def createShift(req,**args):
    _startPage(req)
    _printMenu(req)

    #_endPage(req)
    #return
    dbo = lib.DB_Object()
    shift_types = dbo.execSQL('SELECT * FROM SHIFTTYPE').fetchall()
    #req.write(str(shift_types))
    #users=lib.doGetUsers((*,))
    #for i in range(len(shift_types)):
    #shift_types[0]=shift_types[0]+('is_selected',)   
    template=psp.PSP(req,filename='templates/template_manage_shifts.html')
    template.run({'shift_ActionURI' : (req.parsed_uri[apache.URI_PATH],'Create'),
                  'shift_opts' : shift_types,
                  'sh_resps' : (),
                  'usr_assigned' : (),
                  'usr_available' : ()
                })
#to complete here!!!    

    _endPage(req)

def createPiquet(req,**args):
    _startPage(req)
    _printMenu(req)
    pass
    _endPage(req)

#-------------------------------------------------------------------------------
