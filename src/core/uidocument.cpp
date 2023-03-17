#include "uidocument.h"

#include "logger.h"

#include <QFile>
#include <QUiLoader>
#include <QWidget>

#include <kdalgorithms.h>
#include <spdlog/spdlog.h>

namespace Core {

/*!
 * \qmltype UiDocument
 * \brief Provides access to the content of a Ui file (Qt designer file).
 * \inqmlmodule Script
 * \since 1.0
 */

/*!
 * \qmlproperty array<UiWidget> UiDocument::widgets
 * List of all widgets in the ui file.
 */

UiDocument::UiDocument(QObject *parent)
    : Document(Type::Ui, parent)
{
}

/*!
 * \qmlmethod UiWidget UiDocument::findWidget(string name)
 * Returns the widget for the given `name`.
 */
UiWidget *UiDocument::findWidget(const QString &name) const
{
    LOG("UiDocument::findWidget", name);

    auto result = kdalgorithms::find_if(m_widgets, [name](UiWidget *widget) {
        return widget->name() == name;
    });
    if (!result)
        return nullptr;
    return *result;
}

/*!
 * \qmlmethod UiDocument::preview()
 * Open a dilaog to preview the current ui file.
 */
void UiDocument::preview()
{
    LOG("UiDocument::preview");

    QUiLoader loader;

    QFile file(fileName());
    if (file.open(QIODevice::ReadOnly)) {
        QWidget *widget = loader.load(&file);
        widget->setAttribute(Qt::WA_DeleteOnClose);
        widget->show();
    }
}

bool UiDocument::doSave(const QString &fileName)
{
    return m_document.save_file(fileName.toLatin1().constData(), "    ");
}

bool UiDocument::doLoad(const QString &fileName)
{
    pugi::xml_parse_result result = m_document.load_file(fileName.toLatin1().constData(), pugi::parse_declaration);

    if (!result) {
        spdlog::critical("{}({}): {}", fileName.toStdString(), result.offset, result.description());
        return false;
    }

    auto widgets = m_document.select_nodes("//widget");

    bool isRoot = true;
    for (auto it = widgets.begin(); it != widgets.end(); ++it) {
        pugi::xpath_node node = *it;
        Q_ASSERT(!node.node().empty());
        m_widgets.push_back(new UiWidget(node.node(), isRoot, this));
        isRoot = false;
    }

    return true;
}

/*!
 * \qmltype UiWidget
 * \brief Provides access to widget attributes in the ui files.
 * \inqmlmodule Script
 * \since 1.0
 * \sa UiDocument
 */

/*!
 * \qmlproperty string UiWidget::name
 * Name of the widget.
 */
/*!
 * \qmlproperty string UiWidget::className
 * Name of the widget's class.
 */
/*!
 * \qmlproperty bool UiWidget::isRoot
 * Read-only property returning `true` if the widget is the root widget.
 */

UiWidget::UiWidget(pugi::xml_node widget, bool isRoot, QObject *parent)
    : QObject(parent)
    , m_widget(widget)
    , m_isRoot(isRoot)
{
}

QString UiWidget::name() const
{
    return QString::fromLatin1(m_widget.attribute("name").value());
}

void UiWidget::setName(const QString &newName)
{
    LOG("UiWidget::setName", newName);

    m_widget.attribute("name").set_value(newName.toLatin1().constData());
    if (m_isRoot) {
        // Also change the <class> tag
        auto node = m_widget.previous_sibling();
        node.text().set(newName.toLatin1().constData());
    }
    qobject_cast<UiDocument *>(parent())->setHasChanged(true);
}

QString UiWidget::className() const
{
    return QString::fromLatin1(m_widget.attribute("class").value());
}

void UiWidget::setClassName(const QString &newClassName)
{
    LOG("UiWidget::setClassName", newClassName);

    m_widget.attribute("class").set_value(newClassName.toLatin1().constData());
    qobject_cast<UiDocument *>(parent())->setHasChanged(true);
}

} // namespace Core
