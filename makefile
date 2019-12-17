COPT = -ggdb

test: test_solve test_mesh

test_solve: solver.o test_solver.o
	$(CC) $(COPT) -o $@  $^  -lm
	./$@

solver.o: solver.c solver.h
test_solver.o: test_solver.c solver.h

test_mesh: test_meshreader.o mesh.o okno.1.node okno.1.ele
	$(CC) $(COPT) -o $@  test_meshreader.o mesh.o  -lm
	./$@ okno.1

test_meshreader.o: test_meshreader.c mesh.h
mesh.o: mesh.c mesh.h

okno.1.node okno1.ele: okno.poly
	triangle -pqa0.01 okno.poly
