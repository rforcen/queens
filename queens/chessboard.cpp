#include "chessboard.h"

ChessBoard::ChessBoard(QWidget *p) : QOpenGLWidget(p) {
  imgQ = new QImage(":/VSlibQt/rsc/icons/discRed.png");
}

void ChessBoard::paint(QPainter &p) {
  uint x0 = 0, y0 = 0;
  uint nq = q->nQueens, szb;

  w = p.device()->width(), h = p.device()->height();
  p.fillRect(0, 0, w, h, Qt::white);

  nw = w / nq;
  nh = h / nq;

  if (w > h) {
    sz = nh;
    x0 = (w - h) / 2;
    szb = h;
    y0 = 0;
  } else {
    sz = nw;
    szb = w;
    x0 = 0;
    y0 = (h - w) / 2;
  }

  QImage imgqs = imgQ->scaled(sz, sz); // scale icon

  ff = true;
  for (int i = 0; i < nq; i++) {
    for (int j = 0; j < nq; j++) {
      int xp, yp;

      auto pnt = QPoint(xp = x0 + i * sz, yp = y0 + (nq - j - 1) * sz);

      if (ff)
        p.fillRect(xp, yp, sz, sz, QColor(QRgb(0xdddddd)));
      ff = !ff;

      if (q->queens[i] == j) {
        p.drawImage(pnt, imgqs);
      }
    }
    if ((nq & 1) == 0)
      ff = !ff;
  }

  p.setPen(Qt::black);
  p.drawRect(x0, y0, sz * nq - 1, sz * nq - 1);
}

QImage &ChessBoard::paintImage() {
  if (imgClip)
    delete imgClip;

  imgClip = new QImage(imgSize, imgSize, QImage::Format_RGB32);
  imgClip->fill(Qt::white);

  QPainter p;
  p.begin(imgClip);

  paint(p);

  p.end();
  return *imgClip;
}

void ChessBoard::paintEvent(QPaintEvent *) {
  if (!q)
    return;

  QPainter p(this);
  paint(p);
}
