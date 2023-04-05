#include "widget.h"
#include "edit_sensor.h"


EditSensorConfigWidget::EditSensorConfigWidget( QWidget* parent )
:QWidget( parent )
{
     QGridLayout* lyt = new QGridLayout( this );
     lyt->setColumnStretch( 0, 10 );

     rollbackBtn_ = new QPushButton( this );
     rollbackBtn_->setIcon( QIcon( ":/images/rollback.png" ) );
     rollbackBtn_->setIconSize( QSize( 16, 16 ) );
     rollbackBtn_->setToolTip( tr( "Откатить изменения" ) );
     connect( rollbackBtn_, &QPushButton::clicked, this, &EditSensorConfigWidget::onRollback );
     lyt->addWidget( rollbackBtn_, 0, 1, Qt::AlignLeft );

     updateBtn_ = new QPushButton( this );
     updateBtn_->setIcon( QIcon( ":/images/save.png" ) );
     updateBtn_->setIconSize( QSize( 16, 16 ) );
     updateBtn_->setToolTip( tr( "Применить изменения" ) );
     updateBtn_->setEnabled( false );
     connect( updateBtn_, &QPushButton::clicked, this, &EditSensorConfigWidget::onCommitChanges );
     lyt->addWidget( updateBtn_, 0, 2, Qt::AlignLeft );

     model_ = new EditSensorModel();
     connect( model_, &EditSensorModel::sDataChanged, this, &EditSensorConfigWidget::onNewChanges );
     treeView_ = new QTreeView( this );
     QSortFilterProxyModel* sortModel = new QSortFilterProxyModel( this );
     sortModel->setSourceModel( model_ );
     treeView_->setModel( sortModel );
     treeView_->setRootIsDecorated( false );
     treeView_->setSortingEnabled( true );
     lyt->setRowStretch( 1, 10 );
     lyt->addWidget( treeView_, 1, 0, 1, 3);
}


void EditSensorConfigWidget::onUpdateTable( const QList<SensorParam>& elements )
{
     model_->onSetSensorList( elements );
}


void EditSensorConfigWidget::onNewChanges( const SensorParam& element )
{
     updateBtn_->setEnabled( true );
     localChanges_.push_back( element );
}


void EditSensorConfigWidget::onCommitChanges()
{
     updateBtn_->setEnabled( false );
     emit sSendChanges( localChanges_ );
     localChanges_.clear();
}


void EditSensorConfigWidget::onRollback()
{
     updateBtn_->setEnabled( false );
     localChanges_.clear();
     emit sUpdateTableRequest();
}
