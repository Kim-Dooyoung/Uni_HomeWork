#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <chrono>
#include <queue>
#include <list>
#include <atomic>

using namespace std;
using namespace chrono;

struct Process {
    int pid;
    bool isForeground;
    bool isPromoted;
    int wakeUpTime;
    string command;
};

class Scheduler {
private:
    list<Process> dynamicQueue;
    queue<Process> waitQueue;
    mutex mtx;
    condition_variable cv;
    atomic<int> nextPid;
    bool stop;

public:
    Scheduler() : nextPid(0), stop(false) {}

    ~Scheduler() {
        stop = true;
        cv.notify_all();
    }

    void addProcess(const string& command, bool isForeground) {
        lock_guard<mutex> lock(mtx);
        Process p = { nextPid++, isForeground, false, -1, command };
        dynamicQueue.push_back(p);
        cv.notify_all();
    }

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

    void monitor() {
        while (!stop) {
            this_thread::sleep_for(seconds(2));
            printQueues();
        }
    }

    void shell() {
        ifstream file("commands.txt");
        string line;

        while (!stop && getline(file, line)) {
            if (line.empty()) continue;

            this_thread::sleep_for(seconds(5));
            parseAndExecute(line);
        }
    }

    void parseAndExecute(const string& command) {
        istringstream iss(command);
        string token;
        vector<string> tokens;

        while (getline(iss, token, ' ')) {
            tokens.push_back(token);
        }

        if (!tokens.empty()) {
            if (tokens[0] == "echo") {
                for (size_t i = 1; i < tokens.size(); ++i) {
                    cout << tokens[i] << " ";
                }
                cout << endl;
            }
            else if (tokens[0] == "dummy") {
                // Dummy command implementation
            }
            else if (tokens[0] == "gcd") {
                if (tokens.size() == 3) {
                    int x = stoi(tokens[1]);
                    int y = stoi(tokens[2]);
                    cout << "GCD of " << x << " and " << y << " is " << gcd(x, y) << endl;
                }
            }
            else if (tokens[0] == "prime") {
                if (tokens.size() == 2) {
                    int x = stoi(tokens[1]);
                    cout << "Number of primes less than or equal to " << x << " is " << countPrimes(x) << endl;
                }
            }
            else if (tokens[0] == "sum") {
                if (tokens.size() == 2) {
                    int x = stoi(tokens[1]);
                    cout << "Sum of integers up to " << x << " is " << sum(x) << endl;
                }
            }
            addProcess(command, true); // 기본적으로 FG로 실행
        }
    }

    int gcd(int x, int y) {
        while (y != 0) {
            int temp = y;
            y = x % y;
            x = temp;
        }
        return x;
    }

    int countPrimes(int x) {
        vector<bool> isPrime(x + 1, true);
        isPrime[0] = isPrime[1] = false;
        for (int i = 2; i * i <= x; ++i) {
            if (isPrime[i]) {
                for (int j = i * i; j <= x; j += i) {
                    isPrime[j] = false;
                }
            }
        }
        return count(isPrime.begin(), isPrime.end(), true);
    }

    int sum(int x) {
        long long result = 0;
        for (int i = 1; i <= x; ++i) {
            result += i;
        }
        return result % 1000000;
    }
};

int main() {
    Scheduler scheduler;

    thread monitorThread(&Scheduler::monitor, &scheduler);
    thread shellThread(&Scheduler::shell, &scheduler);

    monitorThread.join();
    shellThread.join();

    return 0;
}
