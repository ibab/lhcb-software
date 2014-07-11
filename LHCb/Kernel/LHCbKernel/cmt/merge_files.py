#!/usr/bin/env python

# @file: GaudiPolicy/cmt/fragments/merge_files.py
# @purpose: merge_files <fragment> file into a 'per-project' <merged> file
# @author: Sebastien Binet <binet@cern.ch>

import os
import sys
import re
from datetime import datetime

if 'GAUDIPOLICYROOT' in os.environ:
    sys.path.append(os.path.join(os.environ['GAUDIPOLICYROOT'], 'scripts'))

import locker

def mergeFiles( fragFileNames, mergedFileName, commentChar, doMerge, ignoreMissing, excludeREs=None ):

    startMark = "%s --Beg " % commentChar
    timeMark  = "%s --Date inserted: %s" % (commentChar, datetime.now())
    endMark   = "%s --End " % commentChar
    nameOffset = len(startMark)

    basenames = map(os.path.basename, fragFileNames)

    isNewFile = not os.path.exists(mergedFileName)

    if not excludeREs:
        excluded = lambda _: False
    else:
        excludeREs = [re.compile(exp) if type(exp) is str else exp
                      for exp in excludeREs]
        def excluded(line):
            for exp in excludeREs:
                if exp.match(line):
                    return True
            return False

    # create an empty file if it does not exist
    # "append mode" ensures that, in case of two processes trying to
    # create the file, they do not truncate each other file
    if isNewFile:
        # check if the destination directory exists
        path_to_file = os.path.split(mergedFileName)[0]
        if path_to_file and not os.path.isdir(path_to_file):
            # if doesn't exist, create it
            os.makedirs(path_to_file)
        open(mergedFileName,'a')

    mergedFile = open( mergedFileName, 'r+' )

    # locking file, gaining exclusive access to it
    lock = locker.lock( mergedFile )
    try:

        newLines = [ ]
        skipBlock = ""
        for line in mergedFile.readlines():
            if line.startswith(startMark) and line[nameOffset:].strip() in basenames:
                skipBlock = endMark + line[nameOffset:].strip()
                # remove all the empty lines occurring before the start mark
                while (len(newLines) > 0) and (newLines[-1].strip() == ''):
                    newLines.pop()
            if not skipBlock:
                newLines.append(line)
            if line.startswith(skipBlock):
                skipBlock = ""
        if skipBlock:
            print "WARNING: missing end mark ('%s')" % skipBlock

        if doMerge:
            for f in fragFileNames:
                if ignoreMissing and not os.path.exists(f):
                    print "WARNING: '%s' does not exist, I'm ignoring it" % f
                    continue
                # I do not want to add 2 empty lines at the beginning of a file
                if newLines:
                    newLines.append('\n\n')
                bf = os.path.basename(f)
                newLines.append(startMark + bf + '\n')
                newLines.append(timeMark + '\n')
                newLines.extend(line
                                for line in open(f, 'r')
                                if not excluded(line))
                if not newLines[-1].endswith('\n'):
                    newLines.append('\n')
                newLines.append(endMark + bf + '\n')

        mergedFile.seek(0)
        mergedFile.truncate(0)
        mergedFile.writelines(newLines)

    finally:
        # unlock file
        locker.unlock( mergedFile )

    return 0

if __name__ == "__main__":

    from optparse import OptionParser
    parser = OptionParser(usage="usage: %prog [options]")
    parser.add_option(
        "-i",
        "--input-file",
        action = "append",
        dest = "fragFileNames",
        default = [],
        help = "The path and name of the file one wants to merge into the 'master' one"
        )
    parser.add_option(
        "-m",
        "--merged-file",
        dest = "mergedFileName",
        default = None,
        help = "The path and name of the 'master' file which will hold the content of all the other fragment files"
        )
    parser.add_option(
        "-c",
        "--comment-char",
        dest = "commentChar",
        default = "#",
        help = "The type of the commenting character for the type of files at hand (this is an attempt at handling the largest possible use cases)"
        )
    parser.add_option(
        "--do-merge",
        dest = "doMerge",
        action = "store_true",
        default = True,
        help = "Switch to actually carry on with the merging procedure"
        )
    parser.add_option(
        "--un-merge",
        dest = "unMerge",
        action = "store_true",
        default = False,
        help = "Switch to remove our fragment file from the 'master' file"
        )
    parser.add_option(
        "--stamp-dir",
        dest = "stampDir",
        action = "store",
        default = None,
        help = "Create the stamp file in the specified directory. If not specified"
              +" the directory of the source file is used."
        )
    parser.add_option(
        "--no-stamp",
        action = "store_true",
        help = "Do no create stamp files."
        )
    parser.add_option(
        "--ignore-missing",
        dest = "ignoreMissing",
        action = "store_true",
        help = "Print a warning if a fragment file is missing, but do not fail."
        )
    parser.add_option(
        "--exclude-re",
        action = "append",
        help = "Exclude lines matching a regular expression when merging a file.",
        default = []
        )

    (options, args) = parser.parse_args()

    # ensure consistency...
    options.doMerge = not options.unMerge

    # allow command line syntax as
    #   merge_files.py [options] <fragment file1> [<fragment file2>...] <merged file>
    if args:
        options.mergedFileName = args[-1]
        options.fragFileNames += args[:-1]

    sc = 1
    if not options.fragFileNames or \
       not options.mergedFileName :
        str(parser.print_help() or "")
        print "*** ERROR ***",sys.argv
        sys.exit(sc)
        pass

    if options.stampDir is None:
        stampFileName = lambda x: x + ".stamp"
    else:
        stampFileName = lambda x: os.path.join(options.stampDir,
                                               os.path.basename(x)
                                               + ".stamp")
    # Configure Python logging
    import logging
    logging.basicConfig(level = logging.INFO)

    if "GAUDI_BUILD_LOCK" in os.environ:
        globalLock = locker.LockFile(os.environ["GAUDI_BUILD_LOCK"], temporary =  True)
    else:
        globalLock = None

    if True: #try:
        sc = mergeFiles( options.fragFileNames, options.mergedFileName,
                         options.commentChar,
                         doMerge = options.doMerge,
                         ignoreMissing = options.ignoreMissing,
                         excludeREs = options.exclude_re)
        if not options.no_stamp:
            for stamp in map(stampFileName, options.fragFileNames):
                open(stamp, 'w')
    #except IOError, err:
    #    print "ERROR:",err
    #except Exception, err:
    #    print "ERROR:",err
    #except:
    #    print "ERROR: unknown error !!"

    del globalLock

    sys.exit( sc )
