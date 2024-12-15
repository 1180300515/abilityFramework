#include <httplib.h>

//int main() {
//  std::cout << "启动一个httpserver" << std::endl;
//  httplib::Server server;
//  server.Put("/ability/abilityHeartbeat", [](const httplib::Request &req,
//                                                 httplib::Response &res) {
//    std::cout << req.body << std::endl;
//    res.status = 200;
//    res.set_content("OK", "text/plain");
//  });
//  server.listen("0.0.0.0" , 8080);
//}

#include <iostream>
#include <queue>
#include <vector>

using namespace std;

// 定义一个结构体
struct Task {
  int id;
  int priority;

  Task(int id, int priority) : id(id), priority(priority) {}

  // 自定义比较器，使得优先级高的任务排在队列前面
  bool operator<(const Task& other) const {
    return priority < other.priority;  // 大的优先级排在前面
  }
};

int main() {
  // 使用自定义结构体创建优先级队列
  priority_queue<Task> pq;

  // 插入任务
  pq.push(Task(1, 5));   // id = 1, priority = 5
  pq.push(Task(2, 1));   // id = 2, priority = 1
  pq.push(Task(3, 3));   // id = 3, priority = 3

  // 输出并删除优先级队列的元素
  while (!pq.empty()) {
    Task t = pq.top();
    cout << "Task ID: " << t.id << ", Priority: " << t.priority << endl;
    pq.pop();
  }

  // 输出:
  // Task ID: 1, Priority: 5
  // Task ID: 3, Priority: 3
  // Task ID: 2, Priority: 1

  return 0;
}
