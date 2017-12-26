#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QClipboard>
#include <QFileDialog>
#include <QMainWindow>
#include <QSettings>
#include <QThread>
#include <QTime>
#include <QTimer>

#include <queen.h>
#include "ui_about.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

 private slots:
  void onDisplay();
  void onTimer();

  void on_actiongenerate_all_triggered();
  void on_actionpermutations_triggered();
  void on_tabSolutions_clicked(const QModelIndex &index);
  void on_actionthread_triggered();
  void on_actionsave_triggered();
  void on_nQueens_valueChanged(int nQueens);
  void on_actiontransformations_triggered();
  void on_actionabout_triggered();
  void on_actioncopy_selected_solutions_triggered();
  void on_actioncopy_all_solutions_triggered();
  void on_actioncopy_image_triggered();
  void on_actioncopy_result_message_triggered();

 signals:
  void display();

 private:
  Ui::MainWindow *ui;

  Queen *q = 0;

  QTimer timer;
  QTime tim;
  QList<QAction *> actList;

  void actionsDisableAll(QAction *act) {
    for (auto a : actList)  // disable all except current
      if (a != act) a->setDisabled(true);
  }
  void actionsEnableAll() {
    for (auto a : actList) {  // all actions->set un checked && enabled
      a->setEnabled(true);
      a->setChecked(false);
    }
  }

  void disp();
  void abort() {
    if (rs) {
      q->abort();
      for (int i = 0; i < nQueens; i++)
        rs[i].wait();  // wait all threads to finish
    }
  }
  void saveSettings(), loadSettings();
  QSettings *settings;

  RecursiveScan *rs = 0;
  int nQueens = 0;

  void doThread(QAction *action, RecursiveScan::ProcType pt);
  QString resultMessage();

  // local thread sample
  class Runner : public QThread {
   public:
    Runner(Queen *q) : q(q) {}
    Queen *q = 0;

   protected:
    void run() override {
      if (q)
        for (q->beginCombinations(); !q->endCombination(); q->nextCombination())
          if (q->isValid()) q->saveSolution();
    }
  } * combiRunner;

  void threadCombi() {
    combiRunner = new Runner(q);
    connect(combiRunner, SIGNAL(finished()), this, SLOT(onDisplay()));
    combiRunner->start();
  }
};

#endif  // MAINWINDOW_H
