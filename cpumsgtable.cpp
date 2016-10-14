#include "cpumsgtable.h"

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QDesktopWidget>
#include <QtCore/qtextcodec.h>

#include <QDateTime>
#include <QMessageBox>

#include "netdatamodel.h"


CpuMsgTable::CpuMsgTable(QWidget *parent)
{
    setMinimumHeight(10);

    isFirstShow = true;
    m_grid = new Qtitan::Grid(parent);
}

CpuMsgTable::~CpuMsgTable()
{
    delete model;
    model = NULL;
    delete m_grid;
    m_grid = NULL;
}

//
void CpuMsgTable::startShow()
{
    //
    Qtitan::GridTableView* view = m_grid->view<Qtitan::GridTableView>();
    view->setModel(model);
}

//显示信息
void CpuMsgTable::showMessage()
{
    if (isFirstShow)
    {
        isFirstShow = false;
        //
        Grid::loadTranslation();
    }
    else
    {
        //delete m_grid;
        delete model;
    }

    //
    model = new CpuDataMsg(m_grid);

    //配置表格显示
    m_grid->setViewType(Qtitan::Grid::TableView);
    Qtitan::GridTableView* view = m_grid->view<Qtitan::GridTableView>();
    view->options().setGridLines(Qtitan::LinesNone);
    view->options().setColumnAutoWidth(true);//使列完全填充并平分
    view->options().setGroupsHeader(false);//隐藏上方的分组栏
    view->options().setShowFocusDecoration(true);
    //view->options().setDropEnabled(true);

    //鼠标右键菜单
    connect(view, SIGNAL(contextMenu(ContextMenuEventArgs*)), this, SLOT(contextMenu(ContextMenuEventArgs* )));

    view->setModel(model);

    //设置表格信息
    //CPU
    Qtitan::GridTableColumn* column = (Qtitan::GridTableColumn *)view->getColumn(0);
    column->setEditorType(Qtitan::StringEditorType);
    column->editorRepository()->setValidateOnEnter(false);
    column->editorRepository()->setEditable(false);//不可编辑
    column->setTextAlignment(Qt::AlignHCenter);

    //代数
    column = (Qtitan::GridTableColumn *)view->getColumn(1);
    column->setEditorType(Qtitan::NumericEditorType);
    column->editorRepository()->setValidateOnEnter(false);
    column->editorRepository()->setEditable(false);//不可编辑
    column->setTextAlignment(Qt::AlignHCenter);

    //启动时间
    column = (Qtitan::GridTableColumn *)view->getColumn(2);
    column->setEditorType(Qtitan::DateTimeEditorType);
    column->editorRepository()->setValidateOnEnter(false);
    column->editorRepository()->setEditable(false);//不可编辑
    column->setTextAlignment(Qt::AlignHCenter);

    //耗时
    column = (Qtitan::GridTableColumn *)view->getColumn(3);
    column->setEditorType(Qtitan::StringEditorType);
    column->editorRepository()->setValidateOnEnter(false);
    column->editorRepository()->setEditable(false);
    column->setTextAlignment(Qt::AlignHCenter);

    //Show button menu for all column headers.
    //不知道有什么用？？？
    //for (int i = 0; i < view->getColumnCount(); ++i)
    //{
        //<GridTableColumn *>(view->getColumn(i))->setMenuButtonVisible(true);
    //}
}


//鼠标右键菜单
void CpuMsgTable::contextMenu(ContextMenuEventArgs* args)
{
    //args->contextMenu()->setVisible(false);
    //args->contextMenu()->addAction("Print Preview", this, SLOT(printPreview()));
    //args->contextMenu()->addSeparator();
    //args->contextMenu()->addAction("Developer Machines on the Web", this, SLOT(showCompanyWebSite()));
}

//增加一条VIPNET信息显示
void CpuMsgTable::addOneVipMsg(int num)
{
    model->addMsg(num,num);
}

//一条正在运行的VIPNET信息改变
void CpuMsgTable::changeOneVipMsg(int num)
{
    model->msgChanged(num,num);
}

//删除一条VIPNET信息
void CpuMsgTable::deleteOneVipMsg(int num)
{
    model->deleteMsg(num,num);
}

void CpuMsgTable::endAddOneVipMsg()
{
    model->endInsert();

    //滚动到最下方
    Qtitan::GridTableView* view = m_grid->view<Qtitan::GridTableView>();
    view->scrollToRow(view->getRow(view->getRowCount() - 1));
}

void CpuMsgTable::endDeleteOneVipMsg()
{
    model->endRemove();

    //这里为了解决表为空时还会显示4行的BUG！！！
    if (NetDataModel::getInstance()->getAllVipNetNum() == 0)
    {
        showMessage();
    }
}

/*
 *
 * LargeDataSetModel
 *
 */
CpuDataMsg::CpuDataMsg(QObject *parent)
: QAbstractItemModel(parent)
{

}

CpuDataMsg::~CpuDataMsg()
{
}

//设置表格表头信息
QVariant CpuDataMsg::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(orientation);
    Q_UNUSED(role);
    switch (section)
    {
    case 0:
        {
            return QString("CPU");
        }
        break;
    case 1:
        {
            return QString("代数");
        }
        break;
    case 2:
        {
            return QString("启动时间");
        }
        break;
    case 3:
        {
            return QString("耗时");
        }
        break;
    }
    return QVariant();
}

QModelIndex CpuDataMsg::parent(const QModelIndex & /*child*/) const
{
    return QModelIndex();
}

bool CpuDataMsg::hasChildren(const QModelIndex &parent) const
{
    if (parent.model() == this || !parent.isValid())
    {
        return rowCount(parent) > 0 && columnCount(parent) > 0;
    }
    return false;
}

int CpuDataMsg::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return NetDataModel::getInstance()->getAllVipNetNum();
}

int CpuDataMsg::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return 4;
}

QModelIndex CpuDataMsg::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid())
        return QModelIndex();

    if (row < 0 || row >= rowCount(parent))
        return QModelIndex();

    if (column < 0 || column >= columnCount(parent))
        return QModelIndex();

    return createIndex(row, column, (void*)NULL);
}

QVariant CpuDataMsg::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() < 0 || index.row() >= rowCount(index.parent()))
        return QVariant();

    if (index.column() < 0 || index.column() >= columnCount(index.parent()))
        return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        switch (index.column())
        {
        //CPU
        case 0:
            //先显示历史，再显示运行中
            if (index.row() < NetDataModel::getInstance()->getPastVipNetNum())
                return NetDataModel::getInstance()->PastVipMsg.at(index.row()).VPname;
            else
                return NetDataModel::getInstance()->VipMsg.at(index.row() - NetDataModel::getInstance()->getPastVipNetNum()).VPname;
            break;
        //代数
        case 1:
            //先显示历史，再显示运行中
            if (index.row() < NetDataModel::getInstance()->getPastVipNetNum())
                return NetDataModel::getInstance()->PastVipMsg.at(index.row()).calculateTime;
            else
                return NetDataModel::getInstance()->VipMsg.at(index.row() - NetDataModel::getInstance()->getPastVipNetNum()).calculateTime;
            break;
        //启动时间
        case 2:
            //先显示历史，再显示运行中
            if (index.row() < NetDataModel::getInstance()->getPastVipNetNum())
                return convertToTime(NetDataModel::getInstance()->PastVipMsg.at(index.row()).startTime);
            else
                return convertToTime(NetDataModel::getInstance()->VipMsg.at(index.row() - NetDataModel::getInstance()->getPastVipNetNum()).startTime);
            break;
        //耗时
        case 3:
            //先显示历史，再显示运行中
            if (index.row() < NetDataModel::getInstance()->getPastVipNetNum())
            {
                if (NetDataModel::getInstance()->PastVipMsg.at(index.row()).time > 0)
                    return tr("%1秒").arg(NetDataModel::getInstance()->PastVipMsg.at(index.row()).time);
                else
                    return tr(" ");
            }
            else
            {
                if (NetDataModel::getInstance()->VipMsg.at(index.row() - NetDataModel::getInstance()->getPastVipNetNum()).time > 0)
                    return tr("%1秒").arg(NetDataModel::getInstance()->VipMsg.at(index.row() - NetDataModel::getInstance()->getPastVipNetNum()).time);
                else
                    return tr(" ");
            }
            break;
        }
    }
    else if (role == Qt::CheckStateRole)
    {
        switch (index.column())
        {
        case 0:
            {
                //先显示历史，再显示运行中
                if (index.row() < NetDataModel::getInstance()->getPastVipNetNum())
                    return NetDataModel::getInstance()->PastVipMsg.at(index.row()).VPname;
                else
                    return NetDataModel::getInstance()->VipMsg.at(index.row() - NetDataModel::getInstance()->getPastVipNetNum()).VPname;
            }
            break;
        }
    }
    return QVariant();
}


Qt::ItemFlags CpuDataMsg::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemFlags();
    return Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

//
QString CpuDataMsg::convertToTime(long long time) const
{
    QDateTime myTime;
    myTime.setTime_t(time);
    return myTime.toString("yyyy-MM-dd hh:mm:ss");
}

//VIPNET信息发生改变
void CpuDataMsg::msgChanged(int firstRow,int lastRow)
{
    emit dataChanged(this->index(firstRow,0),this->index(lastRow,4));
}

//增加一条VIPNET信息
void CpuDataMsg::addMsg(int num,int row)
{
    beginInsertRows(QModelIndex(), num, row);
}

//删除一条VIPNET信息
void CpuDataMsg::deleteMsg(int num,int row)
{

    beginRemoveRows(QModelIndex(), num, row);
}

//
void CpuDataMsg::endInsert()
{
    endInsertRows();
}

void CpuDataMsg::endRemove()
{
    endRemoveRows();
}
