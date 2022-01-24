A = dlmread('A.txt');
B = dlmread('B.txt');
C = A * B;
dlmwrite('C_matlab.txt', C, 'delimiter', ' ', 'precision', 20);
