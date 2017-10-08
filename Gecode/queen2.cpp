/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */
/*
*  Main authors:
*     Christian Schulte <schulte@gecode.org>
*
*  Copyright:
*     Christian Schulte, 2001
*
*  Last modified:
*     $Date: 2016-04-19 17:19:45 +0200 (Tue, 19 Apr 2016) $ by $Author: schulte $
*     $Revision: 14967 $
*
*  This file is part of Gecode, the generic constraint
*  development environment:
*     http://www.gecode.org
*
*  Permission is hereby granted, free of charge, to any person obtaining
*  a copy of this software and associated documentation files (the
*  "Software"), to deal in the Software without restriction, including
*  without limitation the rights to use, copy, modify, merge, publish,
*  distribute, sublicense, and/or sell copies of the Software, and to
*  permit persons to whom the Software is furnished to do so, subject to
*  the following conditions:
*
*  The above copyright notice and this permission notice shall be
*  included in all copies or substantial portions of the Software.
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
*  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
*  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
*  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
*  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
*/

#include <gecode/driver.hh>
#include <gecode/int.hh>
#include <gecode/minimodel.hh>

#if defined(GECODE_HAS_QT) && defined(GECODE_HAS_GIST)
#include <QtGui>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#endif
#endif

using namespace Gecode;

class Queens : public Script {
public:
	IntVarArray q;
	IntVarArray q2;
	enum {
		PROP_BINARY,
		PROP_MIXED,
		PROP_DISTINCT

	};
	Queens(const SizeOptions& opt)
		: Script(opt), q(*this, opt.size(), 0, opt.size() - 1), q2(*this, opt.size()*opt.size(), 0, 1) {
		const int n = q.size();
		Matrix<IntVarArray> qm(q2, n, n);
		switch (opt.propagation()) {
		case PROP_BINARY:
			     for (int i = 0; i<n; i++) {
					 rel(*this, sum(qm.col(i)) == 1);
				
			}
			break;

		}
		branch(*this, q, INT_VAR_NONE(), INT_VAL_MIN());
	}

	Queens(bool share, Queens& s) : Script(share, s) {
		q.update(*this, share, s.q);

	}

	virtual Space *
		copy(bool share) {
		return new Queens(share, *this);

	}

	virtual void
		print(std::ostream& os) const {
		os << "queens\t";
		for (int i = 0; i < q2.size(); i++) {
			os << q2[i] << ", ";
			if ((i + 1) % 10 == 0)
				os << std::endl << "\t";

		}
		os << std::endl;

	}

};

#if defined(GECODE_HAS_QT) && defined(GECODE_HAS_GIST)

class QueensInspector : public Gist::Inspector {
protected:
	QGraphicsScene* scene;
	QMainWindow* mw;
	static const int unit = 20;
public:
	QueensInspector(void) : scene(NULL), mw(NULL) {}
	virtual void inspect(const Space& s) {
		const Queens& q = static_cast<const Queens&>(s);

		if (!scene)
			initialize();
		QList <QGraphicsItem*> itemList = scene->items();
		foreach(QGraphicsItem* i, scene->items()) {
			scene->removeItem(i);
			delete i;

		}

		for (int i = 0; i<q.q.size(); i++) {
			for (int j = 0; j<q.q.size(); j++) {
				scene->addRect(i*unit, j*unit, unit, unit);

			}
			QBrush b(q.q[i].assigned() ? Qt::black : Qt::red);
			QPen p(q.q[i].assigned() ? Qt::black : Qt::white);
			for (IntVarValues xv(q.q[i]); xv(); ++xv) {
				scene->addEllipse(QRectF(i*unit + unit / 4, xv.val()*unit + unit / 4,
					unit / 2, unit / 2), p, b);

			}

		}
		mw->show();

	}

	void initialize(void) {
		mw = new QMainWindow();
		scene = new QGraphicsScene();
		QGraphicsView* view = new QGraphicsView(scene);
		view->setRenderHints(QPainter::Antialiasing);
		mw->setCentralWidget(view);
		mw->setAttribute(Qt::WA_QuitOnClose, false);
		mw->setAttribute(Qt::WA_DeleteOnClose, false);
		QAction* closeWindow = new QAction("Close window", mw);
		closeWindow->setShortcut(QKeySequence("Ctrl+W"));
		mw->connect(closeWindow, SIGNAL(triggered()),
			mw, SLOT(close()));
		mw->addAction(closeWindow);

	}

	virtual std::string name(void) { return "Board"; }
	virtual void finalize(void) {
		delete mw;
		mw = NULL;

	}

};

#endif /* GECODE_HAS_GIST */

int main(int argc, char* argv[]) {
	SizeOptions opt("Queens");
	opt.iterations(500);
	opt.size(100);
	opt.propagation(Queens::PROP_BINARY);
	opt.propagation(Queens::PROP_BINARY, "binary",
		"only binary disequality constraints");

#if defined(GECODE_HAS_QT) && defined(GECODE_HAS_GIST)
	QueensInspector ki;
	opt.inspect.click(&ki);
#endif

	opt.parse(argc, argv);
	Script::run<Queens, DFS, SizeOptions>(opt);
	return 0;

}

// STATISTICS: example-any
