COPT = -ggdb

mes: mes.o mes_solver.o sm.o mesh.o triangle3n.o
	$(CC) $(COPT) -o $@  $^  -lm

test: test_solve test_sm test_mesh test_mes

test_solve: solver.o test_solver.o
	$(CC) $(COPT) -o $@  $^  -lm
	./$@

solver.o: solver.c solver.h
test_solver.o: test_solver.c solver.h

test_sm: sm.o test_sm.o
	$(CC) $(COPT) -o $@  $^  -lm
	./$@

sm.o: sm.c sm.h
test_sm.o: test_sm.c sm.h

test_mesh: test_meshreader.o mesh.o okno.1.node okno.1.ele
	$(CC) $(COPT) -o $@  test_meshreader.o mesh.o  -lm
	./$@ okno.1

test_mes: mes okno.1.node okno.1.ele okno.1x.node okno.1x.ele
	./mes -i okno.1 -b 1:0 -b 3:1 -o out -p mesz -g
	./mes -i okno.1x -b 1:0 -b 3:1 -m 1:1:-30 -o out -p mesz -g


mes.o: mes.c mes.h sm.h mesh.h
mesh.o: mesh.c mesh.h
mes_solver.o: mes_solver.c mes.h sm.h mesh.h
sm.o: sm.c sm.h
test1.o: test1.c mes.h sm.h mesh.h
triangle3n.o: triangle3n.c mes.h sm.h mesh.h

okno.1.node okno.1.ele: okno.poly
	triangle -pqa0.01 okno.poly

okno.1x.node: okno.1.node
	cp okno.1.node okno.1x.node

okno.1x.ele: okno.1.ele
	awk '{ if( NR == 1 ) print $$1, $$2,  1; else print $$0, 1 }' okno.1.ele > okno.1x.ele

clean:
	-rm *.o
	-rm test_solve test_sm test_mesh mes out mesz
	-rm *.ele *.node okno.1.poly

