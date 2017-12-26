#ifndef QUEEN_H
#define QUEEN_H

#include <math.h>

#include <QFile>
#include <QList>
#include <QString>
#include <QThread>

// q8: 15863724
typedef qint64 int64;

class Queen {
 public:
  Queen(int nQueens);
  Queen(Queen *q);
  Queen();
  ~Queen();

  int nQueens = 8;

  bool endSeq = false;
  int64 countSolutions = 0, countEvals = 0, lastVal = 0, nPerm = 0;
  double nCases =
      0;  // pow(nQueens, nQueens), 8=16e6, 387e6, 10=1e10, 11=2.8e12,
          // ... 25=9e34
  int64 nPermutations = 0;

  int *queens = 0;  // row in board, col is index

  typedef QList<int *> SolutionList;
  SolutionList solutions, solSave;
  static int stopSolutions;
  QMap<QString, int *> solutionMap;

  static bool m_abort;  //  abort processing all threads
  void setAbort(int a) { m_abort = a; }

  inline int operator[](int x) { return queens[x]; }
  inline int abs(int x) { return x < 0 ? -x : x; }
  int64 factorial(int x) {
    if (x == 0)
      return 1;
    else
      return x * factorial(x - 1);
  }
  int64 ipow(int x, int y) {
    int64 r = 1;
    for (int i = 0; i < y; i++) r *= x;
    return r;
  }

  void init(int nQueens);
  void abort() { m_abort = true; }

  void random();
  bool isValid();
  bool isValid(int *q);

  void saveSolution(), deleteSolutions();
  QString getSolution(int ix);
  int *getRawSolution(int ix);
  void sortSolutions();
  void setStopSolutions(int ss) { stopSolutions = ss; }

  void initSolutionMap() { solutionMap.clear(); }
  void addSolutionMap(int *v) {
    if (v) {
      copy2q(v);
      if (isValid()) solutionMap[toString()] = queensCopy();
    }
  }
  void addSolutionMap() {
    if (isValid()) solutionMap[toString()] = queensCopy();
  }
  void updateSolutionsWithMap() {
    for (auto v : solSave) delete v;  // init solSave
    solSave.clear();
    for (auto v : solutions) solSave << vectCopy(v);  // save a copy

    deleteSolutions();  // as we've save a new copy of <int*> in solutionMap
    for (auto s : solutionMap) solutions << s;  // sorted & unique
  }
  void restoreSolution() {
    if (!solSave.isEmpty()) {
      deleteSolutions();
      solutions = solSave;
      solSave.clear();
    }
  }

  void generateCombinations();

  void scan();
  void scan(int nq);
  int moves(int nq, int *mvq);

  void zeroQueenCounters();

  void beginCombinations(int initVal = 0), beginPermutations(), initCounters();
  bool nextCombination(int nq = 0);
  bool endCombination();

  // transformations
  void rotate90(), mirrorV(), mirrorH(), translateH(), translateV();
  void copy(int *dest, int *source) {
    memcpy(dest, source, nQueens * sizeof(*queens));
  }
  void copy2q(int *source) {
    memcpy(queens, source, nQueens * sizeof(*queens));
  }
  int *queensCopy() {  // return a new copy of queens
    int *v = new int[nQueens];
    copy(v, queens);
    return v;
  }
  int *vectCopy(int *vc) {  // return a new copy of queens
    int *v = new int[nQueens];
    copy(v, vc);
    return v;
  }
  bool EQvect(int *v1, int *v2) {
    bool eq = true;
    for (int i = 0; i < nQueens && eq; i++) eq = v1[i] == v2[i];
    return eq;
  }

  void permutations(int nCol);
  void permutations();

  void nThPermutation(int index);

  inline void swap(int &a, int &b) {
    int x = a;
    a = b;
    b = x;
  }

  QString toString(), toString(int *q);
  QString toStringRaw(), toStringRaw(int *q), toStringRaw(int *q, int n);
  int *fromString(QString s);

  bool write(QString fileName);
};

class RecursiveScan : public Queen, public QThread {
  Queen *qParent = 0;
  static QMutex mtx;  //  same mtx for all threads

 public:
  typedef enum { eRecurse, ePermute, eCombination } ProcType;
  ProcType pt = eRecurse;

  void run() {
    switch (pt) {  // process from second queen & assign results
      case eRecurse:
        scan(1);
        break;
      case ePermute:
        permutations(1);
        break;
      case eCombination:
        while (!nextCombination(1) && !m_abort)
          if (isValid()) saveSolution();
        break;
      default:
        break;
    }
    addResults();
  }
  void addResults() {  //  update parent results
    mtx.lock();
    {
      qParent->countEvals += countEvals;
      qParent->countSolutions += countSolutions;
      qParent->solutions += solutions;
      solutions.clear();  // once copied, clear to avoid deleting <int*> twice
    }
    mtx.unlock();
  }

  void setProcType(ProcType pt) { this->pt = pt; }
  int getSolutionCount() { return countSolutions; }
  int getEvalCount() { return countEvals; }

  RecursiveScan(Queen *q) : Queen(q), qParent(q) { start(); }

  RecursiveScan() : Queen() {}  //  create uninitializated instance of Queen

  void doit(Queen *q) {
    this->qParent = q;
    init(q->nQueens);

    copy2q(q->queens);  // q contains initial values
    start();
  }
  void process(Queen *q, ProcType pt) {
    this->pt = pt;
    doit(q);
  }
};

/*
int index(char *abc, int size)  // find index of permutation
{
   if(abc ==0 || *abc==0 || size == 0)
  {
    return 0;;
  }
  return ((abc[0] - '0') * pow(2,size-1)) + index(abc + 1, size -1);
}
*/
#endif  // QUEEN_H
