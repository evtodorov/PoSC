read -p "Enter Flags: " flag

make clean
echo "$flag"
sed -i "s/^CFLAGS.*/CFLAGS = $flag/" Makefile
sed -i "s/^#SBATCH -o.*/#SBATCH -o job_${flag// /_}.out/" job_flags.scp
sed -i "s/^#SBATCH -e.*/#SBATCH -e job_${flag// /_}.out/" job_flags.scp
make
sbatch job_flags.scp & PIDJOB=$!
wait $PIDJOB
