#ifndef MYFUNCTIONS_H_
#define MYFUNCTIONS_H_

long int getLong(char* display);
double getDouble(char* display);
void matrixPrint(double* matrix, int size_x, int size_y);
double* matrixAddSym(double* m1, double* m2, int size);
double* matrixMultSym(double* m1, double* m2, int size);
double* matrixAddAsym(double* m1, double* m2, int size_x, int size_y);
double* matrixMultAsym(double* m1, double* m2, int size_x, int size_y);
void sleep_s(double seconds);
void clearKBBuff();


#endif // !MYFUNCTIONS_H_


