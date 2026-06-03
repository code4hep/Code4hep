#!/bin/bash

# put included files in test directory
export TEST_DIR=${LOCALTOP}/src/Code4hep/Geometry/test
export DD4HEP_XML_DIR=$(scram tool tag dd4hep-core DD4HEP_CORE_BASE)/DDDetectors/compact
ln -sf ${DD4HEP_XML_DIR}/elements.xml ${TEST_DIR}/
ln -sf ${DD4HEP_XML_DIR}/materials.xml ${TEST_DIR}/
cmsRun ${TEST_DIR}/testGeo.py
