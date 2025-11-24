#!/bin/bash

# Pass in name and status
function die { echo $1: status $2 ;  exit $2; }

LOCAL_TEST_DIR="${CMSSW_BASE}/src/Code4hep/Test/test"
F1=${LOCAL_TEST_DIR}/testPodioSource_cfg.py

# create an input file to use as input for the test
createEDM4hepFile.py

# We need a second file to test that multiple files can be read.
# The easiest way to create a second file is to copy the first one.
cp edm4hep.root edm4hep_copy.root

# The following test checks that the expected run, lumi and event
# transitions occur as the Framework processes the file.
# It also checks that run, lumi and event numbers delivered from the
# Run, Lumi and Event objects have the expected values.
# The test depends on the content of the input files. These
# input files are created by the createEDM4hepFile.py script
# called above. createEDM4hepFile.py is a script from the
# external repository named EDM4hep. We have no control over
# that repository. If we update the version of EDM4hep and
# createEDM4hepFile.py has changed, then this test may fail.
# If that happens, then we will need to update the expected
# values in the testPodioSource_cfg.py file.
(cmsRun $F1 ) || die "Failure using $F1" $?
