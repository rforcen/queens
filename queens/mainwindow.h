#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <queen.h>
#include <worker.h>

#include <QAction>
#include <QClipboard>
#include <QFileDialog>
#include <QMainWindow>
#include <QSettings>
#include <QTime>
#include <QTimer>

#include "ui_about.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

 private slots:
  void onDisplay();
  void onTimer();

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

 private:
  Ui::MainWindow *ui;

  Queen *q = nullptr;

  QTimer timer;
  QTime time_lap;
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

  void display();
  void abort() {
    if (wks) wks->wait_all();
  }
  void saveSettings(), loadSettings();
  QSettings *settings;

  Workers *wks = nullptr;
  int nQueens = 0;

  void doThread(QAction *action);
  QString resultMessage();
};

#endif  // MAINWINDOW_H
