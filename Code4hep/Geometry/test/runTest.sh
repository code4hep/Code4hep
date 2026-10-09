#!/bin/bash

# put included files in test directory
echo $PWD
export DD4HEP_XML_DIR=$(dirname $(readlink -f $(which ddeve)))/../DDDetectors/compact
ln -sf ${DD4HEP_XML_DIR}/elements.xml .
ln -sf ${DD4HEP_XML_DIR}/materials.xml .
C4H_DIR=$(readlink -f ${PWD}/../../../)
INSTALL_DIR=$(readlink -f ${C4H_DIR}/../)
# this is a very temporary hack around edm::FileInPath restrictions
export CMSSW_SEARCH_PATH=${C4H_DIR}
export CMSSW_BASE=${INSTALL_DIR}
cmsRun testGeo.py
