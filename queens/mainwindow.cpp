#include "mainwindow.h"

#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  qsrand(QTime::currentTime().elapsed());
  ui->centralWidget->hide();

  actList = findChildren<QAction *>();  // action list

  loadSettings();
  connect(&timer, &QTimer::timeout, [=]() { emit onTimer(); });
}

MainWindow::~MainWindow() {
  abort();

  saveSettings();

  delete ui;

  delete q;
}

void MainWindow::saveSettings() {
  settings->setValue("geometry", saveGeometry());
  settings->setValue("windowState", saveState());
  settings->setValue("nqueens", ui->nQueens->value());
  settings->setValue("stopsols", ui->stopSols->value());
  delete settings;
}

void MainWindow::loadSettings() {
  settings = new QSettings("voicesync", "queens");  //  settings
  restoreState(settings->value("windowState").toByteArray());
  restoreGeometry(settings->value("geometry").toByteArray());
  ui->nQueens->setValue(settings->value("nqueens").toInt());
  ui->stopSols->setValue(settings->value("stopsols").toInt());
  on_nQueens_valueChanged(ui->nQueens->value());
}

QString MainWindow::resultMessage() {
  return QString("lap:%1\", solutions:%2, evaluated:%3 of %4")
      .arg(time_lap.elapsed() / 1000., 0, 'f', 1)
      .arg(q->solutions.size())
      .arg(q->countEvals * 1.0, 0, 'g', 2)
      .arg(q->nCases, 0, 'g', 2);
}

void MainWindow::display() {  //  display result
  timer.stop();
  abort();
  statusBar()->showMessage(resultMessage());

  q->sortSolutions();

  ui->tabSolutions->setSolutions(q);  // refresh table & selected 1st solutions
  ui->tabSolutions->model->setSolution();

  ui->chessBoard->refresh();

  actionsEnableAll();
}

void MainWindow::on_tabSolutions_clicked(const QModelIndex &index) {
  ui->tabSolutions->model->setSolution(index);
  ui->chessBoard->refresh();
}

void MainWindow::doThread(QAction *action) {
  if (action->isChecked()) {
    actionsDisableAll(action);  // except current

    nQueens = q->nQueens;
    q->setStopSolutions(ui->stopSols->value());
    q->zeroQueenCounters();  //  queens & counters
    q->setAbort(false);      // before running any thread

    wks = new Workers(q);
    connect(wks->first(), &Worker::finished, this, &MainWindow::onDisplay);

    statusBar()->showMessage("running...");

    timer.start(500);  // init qtimer
    time_lap.start();
  } else {  //  abort
    abort();
  }
}

void MainWindow::on_actionthread_triggered() { doThread(ui->actionthread); }

void MainWindow::onDisplay() {  // after finished thread
  display();
}
void MainWindow::onTimer() {
  if (wks->check_end(ui->stopSols->value())) display();

  statusBar()->showMessage(
      QString("running, lap:%1 sec., solutions:%2, evaluated: %3")
          .arg(time_lap.elapsed() / 1000., 0, 'f', 1)
          .arg(wks->sol_count())
          .arg(1.0 * wks->eval_count(), 0, 'g', 2));
}

void MainWindow::on_actionsave_triggered() {  // save to file
  if (q->solutions.size()) {
    QString filename = QFileDialog::getSaveFileName(
        this, "Write solutions in a Text file", "", "text file (*.txt)");
    if (!filename.isEmpty()) q->write(filename);
  }
}

void MainWindow::on_nQueens_valueChanged(int nq) {  // n queens changed
  if (q) delete q;
  q = new Queen(nq);
  ui->tabSolutions->setSolutions(q);
  ui->chessBoard->setQueen(q);
  statusBar()->showMessage("");
}

void MainWindow::on_actiontransformations_triggered() {  //  apply
                                                         //  transformations to
                                                         //  selected
  auto sil = ui->tabSolutions->selectionModel()->selectedIndexes();
  if (sil.size() > 0) {
    q->initSolutionMap();

    for (auto s : sil) {
      q->addSolutionMap(ui->tabSolutions->model->getData(s));

      for (int mv = 0; mv < 2; mv++) {
        for (int mh = 0; mh < 2; mh++) {
          for (int r90 = 0; r90 < 4; r90++) {
            for (int tv = 0; tv < q->nQueens; tv++) {  // translations
              for (int th = 0; th < q->nQueens; th++) {
                q->translateV();
                q->addSolutionMap();  // tV
              }
              q->translateH();
              q->addSolutionMap();  // tH
            }

            q->rotate90();  // R90
            q->addSolutionMap();
          }
          q->mirrorH();  // mH
          q->addSolutionMap();
        }
        q->mirrorV();  // mV
        q->addSolutionMap();
      }
    }
    q->updateSolutionsWithMap();  // unique & sorted
  } else {
    q->restoreSolution();
  }
  display();
}

void MainWindow::on_actionabout_triggered() {  // about
  QDialog *aboutDialog = new QDialog(this);
  Ui::Dialog ui;
  ui.setupUi(aboutDialog);

  aboutDialog->show();
}

void MainWindow::on_actioncopy_selected_solutions_triggered() {  // copy
                                                                 // selected
  auto sil = ui->tabSolutions->selectionModel()->selectedIndexes();
  QString res;

  for (auto s : sil)
    res += q->toString(ui->tabSolutions->model->getData(s)) + "\n";
  if (!res.isEmpty()) QApplication::clipboard()->setText(res);
}

void MainWindow::on_actioncopy_all_solutions_triggered() {  //  copy all
  QString res;
  for (auto s : q->solutions) res += q->toString(s) + "\n";
  if (!res.isEmpty()) QApplication::clipboard()->setText(res);
}

void MainWindow::on_actioncopy_image_triggered() {  // copy image
  QApplication::clipboard()->setImage(ui->chessBoard->paintImage());
}

// Queens 14, lap:7.7 sec., solutions:365596, evaluated:1623852370 of
// 11112006825558016 = 0.00001%
void MainWindow::on_actioncopy_result_message_triggered() {
  QApplication::clipboard()->setText(resultMessage());
}
