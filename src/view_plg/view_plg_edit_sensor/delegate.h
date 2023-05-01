#pragma once

#include <QtCore>
#include <QtWidgets>

extern QStringList places;
extern QStringList types;
extern QStringList measure;

class Delegate: public QStyledItemDelegate
{
     Q_OBJECT
public:
     Delegate( QObject* parent ):QStyledItemDelegate( parent ){};
     QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
     void setEditorData(QWidget* editor, const QModelIndex& index) const override;

private slots:
     void commitAndCloseEditor();
};
