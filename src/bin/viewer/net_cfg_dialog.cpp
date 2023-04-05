#include "net_cfg_dialog.h"
#include <QtNetwork>

NetCfgDialog::NetCfgDialog( const QString& serverAddr, quint16 port, QWidget* parent )
:QDialog( parent )
{
     resize( 300, 100 );
     setWindowTitle( tr( "Введите адрес сервера и порт подключения" ) );
     QGridLayout* lyt = new QGridLayout( this );

     QLabel* addrLabel = new QLabel( this );
     addrLabel->setText( tr( "Адрес сервера:" ) );
     lyt->addWidget( addrLabel, 0, 0 );

     lyt->setColumnStretch( 1, 10 );
     addr_ = new QLineEdit( this );
     addr_->setText( serverAddr );
     lyt->addWidget( addr_, 0, 1 );

     QLabel* portLabel = new QLabel( this );
     portLabel->setText( tr( "Порт сервера:" ) );
     lyt->addWidget( portLabel, 1, 0 );

     port_ = new QSpinBox( this );
     port_->setRange( 1, 65535 );
     port_->setValue( port );
     lyt->addWidget( port_, 1, 1 );

     QDialogButtonBox* buttons = new QDialogButtonBox( this );
     buttons->setStandardButtons( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );
     buttons->button( QDialogButtonBox::Cancel )->setText( tr( "Отмена" ) );
     connect( buttons->button( QDialogButtonBox::Ok ), &QPushButton::clicked, this, &NetCfgDialog::onOk );
     connect( buttons->button( QDialogButtonBox::Cancel ), &QPushButton::clicked, this, &NetCfgDialog::reject );
     lyt->addWidget( buttons, 2, 0, 1, 2 );
}


quint16 NetCfgDialog::port()
{
     return port_->value();
}


QString NetCfgDialog::addr()
{
     return addr_->text();
}


void NetCfgDialog::onOk()
{
     if( addr_->text().isEmpty() )
     {
          QMessageBox::critical( this, tr( "Не указан адрес сервера" ),
                                 tr( "Работа программы невозможна без адреса сервера. Введите адрес сервера." ), QMessageBox::Ok );
          return;
     }
     if( !QHostAddress( addr_->text() ).isNull() )
     {
          QMessageBox::critical( this, tr( "Невалидный адрес сервера" ),
                                 tr( "Введён невалидный адрес сервера. Введите правильный адрес сервера." ), QMessageBox::Ok );
          return;
     }
     accept();
}
