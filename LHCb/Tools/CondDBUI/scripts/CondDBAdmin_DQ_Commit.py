#!/usr/bin/env python

__author__ = "Illya Shapoval <illya.shapoval@cern.ch>"

import os, sys, stat

def _which(cmd):
    # Absolute paths should be treated differently
    if os.path.isabs(cmd):
        if os.path.exists(cmd):
            return cmd
        else:
            return None

    # search path
    search_path = os.environ["PATH"].split(os.pathsep)
    # on win32, the PATHEXT variable defines the extensions to look for
    if "PATHEXT" in os.environ:
        search_pathext = os.environ["PATHEXT"].split(os.pathsep)
    else:
        # fail-safe default
        search_pathext = [".BAT", ".COM", ".EXE"]
    cmd = cmd.strip()
    for p in search_path:
        if sys.platform[0:3] == "win":
            testc = [ (cmd+x).lower() for x in search_pathext ]
            for c in testc:
                c = os.path.join(p,c)
                if os.path.exists(c):
                    return c
        else: # assume UNIX
            c = os.path.join(p,cmd)
            if os.path.exists(c):
                if os.stat(c)[stat.ST_MODE] & ( stat.S_IXGRP | stat.S_IXOTH | stat.S_IXUSR ):
                    return c

    return None

def _getMessage():
    # look for an editor
    editor_alternatives = [ "pico", "nano", "emacs", "vi", "edit" ]
    # prefer the environment variables VISUAL and EDITOR
    for v in [ "EDITOR", "VISUAL" ]:
        if v in os.environ:
            editor_alternatives.insert(0,os.environ[v])
    # this variable will contain the full path to the editor found
    editor = None
    eargs = []
    for e in editor_alternatives:
        eargs = e.strip().split()
        editor = _which(eargs.pop(0))
        if editor:
            break

    if editor:
        # prepare temporary file
        import tempfile
        (tmpfd, tmpn) = tempfile.mkstemp()
        tmpf = os.fdopen(tmpfd,"w")
        marker = "--- Everything below this line will be ignored ---"
        default = "\n\n%s\nEdit comments for the changes and save.\n"%marker
        tmpf.write(default)
        tmpf.close()
        # start editor
        os.system(" ".join([e] + eargs + [tmpn]))
        # read the edited file
        message = [ l.strip() for l in open(tmpn) ]
        os.remove(tmpn)

        if marker in message:
            message = message[:message.index(marker)]

        # remove trailing empty lines
        while message and not message[-1]:
            message.pop()
        return "\n".join(message)
    else:
        return raw_input("Insert comment: ")

def _set_logging():
    # Prepare local logger
    import logging
    global log
    log = logging.getLogger(os.path.basename(sys.argv[0]))
    log.setLevel(logging.INFO)

    # set the global stream handler
    from CondDBUI import LOG_FORMAT
    hndlr = logging.StreamHandler()
    hndlr.setFormatter(logging.Formatter(LOG_FORMAT))
    logging.getLogger().handlers = [hndlr]

    # decrease verbosity of PyCoolDiff
    import CondDBUI.PyCoolDiff
    CondDBUI.PyCoolDiff._log.setLevel(logging.WARNING)

def _get_options():
    # Configure the parser
    from optparse import OptionParser
    parser = OptionParser(usage = "%prog [options] FLAG FLAG_VALUE local_tag",
                          description =
"""This script adds/removes DQ flags to/from the CondDB, tagging the result and
updating the release notes. The user has to provide a FLAG name to insert/remove
with its value FLAG_VALUE (by convention it is '1' (='BAD')), and the local
tag name to tag achieved changes with. The script will ask for the contributor name
and for a short message for the release notes.""")

    parser.add_option("-d", "--dest", type = "string",
                      help = "Destination to commit flag to. Can be:"
                      " sqlite(default) or oracle."
                      )
    parser.add_option("--rel-notes", type = "string",
                      help = "XML file containing the release notes"
                      )
    parser.add_option("-m", "--message", type = "string",
                      help = "Message to include in the release notes"
                      )
    parser.add_option("-c","--contributor", type = "string",
                      help = "Name of the contributor of the patch"
                      )
    parser.add_option("-w","--with-care", action = "store_true",
                      help = "Before applying check the final result."
                      )
    parser.add_option("-s", "--since", type = "string",
                      help = "Start of the Interval Of Validity (local time)."
                      " Format: YYYY-MM-DD[_HH:MM[:SS.SSS]][UTC]"
                      )
    parser.add_option("-u", "--until", type = "string",
                      help = "End of the Interval Of Validity (local time)"
                      " Format: YYYY-MM-DD[_HH:MM[:SS.SSS]][UTC]"
                      )
    parser.add_option("--ref_tag", type = "string",
                      help = "Reference tag to form modifications starting not from the HEAD"
                      " but from a tag given in this option. BE CAREFULL here:"
                      " the changes formed in this way will contain the payload of"
                      " the 'ref_tag' which was between IOV 'since' and 'until' values"
                      " and will be applied on top of the HEAD."
                      )

    parser.set_default("dest", "sqlite")
    parser.set_default("rel_notes",os.path.join(os.environ["SQLDDDBROOT"], "doc", "release_notes.xml"))
    parser.set_default("message", None)
    parser.set_default("contributor", None)
    parser.set_default("since", None)
    parser.set_default("until", None)
    parser.set_default("ref_tag", '')

    # parse command line
    options, args = parser.parse_args()

    # check arguments
    if len(args) != 3:
        parser.error("Not enough arguments. Try with --help.")

    if options.ref_tag == '':
        log.info("Reference tag = 'HEAD'")
    else:
        log.info("Reference tag = '%s'" % options.ref_tag)


    if options.since:
        log.info("Validity from %s" % format_text(options.since,'yellow'))
    if options.until:
        log.info("Validity until %s" % format_text(options.until,'yellow'))

    from CondDBUI.Admin import timeToValKey
    from PyCool import cool
    options.since = timeToValKey(options.since, cool.ValidityKeyMin)
    options.until = timeToValKey(options.until, cool.ValidityKeyMax)
    if options.since >= options.until:
        parser.error("Invalid IOV: %s to %s" % (options.since, options.until))

    if options.dest.lower() not in ['sqlite', 'oracle']:
        parser.error("'%s' is invalid database destination. Allowed are 'sqlite' and 'oracle'." %options.dest)

    import CondDBUI.Admin.Management
    log.info("Destination = %s" % format_text(CondDBUI.Admin.Management._masterCoolURL('DQFLAGS'),'yellow'))

    log.info("Release notes: %s" % format_text(options.rel_notes,'yellow'))

    if not options.contributor:
        options.contributor = raw_input("Contributor: ")
    log.info("Changes by: %s" % options.contributor)

    ########## Prepare the message ############################################
    if not options.message:
        options.message = _getMessage()

    if not options.message:
        log.warning("Empty message!")
    else:
        options.message = options.message.replace("\\n","\n")
        log.info("Message for the changes: \n\n%s\n" % format_text(options.message,'yellow'))
    return options, args

def _print_dq_summary(action, f_c_s):
    """Prints out info about the changes to the dq flags.
    inputs:
      action - str; specifies the action to be done with new changes
      f_c_s - list of lists; flag changes summary in the form:
              [[flag, new_flag_value],[old_flag_value1,since1,until1],...,
              [old_flag_valueN,sinceN,untilN]]
    outputs:
      No ouput
    """

    if not f_c_s or len(f_c_s) <= 1:
        log.info('There are no changes to show.')
        return

    brick = format_text(u"\u2588", 'white')
    line = format_text(reduce(lambda x,y:x+y,[brick for i in range(80)]), 'white')

    if action == 'change':
        action = format_text(action,'green')
    elif action == 'REMOVE':
        action = format_text(action, 'red', blink = True)
    elif action == 'CREATE NEW':
        action = format_text(action, 'red', blink = True)

    log.info(line)

    log.info(brick + format_text(" You're about to ", 'green') + action +
             format_text(" '%s' flag modifying following IOVs (local time):",
                         'green')%f_c_s[0][0])

    if f_c_s[0][1] == 'remove':
        flag_val = format_text('remove','red')
    else:
        flag_val = f_c_s[0][1]

    for i in f_c_s[1:]:
        if i[0] is None:
            old_flag_val = format_text('None','red')
        else:
            old_flag_val = i[0]
        log.info(brick + " '%s' -> '%s' in : [%s -- %s)"
                 %(old_flag_val, flag_val, ns2date(i[1]), ns2date(i[2])))

    log.info(line + "\n")

def ns2date(ns):
    """Converts number of seconds from the Epoch to local date and time."""
    from PyCool import cool
    if ns == cool.ValidityKeyMax:
        return "End of days..."
    elif ns is None:
        return None

    import time
    return time.ctime(ns/1e9)

def indent_xml(elem, level=0):
    """
    Indent an ElementTree instance to allow pretty print.
    Code taken from http://effbot.org/zone/element-lib.htm
    """
    i = "\n" + level*"  "
    if len(elem):
        if not elem.text or not elem.text.strip():
            elem.text = i + "  "
        if not elem.tail or not elem.tail.strip():
            elem.tail = i
        for child in elem:
            indent_xml(child, level+1)
        if not child.tail or not child.tail.strip():
            child.tail = i
        if not elem.tail or not elem.tail.strip():
            elem.tail = i
    else:
        if level and (not elem.tail or not elem.tail.strip()):
            elem.tail = i

def format_text(text, color_name, blink = False, underline = False):
    color_vocab = {'black':30,'red':31,'green':32,'yellow':33,'blue':34,\
                   'magenta':35,'cyan':36,'white':37,'':''}
    ##########################################################################
    if color_name in color_vocab:
        color_num = str(color_vocab.get(color_name)) + ';'
    else:
        log.info("I don't know the requested color name '%s'. Known are: %s.\
        \nUsing 'White'.."%(color_name,color_vocab.keys()))
        color_num = ''
    ##########################################################################
    if blink: blink = '5;'
    else: blink = ''
    ##########################################################################
    if underline:
        underline = '4;'
    else:
        underline = ''
    ##########################################################################
    return '\x1B[' + '%s%s%s49m' %(color_num, blink, underline) + text + \
           '\x1B['+ '0m'

def connect2db(destination, readOnly = True):
    """Function to connect to CondDB. Returns db instance"""
    destination = destination.lower()
    from CondDBUI import CondDB
    if destination == 'sqlite':
        return CondDB('sqlite_file:%s/DQFLAGS.db/DQFLAGS'
                      %os.environ['SQLITEDBPATH'], readOnly = readOnly)
    elif destination == 'oracle':
        # TODO: connect to Cern Oracle database
        raise Exception, ("Sorry, DQ flag commitment to the Oracle db is not yet implemented.")
    else:
        raise Exception, "Destination '%s' is unknown. Allowed are 'sqlite' and 'oracle'."%destination

def add_xml_head(root_element):
    """ Takes root element instance and returns an xml string with added xml header"""
    indent_xml(root_element)
    import xml.etree.ElementTree as ET
    xml_splitted = ET.tostring(root_element, encoding='ISO-8859-1').split('<DDDB>')
    xml_splitted.insert(1, '<!DOCTYPE DDDB SYSTEM "conddb:/DTD/structure.dtd">\n<DDDB>')
    return reduce(lambda x,y:x+y,xml_splitted)

def get_payloads(db, since, until, tag):
    """Retrieve all payloads in each IOV which is covered by [since, until] period.

    Modify first and last IOV boundaries and eliminate payloads which should
    not be modified."""

    payloads_xml_2Dlist = db.getXMLStringList('/Conditions/DQ/Flags',
                                              since,
                                              until,
                                              tag = tag)
    if len(payloads_xml_2Dlist) == 0:
        return None
    # Check 'until' value is not equal to any IOV boundary value
    if len(payloads_xml_2Dlist) != len(db.getXMLStringList('/Conditions/DQ/Flags',
                                                           since,
                                                           until-1,
                                                           tag = tag)):
        # Remove from the list of payloads the last one which was grabbed into
        #the list by getXMLStringList() just because 'until' value equals the
        #start point of this payload.
        del payloads_xml_2Dlist[-1]

    # Change the the start time point of first payload to 'since' and the end
    # time point of last payload to have 'until' value.
    payloads_xml_2Dlist[0][1] = since
    payloads_xml_2Dlist[-1][2] = until

    return payloads_xml_2Dlist

def _fill_dq_update(data,root_element,payload_list):
    if not (data or root_element or payload_list):
        return
    data.append([add_xml_head(root_element),
                 payload_list[1],
                 payload_list[2],
                 payload_list[3],
                 payload_list[4]])

def prepare_dq_changes(xml_2Dlist, flag, flag_value):
    """Takes 2D list of xml strings and returns list of edited xml strings.

    2D list of xml strings has the form:
    [[payload1,since,until1],...,[payloadN,sinceN,until]],
    where 'since' and 'until' are new IOV boundaries, and all other time points
    are current IOV time points in the database.
    """
    if not xml_2Dlist:
        return None, None
    updated_xml_2Dlist = []
    flag_changes_summary = [[flag,flag_value],]
    import xml.etree.ElementTree as ET
    # Check and modify if needed payload for every IOV.
    for payload in xml_2Dlist:
        # DDDB, map and item below are xml elements
        DDDB = ET.fromstring(payload[0])
        map = DDDB.find('condition/map')
        flag_found = False
        for item in map:
            if item.get('key') == flag:
                flag_found = True
                flag_changes_summary.append([item.get('value'),
                                             payload[1],
                                             payload[2]])

                # Modify or remove the flag from the xml tree
                if flag_value != 'remove':
                    if item.get('value') != flag_value:
                        item.set('value', flag_value)
                    else: break
                else:
                    map.remove(item)
                _fill_dq_update(updated_xml_2Dlist,DDDB,payload)
                break

        # Create new flag
        if not flag_found:
            if flag_value != 'remove':
                flag_changes_summary.append([None, payload[1], payload[2]])
                ET.SubElement(map,'item', {'key':flag, 'value':flag_value})
                _fill_dq_update(updated_xml_2Dlist,DDDB,payload)
            else:
                flag_changes_summary.append([None, payload[1], payload[2]])

    #print xml_2Dlist
    #print len(xml_2Dlist)
    #print updated_xml_2Dlist
    #print len(updated_xml_2Dlist)
    if updated_xml_2Dlist == []:
        log.warning('There is no difference between what you want to '
                    'introduce and what is in the database.\n')
    return updated_xml_2Dlist, flag_changes_summary

def confirm_dq_changes(f_c_s):
    """ Function to get user or script answer should the commitment be
        continued or not.

        Inputs:
          f_c_s - list of lists; flag changes summary in the form:
                  [[flag, new_flag_value],[old_flag_value1,since1,until1],...,
                   [old_flag_valueN,sinceN,untilN]]
        Outputs:
         Boolean - decision should the commitment be continued or not.

    """

    # Show flag changes summary for user
    if f_c_s[0][1] == 'remove':
        _print_dq_summary('REMOVE', f_c_s)
    elif None not in [j for i in f_c_s[1:] for j in i]:
        _print_dq_summary('change', f_c_s)
    else:
        _print_dq_summary('CREATE NEW', f_c_s)
    # Generate script answer 'No', which is obvious if there are no flag changes
    # summary or no flag changes in the summary ################################
    if not f_c_s or len(f_c_s) <= 1:
        log.info("You're proposing nothing new. Exiting.")
        return False

    # Check if requested flag value change is already the current value in the db
    current_flag_values = [i[0] for i in f_c_s[1:]]
    if False not in map(lambda x: x == f_c_s[0][1], current_flag_values):
        log.info("You're proposing nothing new. Exiting.")
        return False
    if  f_c_s[0][1] == 'remove':
        if False not in map(lambda x: x == None, current_flag_values):
            log.info("You're proposing nothing new. Exiting.")
            return False
   #############################################################################

    ans = None
    while ans is None:
        ans = raw_input("Do you really want to commit the changes (Yes,[No])? ")
        if not ans: ans = "No"
        if ans not in [ "Yes", "No" ]:
            print "You have to type exactly 'Yes' or 'No'"
            ans = None
    if ans == "No":
        log.info("Aborted by user cleanly. No changes to the target database were done.")
        return False
    try:
        import time
        log.info("Committing the changes in ...")
        timeout = 5
        while timeout >= 0:
            log.info("%d seconds"%timeout)
            time.sleep(1)
            timeout -= 1
    except KeyboardInterrupt:
        log.info("Aborted by user cleanly. No changes to the target database were done.")
        return False

    log.info("Committing the changes...")
    return True

def apply_dq_changes(db, data):
    if not db:
        log.error("No database given to be tagged.")
        return None
    if data == (None or []):
        log.info("No data was given to be applied to the database.")
        return None
    for payload in data:
        db.storeXMLString('/Conditions/DQ/Flags', payload[0], payload[1], payload[2])
    log.info("Changes applied.")
    return db

def tag_dq_changes(db,local_tag):
    if not db:
        log.error("Nothing to be tagged.")
        return None
    if not local_tag:
        log.error('No local tag name is given.')
        return db
    db.moveTagOnNodes("/",local_tag,{'/Conditions/DQ/Flags':'HEAD'})
    log.info("Changes are tagged as '%s'." %local_tag)
    return db

def close_db(db):
    if not db:
        return None
    db.closeDatabase()
    return True

def update_dom_rel_notes(options, local_tag, flag, flag_value):
    from CondDBUI.Admin import ReleaseNotes
    rel_notes = ReleaseNotes(options.rel_notes)
    descr = options.message.splitlines()
    descr[0] = "%s -> '%s'. "%(flag,flag_value) + descr[0] # not +=
    rel_notes.addNote(contributor = options.contributor,
                      partitions = {'DQFLAGS':(local_tag, {'modified':['/Conditions/DQ/Flags'],'added':[]})},
                      description = descr,
                      datatypes = ["All"])
    return rel_notes

def _commit_dq_flag(db, flag, flag_value, since, until, ref_tag, local_tag, rel_notes):

    if db.resolveTag('/',local_tag):
        log.warning("The tag '%s' is already in the database, check the name.\n"%local_tag)

    prepared_data, flag_changes_summary = \
    prepare_dq_changes(get_payloads(db, since, until, ref_tag),flag, flag_value)

    if confirm_dq_changes(flag_changes_summary):
        try:
            if close_db( tag_dq_changes( apply_dq_changes(db, prepared_data),local_tag) ):
                rel_notes.write()
                log.info("Release notes are updated.")
            return 0
        except KeyboardInterrupt:
            log.info("Aborted by user during the applying stage. Check the target.")
            return 1
    else:
        close_db(db)
        return 0

def main():

    _set_logging()
    options,(flag, flag_value, local_tag) = _get_options()
    return _commit_dq_flag(connect2db(options.dest, False),
                           flag,
                           flag_value,
                           options.since,
                           options.until,
                           options.ref_tag,
                           local_tag,
                           update_dom_rel_notes(options, local_tag, flag, flag_value) )

if __name__ == '__main__':
    import sys
    sys.exit(main())
