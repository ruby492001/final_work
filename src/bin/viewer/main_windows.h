#pragma once


#include <QtWidgets>
#include "net_cfg_dialog.h"
#include "view_connection_client.h"
#include "viewplg_itf.h"

enum ListWidgetRole
{
     ViewPluginWindowRole = Qt::UserRole + 1,
     ViewPluginIdx
};


class MainWindow: public QWidget
{
     Q_OBJECT
public:
     MainWindow();

private slots:
     void onConnectionLost();
     void onConnected();

     void onSelectionChanged( QTreeWidgetItem* current, QTreeWidgetItem* previous );
     void onIconChanged();

private:
     void loadPlugins();

private:
     ViewConnectionClient* client_;
     bool connected_ = false;

     QList< ViewPluginItf* > plugins_;
     QTreeWidget* pluginSelector_;
     QStackedWidget* widgets_;
};