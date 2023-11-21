#include "qmlview.h"

#include "core/qmldocument.h"
#include "guisettings.h"
#include <QFile>
#include <QLabel>
#include <QPlainTextEdit>
#include <QVBoxLayout>

#include <QAction>
#include <QQuickView>
#include <QToolBar>
namespace Gui {

QmlView::QmlView(QWidget *parent)
    : TextView(parent)
{
    auto *action = new QAction(tr("Run"));
    GuiSettings::setIcon(action, ":/gui/eye.png");
    connect(action, &QAction::triggered, this, &QmlView::runQml);
    addAction(action);
}

QmlView::~QmlView() = default;

void QmlView::runQml()
{
    if (!document()) {
        return;
    }

    const QString qmlFilePath = document()->fileName();

    auto *qmlView = new QQuickView();
    qmlView->setSource(QUrl::fromLocalFile(qmlFilePath));
    qmlView->show();
}

} // namespace Gui
