#!/bin/env python

# $Id:

"""
Check the space used by the 3 LHCb AFS project space.
If the Space available is less than 5% a mail is send to lhcb-afs-admins@cern.ch
do make the request of more space
"""

from subprocess import Popen, PIPE
import re, sys
import logging, logging.handlers


def check_space(thelog):
    nb_line = 0
    cmd = ["afs_admin", "list_project", "lhcb"]
    proc = Popen(cmd, stdout=PIPE)

    for line in proc.stdout.xreadlines() :
        thelog.debug(line.strip())
        nb_line += 1
        if nb_line < 6 :
            member = line.split()
            if member[0] in ('lhcb', 'lhcb_q','lhcb_s'):
               free_space = str( int(member[1]) - int(member[2]) )
               space_left = str( int(free_space)  * 100 / int(member[1]))
               if int(space_left) < 10 :
                   size_GB = int(member[1]) / 100000
                   free_GB = int(free_space) / 100000
                   thelog.info('The space left for %s represents %s percent \n (Total is %s GB and free is %s GB)' %( member[0], space_left, size_GB, free_GB))        else:
            break

    return True

def main():
    thelog = logging.getLogger("AFS Project Space checking")
    thelog.setLevel(logging.DEBUG)
    formatter = logging.Formatter("%(asctime)s - %(levelname)-8s: %(message)s")
    sm = logging.handlers.SMTPHandler('localhost','joel.closier@cern.ch','lhcb-afs-admins@cern.ch','LHCb AFS project space')
    sm.setLevel(logging.INFO)
    sm.setFormatter(formatter)
    thelog.addHandler(sm)
    check_space(thelog)
    sys.exit(0)


if __name__ == '__main__':
    main()


