#include "add_sensor_dialog.h"
#include "add_sensor_model.h"

AddSensorDialog::AddSensorDialog( QWidget* parent )
:QDialog( parent )
{
     resize( 800, 400 );
     setWindowTitle( tr( "Добавить датчик в график" ) );
     QGridLayout* mainLyt = new QGridLayout( this );
     mainLyt->setRowStretch( 0, 1 );
     mainLyt->setColumnStretch( 1, 10 );

     list_ = new QTreeView( this );
     model_ = new AddSensorModel;
     QSortFilterProxyModel* sortModel = new QSortFilterProxyModel( this );
     sortModel->setSourceModel( model_ );
     list_->setModel( sortModel );
     list_->setSortingEnabled( true );
     list_->setRootIsDecorated( false );
     list_->setSelectionMode( QAbstractItemView::SingleSelection );
     mainLyt->addWidget( list_, 0, 0, 1, 4 );

     selectColor_ = new QPushButton( tr( "Выбрать цвет" ), this );
     connect( selectColor_, &QPushButton::clicked, this, &AddSensorDialog::onSelectColorBtn );
     mainLyt->addWidget( selectColor_, 1, 0 );

     okBtn_ = new QPushButton( tr( "OK" ), this );
     connect( okBtn_, &QPushButton::clicked, this, &AddSensorDialog::onOkBtn );
     mainLyt->addWidget( okBtn_, 1, 2 );

     cancelBtn_ = new QPushButton( tr( "Отмена" ), this );
     connect( cancelBtn_, &QPushButton::clicked, this, &AddSensorDialog::reject );
     mainLyt->addWidget( cancelBtn_, 1, 3 );
}


void AddSensorDialog::onOkBtn()
{
     if( list_->selectionModel()->selectedRows().isEmpty() )
     {
          QMessageBox::critical( this, tr( "Не выбран датчик" ), tr( "Датчик не выбран. Пожалуйста, выберите датчик") );
          return;
     }
     if( !colorSelected )
     {
          QMessageBox::critical( this, tr( "Не выбран цвет график" ), tr( "Цвет графика не выбран. Пожалуйста, выберите цвет графика") );
          return;
     }
     resName_ = list_->selectionModel()->selectedRows().first().data( Qt::DisplayRole ).toString();
     resId_ = list_->selectionModel()->selectedRows().first().data( Qt::UserRole + 1 ).toLongLong();
     accept();
}


void AddSensorDialog::onSelectColorBtn()
{
     QColorDialog dlg( this );
     if( !dlg.exec() )
     {
          return;
     }
     colorSelected = true;
     resColor_ = dlg.selectedColor();
}


void AddSensorDialog::onSetSensorList( const QList<SensorParam>& sensors )
{
     model_->onSetSensorList( sensors );
}


QString AddSensorDialog::resName()
{
     return resName_;
}


quint64 AddSensorDialog::resId()
{
     return resId_;
}


QColor AddSensorDialog::resColor()
{
     return resColor_;
}
