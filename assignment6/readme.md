## Example of Computation : 200x200 points over a 3x3 process topology

For a resolution of 200x200 points, the total number of computational points for the grid is 200 * 200 = 40000.

From the output files, it can be seen that for each process, the calculation is divided over 68 * 68 points = 4624 points. For 3 * 3 = 9 processes, this gives a total of 4624 * 9 = 41616 computational points. The remaining (41616 - 40000) = 1616 points are for the ghost cells that is to be used for exchanging values at the domain boundaries.