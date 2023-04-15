#include "widget.h"



ViewDataWidget::ViewDataWidget( QWidget* parent )
:QWidget( parent )
{
     QGridLayout* mainLyt = new QGridLayout( this );

     plot_ = new QCustomPlot( this );
     plot_->setInteractions( QCP::iRangeDrag | QCP::iRangeZoom );
     plot_->xAxis->setLabel( tr( "Время" ) );
     plot_->yAxis->setLabel( tr( "Значение" ) );
     plot_->setVisible( false );


     mainLyt->setColumnStretch( 0, 10 );
     mainLyt->setRowStretch( 1, 10 );
     mainLyt->addWidget( plot_, 0, 0, 3, 1 );


     QGroupBox* timeBox = new QGroupBox( tr( "Временные интервалы" ), this );
     QGridLayout* timeLyt = new QGridLayout( timeBox );

     QLabel* startTimeLabel = new QLabel( tr( "Начало промежутка:" ), timeBox );
     timeLyt->addWidget( startTimeLabel, 0, 0 );

     startTimeEdit_ = new QDateTimeEdit( timeBox );
     startTimeEdit_->setDateTime( QDateTime::currentDateTime() );
     startTimeEdit_->setCalendarPopup( true );
     startTimeEdit_->setDisplayFormat( "dd.MM.yyyy hh:mm:ss" );
     timeLyt->addWidget( startTimeEdit_, 0, 1 );

     QLabel* endTimeLabel = new QLabel( tr( "Конец промежутка:" ), timeBox );
     timeLyt->addWidget( endTimeLabel, 1, 0 );

     endTimeEdit_ = new QDateTimeEdit( timeBox );
     endTimeEdit_->setDateTime( QDateTime::currentDateTime() );
     endTimeEdit_->setCalendarPopup( true );
     endTimeEdit_->setDisplayFormat( "dd.MM.yyyy hh:mm:ss" );
     timeLyt->addWidget( endTimeEdit_, 1, 1 );

     mainLyt->addWidget( timeBox, 0, 1 );

     QGroupBox* elementsBox = new QGroupBox( tr( "Датчики" ), this );
     QGridLayout* elementsLyt = new QGridLayout( elementsBox );
     elementsLyt->setRowStretch( 1, 10 );

     addElement_ = new QPushButton( tr( "Добавить датчик" ), elementsBox );
     connect( addElement_, &QPushButton::clicked, this, &ViewDataWidget::onAddElementBtn );
     elementsLyt->addWidget( addElement_, 0, 0 );

     elements_ = new QTreeWidget( elementsBox );
     elements_->setRootIsDecorated( false );
     elements_->setColumnCount( 2 );
     elements_->headerItem()->setText( 0, tr( "Название датчика" ) );
     elements_->headerItem()->setText( 1, tr( "Цвет графика" ) );
     elements_->setContextMenuPolicy( Qt::CustomContextMenu );
     connect( elements_, &QTreeWidget::customContextMenuRequested, this, &ViewDataWidget::onContextMenuRequested );
     elementsLyt->addWidget( elements_, 1, 0 );
     mainLyt->addWidget( elementsBox, 1, 1 );

     commitChanges_ = new QPushButton( "Применить настройки", this );
     connect( commitChanges_, &QPushButton::clicked, this, &ViewDataWidget::onCommitSettings );
     mainLyt->addWidget( commitChanges_, 2, 1 );
}


void ViewDataWidget::onAddElementBtn()
{
     dlg_ = new AddSensorDialog( this );
     emit sGetSensorRequest();
     if( !dlg_->exec() )
     {
          delete dlg_;
          dlg_ = nullptr;
          return;
     }
     for( int curIdx = 0; curIdx < elements_->topLevelItemCount(); curIdx++ )
     {
          if( elements_->model()->data( elements_->model()->index( curIdx, 0 ) ).toString() == dlg_->resName() )
          {
               QMessageBox::critical( this, tr( "Уже существует" ), tr( "Добавляемый элемент уже существует" ) );
               return;
          }
     }

     QTreeWidgetItem* newItem = new QTreeWidgetItem( elements_ );
     if( dlg_->resName().isEmpty() )
     {
          newItem->setText( 0, "ID клиента:" + QString::number( dlg_->resId() >> 32 ) +
                              ". ID датчика: " + QString::number( dlg_->resId() & 0xffffffff ) );
     }
     else
     {
          newItem->setText( 0, dlg_->resName() );
     }
     newItem->setData( 0, Qt::UserRole + 1, dlg_->resId() );
     newItem->setData( 0, Qt::UserRole + 2, dlg_->resColor() );
     QPixmap tmp( 30, 30 );
     tmp.fill( dlg_->resColor() );
     newItem->setIcon( 1, QIcon( tmp ) );
     delete dlg_;
     dlg_ = nullptr;
}


void ViewDataWidget::onGetSensorResponse( const QList<SensorParam>& sensors )
{
     if( !dlg_ )
     {
          return;
     }
     dlg_->onSetSensorList( sensors );
}


void ViewDataWidget::onContextMenuRequested( const QPoint& pos )
{
     QMenu menu( elements_ );
     QAction* act;
     if( elements_->selectedItems().count() > 1 )
     {
          act = menu.addAction( tr( "Удалить датчики" ) );
     }
     else
     {
          act = menu.addAction( tr( "Удалить датчик" ) );
     }
     if( elements_->selectedItems().isEmpty() )
     {
          act->setEnabled( false );
     }
     connect( act, &QAction::triggered, this, &ViewDataWidget::onDeleteItems );
     menu.exec( elements_->viewport()->mapToGlobal( pos ) );
}

void ViewDataWidget::onDeleteItems()
{
     auto selected = elements_->selectedItems();
     QStringList selectedStrings;
     foreach( const auto& sel, selected )
     {
          for( int curIdx = 0; curIdx < elements_->topLevelItemCount(); curIdx++ )
          {
               if( elements_->model()->data( elements_->model()->index( curIdx, 0 ) ).toString() == sel->text( 0 ) )
               {
                    elements_->takeTopLevelItem( curIdx );
               }
          }
     }
}


void ViewDataWidget::onCommitSettings()
{
     QList< qint64 > data;
     data.push_back( startTimeEdit_->dateTime().toSecsSinceEpoch() );
     data.push_back( endTimeEdit_->dateTime().toSecsSinceEpoch() );
     for( int curIdx = 0; curIdx < elements_->topLevelItemCount(); curIdx++ )
     {
          data.push_back( elements_->model()->data( elements_->model()->index( curIdx, 0 ), Qt::UserRole + 1 ).toLongLong() );
     }
     emit sGetData( data );
}


void ViewDataWidget::onGetDataResponse( const QList<DataFormat>& dataRes )
{
     plot_->clearGraphs();
     quint64 startTime = std::numeric_limits<quint64>::max(), endTime = 0;
     double maxVal = -std::numeric_limits<double>::max(), minVal = std::numeric_limits<double>::max();
     bool inited = false;
     foreach( const DataFormat& curData, dataRes )
     {
          QVector< double > data;
          QVector< double > time;
          for( quint32 idx = 0; idx < curData.values.count(); idx++ )
          {
               inited = true;
               data.push_back( curData.values.at( idx ).toDouble() );
               time.push_back( curData.timeWithMsec.at( idx ) );
               if( time.last() > endTime )
               {
                    endTime = time.last();
               }
               if( time.last() < startTime )
               {
                    startTime = time.last();
               }
               if( data.last() > maxVal )
               {
                    maxVal = data.last();
               }
               if( data.last() < minVal )
               {
                    minVal = data.last();
               }
          }
          plot_->addGraph();
          plot_->graph( plot_->graphCount() - 1)->setData( time, data );
          QPen brush =  plot_->graph( plot_->graphCount() - 1)->pen();
          brush.setColor( getColor( curData.id ) );
          plot_->graph( plot_->graphCount() - 1)->setScatterStyle( QCPScatterStyle( QCPScatterStyle::ssCircle, getColor( curData.id ), Qt::white, 5 ) );
          plot_->graph( plot_->graphCount() - 1)->setPen( brush );
     }
     QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
     dateTicker->setDateTimeFormat("hh:mm:ss:zzz\ndd:MM:yyyy");
     plot_->xAxis->setRange( startTime, endTime );
     plot_->xAxis->setTicker(dateTicker);
     plot_->replot();
     plot_->setVisible( inited );
}


QColor ViewDataWidget::getColor( quint64 id )
{
     for( int curIdx = 0; curIdx < elements_->model()->rowCount(); curIdx++ )
     {
          if( elements_->model()->data( elements_->model()->index( curIdx, 0 ), Qt::UserRole + 1 ).toULongLong() == id )
          {
               elements_->topLevelItem( curIdx )->icon( 1 );
               qDebug() << elements_->model()->data( elements_->model()->index( curIdx, 0 ), Qt::UserRole + 2 ).value< QColor >();
               return elements_->model()->data( elements_->model()->index( curIdx, 0 ), Qt::UserRole + 2 ).value< QColor >();
          }
     }
     return {};
}

