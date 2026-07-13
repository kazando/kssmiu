set terminal pdfcairo enhanced color font "Helvetica,10" size 3.4in,2.7in
set output "fig2_euclidean_time_p1.pdf"

set xlabel "Neighborhood parameter k"
set ylabel "Average total running time (s)"
set xrange [0.5:20.5]
set logscale y
set xtics ("1" 1, "2" 2, "3" 3, "4" 4, "5" 5, "8" 8, "10" 10, "12" 12, "20" 20)
set key left top
set grid

plot \
    "euclidean_time_p1.dat" using 1:2 with linespoints lw 2 pt 7 title "n = 100", \
    "euclidean_time_p1.dat" using 1:4 with linespoints lw 2 pt 5 title "n = 250", \
    "euclidean_time_p1.dat" using 1:6 with linespoints lw 2 pt 9 title "n = 500"