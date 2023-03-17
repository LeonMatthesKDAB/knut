#include "document.h"

#include "logger.h"

#include <QFile>
#include <QUrl>

#include <spdlog/spdlog.h>

namespace Core {

/*!
 * \qmltype Document
 * \brief Base class for all documents
 * \inqmlmodule Script
 * \since 1.0
 *
 * The `Document` class is the base class for all documents.
 * A document is a file loaded by Knut and that can be used in script (either to get data or to edit).
 */

/*!
 * \qmlproperty string Document::fileName
 * Filename of the current document, changing it will load a new file **without changing the type**. It's better to use
 * a new `Document` to open a new file.
 *
 * Changing the filename will:
 * - save the current document automatically
 * - load the new document
 * - put an error in `errorString` if it can't be loaded
 * \sa Document::load
 */
/*!
 * \qmlproperty bool Document::exists
 * Returns true if the document is a file on the disk, otherwise returns false.
 */
/*!
 * \qmlproperty Type Document::type
 * Returns the current type of the document, please note that the type is fixed once, and won't change. Available types
 * are:
 *
 * - `Document.Text`
 * - `Document.Rc`
 */
/*!
 * \qmlproperty string Document::errorString
 */
/*!
 * \qmlproperty bool Document::hasChanged
 * Returns true if the document has been edited, otherwise returns false.
 */

Document::Document(Type type, QObject *parent)
    : QObject(parent)
    , m_type(type)
{
}

const QString &Document::fileName() const
{
    return m_fileName;
}

void Document::setFileName(const QString &newFileName)
{
    LOG("Document::setFileName", newFileName);
    if (m_fileName == newFileName)
        return;
    load(newFileName);
}

bool Document::exists() const
{
    return !m_fileName.isEmpty() && QFile::exists(m_fileName);
}

Document::Type Document::type() const
{
    return m_type;
}

const QString &Document::errorString() const
{
    return m_errorString;
}

void Document::setErrorString(const QString &error)
{
    if (m_errorString == error)
        return;
    m_errorString = error;
    emit errorStringChanged();
}

bool Document::hasChanged() const
{
    return m_hasChanged;
}

/*!
 * \qmlmethod bool Document::load(string fileName)
 * Load the document `fileName` **without changing the type**. If the current document has some changes, save them
 * automatically. In case of error put the error text in the `errorString` property.
 */
bool Document::load(const QString &fileName)
{
    LOG("Document::load", fileName);
    if (fileName.isEmpty()) {
        spdlog::warn("Document::load - fileName is empty");
        return false;
    }
    if (m_fileName == fileName)
        return true;
    close();
    bool loadDone = doLoad(fileName);
    m_fileName = fileName;
    didOpen();
    emit fileNameChanged();
    return loadDone;
}

/*!
 * \qmlmethod bool Document::save()
 * Save the current document, in case of error put the error text in the `errorString` property.
 */
bool Document::save()
{
    LOG("Document::save");
    if (m_fileName.isEmpty()) {
        spdlog::error("Document::save - fileName is empty");
        return false;
    }
    bool saveDone = doSave(m_fileName);
    if (saveDone)
        setHasChanged(false);
    return saveDone;
}

/*!
 * \qmlmethod bool Document::saveAs(string fileName)
 * Save the current document as fileName, the previous file (if it exists) is not changed, and the current document
 * takes the new `fileName`. In case of error put the error text in the `errorString` property.
 */
bool Document::saveAs(const QString &fileName)
{
    LOG("Document::saveAs", fileName);
    if (fileName.isEmpty()) {
        spdlog::error("Document::saveAs - fileName is empty");
        return false;
    }
    bool isNewName = m_fileName != fileName;
    if (isNewName) {
        if (!m_fileName.isEmpty())
            didClose();
        m_fileName = fileName;
        emit fileNameChanged();
    }
    bool saveDone = doSave(m_fileName);
    if (saveDone) {
        setHasChanged(false);
        didOpen();
    }
    return saveDone;
}

/*!
 * \qmlmethod bool Document::close()
 * Close the current document. If the current document has some changes, save them
 * automatically.
 */
void Document::close()
{
    LOG("Document::close");
    if (m_fileName.isEmpty())
        return;
    if (m_hasChanged)
        save();
    didClose();
    m_fileName.clear();
}

void Document::setHasChanged(bool newHasChanged)
{
    if (m_hasChanged == newHasChanged)
        return;
    m_hasChanged = newHasChanged;
    emit hasChangedChanged();
}

} // namespace Core
