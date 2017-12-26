#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <queen.h>
#include <QOpenGLWidget>
#include <QPainter>

class ChessBoard : public QOpenGLWidget {
 public:
  ChessBoard(QWidget *p);
  ~ChessBoard() {
    if (imgClip) delete imgClip;
    if (imgQ) delete imgQ;
  }

  void setQueen(Queen *q) {
    this->q = q;
    repaint();
  }
  void refresh() { repaint(); }
  void paint(QPainter &p);
  QImage &paintImage();

  Queen *q = 0;

  float w, h, nw, nh, sz, chipX = 0, chipY = 0;
  QImage *imgQ = 0, *imgClip = 0;

  bool ft = true, ff;
  int imgSize = 2048;

 protected:
  void paintEvent(QPaintEvent *);
};

#endif  // CHESSBOARD_H
