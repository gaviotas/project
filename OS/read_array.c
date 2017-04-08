#include <stdio.h>

#define MAX_ARG 1000000

int main() {

	int n;// 읽은 데이터 수
	
	double sum_x, sum_y;
	double avg_x, avg_y;
	double beta_0, beta_1;
	sum_x = 0;
	sum_y = 0;
	struct data {

		double x[MAX_ARG];
		double y[MAX_ARG];

	};

	struct data data;

	for (int i = 0; i < MAX_ARG; i++) {
		data.x[i] = 0.0;
		data.y[i] = 0.0;
	}

	FILE * fp;

	fopen_s(&fp, "input.txt", "r");
	if (fp == NULL)
	{
		printf("파일 열기 실패\n");
		return -1;
	}
	int r;
	int num = 0;
	for (int j = 0; j < MAX_ARG; j++) {
		r = fscanf_s(fp, "%lf %lf", &data.x[j], &data.y[j]);
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
	printf("%lf %lf\n", avg_x, avg_y);

	double temp_1, temp_2;

	for (int i = 0; i < num; i++) {
		temp_1 = (data.x[i] - avg_x) * (data.y[i] - avg_y);
		temp_2 = (data.x[i] - avg_x) * (data.x[i] - avg_x);
	}

	beta_1 = temp_1 / temp_2;
	beta_0 = avg_y - beta_1 * avg_x;

	printf("L.S : y = %lf + %lf x", beta_0, beta_1);

}