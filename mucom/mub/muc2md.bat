@echo off
mucom88 -c test00.muc -o test00.mub
mucom88 -c test01.muc -o test01.mub

..\bintosMub.exe mub_lists.txt
