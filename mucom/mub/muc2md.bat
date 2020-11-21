rem @echo off
mucom88 -c test00.muc -o test00.mub
mucom88 -c test01.muc -o test01.mub

..\bintosMub.exe mub_list.txt

cp mucom88mdz_mub.h ../../src
cp mucom88mdz_mub.c ../../src
cp mucom88mdz_mub*.s ../../src
