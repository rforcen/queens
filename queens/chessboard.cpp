#include "chessboard.h"

ChessBoard::ChessBoard(QWidget *p) : QOpenGLWidget(p) {}

void ChessBoard::paint(QPainter &p) {
  uint x0 = 0, y0 = 0;
  uint nq = q->nQueens, szb;

  w = p.device()->width();
  h = p.device()->height();
  p.fillRect(0, 0, int(w), int(h), Qt::white);

  nw = w / nq;
  nh = h / nq;

  if (w > h) {
    sz = nh;
    x0 = uint((w - h) / 2);
    szb = uint(h);
    y0 = 0;
  } else {
    sz = nw;
    szb = uint(w);
    x0 = 0;
    y0 = uint((h - w) / 2);
  }

  ff = true;
  for (uint i = 0; i < nq; i++) {
    for (uint j = 0; j < nq; j++) {
      int xp, yp;

      auto pnt =
          QPoint(xp = int(x0 + i * sz), yp = int(y0 + (nq - j - 1) * sz));

      if (ff) p.fillRect(xp, yp, int(sz), int(sz), QColor(QRgb(0xdddddd)));
      ff = !ff;

      if (q->queens[i] == j) {
        p.save();
        {
          auto sz2 = int(sz / 2), sz21 = int(sz / 2.2f), sz22 = int(sz / 4);
          p.setBrush(Qt::red);
          p.drawEllipse(pnt + QPoint(sz2, sz2), sz21, sz21);
          p.setBrush(Qt::darkYellow);
          p.drawEllipse(pnt + QPoint(sz2, sz2), sz22, sz22);
        }
        p.restore();
      }
    }
    if ((nq & 1) == 0) ff = !ff;
  }

  p.setPen(Qt::black);
  p.drawRect(int(x0), int(y0), int(sz * nq - 1), int(sz * nq - 1));
}

QImage &ChessBoard::paintImage() {
  if (imgClip) delete imgClip;

  imgClip = new QImage(imgSize, imgSize, QImage::Format_RGB32);
  imgClip->fill(Qt::white);

  QPainter p;
  p.begin(imgClip);

  paint(p);

  p.end();
  return *imgClip;
}

void ChessBoard::paintEvent(QPaintEvent *) {
  if (!q) return;

  QPainter p(this);
  paint(p);
}
