/*
  This file is part of Knut.

  SPDX-FileCopyrightText: 2024 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>

  SPDX-License-Identifier: GPL-3.0-only

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#pragma once

#include "scriptmanager.h"

#include <QAbstractTableModel>
#include <QString>

namespace Core {

class ScriptManager;

class ScriptModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Column : int {
        NameColumn = 0,
        DescriptionColumn,
        ColumnCount,
    };
    Q_ENUM(Column)

    enum Role : int {
        PathRole = Qt::UserRole,
    };
    Q_ENUM(Role)

public:
    int columnCount(const QModelIndex &parent) const override;
    int rowCount(const QModelIndex &parent) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    QVariant data(const QModelIndex &index, int role) const override;

private:
    friend ScriptManager;
    explicit ScriptModel(ScriptManager *parent);

    void onAboutToAddScript(const Core::ScriptManager::Script &, int);
    void onAboutToRemoveScript(const Core::ScriptManager::Script &, int);

    void onScriptAdded(const Core::ScriptManager::Script &);
    void onScriptRemoved(const Core::ScriptManager::Script &);

    QVariant displayData(const Core::ScriptManager::Script &script, int column) const;

    QVariant columnHeaderDisplayData(int column) const;
};

} // namespace Core
