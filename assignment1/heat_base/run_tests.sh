flags=("-O2" "-O3" "-xhost")

for i in "${flags[@]}"; do
	make clean
	echo "$i"
	sed -i "s/^CFLAGS.*/CFLAGS = $i/" Makefile
	sed -i "s/^#SBATCH -o.*/#SBATCH -o job_${i// /_}.out/" job_flags.scp
	sed -i "s/^#SBATCH -e.*/#SBATCH -e job_${i// /_}.out/" job_flags.scp
	make
	sbatch job_flags.scp & PIDJOB=$!
	wait $PIDJOB
done
