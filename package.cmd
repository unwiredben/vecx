@echo off
rm -rf STAGING
mkdir STAGING
copy vecx STAGING
copy appinfo.json STAGING
copy pdk_logo_64x64.png STAGING
copy rom.dat STAGING
echo filemode.755=vecx>STAGING/package.properties
call palm-package.bat STAGING
