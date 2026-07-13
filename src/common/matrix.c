#include <stdio.h>
#include <stdlib.h>
#include <string.h> // strdup のために必要
#include "matrix.h"

static char *xstrdup(const char *s) {
    if (s == NULL) return NULL;
    size_t len = strlen(s) + 1;
    char *copy = malloc(len);
    if (copy != NULL) {
        memcpy(copy, s, len);
    }
    return copy;
}

/*行列を初期化する関数*/
void init_matrix(Matrix *M, int n){
	int i, j;
	M->n = n;
	
	// M->cpn と M->labels を NULL で初期化 (安全な解放のため)
	M->cpn = NULL;
	M->labels = NULL;

	M->cpn = (double **)malloc((n + 1) * sizeof(double *));
	if(M->cpn == NULL){
		fprintf(stderr, "init_matrix(): Error! malloc failed [M->cpn]\n");
		exit(EXIT_FAILURE);
	}
	// 0番目もNULLで初期化
	M->cpn[0] = NULL; 
	
	for(i = 1; i <= n; i++){
		M->cpn[i] = (double *)malloc((n + 1) * sizeof(double));
		if (M->cpn[i] == NULL){
			fprintf(stderr, "init_matrix(): Error! malloc failed [ M->cpn[i]= malloc()]\n");
			del_matrix(M); // 失敗した場合、確保済みのメモリを解放
			exit(EXIT_FAILURE);
		}

		for(j = 1; j <= n; j++){
			if (i != j)	M->cpn[i][j] = -1.0;
			else M->cpn[i][j] = 0;
		}
	}

	// --- ラベル用メモリの確保 (追加) ---
	M->labels = (char **)malloc((n + 1) * sizeof(char *));
	if(M->labels == NULL){
		fprintf(stderr, "init_matrix(): Error! malloc failed [M->labels]\n");
		del_matrix(M); // 失敗した場合、確保済みのメモリを解放
		exit(EXIT_FAILURE);
	}
	// すべてのポインタを NULL で初期化
	for(i = 0; i <= n; i++){
		M->labels[i] = NULL;
	}
	// --- 追加ここまで ---
}

void init_matrix2(IntMatrix *M, int n){
	int i;
	M->n = n;
	
	M->cpn = (int **)malloc((n + 1) * sizeof(int *));
	if(M->cpn == NULL){
		fprintf(stderr, "init_matrix(): Error! malloc failed [M->cpn]\n");
		exit(EXIT_FAILURE);
	}
	
	for(i = 1; i <= n; i++){
		M->cpn[i] = (int *)malloc((n + 1) * sizeof(int));
		if (M->cpn[i] == NULL){
			fprintf(stderr, "init_matrix(): Error! malloc failed [ M->cpn[i]= malloc()]\n");
			exit(EXIT_FAILURE);
		}
	}
}


/*行列を読み込む関数*/
void read_matrix(const char *filename, Matrix *M) {
	FILE *fp;
	char buff[100];
	int i, j;
	double num;
	int n, m;

	if((fp=fopen(filename,"rt")) == NULL){
		fprintf(stderr,"Error: %s File open failed.\n", filename);
		exit(EXIT_FAILURE);
	}
  
	while(fgets(buff,sizeof(buff), fp) != NULL){
		switch(buff[0])	{
			case 'p':
				sscanf(buff,"p max %d %d\n", &n, &m);
				init_matrix(M, n);
				break;
			case 'a':
				sscanf(buff,"a %d %d %lf\n", &i, &j, &num);
				M->cpn[i][j] = num;
				M->cpn[j][i] = num;
				break;
		}
	}
	
	for(i = 1; i <= n; i++){
		for(j = 1; j <= n; j++){
			if(i != j){
				if(M->cpn[i][j] < 0){
					printf("Matrix Error: M[%d][%d].\n", i, j);
				}
			}else{
				if(M->cpn[i][j] != 0){
					printf("Matrix Error: M[%d][%d].\n", i, j);
				}
			}
		}
	}
	fclose(fp);
}

/*PHYLIP形式の行列を読み込む関数*/
void read_matrix_phylip(const char *filename, Matrix *M) {
    FILE *fp;
    char label[1024]; // ラベル用の一時バッファ
    int n;
    int i, j;
    double num;

    if ((fp = fopen(filename, "rt")) == NULL) {
        fprintf(stderr, "Error: %s File open failed.\n", filename);
        exit(EXIT_FAILURE);
    }

    /* 最初のトークンからサイズを読む */
    if (fscanf(fp, "%d", &n) != 1) {
        fprintf(stderr, "Error: Cannot read matrix size.\n");
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    init_matrix(M, n);

    /* 各行: ラベルを読み込み、保存してから n 個の数を順に読む */
    for (i = 1; i <= n; i++) {
        // --- ラベルの読み込みと保存 (修正) ---
        if (fscanf(fp, "%1023s", label) != 1) {
            fprintf(stderr, "Error: Cannot read label at row %d.\n", i);
            fclose(fp);
            del_matrix(M);
            exit(EXIT_FAILURE);
        }
        
        /* 読み込んだラベルを複製して保存 */
        M->labels[i] = xstrdup(label);
        if (M->labels[i] == NULL) {
            fprintf(stderr, "Error: strdup failed for label at row %d.\n", i);
            fclose(fp);
            del_matrix(M);
            exit(EXIT_FAILURE);
        }
		// --- 修正ここまで ---

        for (j = 1; j <= n; j++) {
            if (fscanf(fp, "%lf", &num) != 1) {
                fprintf(stderr, "Error: Invalid data format at line %d j=%d.\n", i, j);
                fclose(fp);
				del_matrix(M);
                exit(EXIT_FAILURE);
            }
            M->cpn[i][j] = num;
        }
    }

    fclose(fp);

    /* 対称性と対角要素のチェック */
    for (i = 1; i <= n; i++) {
        for (j = 1; j <= n; j++) {
            if (i == j && M->cpn[i][j] != 0) {
                printf("Matrix Error: M[%d][%d] should be 0.\n", i, j);
            }
            if (M->cpn[i][j] != M->cpn[j][i]) {
                // 対称でない場合は警告を出すか、強制的に対称にする
                // printf("Warning: Matrix not symmetric at M[%d][%d]. Forcing symmetry.\n", i, j);
                M->cpn[j][i] = M->cpn[i][j];
            }
        }
    }
}

void write_matrix_phylip(const char filename[], Matrix *M) {
    FILE *fp;
    int i, j;

    if ((fp = fopen(filename, "wt")) == NULL) {
        fprintf(stderr, "Error: Cannot open file %s for writing.\n", filename);
        return;
    }

    fprintf(fp, "%d\n", M->n);
    for (i = 1; i <= M->n; i++) {
        if (M->labels != NULL && M->labels[i] != NULL) {
            fprintf(fp, "%-10s", M->labels[i]);
        } else {
            fprintf(fp, "%-10s", "leaf");
        }
        for (j = 1; j <= M->n; j++) {
            fprintf(fp, " %10.6f", M->cpn[i][j]);
        }
        fprintf(fp, "\n");
    }

    fclose(fp);
}

/*行列を表示する関数*/
void show_matrix(Matrix M){
	int i, j;
	printf("    ");
	for(i = 1; i <= M.n; i++)printf("%3d   ", i);
	putchar('\n');

	for(i = 1; i <= M.n; i++){
		printf("%3d ", i);
		for(j = 1; j <= M.n; j++) printf("%5.2f ", M.cpn[i][j]);
		putchar('\n');
	}
}

void show_matrix2(IntMatrix M){
	int i, j;
	for(i = 1; i <= M.n; i++){
		for(j = 1; j <= M.n; j++) printf("%3d ", M.cpn[i][j]);
		putchar('\n');
	}
}

/*行列を削除する関数*/
/* 共通ヘルパ：cpn 配列を解放する */
static void free_cpn(void **cpn, int n){
    int i;
    if (cpn != NULL){
        for (i = 1; i <= n; i++){ // 1-indexed
            if (cpn[i] != NULL){
                free(cpn[i]);
            }
        }
        free(cpn);
    }
}

void del_matrix(Matrix *M) {
    // 1. cpn (行列成分) の解放
	if (M->cpn != NULL){
        free_cpn((void **)M->cpn, M->n);
        M->cpn = NULL;
    }

	// 2. labels (ラベル) の解放 (追加)
	if (M->labels != NULL) {
		int i;
		for (i = 1; i <= M->n; i++) { // 1-indexed
			if (M->labels[i] != NULL) {
				free(M->labels[i]); // 各文字列を解放
			}
		}
		free(M->labels); // ポインタ配列を解放
		M->labels = NULL;
	}
	// --- 追加ここまで ---

    M->n = 0;
}

void del_matrix2(IntMatrix *M) {
    if (M->cpn != NULL){
        free_cpn((void **)M->cpn, M->n);
        M->cpn = NULL;
        M->n = 0;
    }
}
