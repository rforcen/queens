#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  qsrand(uint(QTime::currentTime().elapsed()));
  ui->centralWidget->hide();

  actList = findChildren<QAction *>(); // action list

  loadSettings();
}

MainWindow::~MainWindow() {
  abort();

  saveSettings();

  delete ui;

  if (rs)
    delete[] rs;
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
  settings = new QSettings("voicesync", "queens"); //  settings
  restoreState(settings->value("windowState").toByteArray());
  restoreGeometry(settings->value("geometry").toByteArray());
  ui->nQueens->setValue(settings->value("nqueens").toInt());
  ui->stopSols->setValue(settings->value("stopsols").toInt());
  on_nQueens_valueChanged(uint(ui->nQueens->value()));
}

QString MainWindow::resultMessage() {
  return QString(
             "Queens %1, lap:%4 sec., solutions:%2, evaluated:%3 of %5 = %6% ")
      .arg(q->nQueens)
      .arg(q->solutions.size())
      .arg(q->countEvals)
      .arg(tim.elapsed() / 1000., 0, 'f', 1)
      .arg(q->nCases)
      .arg(100.0 * q->countEvals / q->nCases, 0, 'f', 5);
}

void MainWindow::disp() { //  display result
  statusBar()->showMessage(resultMessage());

  q->sortSolutions();

  ui->tabSolutions->setSolutions(q); // refresh table & selected 1st solutions
  ui->tabSolutions->model->setSolution();

  ui->chessBoard->refresh();

  actionsEnableAll();

  timer.stop();
}

void MainWindow::on_tabSolutions_clicked(const QModelIndex &index) {
  ui->tabSolutions->model->setSolution(index);
  ui->chessBoard->refresh();
}

void MainWindow::doThread(QAction *action, RecursiveScan::ProcType pt) {
  if (action->isChecked()) {
    actionsDisableAll(action); // except current

    nQueens = q->nQueens;
    q->setStopSolutions(ui->stopSols->value());

    if (rs)
      delete[] rs; // init threads
    rs = new RecursiveScan[nQueens];

    timer.start(500); // init timer
    tim.start();
    connect(&timer, &QTimer::timeout, [=]() { emit onTimer(); });

    q->zeroQueenCounters(); //  queens & counters
    q->setAbort(false);     // before running any thread

    for (uint i = 0; i < nQueens; i++) {
      switch (pt) {
      case RecursiveScan::eRecurse: // q[0]=i
      case RecursiveScan::eCombination:
        q->queens[0] = i;
        break;

      case RecursiveScan::ePermute: // in q[i]=i, nth perm is swap (0,i)
        q->beginPermutations();
        q->swap(q->queens[0], q->queens[i]);
        break;
      }

      rs[i].process(q, pt); //  will process(1) all results from 2nd queen

      connect(&rs[i], SIGNAL(finished()), this,
              SLOT(onDisplay())); // thread finished -> display
    }
    statusBar()->showMessage("running...");
  } else { //  abort
    abort();
  }
}

void MainWindow::on_actionthread_triggered() {
  doThread(ui->actionthread, RecursiveScan::eRecurse);
}
void MainWindow::on_actionpermutations_triggered() {
  doThread(ui->actionpermutations, RecursiveScan::ePermute);
}
void MainWindow::on_actiongenerate_all_triggered() {
  doThread(ui->actiongenerate_all, RecursiveScan::eCombination);
}

void MainWindow::onDisplay() { // after finished thread
  if (--nQueens <= 0)
    disp();
}
void MainWindow::onTimer() {
  uint stopsol = uint(ui->stopSols->value());
  uint64 sc = 0, sev = 0; // count solutions, evals

  for (uint i = 0; i < nQueens; i++) {
    sc += rs[i].getSolutionCount();
    sev += rs[i].getEvalCount();
  }

  if (stopsol != 0 && sc >= stopsol) // stop scan when req. solutions found
    q->abort();

  statusBar()->showMessage(
      QString("running, lap:%1 sec., solutions:%2, evaluated: %3")
          .arg(tim.elapsed() / 1000., 0, 'f', 1)
          .arg(sc)
          .arg(sev));

  ui->chessBoard->setQueen(q);
  ui->chessBoard->refresh();
}

void MainWindow::on_actionsave_triggered() { // save to file
  if (q->solutions.size()) {
    QString filename = QFileDialog::getSaveFileName(
        this, "Write solutions in a Text file", "", "text file (*.txt)");
    if (!filename.isEmpty())
      q->write(filename);
  }
}

void MainWindow::on_nQueens_valueChanged(int nq) { // n queens changed
  if (q)
    delete q;
  q = new Queen(uint(nq));
  ui->tabSolutions->setSolutions(q);
  ui->chessBoard->setQueen(q);
  statusBar()->showMessage("");
}

void MainWindow::on_actiontransformations_triggered() { //  apply
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
            for (uint tv = 0; tv < q->nQueens; tv++) { // translations
              for (uint th = 0; th < q->nQueens; th++) {
                q->translateV();
                q->addSolutionMap(); // tV
              }
              q->translateH();
              q->addSolutionMap(); // tH
            }

            q->rotate90(); // R90
            q->addSolutionMap();
          }
          q->mirrorH(); // mH
          q->addSolutionMap();
        }
        q->mirrorV(); // mV
        q->addSolutionMap();
      }
    }
    q->updateSolutionsWithMap(); // unique & sorted
  } else {
    q->restoreSolution();
  }
  disp();
}

void MainWindow::on_actionabout_triggered() { // about
  QDialog *aboutDialog = new QDialog(this);
  Ui::Dialog ui;
  ui.setupUi(aboutDialog);

  aboutDialog->show();
}

void MainWindow::on_actioncopy_selected_solutions_triggered() { // copy
                                                                // selected
  auto sil = ui->tabSolutions->selectionModel()->selectedIndexes();
  QString res;

  for (auto s : sil)
    res += q->toString(ui->tabSolutions->model->getData(s)) + "\n";
  if (!res.isEmpty())
    QApplication::clipboard()->setText(res);
}

void MainWindow::on_actioncopy_all_solutions_triggered() { //  copy all
  QString res;
  for (auto s : q->solutions)
    res += q->toString(s) + "\n";
  if (!res.isEmpty())
    QApplication::clipboard()->setText(res);
}

void MainWindow::on_actioncopy_image_triggered() { // copy image
  QApplication::clipboard()->setImage(ui->chessBoard->paintImage());
}

// Queens 14, lap:7.7 sec., solutions:365596, evaluated:1623852370 of
// 11112006825558016 = 0.00001%
void MainWindow::on_actioncopy_result_message_triggered() {
  QApplication::clipboard()->setText(resultMessage());
}
