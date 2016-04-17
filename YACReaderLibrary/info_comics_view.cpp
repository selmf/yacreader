#include "info_comics_view.h"

#include <QtQuick>

#include "comic_model.h"
#include "comic_db.h"

#include "QsLog.h"

InfoComicsView::InfoComicsView(QWidget *parent)
    :ComicsView(parent),_selectionModel(nullptr)
{
    qmlRegisterType<ComicModel>("com.yacreader.ComicModel",1,0,"ComicModel");
    qmlRegisterType<ComicDB>("com.yacreader.ComicDB",1,0,"ComicDB");
    qmlRegisterType<ComicInfo>("com.yacreader.ComicInfo",1,0,"ComicInfo");

    view = new QQuickView();
    container = QWidget::createWindowContainer(view, this);

    container->setFocusPolicy(Qt::StrongFocus);

    view->setSource(QUrl("qrc:/qml/InfoComicsView.qml"));


    QObject *rootObject = dynamic_cast<QObject*>(view->rootObject());
    flow = rootObject->findChild<QObject*>("flow");
    list = rootObject->findChild<QObject*>("list");

    connect(flow, SIGNAL(currentCoverChanged(int)), this, SLOT(updateInfoForIndex(int)));
    connect(flow, SIGNAL(currentCoverChanged(int)), this, SLOT(setCurrentIndex(int)));

    QVBoxLayout * l = new QVBoxLayout;
    l->addWidget(container);
    this->setLayout(l);

    setContentsMargins(0,0,0,0);
    l->setContentsMargins(0,0,0,0);
    l->setSpacing(0);

    setShowMarks(true);

    QLOG_TRACE() << "GridComicsView";
}

InfoComicsView::~InfoComicsView()
{
    delete view;
}

void InfoComicsView::setToolBar(QToolBar *toolBar)
{
    static_cast<QVBoxLayout *>(this->layout())->insertWidget(1,toolBar);
    this->toolbar = toolBar;
}

void InfoComicsView::setModel(ComicModel *model)
{
    if(model == NULL)
        return;

    if(_selectionModel != nullptr)
        delete _selectionModel;

    _selectionModel = new QItemSelectionModel(model);

    ComicsView::setModel(model);

    QQmlContext *ctxt = view->rootContext();

    /*if(_selectionModel != NULL)
        delete _selectionModel;

    _selectionModel = new QItemSelectionModel(model);*/

    ctxt->setContextProperty("comicsList", model);
    if(model->rowCount()>0)
        ctxt->setContextProperty("backgroundImage", this->model->data(this->model->index(0, 0), ComicModel::CoverPathRole));
    else
        ctxt->setContextProperty("backgroundImage", QUrl());

    ctxt->setContextProperty("comicsSelection", _selectionModel);
    /*ctxt->setContextProperty("contextMenuHelper",this);
    ctxt->setContextProperty("comicsSelectionHelper", this);
    ctxt->setContextProperty("comicRatingHelper", this);
    ctxt->setContextProperty("dummyValue", true);
    ctxt->setContextProperty("dragManager", this);
    ctxt->setContextProperty("dropManager", this);

    updateBackgroundConfig();*/

    if(model->rowCount()>0)
    {
        setCurrentIndex(model->index(0,0));
        updateInfoForIndex(0);
    }
}

void InfoComicsView::setCurrentIndex(const QModelIndex &index)
{
    QQmlProperty(list, "currentIndex").write(index.row());

    _selectionModel->clear();
    _selectionModel->select(index, QItemSelectionModel::Select | QItemSelectionModel::Rows);
}

void InfoComicsView::setCurrentIndex(int index)
{
    _selectionModel->clear();
    _selectionModel->select(model->index(index,0), QItemSelectionModel::Select | QItemSelectionModel::Rows);
}

QModelIndex InfoComicsView::currentIndex()
{
    if(!_selectionModel)
        return QModelIndex();

    QModelIndexList indexes = _selectionModel->selectedRows();
    if(indexes.length()>0)
        return indexes[0];

    this->selectIndex(0);
    indexes = _selectionModel->selectedRows();
    if(indexes.length()>0)
        return indexes[0];
    else
        return QModelIndex();
}

QItemSelectionModel *InfoComicsView::selectionModel()
{
    QModelIndexList indexes = _selectionModel->selectedRows();
     if(indexes.length()==0)
         this->selectIndex(0);

    return _selectionModel;
}

void InfoComicsView::scrollTo(const QModelIndex &mi, QAbstractItemView::ScrollHint hint)
{
    Q_UNUSED(mi);
    Q_UNUSED(hint);
}

void InfoComicsView::toFullScreen()
{
    toolbar->hide();
}

void InfoComicsView::toNormal()
{
    toolbar->show();
}

void InfoComicsView::updateConfig(QSettings *settings)
{
    Q_UNUSED(settings);
}

void InfoComicsView::enableFilterMode(bool enabled)
{
    Q_UNUSED(enabled);
}

void InfoComicsView::selectIndex(int index)
{
    if(_selectionModel != nullptr && model!=NULL)
    {
        _selectionModel->select(model->index(index,0),QItemSelectionModel::Select | QItemSelectionModel::Rows);
    }
}

void InfoComicsView::setShowMarks(bool show)
{
    QQmlContext *ctxt = view->rootContext();
    ctxt->setContextProperty("show_marks", show);
}

void InfoComicsView::selectAll()
{
    QModelIndex top = model->index(0, 0);
    QModelIndex bottom = model->index(model->rowCount()-1, 0);
    QItemSelection selection(top, bottom);
    _selectionModel->select(selection, QItemSelectionModel::Select | QItemSelectionModel::Rows);
}
