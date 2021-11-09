array=("-O0" "-O1" "-O2" "-O3" "-Ofast" "-xhost" "-O0 -ipo" "-O1 -ipo" "-O2 -ipo" "-O3 -ipo" "-Ofast -ipo" "-xhost -ipo" "-O0 -fno-alias" "-O1 -fno-alias" "-O2 -fno-alias" "-O3 -fno-alias" "-Ofast -fno-alias" "-xhost -fno-alias" "-O0 -xCORE-AVX512" "-O1 -xCORE-AVX512" "-O2 -xCORE-AVX512" "-O3 -xCORE-AVX512" "-Ofast -xCORE-AVX512" "-xhost -xCORE-AVX512" "-O0 -xCORE-AVX512 -qopt-zmm-usage=high" "-O1 -xCORE-AVX512 -qopt-zmm-usage=high" "-O2 -xCORE-AVX512 -qopt-zmm-usage=high" "-O3 -xCORE-AVX512 -qopt-zmm-usage=high" "-Ofast -xCORE-AVX512 -qopt-zmm-usage=high" "-xhost -xCORE-AVX512 -qopt-zmm-usage=high" "-O0 -ipo -fno-alias" "-O1 -ipo -fno-alias" "-O2 -ipo -fno-alias" "-O3 -ipo -fno-alias" "-Ofast -ipo -fno-alias" "-xhost -ipo -fno-alias" "-O0 -ipo -xCORE-AVX512" "-O1 -ipo -xCORE-AVX512" "-O2 -ipo -xCORE-AVX512" "-O3 -ipo -xCORE-AVX512" "-Ofast -ipo -xCORE-AVX512" "-xhost -ipo -xCORE-AVX512" "-O0 -ipo -xCORE-AVX512 -qopt-zmm-usage=high" "-O1 -ipo -xCORE-AVX512 -qopt-zmm-usage=high" "-O2 -ipo -xCORE-AVX512 -qopt-zmm-usage=high" "-O3 -ipo -xCORE-AVX512 -qopt-zmm-usage=high" "-Ofast -ipo -xCORE-AVX512 -qopt-zmm-usage=high" "-xhost -ipo -xCORE-AVX512 -qopt-zmm-usage=high" "-O0 -fno-alias -xCORE-AVX512" "-O1 -fno-alias -xCORE-AVX512" "-O2 -fno-alias -xCORE-AVX512" "-O3 -fno-alias -xCORE-AVX512" "-Ofast -fno-alias -xCORE-AVX512" "-xhost -fno-alias -xCORE-AVX512" "-O0 -fno-alias -xCORE-AVX512 -qopt-zmm-usage=high" "-O1 -fno-alias -xCORE-AVX512 -qopt-zmm-usage=high" "-O2 -fno-alias -xCORE-AVX512 -qopt-zmm-usage=high" "-O3 -fno-alias -xCORE-AVX512 -qopt-zmm-usage=high" "-Ofast -fno-alias -xCORE-AVX512 -qopt-zmm-usage=high" "-xhost -fno-alias -xCORE-AVX512 -qopt-zmm-usage=high" "-O0 -ipo -fno-alias -xCORE-AVX512" "-O1 -ipo -fno-alias -xCORE-AVX512" "-O2 -ipo -fno-alias -xCORE-AVX512" "-O3 -ipo -fno-alias -xCORE-AVX512" "-Ofast -ipo -fno-alias -xCORE-AVX512" "-xhost -ipo -fno-alias -xCORE-AVX512" "-O0 -ipo -fno-alias -xCORE-AVX512 -qopt-zmm-usage=high" "-O1 -ipo -fno-alias -xCORE-AVX512 -qopt-zmm-usage=high" "-O2 -ipo -fno-alias -xCORE-AVX512 -qopt-zmm-usage=high" "-O3 -ipo -fno-alias -xCORE-AVX512 -qopt-zmm-usage=high" "-Ofast -ipo -fno-alias -xCORE-AVX512 -qopt-zmm-usage=high" "-xhost -ipo -fno-alias -xCORE-AVX512 -qopt-zmm-usage=high")

for i in "${array[@]}"; do
	make clean
	echo "$i"
	sed -i "s/^CFLAGS.*/CFLAGS = $i/" Makefile
	sed -i "s/^#SBATCH -o.*/#SBATCH -o job_${i// /_}.out/" job_flags.scp
	sed -i "s/^#SBATCH -e.*/#SBATCH -e job_${i// /_}.out/" job_flags.scp
	make
	sbatch job_flags.scp & PIDJOB=$!
	wait $PIDJOB
	sleep 30
done
