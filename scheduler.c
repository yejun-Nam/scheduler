#include "scheduler.h"

# define MAXSIZE 1000
# define MINHRR -9999
# define MINSJ 9999

// 프로세스 정보 담은 구조체
typedef struct process {
	int number;      // 프로세스 #
	int arrive_t;    // 도착시간
	int burst_t;     // 실행시간
	int wait_t;      // 대기시간
	int ta_t;        // 반환시간 = 실행시간 + 대기시
	int complete;
} process;


//도착 시간 순서대로 프로세스 정렬 -> 알고리즘 구현 때 편하게 하기 위해
void arr_sort(process *pro, int n) {
	process temp;
	int i, j;
	for(i = n - 1; i > 0; i--) {
		for(j = 0; j < i; j++){
			if(pro[j].arrive_t > pro[j+1].arrive_t) {  // 도착 시간이 큰 것을 뒤로
				temp = pro[j + 1];
				pro[j + 1] = pro[j];
				pro[j] = temp;
			}
			else if(pro[j].arrive_t == pro[j + 1].arrive_t && pro[j].number > pro[j + 1].number) {  // 도착 시간이 같을 경우 큰 번호를 뒤로
				temp = pro[j + 1];
				pro[j + 1] = pro[j];
				pro[j] = temp;
			}
		}
	}
}


// 번호 순서대로 재정렬 -> 알고리즘 통과 후 재정렬
void resort(process *pro, int n) {
	process temp;
	int i, j;
	for(i = n - 1; i > 0; i--) {
		for(j = 0; j < i; j++) {
			if(pro[j].number > pro[j + 1].number) {
				temp = pro[j + 1];
				pro[j + 1] = pro[j];
				pro[j] = temp;
			}
		}
	}
}


// 랜덤 프로세스 생성
int process_generate(process *pro, int n) {
	FILE *fp2;
	int j, found;
	fp2 = fopen("proc.txt","a+");
	int i = n;

	int bt = (rand()%25) + 1;

	pro[i].number = i + 1;
	pro[i].burst_t = bt;


	while(1) {
		pro[i].arrive_t = rand()%50;

		found = 0;

		for(j = 0; j < i; ++j) {
			if(pro[j].arrive_t == pro[i].arrive_t) {
				found = 1;
				break;
			}
		}

		if(!found) break;
	}

	fprintf(fp2, "\r\n%d %d %d", pro[i].number, pro[i].arrive_t, pro[i].burst_t);

	fclose(fp2);
}


//sjf 알고리즘(비선점형)
int sjf_non(process *pro, int n) {
	int time, sum_bt = 0;
	int ta_avg, wait_avg = 0;
	int i, j;
	int sp;
	// 실행시간이 짧은 시간 순서대로 정렬 (처음으로 도착한 프로세스는 제외)
	// for(i = n - 1; i > 1; i--) {
	// 	for(j = 1; j < i; j++) {
	// 		if(pro[j].burst_t > pro[j + 1].burst_t) {
	// 			temp = pro[j + 1];
	// 			pro[j + 1] = pro[j];
	// 			pro[j] = temp;
	// 		}
	// 		else if(pro[j].burst_t == pro[j +1].burst_t && pro[j].number > pro[j + 1].number) { // 실행시간이 같을 경우 높은 번호를 뒤로
	// 			temp = pro[j + 1];
	// 			pro[j + 1] = pro[j];
	// 			pro[j] = temp;
	// 		}
	// 	}
	// }

	for (int k=0; k < n; k++){
		sum_bt += pro[k].burst_t; 
		pro[k].complete = 0;
	}

	for (time = pro[0].arrive_t; time < sum_bt; ){
		float sj = MINSJ;
		process temp;
		for (i = 0; i < n; i++){
			// 도착한 프로세스가 있고, 완료되지 않았는지 판단
			if(pro[i].arrive_t <= time && pro[i].complete != 1){
				if(sj >= pro[i].burst_t){
					sj = pro[i].burst_t;
					temp = pro[i];
				}
			}
		}
		// 방금 선택된 프로세스의 burst time 을 이용해 시간 업데이트
		time += temp.burst_t;

		// 대기 시간 계산
		temp.wait_t = time - temp.arrive_t - temp.burst_t;
		// 반환 시간 계산
		temp.ta_t = time - temp.arrive_t;

		// 프로세스 완료 선언
		temp.complete = 1;
	}
	
}

// hrn 알고리즘 그때 
int hrn(process *pro,int n,int *solution){
	int time;
	int i, sum_bt = 0;
	int loc;
	int ac = 0; // solution counter
	process temp;


	// 초기화
	for (int k=0; k < n; k++){
		sum_bt += pro[k].burst_t; 
		pro[k].complete = 0;
	}

	for (time = pro[0].arrive_t; time < sum_bt; ){
		float hrr = MINHRR;

		// Ratio Variable 을 임시적으로 저장
		float temp;

		for (i = 0; i < n; i++){

			// 도착한 프로세스가 있고, 완료되지 않았는지 판단
			if(pro[i].arrive_t <= time && pro[i].complete != 1){

				//  Response ratio 계산
				temp = (double)(pro[i].burst_t + (time - pro[i].arrive_t)) / pro[i].burst_t;
				
				// 방금 계산한 새로운 프로세스가 이전에 가장 컸던 hrr 보다 크다면 업데이트 해준다.
				if(hrr < temp){
					hrr =temp;
					loc = i;
				}
			}
		}

		// 방금 선택된 프로세스의 burst time 을 이용해 시간 업데이트
		time += pro[loc].burst_t;

		// 대기 시간 계산
		pro[loc].wait_t = time - pro[loc].arrive_t - pro[loc].burst_t;
		// 반환 시간 계산
		pro[loc].ta_t = time - pro[loc].arrive_t;

		// 프로세스 완료 선언
		pro[loc].complete = 1;

		// 실행순서 출력을 위한 배열에 추가
		solution[ac] = loc + 1;
		ac += 1; 

	}

	// write gantt
	

	char *path = malloc(sizeof(char) * (strlen(OUTPUT_DATA_DIR) + strlen(OUTPUT_HRN_DATA_NAME)) + 2);
	strcpy(path, OUTPUT_DATA_DIR);
	strcat(path, "/");
	strcat(path, OUTPUT_HRN_DATA_NAME);

	if (mkdir(OUTPUT_DATA_DIR, 0755) == -1 && errno != EEXIST) {
		printf("Can't create directory in: \"%s\"\nError detected: %s\n", OUTPUT_DATA_DIR, strerror(errno));
	}
	FILE *fp = fopen(path, "w+");
	
	if (fp == NULL) {
		printf("Can't write gantt data in directory: \"%s\"\n", path);
	}

	// 적는 부분
	// start_time = arrive_time + waiting_time
	// state_time 부터 burst time 만큼 프린트해줌 실행순서 순서로
	for (int j = 0 ; j < pro[solution[0] - 1].arrive_t; j++){
		fputs("0",fp);
	}

	
	for(int j =0 ; j < n ; j++){
		for(int m = 0 ; m < pro[solution[j] - 1].burst_t ; m++){
			fprintf(fp, "%d", solution[j]); 
		}
	}

}


int main() {
	int i = 0;
	int n = 0;
	int Q = 0;
	int index = 0;
	float tat;
	float waitt;	
	FILE *fp;
	fp = fopen("sample_process.txt","r");
	process ready_queue[MAXSIZE];
	int solution[MAXSIZE];


	printf("파일 읽기 시작\n");

	while(!feof(fp)) {
		fscanf(fp, "%d", &ready_queue[i].number);
		fscanf(fp, "%d", &ready_queue[i].arrive_t);
		fscanf(fp, "%d", &ready_queue[i].burst_t);
		index = index + 1;
		printf("%d\n", index);
		i++;
	}

	fclose(fp);

	printf("파일 읽기 종료\n");

	n = index - 1;

	printf("프로세스 개수 : %d",n);

	printf("\n==================== Main Menu ====================\n\n1. Read processes from sample_process.txt\n2. Generate random processes\n3. Shortest Job First(SJF) : Non-preemption\n4. Highest Response Ratio Next(HRN) : Non-preemption\n5. Exit\n===================================================\n");

	while(1){
		int ch = 0;

		printf("메뉴를 선택해주세요");

		scanf("%d", &ch);

		float tat = 0.0;
		float waitt = 0.0;

		switch(ch) {
			case 1:
				printf("\n1. Read processes from proc.txt\n==================== PROC ====================\n");

				printf("P#     AT     BT     Pri\n");

				for(i = 0; i < n; i++) {
					printf("%d     %d    %d\n", ready_queue[i].number, ready_queue[i].arrive_t, ready_queue[i].burst_t);
				}

				printf("==============================================\n\n\n");

				continue;

				break;

			case 2:
				printf("\n2. Generate random processes\n");

				process_generate(ready_queue, n);

				printf("\n\n생성 완료\n\n");

				n++;

				continue;

				break;

			case 3:
				arr_sort(ready_queue, n);

				sjf_non(ready_queue, n);

				printf("실행 순서 :");
				for(i = 0; i < n; i++) {
					printf("  %d  ", ready_queue[i].number);
				}
				printf("\n");

				resort(ready_queue, n);

				printf("\n3. Shortest Job First(SJF) : Non-preemption\n==================== SJF_non ====================\n");

				printf("P#     AT     BT     Pri     WT     TAT\n");

				for(i = 0; i < n; i++) {

					tat = tat + ready_queue[i].ta_t;
					waitt = waitt + ready_queue[i].wait_t;

					printf("%d     %d     %d      %d     %d\n", ready_queue[i].number, ready_queue[i].arrive_t, ready_queue[i].burst_t, ready_queue[i].wait_t, ready_queue[i].ta_t);
				}

				printf("반환시간 평균 : %.2f\n",tat/n);
				printf("대기시간 평균 : %.2f\n\n",waitt/n);

				printf("=================================================\n\n");

				continue;

				break;
			case 4:

				hrn(ready_queue, n,solution);

				printf("\n실행 순서 :");
				for(i = 0; i < n; i++) {
					printf("  %d  ", solution[i]);
				}
				printf("\n");

				resort(ready_queue, n);

				printf("\n4. Highest Response Ratio Next(HRN) :\n==================== hrn ====================\n");

				printf("P#\tAT\tBT\tWT\tTAT\tNTT\n");

				for(i = 0; i < n; i++) {
					tat = tat + ready_queue[i].ta_t;
					waitt = waitt + ready_queue[i].wait_t;
					printf("%d\t%d\t%d\t%d\t%d\t%f\n", ready_queue[i].number, ready_queue[i].arrive_t, ready_queue[i].burst_t, ready_queue[i].wait_t, ready_queue[i].ta_t ,(double)ready_queue[i].ta_t / ready_queue[i].burst_t);
				}
				printf("반환시간 평균 : %.2f\n",tat/n);
				printf("대기시간 평균 : %.2f\n\n",waitt/n);

				continue;
				break;

			case 5:
				printf("\n==================== EXIT ===================\n");

				exit(0);

				break;

		}
	}
}
