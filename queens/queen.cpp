#include "queen.h"

Queen::Queen() {}
Queen::Queen(uint nQueens) { init(nQueens); }

Queen::Queen(Queen *q) {
  init(q->nQueens);
  copy2q(q->queens);
}

Queen::~Queen() {
  deleteSolutions();
  if (queens) delete queens;
}

void Queen::init(uint nQueens) {
  this->nQueens = nQueens;

  queens = new uint[nQueens];

  nCases = pow(nQueens, nQueens);
  nPermutations = factorial(nQueens);
  zeroQueenCounters();
}

void Queen::random() {
  for (uint i = 0; i < nQueens; i++) queens[i] = uint(qrand()) % nQueens;
}

bool Queen::isValid() {  // find if a board is valid
  bool valid = true;
  countEvals++;

  for (uint i = 0; i < nQueens - 1 && valid; i++)
    for (uint j = i + 1; j < nQueens; j++) {
      if (queens[i] == queens[j]) {  // horizontal -> ci=cj
        valid = false;
        break;
      }
      if (i - queens[i] == j - queens[j]) {  // vertical  / ri-ci = rj-cj
        valid = false;
        break;
      }
      if (abs_sub(queens[i], queens[j]) ==
          abs_sub(i, j)) {  // vertical \ |ci-cj| = |i-j|
        valid = false;
        break;
      }
    }
  if (valid) countSolutions++;
  return valid;
}

// solution mgr.
void Queen::saveSolution() {  // in queens
  if (!m_abort) {
    uint *q = new uint[nQueens];
    copy(q, queens);
    solutions << q;

    if (stopSolutions != 0 && solutions.size() >= stopSolutions) abort();
  }
}
void Queen::deleteSolutions() {
  for (auto s : solutions) delete s;
  solutions.clear();
}
QString Queen::getSolution(int ix) {
  QString s;
  if (ix >= 0 && ix < solutions.size()) s = toString(solutions[ix]);
  return s;
}

uint *Queen::getRawSolution(int ix) {
  if (ix >= 0 && ix < solutions.size()) return solutions[ix];
  return nullptr;
}

void Queen::sortSolutions() {
  if (solutions.size()) {
    std::sort(solutions.begin(), solutions.end(),
              [=](uint *a, uint *b) -> bool {
                for (uint i = 0; i < nQueens; i++) {
                  auto ia = a[i], ib = b[i];
                  if (ia != ib) return ia < ib;
                }
                return false;  // a==b -> ! a<b
              });
  }
}

// recursive solutions generation

void Queen::scan() {
  m_abort = false;

  deleteSolutions();
  zeroQueenCounters();

  scan(0);
}

void Queen::scan(uint nq) {
  if (!m_abort) {
    if (nq < nQueens) {
      uint mvq[nQueens];  // generate moves -> mvq
      uint nmv = moves(nq, mvq);

      for (uint i = 0; i < nmv; i++) {
        uint mvtmp = queens[nq];  // keep pos.

        queens[nq] = mvq[i];  // move(i)
        scan(nq + 1);

        queens[nq] = mvtmp;  // unmove
      }
    } else if (isValid())
      saveSolution();
  }
}

uint Queen::moves(uint nq,
                  uint *mvq) {  // generate all possible moves of 'nq' queen
  uint cm = 0;

  for (uint i = 0; i < nQueens; i++) {
    bool valid = true;
    for (uint c = nq, j = nq - 1; c != 0 && valid;
         c--, j--) {  // check against prev. queens

      if (i != queens[j]) {                     // - horizontal
        if ((i - nq) != (queens[j] - j)) {      // / diag.
          valid = abs_sub(j , nq) != abs_sub(queens[j] , i);  // \ diag.
        } else {
          valid = false;
        }
      } else {
        valid = false;
      }

      countEvals++;
    }
    if (valid) mvq[cm++] = i;
  }
  return cm;
}

void Queen::zeroQueenCounters() {
  for (uint i = 0; i < nQueens; i++) queens[i] = 0;
  initCounters();
}

void Queen::initCounters() {
  endSeq = false;

  countSolutions = 0;
  countEvals = 0;
  lastVal = 0;
  nPerm = 0;

  deleteSolutions();
}

void Queen::beginCombinations(uint initVal) {
  for (uint i = 0; i < nQueens; i++) queens[i] = initVal;
  initCounters();
}

bool Queen::nextCombination(uint nq) {
  bool cont = true;
  for (uint i = nq; i < nQueens && cont; i++) {
    if (++queens[i] == nQueens)
      queens[i] = 0;
    else
      cont = false;
  }
  return endSeq = cont;
}

bool Queen::endCombination() { return endSeq; }

// transformations
void Queen::rotate90() {
  uint *rotQueens = new uint[nQueens];
  for (uint i = 0; i < nQueens; i++) {
    rotQueens[i] = 0;
    for (uint j = 0; j < nQueens; j++) {  // find i
      if (queens[j] == i) {
        rotQueens[i] = nQueens - j - 1;
        break;
      }
    }
  }
  copy2q(rotQueens);
  delete[] rotQueens;
}
void Queen::mirrorH() {
  for (uint i = 0; i < nQueens; i++) queens[i] = (nQueens - 1) - queens[i];
}
void Queen::mirrorV() {
  for (uint i = 0; i < nQueens / 2; i++)
    swap(queens[i], queens[nQueens - 1 - i]);
}
void Queen::translateV() {  // up
  for (uint i = 0; i < nQueens; i++) queens[i] = (queens[i] + 1) % nQueens;
}
void Queen::translateH() {  // right
  uint *v = new uint[nQueens];
  for (uint i = 0; i < nQueens - 1; i++) v[i + 1] = queens[i];
  v[0] = queens[nQueens - 1];
  copy2q(v);
  delete[] v;
}

QString Queen::toString() {
  QString s;
  for (uint i = 0; i < nQueens; i++)
    s += QString::number(queens[i] + 1) + (i != nQueens - 1 ? " " : "");
  return s;
}
QString Queen::toString(uint *q) {
  QString s;
  for (uint i = 0; i < nQueens; i++) s += QString::number(q[i] + 1) + " ";
  return s;
}

QString Queen::toStringRaw() {
  QString s;
  for (uint i = 0; i < nQueens; i++) s += QString::number(queens[i]);
  return s;
}
QString Queen::toStringRaw(uint *q) {
  QString s;
  for (uint i = 0; i < nQueens; i++) s += QString::number(q[i]);
  return s;
}
QString Queen::toStringRaw(uint *q, uint n) {
  QString s;
  for (uint i = 0; i < n; i++) s += QString::number(q[i]);
  return s;
}
uint *Queen::fromString(QString s) {  //  to queens
  auto sl = s.split(" ", QString::SkipEmptyParts);
  if (uint(sl.size()) == nQueens) {
    for (uint i = 0; i < nQueens; i++) queens[i] = uint(sl[int(i)].toInt());
  }
  return queens;
}

// permutations O(n!)

void Queen::beginPermutations() {  // start w/ q[i]=i;
  zeroQueenCounters();
  for (uint i = 0; i < nQueens; i++) queens[i] = i;
}

void Queen::permutations(uint nCol) {
  if (!m_abort) {
    if (nCol != nQueens)
      for (uint i = nCol; i < nQueens; i++) {
        swap(queens[i], queens[nCol]);
        permutations(nCol + 1);
        swap(queens[i], queens[nCol]);
      }
    else {
      nPerm++;
      if (isValid()) saveSolution();
    }
  }
}

void Queen::permutations() {
  beginPermutations();
  permutations(0);
}

void Queen::nThPermutation(uint index) {
  uint64 scale;
  uint64 d;

  scale = factorial(nQueens);

  for (uint i = 0; i < nQueens - 1; i++) {
    scale /= uint64(nQueens - i);
    d = index / scale;
    index %= scale;
    if (d > 0) {
      auto c = queens[i + d];
      memmove(queens + i + 1, queens + i, d * sizeof(*queens));
      queens[i] = c;
    }
  }
}

// combinations: O(n^n)
void Queen::generateCombinations() {
  zeroQueenCounters();
  for (beginCombinations(); !endCombination(); nextCombination())
    if (isValid()) saveSolution();
}

bool Queen::write(QString fileName) {
  bool res = false;
  QFile file(fileName);
  if (file.open(QIODevice::WriteOnly)) {
    for (auto s : solutions) file.write((toString(s) + "\n").toLatin1());
    file.close();
    res = true;
  }
  return res;
}

// static mutex def.
QMutex RecursiveScan::mtx;
// static Queen
bool Queen::m_abort = false;
int Queen::stopSolutions = 0;
