#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <chrono>
#include <queue>
#include <list>

using namespace std;
using namespace chrono;

// 프로세스 정보를 저장하는 구조체
struct Process {
    int pid;
    bool isForeground;
    bool isPromoted;
    int wakeUpTime;
};

// 동적 큐와 대기 큐를 관리하는 클래스
class Scheduler {
private:
    list<Process> dynamicQueue; // 동적 큐
    queue<Process> waitQueue; // 대기 큐
    mutex mtx;
    condition_variable cv;
    int nextPid;
    bool stop;

public:
    Scheduler() : nextPid(0), stop(false) {}

    ~Scheduler() {
        stop = true;
        cv.notify_all();
    }

    // 새로운 프로세스를 생성하고 동적 큐에 추가
    void addProcess(bool isForeground) {
        lock_guard<mutex> lock(mtx);
        Process p = { nextPid++, isForeground, false, -1 };
        dynamicQueue.push_back(p);
        cv.notify_all();
    }

    // 대기 큐에서 프로세스를 꺼내 동적 큐에 추가
    void wakeUpProcess() {
        lock_guard<mutex> lock(mtx);
        if (!waitQueue.empty()) {
            Process p = waitQueue.front();
            waitQueue.pop();
            p.wakeUpTime = -1;
            dynamicQueue.push_back(p);
            cv.notify_all();
        }
    }

    // 대기 큐에 프로세스를 추가하고 일정 시간 후 깨어남
    void sleepProcess(int pid, int duration) {
        lock_guard<mutex> lock(mtx);
        for (auto it = dynamicQueue.begin(); it != dynamicQueue.end(); ++it) {
            if (it->pid == pid) {
                it->wakeUpTime = duration_cast<seconds>(system_clock::now().time_since_epoch()).count() + duration;
                waitQueue.push(*it);
                dynamicQueue.erase(it);
                break;
            }
        }
        cv.notify_all();
    }

    // 동적 큐와 대기 큐의 상태를 출력
    void printQueues() {
        lock_guard<mutex> lock(mtx);
        cout << "Running: [";
        for (const auto& p : dynamicQueue) {
            cout << p.pid << (p.isForeground ? "F" : "B") << (p.isPromoted ? "*" : "") << " ";
        }
        cout << "]" << endl;

        cout << "Wait Queue: [";
        for (queue<Process> q = waitQueue; !q.empty(); q.pop()) {
            const auto& p = q.front();
            cout << p.pid << (p.isForeground ? "F" : "B") << ":" << p.wakeUpTime << " ";
        }
        cout << "]" << endl;
    }

    // Monitor 프로세스
    void monitor() {
        while (!stop) {
            this_thread::sleep_for(seconds(2)); // 2초마다 상태 출력
            printQueues();
        }
    }

    // Shell 프로세스
    void shell() {
        while (!stop) {
            this_thread::sleep_for(seconds(5)); // 5초마다 새로운 프로세스 추가
            addProcess(rand() % 2 == 0);
            printQueues();
        }
    }
};

int main() {
    Scheduler scheduler;

    thread monitorThread(&Scheduler::monitor, &scheduler);
    thread shellThread(&Scheduler::shell, &scheduler);

    // 예제용 슬립/웨이크업 테스트
    this_thread::sleep_for(seconds(1));
    scheduler.sleepProcess(0, 3);
    this_thread::sleep_for(seconds(4));
    scheduler.wakeUpProcess();

    monitorThread.join();
    shellThread.join();

    return 0;
}
