#!/bin/bash
APP_INSTALL=/apps/regtool
echo "$APP_INSTALL install..."
cp $APP_INSTALL/bin/regtool_client /usr/bin
cp $APP_INSTALL/bin/regtool_server /usr/bin
cd /usr/bin
ln -s regtool_client regset
ln -s regtool_client regget
ln -s regtool_client regdel
ln -s regtool_client regrm
/usr/bin/regtool_server
echo "$APP_INSTALL done!"
