flagarray=("-O0" "-O1" "-O2" "-O3" "-Ofast" "-xhost" "-ipo" "-fno-alias" "-xCORE-AVX512")



for i in "${flagarray[@]}"
do
    make clean
    sed "s/^CFLAGS.*/CFLAGS = $i/" -i Makefile
    sed "s/^#SBATCH -o job_.*/#SBATCH -o job_$i" -i job_flags.scp
    make
    sbatch job_flags.scp
done
