from DaVinci.Configuration import isNewCondDBTag

def test_new_tags() :
    tags = ["sim-20100510-vc10mm-mu100",
            "sim-20100510-vc10mm-md100",
            "sim-20100510-vc-mu100",
            "sim-20100510-vc-moff",
            "sim-20100510-vc-md100",
            "hlt-20100509",
            "head-20100504",
            "MC09-20100430-vc-md100",
            "MC09-20100430-vc-moff",
            "MC09-20100430-vc-mu100",
            "MC09-20100430-vo-mu100",
            "MC09-20100430-vo-moff",
            "MC09-20100430-vo-md100",
            "hlt-20100430",
            "head-20100430",
            "sim-20100429-vc-md100"]

    for tag in tags :
        assert isNewCondDBTag(tag) == True

def test_old_tags() :
    tags = ["hlt-20100414",
            "head-20100414",
            "MC-20100412-vc15mm-md100",
            "sim-20100412-vc-mu100",
            "default"]

    for tag in tags :
        assert isNewCondDBTag(tag) == False
