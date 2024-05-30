#include <iostream>
#include <list>
#include <stack>
#include <mutex>
#include <thread>

using namespace std;

class DynamicQueue {
private:
    struct Node {
        list<int> processList; // 프로세스 리스트
        Node* next; // 다음 스택 노드
    };

    Node* stackTop; // 스택의 최상위 노드
    mutex mtx;

public:
    DynamicQueue() {
        stackTop = new Node(); // 초기 스택 노드 생성
    }

    ~DynamicQueue() {
        while (stackTop != nullptr) {
            Node* temp = stackTop;
            stackTop = stackTop->next;
            delete temp;
        }
    }

    void enqueue(int processId, bool isForeground) {
        lock_guard<mutex> lock(mtx);
        if (isForeground) {
            stackTop->processList.push_back(processId);
        }
        else {
            stackTop->processList.push_front(processId);
        }
    }

    int dequeue() {
        lock_guard<mutex> lock(mtx);
        if (stackTop->processList.empty()) {
            return -1; // 빈 경우
        }

        int processId = stackTop->processList.front();
        stackTop->processList.pop_front();

        if (stackTop->processList.empty() && stackTop->next != nullptr) {
            Node* temp = stackTop;
            stackTop = stackTop->next;
            delete temp;
        }

        return processId;
    }

    void promote() {
        lock_guard<mutex> lock(mtx);
        if (stackTop->next == nullptr) {
            return; // 다음 노드가 없으면 종료
        }

        Node* temp = stackTop->next;
        stackTop->next = temp->next;

        temp->next = stackTop;
        stackTop = temp;
    }

    void split_n_merge() {
        lock_guard<mutex> lock(mtx);
        Node* current = stackTop;

        while (current != nullptr) {
            if (current->processList.size() > threshold) {
                list<int> newList;
                auto it = current->processList.begin();
                advance(it, current->processList.size() / 2);

                newList.splice(newList.begin(), current->processList, current->processList.begin(), it);

                Node* newNode = new Node();
                newNode->processList = newList;
                newNode->next = current->next;
                current->next = newNode;

                current = newNode->next;
            }
            else {
                current = current->next;
            }
        }
    }

    void print() {
        lock_guard<mutex> lock(mtx);
        Node* current = stackTop;

        while (current != nullptr) {
            for (int processId : current->processList) {
                cout << processId << " ";
            }
            cout << endl;
            current = current->next;
        }
    }

    static const int threshold = 10; // 임계치
};

int main() {
    DynamicQueue dq;

    dq.enqueue(1, true); // Foreground 프로세스
    dq.enqueue(2, false); // Background 프로세스
    dq.enqueue(3, true);

    dq.print();

    dq.promote();
    dq.print();

    dq.split_n_merge();
    dq.print();

    cout << "Dequeued: " << dq.dequeue() << endl;
    dq.print();

    return 0;
}
