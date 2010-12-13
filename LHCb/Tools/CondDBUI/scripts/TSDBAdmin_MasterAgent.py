#!/usr/bin/env python

__author__ = "Illya Shapoval <illya.shapoval@cern.ch>"
__version__ = "$Id: TSDBAdmin_MasterAgent.py,v 1.0 2010-11-15 00:00:00 ishapova Exp $"

import urllib
import cPickle as pickle
from datetime import datetime

import cgi
import cgitb
cgitb.enable()
from CondDBUI.Admin.TagStatusDB import *

def print_html(response):
    print "Content-Type: text/html\n"
    print response

def convert_response_2_txt(response):
    lines = []
    for key in response.keys():
        if key == 'CHECK': color = 'red'
        elif key == 'GOOD': color = 'green'
        elif key == 'HalfWay': color = 'yellow'
        else: color = '-'

        for partition in response[key].keys():
            if len(response[key][partition]) != 0:
                for tag_dict in response[key][partition]:
                    lines.append("%s %s/%s %s %s -\n"
                                 %(str(datetime.now()).split('.')[0].replace(':','-'),
                                   partition,
                                   str(tag_dict['TagName']),
                                   key,
                                   color))
    return reduce(lambda x,y:x+y, lines)

def main():
    FormData = cgi.FieldStorage()

    try:
        request = FormData["request"].value
        tier = unicode(FormData["tier"].value)
    except KeyError:
        print_html("What do you want from me?")
        return 0

    db = TagStatusDB("sqlite:../tsdb/db/TAGSTATUS.db")

    # Check arguments
    if tier not in db.sites:
        print_html("Sorry, '%s' site is unknown to me." %tier)
        return 0
    if request not in ["bad","good-bad-ugly"]:
        print_html("Sorry, request '%s' is unknown to me." %request)
        return 0

    tags_to_check_info = {}
    tags_being_verified_info = {}
    good_tags_info = {}
    response = {}

    # Process the request
    if request == "bad":
        tags_to_check = {"DDDB":db.getTagsToCheck(tier,u"DDDB"),
                         "LHCBCOND":db.getTagsToCheck(tier,u"LHCBCOND"),
                         "SIMCOND":db.getTagsToCheck(tier,u"SIMCOND")}

        for key in tags_to_check.keys():
            tags_to_check_info[key] = [i.dumpInfo() for i in tags_to_check[key]]
        response["CHECK"] = tags_to_check_info

        print_html(pickle.dumps(response))

    elif request == "good-bad-ugly":
        # Tags which must be checked
        tags_to_check = {"DDDB":db.getTagsToCheck(tier,u"DDDB",withLatency=True),
                         "LHCBCOND":db.getTagsToCheck(tier,u"LHCBCOND",withLatency=True),
                         "SIMCOND":db.getTagsToCheck(tier,u"SIMCOND",withLatency=True)}
        for key in tags_to_check.keys():
            tags_to_check_info[key] = [i.dumpInfo() for i in tags_to_check[key]]
        response["CHECK"] = tags_to_check_info

        # Tags which are probably under verification now
        tags_being_verified = {"DDDB":db.getHalfWayTags(tier,u"DDDB"),
                               "LHCBCOND":db.getHalfWayTags(tier,u"LHCBCOND"),
                               "SIMCOND":db.getHalfWayTags(tier,u"SIMCOND")}
        for key in tags_being_verified.keys():
            tags_being_verified_info[key] = [i.dumpInfo() for i in tags_being_verified[key]]
        response["HalfWay"] = tags_being_verified_info

        # Good tags
        good_tags = {"DDDB":db.getGoodTags(tier,u"DDDB"),
                     "LHCBCOND":db.getGoodTags(tier,u"LHCBCOND"),
                     "SIMCOND":db.getGoodTags(tier,u"SIMCOND")}
        for key in tags_to_check.keys():
            good_tags_info[key] = [i.dumpInfo() for i in good_tags[key]]
        response["GOOD"] = good_tags_info

        print_html(convert_response_2_txt(response))
    else:
        print_html(pickle.dumps(None))


if __name__ == '__main__':
    import sys
    sys.exit(main())
