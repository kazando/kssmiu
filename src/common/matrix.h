#ifndef _MATRIX
#define _MATRIX

typedef struct {
	int n;				//点の数	
	double **cpn;		//行列成分
	char **labels;		// ラベル (文字列の配列)  <-- 追加
} Matrix;

typedef struct {
	int n;				//点の数	
	int **cpn;		//行列成分
} IntMatrix;

void read_matrix(const char *filename, Matrix *M);
void read_matrix_phylip(const char *filename, Matrix *M);
void write_matrix_phylip(const char *filename, Matrix *M);
void init_matrix(Matrix *M, int n);
void init_matrix2(IntMatrix *M, int n);
void show_matrix(Matrix M);
void show_matrix2(IntMatrix M);
void del_matrix(Matrix *M);
void del_matrix2(IntMatrix *M);

#endif
