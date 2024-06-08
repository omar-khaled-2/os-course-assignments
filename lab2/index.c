#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include <sys/stat.h>



struct RowMultiplyParams {
    int a_row;
    int a_rows;
    int a_cols;
    int b_rows;
    int b_cols;
    int (*a_matrix)[];
    int (*b_matrix)[];
    int (*result_matrix)[];
};

void *multiply_row(void *params) {
    struct RowMultiplyParams *row_params = (struct RowMultiplyParams *)params;
    int a_row = row_params->a_row;
    int a_cols = row_params->a_cols;
    int b_cols = row_params->b_cols;
    int (*a_matrix)[a_cols] = row_params->a_matrix;
    int (*b_matrix)[b_cols] = row_params->b_matrix;
    int (*result_matrix)[b_cols] = row_params->result_matrix;

    for (int j = 0; j < b_cols; j++) {
        for (int k = 0; k < a_cols; k++) {
            result_matrix[a_row][j] += a_matrix[a_row][k] * b_matrix[k][j];
        }
    }
    pthread_exit(NULL);
}

void multiply_with_threads_each_row(int a_rows, int a_cols, int b_rows, int b_cols, int a_matrix[a_rows][a_cols], int b_matrix[b_rows][b_cols], int result_matrix[a_rows][b_cols]) {
    pthread_t threads[a_rows];

    for (int i = 0; i < a_rows; i++) {
        struct RowMultiplyParams* row_params = (struct RowMultiplyParams*)malloc(sizeof(struct RowMultiplyParams));
        row_params->a_row = i;
        row_params->a_rows = a_rows;
        row_params->a_cols = a_cols;
        row_params->b_rows = b_rows;
        row_params->b_cols = b_cols;
        row_params->a_matrix = a_matrix;
        row_params->b_matrix = b_matrix;
        row_params->result_matrix = result_matrix;
        pthread_create(&threads[i], NULL, multiply_row, (void*)row_params);
    }

    for (int i = 0; i < a_rows; i++) {
        pthread_join(threads[i], NULL);
    }
}



struct CellMultiplyParams {
    int a_row;
    int a_col;
    int a_rows;
    int a_cols;
    int b_rows;
    int b_cols;
    int (*a_matrix)[];
    int (*b_matrix)[];
    int (*result_matrix)[];
};

void *cell_multiply(void* params) {
    struct CellMultiplyParams* row_params = (struct CellMultiplyParams*)params;
    int a_row = row_params->a_row;
    int a_col = row_params->a_col;
    int a_rows = row_params->a_rows;
    int a_cols = row_params->a_cols;
    int b_rows = row_params->b_rows;
    int b_cols = row_params->b_cols;
    int (*a_matrix)[a_cols] = row_params->a_matrix;
    int (*b_matrix)[b_cols] = row_params->b_matrix;
    int (*result_matrix)[b_cols] = row_params->result_matrix;

    for (int k = 0; k < b_cols; k++) {
        result_matrix[a_row][k] += a_matrix[a_row][a_col] * b_matrix[a_col][k];
    }

    pthread_exit(NULL);
}

void muliply_with_threads_each_cell(int a_rows, int a_cols, int b_rows, int b_cols, int a_matrix[a_rows][a_cols], int b_matrix[b_rows][b_cols], int result_matrix[a_rows][b_cols]) {
    pthread_t threads[a_rows * a_cols];
    
    for (int i = 0; i < a_rows; i++) {
        for (int j = 0; j < a_cols; j++) {
            struct CellMultiplyParams* cell_params = (struct CellMultiplyParams*)malloc(sizeof(struct CellMultiplyParams));
            cell_params->a_row = i;
            cell_params->a_col = j;
            cell_params->a_rows = a_rows;
            cell_params->a_cols = a_cols;
            cell_params->b_rows = b_rows;
            cell_params->b_cols = b_cols;
            cell_params->a_matrix = a_matrix;
            cell_params->b_matrix = b_matrix;
            cell_params->result_matrix = result_matrix;
            pthread_create(&threads[i * a_cols + j], NULL, cell_multiply, (void*)cell_params);
        }
    }
}

void rows_cols(FILE *file, int* rows, int* cols) {
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

void fill_matrix(FILE *file, int rows, int cols, int matrix[rows][cols]) {

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fscanf(file, "%d", &matrix[i][j]);
        }
    }

}



void muliply_without_threads(int a_rows, int a_cols, int b_rows, int b_cols, int a_matrix[a_rows][a_cols], int b_matrix[b_rows][b_cols], int result_matrix[a_rows][b_cols]) {

    for (int i = 0; i < a_rows; i++) {
        for (int j = 0; j < b_cols; j++) {
            result_matrix[i][j] = 0;
            for (int k = 0; k < a_cols; k++) {
                result_matrix[i][j] += a_matrix[i][k] * b_matrix[k][j];
            }
        }
    }

}





void reset_matrix(int rows, int cols, int matrix[rows][cols]) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            matrix[i][j] = 0;
        }
    }
}

void write_matrix_to_file(FILE *file,int rows, int cols, int matrix[rows][cols]) {

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {

            fprintf(file, "%d ", matrix[i][j]);
        }
        fprintf(file, "\n");
    }

}


char *string_concat(char *str1, char *str2) {
    char *result = (char *)malloc(strlen(str1) + strlen(str2) + 1);
    strcpy(result, str1);
    strcat(result, str2);

    return result;
}

int main(int argc, char *argv[]) {

    struct timeval stop, start;


    char* matrix_a_name;
    char* matrix_b_name;
    char* matrix_c_name;

    if(argc == 4) {
        matrix_a_name = argv[1];
        matrix_b_name = argv[2];
        matrix_c_name = argv[3];
    } else {
        matrix_a_name = "a";
        matrix_b_name = "b";
        matrix_c_name = "c";
    }










    FILE *file;
    


    file = fopen(string_concat(matrix_a_name, ".txt"), "r");
    int a_rows, a_cols, b_rows, b_cols;
    rows_cols(file, &a_rows, &a_cols);
    int a_matrix[a_rows][a_cols];
    fill_matrix(file, a_rows, a_cols, a_matrix);
    fclose(file);

    file = fopen(string_concat(matrix_b_name, ".txt"), "r");
    rows_cols(file, &b_rows, &b_cols);
    int b_matrix[b_rows][b_cols];
    fill_matrix(file, b_rows, b_cols, b_matrix);
    fclose(file);







    mkdir("output", 0777);

    
    int result_matrix[a_rows][b_cols];

    reset_matrix(a_rows, b_cols, result_matrix);


    gettimeofday(&start, NULL);
    muliply_without_threads(a_rows, a_cols, b_rows, b_cols, a_matrix, b_matrix, result_matrix);
    gettimeofday(&stop, NULL);



    file = fopen(string_concat(matrix_c_name, "_each_matrix.txt"), "w");
    write_matrix_to_file(file, a_rows, b_cols, result_matrix);
    fprintf(file,"\n\n");
    fprintf(file,"This approach without threads Microseconds taken: %d", stop.tv_usec - start.tv_usec);
    fclose(file);
    


    reset_matrix(a_rows, b_cols, result_matrix);

    gettimeofday(&start, NULL);
    multiply_with_threads_each_row(a_rows, a_cols, b_rows, b_cols, a_matrix, b_matrix, result_matrix);
    gettimeofday(&stop, NULL);



    
    file = fopen(string_concat(matrix_c_name, "_each_row.txt"), "w");
    write_matrix_to_file(file, a_rows, b_cols, result_matrix);
    fprintf(file,"\n\n");
    fprintf(file,"This approach with threads each row Microseconds taken: %d", stop.tv_usec - start.tv_usec);
    fclose(file);

    reset_matrix(a_rows, b_cols, result_matrix);

    gettimeofday(&start, NULL);
    muliply_with_threads_each_cell(a_rows, a_cols, b_rows, b_cols, a_matrix, b_matrix, result_matrix);
    gettimeofday(&stop, NULL);


  
    
    file = fopen(string_concat(matrix_c_name, "_each_cell.txt"), "w");
    write_matrix_to_file(file, a_rows, b_cols, result_matrix);
    fprintf(file,"\n\n");
    fprintf(file,"This approach with threads each cell Microseconds taken: %d \n", stop.tv_usec - start.tv_usec);
    fclose(file);



    return 0;
}