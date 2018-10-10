#!/bin/bash
pushd tool
make clean
popd
pushd Auth_Info_Dumper
make clean
popd
rm -f html_payload/Auth_Info_Dumper.html
rm -f bin/Auth_Info_Dumper.bin

