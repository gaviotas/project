#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
/* 최대쓰레드갯수*/
#define MAX_THREAD_NUM 5
#define MAX_ARG 50000000
/* thread main function 선언*/
void *thread_function(void *arg);

int num = 0;
double sum_x, sum_y;
double avg_x, avg_y;
double beta_0, beta_1;
double temp_1_t, temp_2_t;
double temp_1[MAX_THREAD_NUM];
double temp_2[MAX_THREAD_NUM];

struct data {
	double x[MAX_ARG];
	double y[MAX_ARG];
};

static struct data data;

void read_data() {
    for (int i = 0; i < MAX_ARG; i++) {
		data.x[i] = 0.0;
		data.y[i] = 0.0;
	}
	FILE * fp;

	fp = fopen("input.txt", "r");
	if (fp == NULL)
	{
		return -1;
	}
	int r;

	for (int j = 0; j < MAX_ARG; j++) {
		r = fscanf(fp, "%lf %lf", &data.x[j], &data.y[j]);
		if (r < 1) break;
		num++;
	}
//	for (int j = 0; j < num; j++) {
//		printf("%lf %lf\n", data.x[j], data.y[j]);
//	}
//	printf("%d", num);
    for (int j = 0; j < num; j++) {
		sum_x += data.x[j];
		sum_y += data.y[j];
	}
	avg_x = sum_x / num;
	avg_y = sum_y / num;
//	printf("%lf %lf\n", avg_x, avg_y);
}

int main()
{
    int res;
    pthread_t thread_handle[ MAX_THREAD_NUM ];
    void *thread_result;
    int i;
    read_data();
/* MAX_THREAD_NUM 만큼pthread 를생성한다. */
    for ( i = 0 ; i < MAX_THREAD_NUM ; i++ ) {
/* pthread 를생성하여생성된pthread 에대한handle 을
tread_handle 배열에저장한다.
pthread 의main function 은세번째인자인thread_function 이되며, 인자로몇번째로생성된pthread 인지를나타내는i 값을넘긴다.
*/
        res = pthread_create(&(thread_handle[i]),NULL,thread_function,(void *)i);
/* 결과값을확인하여Error 가발생하였는지확인한다. */
        if ( res != 0 ) {
            printf( "[Main] Thread Creation Fail.\n" );
            exit(1);
        }
    }
/* 메세지출력*/
    printf( "[Main] Waiting for threads to finish...\n" );
/* MAX_THREAD_NUM 만큼생성된모든pthread 가종료되기를기다린다*/
    for ( i = 0 ; i < MAX_THREAD_NUM ; i++ ) {
/* 생성된pthread 가종료되기를기다린다.
* pthread 내부에서pthread_exit 가호출되기전까지블럭된다.
* pthread_exit 의인자를thread_result 값으로전달받는다.
*/
        res = pthread_join( thread_handle[i], &thread_result );
/* 결과값을확인하여pthread 가올바로join 되었는지확인한다. */
    if ( res == 0 ) {
        printf( "[Main] join thread(%d)\n", i );
    }
    else {
        printf( "[Main] join thread(%d) failed\n", i );
    }
    }
/* 메세지출력*/
    printf( "[Main] All done\n" );
    for(int i=0; i<MAX_THREAD_NUM; i++) {
        temp_1_t += temp_1[i];
        temp_2_t += temp_2[i];
    }
    beta_1 = temp_1_t / temp_2_t;
	beta_0 = avg_y - beta_1 * avg_x;
    printf("%lf %lf %lf %lf\n",sum_x,sum_y,avg_x,avg_y);
	printf("L.S : y = %lf + %lf x\n", beta_0, beta_1);
    printf("%d", num);
    exit(1);
}

/* pthread 의 main function */
void *thread_function( void *arg )
{
    int my_number = (int)arg;
    int n = num / MAX_THREAD_NUM;
    printf("%d %d %d\n",my_number, my_number*n, (my_number+1)*n);
    for(int i = my_number*n; i < n*(my_number+1); i++) {
    	temp_1[my_number] += (data.x[i] - avg_x) * (data.y[i] - avg_y);
		temp_2[my_number] += (data.x[i] - avg_x) * (data.x[i] - avg_x);
    }
    pthread_exit( NULL );
}

