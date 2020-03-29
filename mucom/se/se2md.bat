mucom88 -c BOM.muc -o BOM.mub
mucom88 -c CANCEL00.muc -o CANCEL00.mub 
mucom88 -c CANCEL01.muc -o CANCEL01.mub
mucom88 -c COIN.muc -o COIN.mub
mucom88 -c CURSOR.muc -o CURSOR.mub
mucom88 -c OK.muc -o OK.mub
mucom88 -c PAUSE.muc -o PAUSE.mub
mucom88 -c SHOT.muc -o SHOT.mub 
mucom88 -c NOISE0000.muc -o NOISE0000.mub

..\cutoutMub.exe se_list.txt 

copy *.c ..\..\src
copy *.h ..\..\src
copy *.s ..\..\src
