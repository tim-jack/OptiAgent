#ifndef CPUMSGTABLE_H
#define CPUMSGTABLE_H

#include <QWidget>
#include <Qtitan/include/QtitanGrid.h>

using namespace std;

//
class CpuDataMsg : public QAbstractItemModel
{
    Q_OBJECT

public:
    CpuDataMsg(QObject *parent = 0);
    virtual ~CpuDataMsg();

    //设置表头信息
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    //
    virtual QModelIndex parent(const QModelIndex &child) const;

    //
    virtual bool hasChildren(const QModelIndex &parent) const;

    //
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

    //
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

    //
    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;

    //
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    //
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;

    //
    void msgChanged(int firstRow,int lastRow);

    //
    void addMsg(int num,int row);

    //
    void deleteMsg(int num,int row);

    //
    void endInsert();
    void endRemove();

private:

    //
    QString convertToTime(long long time) const;
};




class CpuMsgTable : public QWidget
{
    Q_OBJECT

public:
    CpuMsgTable(QWidget *parent = 0);
    ~CpuMsgTable();


    //
    Qtitan::Grid* m_grid;
    CpuDataMsg* model;

    //
    void startShow();

    //
    void showMessage();

    //
    void addOneVipMsg(int num);

    //
    void changeOneVipMsg(int num);

    //
    void deleteOneVipMsg(int num);

    //
    void endAddOneVipMsg();
    void endAddOneHistoryVipMsg();
    void endDeleteOneVipMsg();

private:

    //
    bool isFirstShow;

private slots:

    //
    void contextMenu(ContextMenuEventArgs* args);
};

#endif // CPUMSGTABLE_H
