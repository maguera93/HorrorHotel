ccpsx -Xo0x80010000 -Wall -O2 main.c -o main.cpe
cpe2x /ca main.cpe

.\mkpsxiso\mkpsxiso.exe -o game.bin -y .\mkpsxiso\cuesheet.xml

"C:\ePSXe202-1\ePSXe.exe" game.cue

del mem.map
del main.sym
del main.cpe
del image.cue
del game.iso