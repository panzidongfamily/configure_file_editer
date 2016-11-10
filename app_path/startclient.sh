#!/bin/bash
export APP_INSTALL_PATH=/apps/regtool
export PATH=${APP_INSTALL_PATH}/bin:$PATH
export PWD=${APP_INSTALL_PATH}/bin
regtool_client
