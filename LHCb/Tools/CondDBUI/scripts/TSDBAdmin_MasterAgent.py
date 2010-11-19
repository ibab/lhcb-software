#!/usr/bin/env python

__author__ = "Illya Shapoval <illya.shapoval@cern.ch>"
__version__ = "$Id: TSDBAdmin_MasterAgent.py,v 1.0 2010-11-15 00:00:00 ishapova Exp $"

import urllib
import cPickle as pickle
import cgi
import cgitb
cgitb.enable()
from CondDBUI.Admin.TagStatusDB import *

def print_html_response(response):
    print "Content-Type: text/html\n"
    print response

def main():
    FormData = cgi.FieldStorage()

    db = TagStatusDB("sqlite:TAGSTATUS.db")

    request = FormData["request"].value
    tier = unicode(FormData["tier"].value)
    tags_to_check_info = {}
    good_tags_info = {}

    if request == "bad":
        tags_to_check = {"DDDB":db.getTagsToCheck(tier,u"DDDB"),
                         "LHCBCOND":db.getTagsToCheck(tier,u"LHCBCOND"),
                         "SIMCOND":db.getTagsToCheck(tier,u"SIMCOND")}

        for key in tags_to_check.keys():
            tags_to_check_info[key] = [i.dumpInfo() for i in tags_to_check[key]]
        response = {"CHECK":tags_to_check_info}
        print_html_response(pickle.dumps(response))

    elif request == "good-bad-ugly":
        tags_to_check = {"DDDB":db.getTagsToCheck(tier,u"DDDB"),
                         "LHCBCOND":db.getTagsToCheck(tier,u"LHCBCOND"),
                         "SIMCOND":db.getTagsToCheck(tier,u"SIMCOND")}

        for key in tags_to_check.keys():
            tags_to_check_info[key] = [i.dumpInfo() for i in tags_to_check[key]]
        response = {"CHECK":tags_to_check_info}

        good_tags = {"DDDB":db.getGoodTags(tier,u"DDDB"),
                     "LHCBCOND":db.getGoodTags(tier,u"LHCBCOND"),
                     "SIMCOND":db.getGoodTags(tier,u"SIMCOND")}
        for key in tags_to_check.keys():
            good_tags_info[key] = [i.dumpInfo() for i in good_tags[key]]
        response["GOOD"] = good_tags_info

        print_html_response(pickle.dumps(response))
    else:
        print_html_response(pickle.dumps(None))


if __name__ == '__main__':
    import sys
    sys.exit(main())