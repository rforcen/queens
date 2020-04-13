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
  }

  void setQueen(Queen *q) {
    this->q = q;
    repaint();
  }
  void refresh() { repaint(); }
  void paint(QPainter &p);
  QImage &paintImage();

  Queen *q = nullptr;

  float w, h, nw, nh, sz, chipX = 0, chipY = 0;
  QImage *imgClip = nullptr;

  bool ft = true, ff;
  int imgSize = 2048;

 protected:
  void paintEvent(QPaintEvent *);
};

#endif  // CHESSBOARD_H
