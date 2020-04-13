#ifndef WORKER_H
#define WORKER_H

#include <queen.h>

#include <QThread>

class Worker : public QThread, public Queen {
  void run() override {
    scan(1);
    addResults();
  }

 public:
  Worker(Queen *q, int i) : Queen(q), root_queen(q) { process(q, i); }
  Worker() : Queen() {}

  void addResults() {  //  update root results
    mtx.lock();
    {
      root_queen->countEvals += countEvals;
      root_queen->countSolutions += countSolutions;
      root_queen->solutions += solutions;
      solutions.clear();  // once copied, clear to avoid deleting <int*> twice
    }
    mtx.unlock();
  }

  int64 getSolutionCount() { return countSolutions; }
  int64 getEvalCount() { return countEvals; }

  void process(Queen *q, int nq) {
    q->queens[0] = nq;
    this->root_queen = q;

    init(q->nQueens);
    copy2q(q->queens);  // q contains initial values

    start();
  }

  void abort() {
    if (root_queen) root_queen->abort();
  }

 private:
  Queen *root_queen = nullptr;
  static QMutex mtx;  //  same mtx for all threads
};

class Workers : public QVector<Worker *> {
 public:
  Workers(Queen *q) : QVector<Worker *>() {
    for (int i = 0; i < q->nQueens; i++) push_back(new Worker(q, i));
  }

  void totals(qint64 &sol_count, qint64 &eval_count) {  //
    sol_count = this->sol_count();
    eval_count = this->eval_count();
  }

  qint64 sol_count() {
    qint64 _sol_count = 0;
    for (auto &w : *this) _sol_count += w->getSolutionCount();
    return _sol_count;
  }
  qint64 eval_count() {
    qint64 _eval_count = 0;
    for (auto &w : *this) _eval_count += w->getEvalCount();
    return _eval_count;
  }
  bool check_end(int req_sols) {
    bool end = false;
    if (req_sols != 0 && sol_count() >= req_sols) {
      wait_all();
      end = true;
    }
    return end;
  }
  void wait_all() {
    (*this)[0]->abort();
    for (auto &w : *this) w->wait();
  }
};
#endif  // WORKER_H
