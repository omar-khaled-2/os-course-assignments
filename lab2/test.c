#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include <sys/time.h>
#include<pthread.h>

#define MaxRows 20
#define Maxcolumns 20 
#define FILE_NAME_MAX 1024

int ** A;
int ** B;
int ** C;
int Rows1_Num,Col1_Num,Rows2_Num,Col2_Num;

struct CellThreadParams {
    int i;
    int j; 
};

struct RowThreadParams {
    int row;
};





void get_RowColumn_Number(FILE *file, int* rows, int* cols) {
    char input[20];
    fscanf(file, "%s", input);
    strtok(input, "=");
    char* token = strtok(NULL, "=");
    *rows = strtol(token, &token, 10);
    fscanf(file, "%s", input);
    strtok(input, "=");
    token = strtok(NULL, "=");
    *cols = strtol(token, &token, 10);
}
int** Create2DArray(int rows, int cols){
    int** arr=(int **)malloc(rows *sizeof(int*));
    if (arr == NULL) {
        printf("Memory allocation failed.\n");
        //return 1;
    }
    for (int i = 0; i < rows; i++) {
        arr[i] = (int *)malloc(cols* sizeof(int));
        if (arr[i] == NULL) {
            printf("Memory allocation failed.\n");
            //return 1;
        }
    }
    return arr;
}
void fillArray(FILE* file,int** arr,int rows, int cols){

for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fscanf(file, "%d", &(arr[i][j]));
        }

    }
}
void* Cells_Multi(void *arg){
    struct CellThreadParams* pr =(struct CellThreadParams*)arg;
    int i=pr->i;
    int j=pr->j;
    
    for(int k=0;k<Col1_Num;k++){
        C[i][j] += ( A[i][k] * B[k][i] );
    }
    pthread_exit(0);
}

void* Rows_Multi(void *arg){
    struct RowThreadParams* pr =(struct RowThreadParams*)arg;
    int row=pr->row;
    for(int j=0;j<Col2_Num;j++){
        for(int k=0;k<Col1_Num;k++){
            C[row][j] += ( A[row][k] * B[k][j] );
        }
    }
    pthread_exit(0);
}





void print_matrix(int** arr,int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%d ", arr[i][j]);
        }
        printf("\n");
    }
}

void reset_matrix(int rows, int cols, int ** matrix) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            matrix[i][j] = 0;
        }
    }
}
int main(){
    char file1_Name[FILE_NAME_MAX];
   char file2_Name[FILE_NAME_MAX];
   //int threads_no=0;
   printf("Enter the first file: ");
   scanf("%s",file1_Name);
   printf("Enter the Second file: ");
   scanf("%s",file2_Name);
  
   FILE *file1;
   FILE *file2;

   file1=fopen(file1_Name,"r");
   file2=fopen(file2_Name,"r");

  /** if(Rows1_Num>MaxRows || Rows2_Num>MaxRows || Col1_Num>Maxcolumns ||Col2_Num>Maxcolumns){
    printf("This is a large array ");
    exit(0);
   }*/
   ///
   get_RowColumn_Number(file1,&Rows1_Num,&Col1_Num);
   printf(" rows = %d , cols = %d \n",Rows1_Num,Col1_Num);
   get_RowColumn_Number(file2,&Rows2_Num,&Col2_Num);
   printf(" rows = %d  , cols = %d \n",Rows2_Num,Col2_Num);
   ///// 
   A=Create2DArray(Rows1_Num,Col1_Num);
   B=Create2DArray(Rows2_Num,Col2_Num);
   C=Create2DArray(Rows1_Num,Col2_Num);
   ////
   
   fillArray(file1,A,Rows1_Num,Col1_Num);
   fillArray(file2,B,Rows2_Num,Col2_Num);
   ////
   printf("A ........\n");
   print_matrix(A,Rows1_Num,Col1_Num);
   printf("\n");
   printf("B ........\n");
   print_matrix(B,Rows2_Num,Col2_Num);
   
   printf("\n");


    for (int i = 0; i < Rows1_Num; i++) {
        for (int j = 0; j < Col2_Num; j++) {
        
            for (int k = 0; k < Col1_Num; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    print_matrix(C,Rows1_Num,Col2_Num);
    reset_matrix(Rows1_Num,Col2_Num,C);
   pthread_t th_row[Rows1_Num];
    for (int i = 0; i < Rows1_Num; i++) {
        struct RowThreadParams* row_thread_params = (struct RowThreadParams*)malloc(sizeof(struct RowThreadParams));
        row_thread_params->row = i;
        pthread_create(&th_row[i], NULL, Rows_Multi, (void*)row_thread_params);
    }

    for (int i = 0; i < Rows1_Num; i++) {
        pthread_join(th_row[i], NULL);
    }

    print_matrix(C,Rows1_Num,Col2_Num);
    reset_matrix(Rows1_Num,Col2_Num,C);
   ////
   pthread_t th[Rows1_Num][Col2_Num];
    for (int i = 0; i < Rows1_Num; i++) {
        for (int j = 0; j < Col2_Num; j++) {
            struct CellThreadParams* cell_thread_params = (struct CellThreadParams*)malloc(sizeof(struct CellThreadParams));
            cell_thread_params->i = i;
            cell_thread_params->j = j;
            pthread_create(&th[i][j], NULL, Cells_Multi, (void*)cell_thread_params);

        }
    }

    

   
    for (int i = 0; i < Rows1_Num; i++) {
        for (int j = 0; j < Col2_Num; j++) {
            pthread_join(th[i][j], NULL);
        }
    }



    print_matrix(C,Rows1_Num,Col2_Num);

    return 0;
}

   