flagarray=("-O0" "-O1" "-O2" "-O3" "-Ofast" "-xhost" "-ipo" "-fno-alias" "-xCORE-AVX512")

echo "Run on login node: "`date` >> full.out
echo "Run on login node: "`date` >> filtered.out


for i in "${flagarray[@]}"
do
    make clean
    sed "s/^CFLAGS.*/CFLAGS = $i/" -i Makefile
    make
    ./heat test.dat >> full.out
    echo "#CFLAGS = "$i >> filtered.out
    tail -6 full.out >> filtered.out
done
