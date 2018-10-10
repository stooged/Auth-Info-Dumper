#!/bin/bash
set -e
pushd tool
make
popd
pushd Auth_Info_Dumper
make
popd
mkdir -p bin
rm -f bin/Auth_Info_Dumper.bin
cp Auth_Info_Dumper/Auth_Info_Dumper.bin bin/Auth_Info_Dumper.bin
mkdir -p html_payload
tool/bin2js bin/Auth_Info_Dumper.bin > html_payload/payload.js
FILESIZE=$(stat -c%s "bin/Auth_Info_Dumper.bin")
PNAME=$"Auth-Info Dumper"
cp exploit.template html_payload/Auth_Info_Dumper.html
sed -i -f - html_payload/Auth_Info_Dumper.html << EOF
s/#NAME#/$PNAME/g
s/#BUF#/$FILESIZE/g
s/#PAY#/$(cat html_payload/payload.js)/g
EOF
rm -f Auth_Info_Dumper/Auth_Info_Dumper.bin
rm -f html_payload/payload.js
