#pragma once


#include <QtWidgets>


class NetCfgDialog: public QDialog
{
     Q_OBJECT
public:
     NetCfgDialog( const QString& serverAddr = QString(), quint16 port = 54321, QWidget* parent = nullptr );

     quint16 port();
     QString addr();

private slots:
     void onOk();

private:
     QLineEdit* addr_;
     QSpinBox* port_;
};