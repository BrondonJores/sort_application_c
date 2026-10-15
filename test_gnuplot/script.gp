set terminal png size 800,600
set output 'test_gnuplot/plot.png'
set style fill solid 0.8
set grid y
set title "Temps d'exécution moyen (s)"
set ylabel "Temps (s)"
set boxwidth 0.5
set yrange [0:*]
set xtics rotate by -30
plot 'test_gnuplot/data.dat' using 0:2:3:xtic(1) with boxerrorbars title 'Moyenne (+/- Ecart-type)'
