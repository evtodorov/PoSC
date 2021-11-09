1.1.1 Obtain the initial performance data.

1.1.2. Investigate how the FLOP/s metric is measured. Which floating point operations are taken into account?

    FLOP = total_iterations * 11.0 * x_resolution * y_resolution.
    runtime = wtime() - runtime(equals wall time at start of simulation).
    FLOPS/s = FLOP/runtime (scaled appropriately to mega-, giga-, etc).

    How FLOPs are calculated (In relax_jacobi.c)?
        1. There are 3 additions and 1 multiplication in the relax_jacobi() method, used for updating every cell, giving 4 FLOPs.
        2. In residual_jacobi(), there are also 3 additions and 1 multiplication while calculating the 'unew'. Then, there is 1 subtraction while calculating 'diff' and lastly, 1 multiplication and 1 addition while updating 'sum'. So, 7 FLOPs here altogether.
        3. In total, for each iteration, we use 7+4 = 11 FLOPs in total.