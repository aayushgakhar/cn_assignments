set terminal png size 600,400
set output "q1/cwnd.png"
set title "cwndplot"
set xlabel "Time (in Seconds)"
set ylabel "cwnd (in Bytes)"
set key top right
plot "q1/tcp-example.cwnd" using 1:2 with lines lw 3

set terminal png size 600,400
set output "q2/cwnd.png"
set title "cwndplot"
set xlabel "Time (in Seconds)"
set ylabel "cwnd (in Bytes)"
set key top right
plot "q2/tcp-example.cwnd" using 1:2 with lines lw 3

set terminal png size 600,400
set output "q3/cwnd.png"
set title "cwndplot"
set xlabel "Time (in Seconds)"
set ylabel "cwnd (in Bytes)"
set key top right
plot "q3/tcp-example.cwnd" using 1:2 with lines lw 3