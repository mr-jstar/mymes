test_solve: solver.o test_solver.o
	$(CC) $(COPT) -o $@  $^
	./$@

solver.o: solver.c solver.h
test_solver.o: test_solver.c solver.h
