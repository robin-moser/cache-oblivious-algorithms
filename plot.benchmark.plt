# vi: set ft=gnuplot :
set terminal pdf enhanced font "Times-New-Roman,12" size 14cm, 8cm
set output 'plot.benchmark.pdf'
set datafile separator ","

set grid mxtics xtics ytics
set key top left

# ################

set ylabel 'Laufzeit (s)'
set xlabel 'Matrixgröße (n)'

set title 'Matrix Transposition'
plot \
     '< sort -n data.transpose.csv' using 1:2 title 'standard' lc "red" with linespoints, \
     '< sort -n data.transpose.csv' using 1:6 title 'tiled (bs=32)' lc "grey" with linespoints, \
     '< sort -n data.transpose.csv' using 1:5 title 'tiled (bs=16)' lc "grey" with linespoints, \
     '< sort -n data.transpose.csv' using 1:4 title 'tiled (bs=8)' lc "magenta" with linespoints, \
     '< sort -n data.transpose.csv' using 1:3 title 'cache oblivious' lc "blue" with linespoints, \

set xrange [0:*]
set title 'Matrix Multiplikation'
plot \
     '< sort -n data.multiply.csv' using 1:2 title 'standard' lc "red" with linespoints, \
     '< sort -n data.multiply.csv' using 1:6 title 'tiled (bs=32)' lc "grey" with linespoints, \
     '< sort -n data.multiply.csv' using 1:5 title 'tiled (bs=16)' lc "grey" with linespoints, \
     '< sort -n data.multiply.csv' using 1:4 title 'tiled (bs=8)' lc "magenta" with linespoints, \
     '< sort -n data.multiply.csv' using 1:3 title 'cache oblivious' lc "blue" with linespoints, \
