# include <stdio.h>
# include <stdlib.h>
# define MAXSIZE 1000

// 프로세스 정보 담은 구조체
typedef struct process {
	int number;      // 프로세스 #
	int arrive_t;    // 도착시간
	int burst_t;     // 실행시간
	int priority;    // 우선순위
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
		pro[i].priority = rand()%50;

		found = 0;

		for(j = 0; j < i; ++j) {
			if(pro[j].priority == pro[i].priority) {
				found = 1;
				break;
			}
		}

		if(!found) break;
	}

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

	fprintf(fp2, "\r\n%d %d %d %d", pro[i].number, pro[i].arrive_t, pro[i].burst_t, pro[i].priority);

	fclose(fp2);
}


//sjf 알고리즘(비선점형)
int sjf_non(process *pro, int n) {
	int time;
	int ta_avg, wait_avg = 0;
	int i, j;
	process temp;
	int sp;
	// 실행시간이 짧은 시간 순서대로 정렬 (처음으로 도착한 프로세스는 제외)
	for(i = n - 1; i > 1; i--) {
		for(j = 1; j < i; j++) {
			if(pro[j].burst_t > pro[j + 1].burst_t) {
				temp = pro[j + 1];
				pro[j + 1] = pro[j];
				pro[j] = temp;
			}
			else if(pro[j].burst_t == pro[j +1].burst_t && pro[j].number > pro[j + 1].number) { // 실행시간이 같을 경우 높은 번호를 뒤로
				temp = pro[j + 1];
				pro[j + 1] = pro[j];
				pro[j] = temp;
			}
		}
	}
	// 반환시간, 대기시간
	for(i = 0; i < n; i++) {
		sp += pro[i].burst_t;
		pro[i].ta_t = sp - (pro[i].arrive_t - pro[0].arrive_t);
		pro[i].wait_t = pro[i].ta_t - pro[i].burst_t;
	}
}

// hrn 알고리즘 그때 
int hrn(process *pro,int n){
	int time;
	int ta_avg, wait_avg = 0;
	int i, j  , sum_bt = 0;
	process temp;
	int sp;
	int loc;

	for (int k=0; k < n; k++){
		sum_bt += pro[k].wait_t; 
	}

	for (time = pro[0].arrive_t; time < sum_bt; ){
		// set the lower limit to response ratio
		float hrr = -9999;

		// The Response Ratio Variable
		float temp;

		// variable used to store the next process selected

		for (i = 0; i < n; i++){

			// check if the process has arrived and is Incomplete
			if(pro[i].arrive_t <= time && pro[i].complete != 1){

				// calculate the response ratio
				temp = (pro[i].burst_t + (time - pro[i].arrive_t)) / pro[i].burst_t;

				//checking for the higest response ratio

				if(hrr < temp){

					// storing the response ratio
					hrr =temp;

					// storing the location

					loc = i;
				}
			}
		}

		// updating the time value
		time += pro[loc].burst_t;

		// calculation of the waiting time
		pro[loc].wait_t = time - pro[loc].arrive_t - pro[loc].burst_t;
		printf("%d",pro[loc].wait_t)
		//calculation of the turn  around time
		pro[loc].ta_t = time - pro[loc].arrive_t;

		pro[loc].complete = 1;

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
	fp = fopen("proc.txt","r");
	process ready_queue[MAXSIZE];

	printf("파일 읽기 시작\n");

	while(!feof(fp)) {
		fscanf(fp, "%d", &ready_queue[i].number);
		fscanf(fp, "%d", &ready_queue[i].arrive_t);
		fscanf(fp, "%d", &ready_queue[i].burst_t);
		fscanf(fp, "%d", &ready_queue[i].priority);
		ready_queue[i].complete = 0;
		index = index + 1;
		printf("%d\n", index);
		i++;
	}

	fclose(fp);

	printf("파일 읽기 종료\n");

	n = index - 1;

	printf("프로세스 개수 : %d",n);

	printf("\n==================== Main Menu ====================\n\n1. Read processes from proc.txt\n2. Generate random processes\n3. Shortest Job First(SJF) : Non-preemption\n4. Highest Response Ratio Next(HRN) : Non-preemption\n5. Exit\n===================================================\n");

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
					printf("%d     %d     %d     %d\n", ready_queue[i].number, ready_queue[i].arrive_t, ready_queue[i].burst_t, ready_queue[i].priority);
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

					printf("%d     %d     %d     %d     %d     %d\n", ready_queue[i].number, ready_queue[i].arrive_t, ready_queue[i].burst_t, ready_queue[i].priority, ready_queue[i].wait_t, ready_queue[i].ta_t);
				}

				printf("반환시간 평균 : %.2f\n",tat/n);
				printf("대기시간 평균 : %.2f\n\n",waitt/n);

				printf("=================================================\n\n");

				continue;

				break;
			case 4:
				arr_sort(ready_queue, n);

				hrn(ready_queue, n);

				printf("실행 순서 :");
				for(i = 0; i < n; i++) {
					printf("  %d  ", ready_queue[i].number);
				}
				printf("\n");

				resort(ready_queue, n);

				printf("\n4. Highest Response Ratio Next(HRN) :\n==================== hrn ====================\n");

				printf("P#     AT     BT     Pri     WT     TAT\n");

				for(i = 0; i < n; i++) {

					tat = tat + ready_queue[i].ta_t;
					waitt = waitt + ready_queue[i].wait_t;

					printf("%d     %d     %d     %d     %d     %d\n", ready_queue[i].number, ready_queue[i].arrive_t, ready_queue[i].burst_t, ready_queue[i].priority, ready_queue[i].wait_t, ready_queue[i].ta_t);
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
