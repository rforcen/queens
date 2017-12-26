#ifndef TABSOLUTIONS_H
#define TABSOLUTIONS_H

#include <queen.h>
#include <QTableView>

#include <QDebug>

class SolutionsDispModel : public QAbstractTableModel {
  Q_OBJECT
 private:
  static const int nCols = 4;

 public:
  Queen *q;

  SolutionsDispModel(QObject *parent, Queen *q)
      : QAbstractTableModel(parent), q(q) {}

  void setSolution(const QModelIndex &index) {
    auto qs = data(index).toString().split(" ", QString::SkipEmptyParts);
    if (qs.size() == q->nQueens)
      for (int i = 0; i < q->nQueens; i++) q->queens[i] = qs[i].toInt() - 1;
  }
  void setSolution() {
    auto qs = data(index(0, 0)).toString().split(" ", QString::SkipEmptyParts);
    if (qs.size() == q->nQueens)
      for (int i = 0; i < q->nQueens; i++) q->queens[i] = qs[i].toInt() - 1;
  }

  void refresh() {
    emit dataChanged(index(0, 0),
                     index(rowCount(), columnCount()));  // update whole view
  }
  int *getData(const QModelIndex &index) {
    return q->getRawSolution(index.row() * nCols + index.column());
  }

  int rowCount(const QModelIndex &index = QModelIndex()) const {
    (void)index;
    return (q->solutions.size()) ? (q->solutions.size() / nCols) + 1 : 0;
  }
  int columnCount(const QModelIndex &index = QModelIndex()) const {
    (void)index;
    return nCols;
  }
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const {
    if (role == Qt::DisplayRole) {
      return q->getSolution(index.row() * nCols + index.column());
    }
    return QVariant();
  }
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role) const {
    switch (orientation) {
      case Qt::Horizontal:
        if (role == Qt::DisplayRole && section == 0)
          return QVariant("solutions");
        break;
      case Qt::Vertical:
        if (role == Qt::DisplayRole) return QVariant(section * nCols + 1);
        break;
      default:;
    }
    return QVariant();
  }
};

class TabSolutions : public QTableView {
 public:
  TabSolutions(QWidget *p = 0);
  SolutionsDispModel *model = 0;

  void setSolutions(Queen *q) {
    if (model) delete model;
    model = new SolutionsDispModel(this, q);
    setModel(model);
  }
};

#endif  // TABSOLUTIONS_H
