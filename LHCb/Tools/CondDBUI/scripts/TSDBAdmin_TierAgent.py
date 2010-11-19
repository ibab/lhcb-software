#!/usr/bin/env python

__author__ = "Illya Shapoval <illya.shapoval@cern.ch>"
__version__ = "$Id: TSDBAdmin_TierAgent.py,v 1.0 2010-11-15 00:00:00 ishapova Exp $"

import urllib
import cPickle as pickle
import CondDBUI
import os
os.environ["COOL_IGNORE_LFC"] = "1"

def main():
    ###########################################################################
    # Configure the parser
    ###########################################################################
    from optparse import OptionParser
    parser = OptionParser(usage = "%prog [options]",
                          version = __version__,
                          description =
"""This script requests a web server for invalid (or both: valid & invalid) tags at Tier.

With default request type (='bad'), the script will get (from TagStatusDB) all tags
known currently to be invalid. re-check them if they become valid, and return
the list of those which become valid.
""")

    parser.add_option("-r", "--request", type = "string",
                      help = "Two request types are possible: 'bad' (get invalid tags)"
                      " and 'good-bad-ugly' (get valid and invalid tags). "
                      "DEFAULT: 'bad'."
                      )
    parser.add_option("-w", "--web-server", type = "string",
                      help = "Web server URL where the script for interaction"
                      " with the TagStatusDB is located."
                      )
    parser.add_option("-t", "--tier", type = "string",
                      help = "Tier site name (e.g.: LCG.CNAF.it)."
                      )
    parser.add_option("--recheck", action = "store_true",
                      help = "Re-check invalid tags and return those of them "
                      "which have resurrected."
                      )

    parser.set_default("request", 'bad')
    parser.set_default("web_server", "http://127.0.0.1/cgi-bin/TSDBAdmin_Scout.cgi")
    parser.set_default("tier", None)

    # parse command line
    options, args = parser.parse_args()

    ###########################################################################
    # Prepare local logger
    ###########################################################################
    import logging
    log = logging.getLogger(parser.prog or os.path.basename(sys.argv[0]))
    log.setLevel(logging.INFO)

    # set the global stream handler
    from CondDBUI import LOG_FORMAT
    hndlr = logging.StreamHandler()
    hndlr.setFormatter(logging.Formatter(LOG_FORMAT))
    logging.getLogger().handlers = [hndlr]

    ###########################################################################
    # Check and set options
    ###########################################################################
    request = options.request
    if request not in ["bad","good-bad-ugly"]:
        parser.error( "Request type is unfamiliar.")
    if request == "good-bad-ugly" and options.recheck:
        parser.error("Re-checking tags is not applicable with this request type.")

    url = options.web_server

    if not options.tier:
        parser.error("You have to give the name of Tier site to check a tag at.")
    tier = unicode(options.tier)

    ###########################################################################
    # Interact with the server-side script
    ###########################################################################
    response = urllib.urlopen(url+"?request=%s&tier=%s"%(request,tier))

    if request == "bad":
        tags_to_check = pickle.loads(response.read())['CHECK']
        if options.recheck:
            tags_to_update = {}
            import hashlib, datetime
            for partition in tags_to_check.keys():
                tags_to_update[partition]=[]
                tier_db_conn_str = str(tier).split(".")[1].lower() + "/%s"%partition
                db = CondDBUI.CondDB(tier_db_conn_str)
                for tag_dict in tags_to_check[partition]:
                    log.info("Checking '%s' tag:"%tag_dict["TagName"])
                    initHashObj = getattr(hashlib,tag_dict["Hash_alg"])()
                    hash_sum = db.payloadToHash(initHashObj,tag=str(tag_dict["TagName"])).hexdigest()
                    if unicode(hash_sum) == tag_dict["Master_hash_sum"]:
                        tag_dict["Last_ok_time"] = datetime.datetime.now()
                        tags_to_update[partition].append(tag_dict)
            print tags_to_update
        else:
            print tags_to_check
    elif request == "good-bad-ugly":
        good_bad_tags = pickle.loads(response.read())
        print "Tags to check are: ", good_bad_tags['CHECK']
        print "Good tags are: ", good_bad_tags['GOOD']

if __name__ == '__main__':
    import sys
    sys.exit(main())