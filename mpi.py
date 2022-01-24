from mpi4py import MPI
import numpy as np
import sys
import time

comm = MPI.COMM_WORLD
proc_num = comm.Get_rank()
procs_num = comm.Get_size()

if len(sys.argv) < 2:
	print("Program needs 1 argument: n")
	exit(1)

n = int(sys.argv[1])
rows_by_proc = n // procs_num;

B = np.zeros((n, n), dtype=np.int64)
C = np.zeros((n, n), dtype=np.int64)
buf = np.zeros((rows_by_proc, n), dtype=np.int64)
ans = np.zeros((rows_by_proc, n), dtype=np.int64)

if proc_num == 0:
	print(f"n = {n}")
	print(f"Processes number: {procs_num}")

	with open('./A.txt', 'r') as A_file:
		A = np.array([[np.int64(v) for v in line.split()] for line in A_file], dtype=np.int64)
	with open('./B.txt', 'r') as B_file:
		B = np.array([[np.int64(v) for v in line.split()] for line in B_file], dtype=np.int64)

	for i in range(0, n):
		for j in range(i + 1, n):
			temp = B[i, j]
			B[i, j] = B[j, i]
			B[j, i] = temp

	begin = time.time()

	for i in range(1, procs_num):
		comm.Send([B, MPI.LONG], dest=i, tag=0)
		proc_first_row = i * rows_by_proc
		proc_last_row = proc_first_row + rows_by_proc
		buf = A[proc_first_row:proc_last_row]
		comm.Send([buf, MPI.LONG], dest=i, tag=1)
	  
	for i in range(1, procs_num):
		comm.Recv([ans, MPI.LONG], source=i, tag=2)
		proc_first_row = i * rows_by_proc
		proc_last_row = proc_first_row + rows_by_proc
		C[proc_first_row:proc_last_row] = ans

	for i in range(rows_by_proc):
		for j in range(n):
			C[i, j] = sum(A[i, :] * B[j, :])

	print(f"Time of calculating  matix C = {time.time() - begin}")

	np.savetxt("./C_py_mpi.txt", C, "%.f")
else:
	comm.Recv([B, MPI.LONG], source=0, tag=0)
	comm.Recv([buf, MPI.LONG], source=0, tag=1)

	for i in range(rows_by_proc):
		for j in range(n):
			ans[i, j] = sum(buf[i] * B[j])
	  
	comm.Send([ans, MPI.LONG], dest=0, tag=2)
