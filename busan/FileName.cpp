#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// 상수 정의
#define LEN_MIN 15 // 기차 길이 최소값
#define LEN_MAX 50 // 기차 길이 최대값
#define STM_MIN 0 // 마동석 체력 최소값
#define STM_MAX 5 // 마동석 체력 최대값
#define PROB_MIN 10 // 확률 최소값
#define PROB_MAX 90 // 확률 최대값
#define AGGRO_MIN 0 // 어그로 최소값
#define AGGRO_MAX 5 // 어그로 최대값

// 마동석 이동 방향
#define MOVE_LEFT 1 // 왼쪽으로 이동
#define MOVE_STAY 0 // 제자리

// 좀비의 공격 대상
#define ATK_NONE 0 // 공격 없음
#define ATK_CITIZEN 1 // 시민 공격
#define ATK_DONGSEOK 2 // 마동석 공격

// 마동석 행동
#define ACTION_REST 0 // 휴식
#define ACTION_PROVOKE 1 // 도발
#define ACTION_PULL 2 // 붙들기

// 전역 변수 선언
int len_train, stmMadongseok;
int p, m_p;
int C, Z, M;
int turn;
int move_p, move_m;
int aggroC, aggroM;
int behaviorM;

void readyTrain();
void readyMadongseok();
void readyProbability();
void trainState();
void move();
void printState();
void outtro();
void printMadongseok();
void phasesBehaviorZ();
void phasesBehaviorM();

int main(void) {
    // 난수 초기화
    srand((unsigned)time(NULL));

    printf("=======게임 시작=======\n");

    // 유효한 값 입력받기
    readyTrain();
    readyMadongseok();
    readyProbability();

    // 초기 위치 설정
    C = len_train - 7; // 시민 위치
    Z = len_train - 4; // 좀비 위치
    M = len_train - 3; // 마동석 위치

    turn = 1; // 현재 턴 초기화

    trainState(); // 초기 상태 출력

    // 게임 루프
    while (C > 2 && C + 1 != Z) {

        move_p = rand() % 100; // 난수 생성

        // 이동 페이즈
        move();
        trainState(); // 열차 상태 출력
        printState(); // 시민과 좀비 이동 현황 출력
        printMadongseok(); // 마동석 이동 입력받고 상태 출력

        // 행동 페이즈
        phasesBehaviorZ(); // 좀비 행동 페이즈
        phasesBehaviorM(); // 마동석 행동 페이즈

        // 게임오버 조건들 추가
        if (stmMadongseok <= STM_MIN) {
            printf("\nGAME OVER! Madongseok is dead.\n");
            break;
        }
        if (C == Z) {
            // 조건 바꾸고 좀비가 시민을 공격했을 때 게임 종료되도록
        }

        turn++; // 턴 증가
    }

    outtro(); // 게임 결과 출력

    return 0;
}

void readyTrain() {
    // 기차 길이 입력받기
    printf("train length(15~50)>> ");
    scanf_s("%d", &len_train); //기차 길이 입력받음
    if (len_train < LEN_MIN || len_train > LEN_MAX) {
        readyTrain(); // 유효한 값이 입력될 때까지 다시 입력받기
    }
}

void readyMadongseok() {
    // 마동석 체력 입력받기
    printf("madongseok stamina(0~5)>> ");
    scanf_s("%d", &stmMadongseok); //마동석 체력 입력받음
    if (stmMadongseok < STM_MIN || stmMadongseok > STM_MAX) {
        readyMadongseok(); // 유효한 값이 입력될 때까지 다시 입력받기
    }
}

void readyProbability() {
    // 확률 p 입력받기
    printf("percentile probability 'p' (10~90)>> ");
    scanf_s("%d", &p); //확률 입력받음
    if (p < PROB_MIN || p > PROB_MAX) {
        readyProbability(); // 유효한 값이 입력될 때까지 다시 입력받기
    }
}

void trainState() {
    // 열차의 상태를 출력
    printf("\n");
    for (int i = 0; i < len_train; i++) printf("#");
    printf("\n#");
    for (int i = 0; i < len_train - 2; ++i) {
        if (i == C) printf("C"); // 시민 위치
        else if (i == Z) printf("Z"); // 좀비 위치
        else if (i == M) printf("M"); // 마동석 위치
        else printf(" "); // 빈 공간
    }
    printf("#\n");
    for (int i = 0; i < len_train; i++) printf("#");
    printf("\n");
}

void move() {
    // 시민 이동
    if (move_p < (100 - p)) {
        C--;
        if (aggroC < AGGRO_MAX) {
            aggroC++;
        }
    }
    else {
        if (aggroC > AGGRO_MIN) {
            aggroC--;
        }
    }

    // 좀비 이동 (홀수 턴에만 이동)
    if (turn % 2 == 1) {
        if (M == Z + 1 && turn >= 2) {
            // 마동석 붙들기 효과 추가
        }
        else {
            if (aggroC >= aggroM) {
                Z--;
            }
            else {
                Z++;
            }
        }
    }
}

void printState() {
    // 시민 상태 출력
    if (move_p < (100 - p)) {
        printf("\ncitizen: %d -> %d (aggro: %d)\n", C + 1, C, aggroC);
    }
    else {
        printf("\ncitizen does nothing.\n");
    }

    // 좀비 상태 출력
    if (turn % 2 == 1) {
        if (aggroC >= aggroM) {
            printf("zombie: %d -> %d\n", Z + 1, Z);
        }
        else if (aggroC < aggroM) {
            printf("zombie: %d -> %d\n", Z - 1, Z);
        }
    }
    else {
        printf("zombie: stay %d (cannot move)\n", Z);
    }
}

void printMadongseok() {
    // 마동석 이동 입력 받음
    if (M == Z + 1) {
        printf("\nMadongseok move(%d:stay)\n", M);
    }
    else {
        printf("\nmadongseok move(0:stay, 1:left)>> ");
        scanf_s("%d", &move_m);
        if (move_m != 0 && move_m != 1) {
            printMadongseok();
            return;
        }
    }

    // 마동석 이동
    if (move_m == MOVE_LEFT) {
        M--;
        if (aggroM < AGGRO_MAX) {
            aggroM++;
        }
    }
    else if (move_m == MOVE_STAY) {
        if (aggroM > AGGRO_MIN) {
            aggroM--;
        }
    }

    trainState();

    // 마동석 이동 현황 출력
    if (move_m == MOVE_LEFT) {
        printf("\nmadongseok move %d -> %d (aggro: %d, stamina: %d)\n", M + 1, M, aggroM, stmMadongseok);
    }
    else {
        printf("\nmadongseok: stay %d (aggro: %d, stamina: %d)\n", M, aggroM, stmMadongseok);
    }
}

void phasesBehaviorZ() {
    if (C == Z + 1 && Z == M + 1) {
        if (aggroC > aggroM) {
            printf("GAME OVER! Citizen dead...\n");
            exit(0); // 게임 종료
        }
        else {
            stmMadongseok--;
        }
    }
    else if (C == Z + 1 && Z > M + 1) {
        printf("GAME OVER! Citizen dead...\n");
        exit(0); // 게임 종료
    }
    else if (C > Z + 1 && Z == M + 1) {
        stmMadongseok--;
    }
}

void phasesBehaviorM() {
    if (Z != M + 1) {
        printf("\nmadongseok action(0:rest, 1:provoke)>> ");
        scanf_s("%d", &behaviorM);

        if (behaviorM == ACTION_REST) {
            printf("madongseok rests...\n");
            if (aggroM > AGGRO_MIN) {
                aggroM--;
            }
            if (stmMadongseok < STM_MAX) {
                stmMadongseok++;
            }
            printf("madongseok: %d (aggro: %d, stamina: %d)\n", M, aggroM, stmMadongseok);
        }
        else if (behaviorM == ACTION_PROVOKE) {
            printf("madongseok provoked zombie...\n");
            aggroM = AGGRO_MAX;
            printf("madongseok: %d (aggro: 5, stamina: %d)\n", M, stmMadongseok);
        }
    }
    else if (Z != M + 1) {
        printf("madongseok action(0:rest, 1:provoke, 2:pull)>> ");
        scanf_s("%d", &behaviorM);

        if (behaviorM == ACTION_REST) {
            printf("madongseok rests...\n");
            if (aggroM > AGGRO_MIN) {
                aggroM--;
            }
            if (stmMadongseok < STM_MAX) {
                stmMadongseok++;
            }
            printf("madongseok: %d (aggro: %d, stamina: %d)\n", M, aggroM, stmMadongseok);
        }
        else if (behaviorM == ACTION_PROVOKE) {
            printf("madongseok provoked zombie...\n");
            aggroM = AGGRO_MAX;
            printf("madongseok: %d (aggro: 5, stamina: %d)\n", M, stmMadongseok);
        }
        else if (behaviorM == ACTION_PULL) {
            m_p = rand() % 100; // 난수 생성
            if (m_p < (100 - p)) {
                if (aggroM < AGGRO_MIN - 1) {
                    aggroM = aggroM + 2;
                }
                if (stmMadongseok > STM_MAX) {
                    stmMadongseok--;
                }
                // 이 자리에는 확률에 성공하면 좀비가 다음 턴에 움직이지 않도록 하는 코드 작성
                printf("madongseok pulled zombie... Next turn, it can't move\n");
            }
            else {
                printf("madongseok failed to pull the zombie.\n");
            }
        }
    }
}

void outtro() {
    // 게임 종료 메시지 출력
    if (C <= 2) {
        printf("\nYOU WIN! Citizen has escaped.\n");
    }
    else {
        printf("GAME OVER.\n");
    }
}
