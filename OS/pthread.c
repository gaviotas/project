#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
/* 최대쓰레드갯수*/
#define MAX_THREAD_NUM5
/* thread main function 선언*/
void *thread_function(void *arg);
int main()
{
int res;
pthread_t thread_handle[ MAX_THREAD_NUM ];
void *thread_result;
int i;
/* MAX_THREAD_NUM 만큼pthread 를생성한다. */
for ( i = 0 ; i < MAX_THREAD_NUM ; i++ ) {
/* pthread 를생성하여생성된pthread 에대한handle 을
tread_handle 배열에저장한다.
pthread 의main function 은세번째인자인thread_function 이되며, 인자로몇번째로생성된pthread 인지를나타내는i 값을넘긴다.
*/
res = pthread_create( &(thread_handle[i]),
NULL,
thread_function,
(void *)i );
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
exit(1);
}