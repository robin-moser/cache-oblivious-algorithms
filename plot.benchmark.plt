# vi: set ft=gnuplot :
set terminal pdf enhanced font "Times-New-Roman,12" size 16cm, 6cm
set output 'plot.benchmark.pdf'
set datafile separator ","

set grid mxtics xtics ytics
set key top left

# ################

set ylabel 'Laufzeit (s)'
set xlabel 'Matrixgröße (n)'

set title 'Matrix Transpose'
plot 'data.transpose.csv' using 1:2 title 'standard' with linespoints, \
     'data.transpose.csv' using 1:3 title 'tiled (bs=32)' with linespoints, \
     'data.transpose.csv' using 1:4 title 'cache oblivious' with linespoints
