#!/bin/sh

$DOOLEY = "dooley.ppm"
$CLOCK = "clock.ppm"

make

echo "Starting Test Cases"

echo "\n./ppmcvt -o out.pbm dooley.ppm"
./ppmcvt -o out.pbm dooley.ppm
.././ppmcvt -o prof-out.pbm dooley.ppm
diff -q out.pbm prof-out.pbm
rm out.pbm prof-out.pbm
echo "./ppmcvt -o out.pbm clock.ppm"
./ppmcvt -o out.pbm clock.ppm
.././ppmcvt -o prof-out.pbm clock.ppm
diff -q out.pbm prof-out.pbm
rm out.pbm prof-out.pbm
echo
echo "./ppmcvt dooley.ppm -o out.pbm"
./ppmcvt dooley.ppm -o out.pbm
.././ppmcvt dooley.ppm -o prof-out.pbm
diff -q out.pbm prof-out.pbm
rm out.pbm prof-out.pbm
echo
echo "./ppmcvt clock.ppm -o out.pbm"
./ppmcvt clock.ppm -o out.pbm
.././ppmcvt clock.ppm -o prof-out.pbm
diff -q out.pbm prof-out.pbm
rm out.pbm prof-out.pbm
echo
echo "./ppmcvt -o out.pbm -b dooley.ppm"
./ppmcvt -o out.pbm -b dooley.ppm
.././ppmcvt -o prof-out.pbm -b dooley.ppm
diff -q out.pbm prof-out.pbm
rm out.pbm prof-out.pbm
echo
echo "./ppmcvt -o out.pbm -b clock.ppm"
./ppmcvt -o out.pbm -b clock.ppm
.././ppmcvt -o prof-out.pbm -b clock.ppm
diff -q out.pbm prof-out.pbm
rm out.pbm prof-out.pbm
echo
echo "./ppmcvt clock.ppm -o out.pbm -b"
./ppmcvt clock.ppm -o out.pbm -b
.././ppmcvt clock.ppm -o prof-out.pbm -b
diff -q out.pbm prof-out.pbm
rm out.pbm prof-out.pbm
echo
echo "./ppmcvt dooley.ppm -o out.pbm -b"
./ppmcvt dooley.ppm -o out.pbm -b
.././ppmcvt dooley.ppm -o prof-out.pbm -b
diff -q out.pbm prof-out.pbm
rm out.pbm prof-out.pbm
echo
echo "./ppmcvt -o out.pgm -g 16 dooley.ppm"
./ppmcvt -o out.pgm -g 16 dooley.ppm
.././ppmcvt -o prof-out.pgm -g 16 dooley.ppm
diff -q out.pgm prof-out.pgm
rm out.pgm prof-out.pgm
echo
echo "./ppmcvt -o out.pgm -g 1 clock.ppm"
./ppmcvt -o out.pgm -g 1 clock.ppm
.././ppmcvt -o prof-out.pgm -g 1 clock.ppm
diff -q out.pgm prof-out.pgm
rm out.pgm prof-out.pgm
echo
echo "./ppmcvt clock.ppm -o out.pgm -g 65535"
./ppmcvt clock.ppm -o out.pgm -g 65535
.././ppmcvt clock.ppm -o prof-out.pgm -g 65535
diff -q out.pgm prof-out.pgm
rm out.pgm prof-out.pgm
echo
echo "./ppmcvt dooley.ppm -o out.pgm -g 9999"
./ppmcvt dooley.ppm -o out.pgm -g 9999
.././ppmcvt dooley.ppm -o prof-out.pgm -g 9999
diff -q out.pgm prof-out.pgm
rm out.pgm prof-out.pgm
echo
echo "./ppmcvt -o out.pgm -g clock.ppm"
./ppmcvt -o out.pgm -g clock.ppm
.././ppmcvt -o prof-out.pgm -g clock.ppm
echo "******* ERROR EXPECTED ***********"
echo
echo "./ppmcvt dooley.ppm -o out.pgm -g"
./ppmcvt dooley.ppm -g -o out.pgm 
.././ppmcvt dooley.ppm -g -o prof-out.pgm 
echo "******* ERROR EXPECTED ***********"
echo
echo "./ppmcvt -o out.ppm -i red clock.ppm"
./ppmcvt -o out.ppm -i red clock.ppm
.././ppmcvt -o prof-out.ppm -i red clock.ppm
diff -q out.ppm prof-out.ppm
rm out.ppm prof-out.ppm
echo
echo "./ppmcvt -o out.ppm dooley.ppm -i green "
./ppmcvt -o out.ppm dooley.ppm -i green 
.././ppmcvt -o prof-out.ppm dooley.ppm -i green 
diff -q out.ppm prof-out.ppm
rm out.ppm prof-out.ppm
echo
echo "./ppmcvt dooley.ppm -o out.ppm -i blue "
./ppmcvt dooley.ppm -o out.ppm -i blue
.././ppmcvt dooley.ppm -o prof-out.ppm -i blue 
diff -q out.ppm prof-out.ppm
rm out.ppm prof-out.ppm
echo
echo "./ppmcvt -o out.ppm -i greennn dooley.ppm"
./ppmcvt -o out.ppm -i greennn dooley.ppm
.././ppmcvt -o prof-out.ppm -i greennn dooley.ppm
echo "******* ERROR EXPECTED ***********"
echo
echo "./ppmcvt -o out.ppm -i yellow dooley.ppm"
./ppmcvt -o out.ppm -i yellow dooley.ppm
.././ppmcvt -o prof-out.ppm -i yellow dooley.ppm
echo "******* ERROR EXPECTED ***********"
echo
echo "./ppmcvt -o out.ppm -i 5 dooley.ppm"
./ppmcvt -o out.ppm -i 5 dooley.ppm
.././ppmcvt -o prof-out.ppm -i 5 dooley.ppm
echo "******* ERROR EXPECTED ***********"
echo
echo "./ppmcvt -o out.ppm dooley.ppm -i"
./ppmcvt -o out.ppm dooley.ppm -i
.././ppmcvt -o prof-out.ppm dooley.ppm -i
echo "******* ERROR EXPECTED ***********"
echo
echo "./ppmcvt -i -o out.ppm dooley.ppm"
./ppmcvt -i -o out.ppm dooley.ppm
.././ppmcvt -i -o prof-out.ppm dooley.ppm
echo "******* ERROR EXPECTED ***********"
echo
echo "./ppmcvt -o out.ppm -r red clock.ppm"
./ppmcvt -o out.ppm -r red clock.ppm
.././ppmcvt -o prof-out.ppm -r red clock.ppm
diff -q out.ppm prof-out.ppm
rm out.ppm prof-out.ppm
echo
echo "./ppmcvt -o out.ppm dooley.ppm -r green "
./ppmcvt -o out.ppm dooley.ppm -r green 
.././ppmcvt -o prof-out.ppm dooley.ppm -r green 
diff -q out.ppm prof-out.ppm
rm out.ppm prof-out.ppm
echo
echo "./ppmcvt dooley.ppm -o out.ppm -r blue "
./ppmcvt dooley.ppm -o out.ppm -r blue
.././ppmcvt dooley.ppm -o prof-out.ppm -r blue 
diff -q out.ppm prof-out.ppm
rm out.ppm prof-out.ppm
echo
echo "./ppmcvt -o out.ppm -r greennn dooley.ppm"
./ppmcvt -o out.ppm -r greennn dooley.ppm
.././ppmcvt -o prof-out.ppm -r greennn dooley.ppm
echo "******* ERROR EXPECTED ***********"
echo
echo "./ppmcvt -o out.ppm -r yellow dooley.ppm"
./ppmcvt -o out.ppm -r yellow dooley.ppm
.././ppmcvt -o prof-out.ppm -r yellow dooley.ppm
echo "******* ERROR EXPECTED ***********"
echo
echo "./ppmcvt -o out.ppm -r 5 dooley.ppm"
./ppmcvt -o out.ppm -r 5 dooley.ppm
.././ppmcvt -o prof-out.ppm -r 5 dooley.ppm
echo "******* ERROR EXPECTED ***********"
echo
echo "./ppmcvt -o out.ppm dooley.ppm -r"
./ppmcvt -o out.ppm dooley.ppm -r
.././ppmcvt -o prof-out.ppm dooley.ppm -r
echo "******* ERROR EXPECTED ***********"
echo
echo "./ppmcvt -o out.ppm -r dooley.ppm"
./ppmcvt -o out.ppm -r dooley.ppm
.././ppmcvt -o prof-out.ppm -r dooley.ppm
echo "******* ERROR EXPECTED ***********"
echo
echo "./ppmcvt -r -o out.ppm dooley.ppm"
./ppmcvt -r -o out.ppm dooley.ppm
.././ppmcvt -r -o prof-out.ppm dooley.ppm
echo "******* ERROR EXPECTED ***********"
echo
echo "./ppmcvt dooley.ppm -s -o out.ppm"
./ppmcvt dooley.ppm -s -o out.ppm
.././ppmcvt dooley.ppm -s -o prof-out.ppm
diff -q out.ppm prof-out.ppm
rm out.ppm prof-out.ppm
echo
echo "./ppmcvt dooley.ppm -o out.ppm -s "
./ppmcvt dooley.ppm -o out.ppm -s
.././ppmcvt dooley.ppm -o prof-out.ppm -s
diff -q out.ppm prof-out.ppm
rm out.ppm prof-out.ppm
echo
echo "./ppmcvt -s dooley.ppm -o out.ppm"
./ppmcvt -s dooley.ppm -o out.ppm
.././ppmcvt -s dooley.ppm -o prof-out.ppm
diff -q out.ppm prof-out.ppm
rm out.ppm prof-out.ppm
echo
echo "./ppmcvt -s dooley.ppm out.ppm"
./ppmcvt -s dooley.ppm out.ppm
.././ppmcvt -s dooley.ppm prof-out.ppm
echo "******* ERROR EXPECTED ***********"
echo
echo "./ppmcvt dooley.ppm out.ppm -s"
./ppmcvt dooley.ppm out.ppm -s
.././ppmcvt dooley.ppm prof-out.ppm -s
echo "******* ERROR EXPECTED ***********"
echo
echo "./ppmcvt -r -o out.ppm clock.ppm"
./ppmcvt -r -o out.ppm clock.ppm
.././ppmcvt -r -o prof-out.ppm clock.ppm
echo "******* ERROR EXPECTED ***********"
echo
echo "./ppmcvt clock.ppm -s -o out.ppm"
./ppmcvt clock.ppm -s -o out.ppm
.././ppmcvt clock.ppm -s -o prof-out.ppm
diff -q out.ppm prof-out.ppm
rm out.ppm prof-out.ppm
echo
echo "./ppmcvt clock.ppm -o out.ppm -s "
./ppmcvt clock.ppm -o out.ppm -s
.././ppmcvt clock.ppm -o prof-out.ppm -s
diff -q out.ppm prof-out.ppm
rm out.ppm prof-out.ppm
echo
echo "./ppmcvt -s clock.ppm -o out.ppm"
./ppmcvt -s clock.ppm -o out.ppm
.././ppmcvt -s clock.ppm -o prof-out.ppm
diff -q out.ppm prof-out.ppm
rm out.ppm prof-out.ppm
echo
echo "./ppmcvt -s clock.ppm out.ppm"
./ppmcvt -s clock.ppm out.ppm
.././ppmcvt -s clock.ppm prof-out.ppm
echo "******* ERROR EXPECTED ***********"
echo
echo "./ppmcvt clock.ppm out.ppm -s"
./ppmcvt clock.ppm out.ppm -s
.././ppmcvt clock.ppm prof-out.ppm -s
echo "******* ERROR EXPECTED ***********"
echo

echo
echo "./ppmcvt dooley.ppm -m -o out.ppm"
./ppmcvt dooley.ppm -m -o out.ppm
.././ppmcvt dooley.ppm -m -o prof-out.ppm
diff -q out.ppm prof-out.ppm
rm out.ppm prof-out.ppm
echo
echo "./ppmcvt dooley.ppm -o out.ppm -m "
./ppmcvt dooley.ppm -o out.ppm -m
.././ppmcvt dooley.ppm -o prof-out.ppm -m
diff -q out.ppm prof-out.ppm
rm out.ppm prof-out.ppm
echo
echo "./ppmcvt -m dooley.ppm -o out.ppm"
./ppmcvt -m dooley.ppm -o out.ppm
.././ppmcvt -m dooley.ppm -o prof-out.ppm
diff -q out.ppm prof-out.ppm
rm out.ppm prof-out.ppm
echo
echo "./ppmcvt -m dooley.ppm out.ppm"
./ppmcvt -m dooley.ppm out.ppm
.././ppmcvt -m dooley.ppm prof-out.ppm
echo "******* ERROR EXPECTED ***********"
echo
echo "./ppmcvt dooley.ppm out.ppm -m"
./ppmcvt dooley.ppm out.ppm -m
.././ppmcvt dooley.ppm prof-out.ppm -m
echo "******* ERROR EXPECTED ***********"
echo
echo "./ppmcvt clock.ppm -m -o out.ppm"
./ppmcvt clock.ppm -m -o out.ppm
.././ppmcvt clock.ppm -m -o prof-out.ppm
diff -q out.ppm prof-out.ppm
rm out.ppm prof-out.ppm
echo
echo "./ppmcvt clock.ppm -o out.ppm -m "
./ppmcvt clock.ppm -o out.ppm -m
.././ppmcvt clock.ppm -o prof-out.ppm -m
diff -q out.ppm prof-out.ppm
rm out.ppm prof-out.ppm
echo
echo "./ppmcvt -m clock.ppm -o out.ppm"
./ppmcvt -m clock.ppm -o out.ppm
.././ppmcvt -m clock.ppm -o prof-out.ppm
diff -q out.ppm prof-out.ppm
rm out.ppm prof-out.ppm
echo
echo "./ppmcvt -m clock.ppm out.ppm"
./ppmcvt -m clock.ppm out.ppm
.././ppmcvt -m clock.ppm prof-out.ppm
echo "******* ERROR EXPECTED ***********"
echo
echo "./ppmcvt clock.ppm out.ppm -m"
./ppmcvt clock.ppm out.ppm -m
.././ppmcvt clock.ppm prof-out.ppm -m
echo "******* ERROR EXPECTED ***********"
echo
echo "./ppmcvt -t 1 clock.ppm -o out.ppm"
./ppmcvt -t 1 clock.ppm -o out.ppm
.././ppmcvt -t 1 clock.ppm -o prof-out.ppm
diff -q out.ppm prof-out.ppm
rm out.ppm prof-out.ppm
echo
echo "./ppmcvt -t 8 clock.ppm -o out.ppm"
./ppmcvt -t 8 clock.ppm -o out.ppm
.././ppmcvt -t 8 clock.ppm -o prof-out.ppm
diff -q out.ppm prof-out.ppm
rm out.ppm prof-out.ppm
echo
echo "./ppmcvt -t 7 clock.ppm -o out.ppm"
./ppmcvt -t 7 clock.ppm -o out.ppm
.././ppmcvt -t 7 clock.ppm -o prof-out.ppm
diff -q out.ppm prof-out.ppm
rm out.ppm prof-out.ppm
echo
echo
echo "./ppmcvt -t 4 clock.ppm -o out.ppm"
./ppmcvt -t 7 clock.ppm -o out.ppm
.././ppmcvt -t 7 clock.ppm -o prof-out.ppm
diff -q out.ppm prof-out.ppm
rm out.ppm prof-out.ppm
echo
echo
echo "./ppmcvt clock.ppm -t 4 -o out.ppm"
./ppmcvt clock.ppm -t 4 -o out.ppm
.././ppmcvt clock.ppm -t 4 -o prof-out.ppm
diff -q out.ppm prof-out.ppm
rm out.ppm prof-out.ppm
echo
echo
echo "./ppmcvt clock.ppm -o out.ppm -t 4"
./ppmcvt clock.ppm -o out.ppm -t 4
.././ppmcvt clock.ppm -o prof-out.ppm -t 4
diff -q out.ppm prof-out.ppm
rm out.ppm prof-out.ppm

echo
echo "./ppmcvt -t 1 dooley.ppm -o out.ppm"
./ppmcvt -t 1 dooley.ppm -o out.ppm
.././ppmcvt -t 1 dooley.ppm -o prof-out.ppm
diff -q out.ppm prof-out.ppm
rm out.ppm prof-out.ppm
echo
echo "./ppmcvt -t 8 dooley.ppm -o out.ppm"
./ppmcvt -t 8 dooley.ppm -o out.ppm
.././ppmcvt -t 8 dooley.ppm -o prof-out.ppm
diff -q out.ppm prof-out.ppm
rm out.ppm prof-out.ppm
echo
echo "./ppmcvt -t 7 dooley.ppm -o out.ppm"
./ppmcvt -t 7 dooley.ppm -o out.ppm
.././ppmcvt -t 7 dooley.ppm -o prof-out.ppm
diff -q out.ppm prof-out.ppm
rm out.ppm prof-out.ppm
echo
echo
echo "./ppmcvt -t 4 dooley.ppm -o out.ppm"
./ppmcvt -t 7 dooley.ppm -o out.ppm
.././ppmcvt -t 7 dooley.ppm -o prof-out.ppm
diff -q out.ppm prof-out.ppm
rm out.ppm prof-out.ppm
echo
echo
echo "./ppmcvt dooley.ppm -t 4 -o out.ppm"
./ppmcvt dooley.ppm -t 4 -o out.ppm
.././ppmcvt dooley.ppm -t 4 -o prof-out.ppm
diff -q out.ppm prof-out.ppm
rm out.ppm prof-out.ppm
echo
echo
echo "./ppmcvt dooley.ppm -o out.ppm -t 4"
./ppmcvt dooley.ppm -o out.ppm -t 4
.././ppmcvt dooley.ppm -o prof-out.ppm -t 4
diff -q out.ppm prof-out.ppm
rm out.ppm prof-out.ppm
echo
echo "./ppmcvt dooley.ppm -o out.ppm -t 4.3"
./ppmcvt dooley.ppm -o out.ppm -t 4.3
.././ppmcvt dooley.ppm -o prof-out.ppm -t 4.3
diff -q out.ppm prof-out.ppm
rm out.ppm prof-out.ppm
echo
echo "./ppmcvt dooley.ppm -o out.ppm -t"
./ppmcvt dooley.ppm -o out.ppm -t
.././ppmcvt dooley.ppm -o prof-out.ppm -t
echo "******* ERROR EXPECTED ***********"
echo
echo "./ppmcvt -t dooley.ppm -o out.ppm"
./ppmcvt -t dooley.ppm -o out.ppm
.././ppmcvt -t dooley.ppm -o prof-out.ppm
echo "******* ERROR EXPECTED ***********"
echo
echo "./ppmcvt -t 10 dooley.ppm -o out.ppm"
./ppmcvt -t 10 dooley.ppm -o out.ppm
.././ppmcvt -t 10 dooley.ppm -o prof-out.ppm
echo "******* ERROR EXPECTED ***********"
echo
echo "./ppmcvt -t -2 dooley.ppm -o out.ppm"
./ppmcvt -t -2 dooley.ppm -o out.ppm
.././ppmcvt -t -2 dooley.ppm -o prof-out.ppm
echo
echo "******* ERROR EXPECTED ***********"
echo "./ppmcvt -t -2 dooley.ppm out.ppm"
./ppmcvt -t -2 dooley.ppm out.ppm
.././ppmcvt -t -2 dooley.ppm prof-out.ppm
echo "******* ERROR EXPECTED ***********"
echo
echo "./ppmcvt -t -2 dooley.ppm out.ppm -s"
./ppmcvt -t -2 dooley.ppm out.ppm -s
.././ppmcvt -t -2 dooley.ppm prof-out.ppm -s
echo "******* ERROR EXPECTED ***********"
echo
echo "./ppmcvt -t 2 dooley.ppm out.ppm -s"
./ppmcvt -t 2 dooley.ppm out.ppm -s
.././ppmcvt -t 2 dooley.ppm prof-out.ppm -s
echo "******* ERROR EXPECTED ***********"

echo
echo "./ppmcvt -n 1 dooley.ppm -o out.ppm"
./ppmcvt -n 1 dooley.ppm -o out.ppm
.././ppmcvt -n 1 dooley.ppm -o prof-out.ppm
diff -q out.ppm prof-out.ppm
rm out.ppm prof-out.ppm
echo
echo "./ppmcvt -n 8 dooley.ppm -o out.ppm"
./ppmcvt -n 8 dooley.ppm -o out.ppm
.././ppmcvt -n 8 dooley.ppm -o prof-out.ppm
diff -q out.ppm prof-out.ppm
rm out.ppm prof-out.ppm
echo
echo "./ppmcvt -t n dooley.ppm -o out.ppm"
./ppmcvt -t n dooley.ppm -o out.ppm
.././ppmcvt -t n dooley.ppm -o prof-out.ppm
diff -q out.ppm prof-out.ppm
rm out.ppm prof-out.ppm
echo
echo "./ppmcvt -n 4 dooley.ppm -o out.ppm"
./ppmcvt -n 7 dooley.ppm -o out.ppm
.././ppmcvt -n 7 dooley.ppm -o prof-out.ppm
diff -q out.ppm prof-out.ppm
rm out.ppm prof-out.ppm
echo
echo "./ppmcvt dooley.ppm -n 4 -o out.ppm"
./ppmcvt dooley.ppm -n 4 -o out.ppm
.././ppmcvt dooley.ppm -n 4 -o prof-out.ppm
diff -q out.ppm prof-out.ppm
rm out.ppm prof-out.ppm
echo
echo "./ppmcvt dooley.ppm -o out.ppm -n 4"
./ppmcvt dooley.ppm -o out.ppm -n 4
.././ppmcvt dooley.ppm -o prof-out.ppm -n 4
diff -q out.ppm prof-out.ppm
rm out.ppm prof-out.ppm
echo
echo "./ppmcvt dooley.ppm -o out.ppm -n 4.4"
./ppmcvt dooley.ppm -o out.ppm -n 4.4
.././ppmcvt dooley.ppm -o prof-out.ppm -n 4.4
diff -q out.ppm prof-out.ppm
rm out.ppm prof-out.ppm
echo
echo "./ppmcvt dooley.ppm -o out.ppm -n"
./ppmcvt dooley.ppm -o out.ppm -n
.././ppmcvt dooley.ppm -o prof-out.ppm -n
echo "******* ERROR EXPECTED ***********"
echo
echo "./ppmcvt -n dooley.ppm -o out.ppm"
./ppmcvt -n dooley.ppm -o out.ppm
.././ppmcvt -n dooley.ppm -o prof-out.ppm
echo "******* ERROR EXPECTED ***********"
echo
echo "./ppmcvt -n 10 dooley.ppm -o out.ppm"
./ppmcvt -n 10 dooley.ppm -o out.ppm
.././ppmcvt -n 10 dooley.ppm -o prof-out.ppm
echo "******* ERROR EXPECTED ***********"
echo
echo "./ppmcvt -n -2 dooley.ppm -o out.ppm"
./ppmcvt -n -2 dooley.ppm -o out.ppm
.././ppmcvt -n -2 dooley.ppm -o prof-out.ppm
echo "******* ERROR EXPECTED ***********"
echo
echo "./ppmcvt -n -2 dooley.ppm out.ppm"
./ppmcvt -n -2 dooley.ppm out.ppm
.././ppmcvt -n -2 dooley.ppm prof-out.ppm
echo "******* ERROR EXPECTED ***********"
echo
echo "./ppmcvt -n -2 dooley.ppm out.ppm -s"
./ppmcvt -n -2 dooley.ppm out.ppm -s
.././ppmcvt -n -2 dooley.ppm prof-out.ppm -s
echo "******* ERROR EXPECTED ***********"
echo
echo "./ppmcvt -n 2 dooley.ppm out.ppm -s"
./ppmcvt -n 2 dooley.ppm out.ppm -s
.././ppmcvt -n 2 dooley.ppm prof-out.ppm -s
echo "******* ERROR EXPECTED ***********"

echo "./ppmcvt "
./ppmcvt 
.././ppmcvt 
echo "******* ERROR EXPECTED ***********"
echo
echo "./ppmcvt -s -b"
./ppmcvt -s -b
.././ppmcvt -s -b
echo "******* ERROR EXPECTED ***********"
echo
echo "./ppmcvt dooley.ppm"
./ppmcvt dooley.ppm
.././ppmcvt dooley.ppm
echo "******* ERROR EXPECTED ***********"
echo
echo "./ppmcvt -o dooley.ppm"
./ppmcvt -o dooley.ppm
.././ppmcvt -o dooley.ppm
echo "******* ERROR EXPECTED ***********"
echo
echo "./ppmcvt -o dooley.ppm"
./ppmcvt -o dooey.ppm
.././ppmcvt -o doley.ppm
echo "******* ERROR EXPECTED ***********"
echo
echo "./ppmcvt -o out.ppm dooley.ppm -t"
./ppmcvt -o out.ppm dooley.ppm -t
.././ppmcvt -o prof-out.ppm dooley.ppm -t
echo "******* ERROR EXPECTED ***********"
echo
echo "./ppmcvt -o out.ppm dooley.ppm"
./ppmcvt -o out.ppm doley.ppm
.././ppmcvt -o prof-out.ppm doley.ppm
echo "******* ERROR EXPECTED ***********"
echo
echo "./ppmcvt out.ppm dooley.ppm"
./ppmcvt out.ppm doley.ppm
.././ppmcvt prof-out.ppm doley.ppm
echo "******* ERROR EXPECTED ***********"
echo
echo "./ppmcvt out.ppm dooley.ppm -o"
./ppmcvt out.ppm doley.ppm -o
.././ppmcvt prof-out.ppm doley.ppm -o
echo "******* ERROR EXPECTED ***********"
echo
echo "./ppmcvt -o out.ppm"
./ppmcvt -o out.ppm
.././ppmcvt -o prof-out.ppm
echo "******* ERROR EXPECTED ***********"
echo
echo "./ppmcvt -o out.ppm dooley.ppm -t 5 -n 2"
./ppmcvt -o out.ppm dooley.ppm -t 5 -n 2
.././ppmcvt -o prof-out.ppm dooley.ppm -t 5 -n 2
echo "******* ERROR EXPECTED ***********"
echo
echo "./ppmcvt -o out.ppm dooley.ppm -c 5"
./ppmcvt -o out.ppm dooley.ppm -c 5
.././ppmcvt -o prof-out.ppm dooley.ppm -c 5
echo "******* ERROR EXPECTED ***********"
echo
echo "./ppmcvt -o out.ppm dooley.ppm -c"
./ppmcvt -o out.ppm dooley.ppm -c
.././ppmcvt -o prof-out.ppm dooley.ppm -c
echo "******* ERROR EXPECTED ***********"
echo
echo "./ppmcvt -c -o out.ppm dooley.ppm"
./ppmcvt -c -o out.ppm dooley.ppm
.././ppmcvt -c -o prof-out.ppm dooley.ppm
echo "******* ERROR EXPECTED ***********"
echo
echo "./ppmcvt dooley.ppm -c -o out.ppm"
./ppmcvt dooley.ppm -c -o out.ppm
.././ppmcvt dooley.ppm -c -o prof-out.ppm
echo "******* ERROR EXPECTED ***********"
echo
echo "./ppmcvt dooley.ppm -o out.ppm -c"
./ppmcvt dooley.ppm -o out.ppm -c
.././ppmcvt dooley.ppm -o prof-out.ppm  -c
echo "******* ERROR EXPECTED ***********"
echo
echo "./ppmcvt dooley.ppm -t 5 -n 2 -o out.ppm"
./ppmcvt dooley.ppm -t 5 -n 2 -o out.ppm
.././ppmcvt dooley.ppm -t 5 -n 2 -o prof-out.ppm 
echo "******* ERROR EXPECTED ***********"
echo
echo "./ppmcvt dooley.ppm -t 5 -n 2 -o out.ppm"
./ppmcvt dooley.ppm -t 5 -n 2 -o out.ppm
.././ppmcvt dooley.ppm -t 5 -n 2 -o prof-out.ppm 
echo "******* ERROR EXPECTED ***********"
echo
echo "./ppmcvt dooley.ppm -t5-n 2 -o out.ppm"
./ppmcvt dooley.ppm -t5-n 2 -o out.ppm
.././ppmcvt dooley.ppm -t5-n 2 -o prof-out.ppm 
echo "******* ERROR EXPECTED ***********"