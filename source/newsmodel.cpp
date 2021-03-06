#include "newsmodel.h"

NewsModel::NewsModel(QObject *parent)
    : QAbstractListModel(parent), loaded{false}, currentrequestnumber{0},
     finalrequestnumber{10}, isidfetched{false}
{

}

int NewsModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    return vlist.size();
}

QVariant NewsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || vlist.size() <= 0)
        return QVariant();

    if (role <= ROLE_START || role >= ROLE_END) {
        return QVariant();
    }

    QVariantList temp = vlist.at(index.row());
    const int column = role - (ROLE_START + 1);
    return temp[column];
}


//bool Model::setData(const QModelIndex &index, const QVariant &value, int role)
//{
//    int indexrow = index.row();
//    if (data(index, role) != value && vlist.size() > indexrow) {

//        switch (role) {

//            case fnameRole:
//                vlist[indexrow][1] = value.toString();
//            break;
//            case debtRole:
//                vlist[indexrow][2] = value.toString();
//            break;
//            case picRole:
//                vlist[indexrow][3] = value.toString();
//            break;
//        }
//        emit dataChanged(index, index, QVector<int>() << role);
//        return true;
//    }
//    return false;
//}

Qt::ItemFlags NewsModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable;
}

QHash<int, QByteArray> NewsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    int column_number{0};
    for (int i{ROLE_START + 1}; i != ROLE_END; ++i, ++column_number) {
        roles.insert(i, this->columns[column_number]);
    }
    return roles;
}

bool NewsModel::insert(const int id, const QString& author, const QString& url,
                       const QString& title, const QString& date,
                       const int point, const int comment, const QModelIndex &parent)
{
//    int rowcount = rowCount();
    beginInsertRows(parent, 0, 0);

    QVariantList temp;
    temp.append(id);
    temp.append(author);
    temp.append(url);
    temp.append(title);
    temp.append(date);
    temp.append(point);
    temp.append(comment);
    vlist.push_front(temp);
    endInsertRows();
    return true;
}

//bool NewsModel::prepareAndInsert(QString filepath)
//{
//    filepath.replace("file://", "");
//    QFileInfo info(filepath);
//    QStringList postfix{" B", " KB", " MB", " GB", " TB"};
//    qint64 size{info.size()};
//    if (size == 0) {
//        return false;
//    }
//    size_t i{0};
//    for (; i <= 4; ++i) {
//        if (size > 1024) {
//            size /= 1024;
//        }else {
//            break;
//        }
//    }
//    insert(info.fileName(), QString(QString::number(size) + postfix[i]), info.completeSuffix(), filepath);
//    return true;
//}

QStringList NewsModel::get(int index)
{
    QStringList list;
    foreach (QVariant var, vlist[index]) {
        list.append(var.toString());
    }
    return list;
}

void NewsModel::parsePostId(const QByteArray &datas)
{
    disconnect(&networkrequest, &Network::complete, this, &NewsModel::parsePostId);
    connect(&networkrequest, &Network::complete, this, &NewsModel::parsePostInfo);
    QJsonDocument jsonresponse = QJsonDocument::fromJson(datas);
    QJsonArray jsonarray = jsonresponse.array();
    if (jsonarray == QJsonArray()) {
        return;
    }
    foreach (QJsonValue val, jsonarray) {
        postids << val.toInt();
    }
    setIsidfetched(true);
}


void NewsModel::parsePostInfo(const QByteArray &data)
{
    QJsonDocument jsonresponse = QJsonDocument::fromJson(data);
    QJsonObject jsonobject = jsonresponse.object();
    QVariantList temp;
    int comments{0};
    QDateTime date;
    date.setSecsSinceEpoch(jsonobject["time"].toInt());
    if (jsonobject["kids"] != QJsonValue()) {
        comments = jsonobject["kids"].toArray().size();
    }

    insert(jsonobject["id"].toInt(), jsonobject["by"].toString(),
           jsonobject["url"].toString(),jsonobject["title"].toString(),
           date.toString("dd MMM hh:mm"), comments, jsonobject["score"].toInt());

    checkRequestJobDone();
}

void NewsModel::checkRequestJobDone()
{
    if (currentrequestnumber == finalrequestnumber - 1) {
        finalrequestnumber += 10;
        setLoaded(true);
        return;
    }
    ++currentrequestnumber;
}

bool NewsModel::getIsidfetched() const
{
    return isidfetched;
}

void NewsModel::setIsidfetched(bool value)
{
    if (value != isidfetched) {
        isidfetched = value;
        emit isidfetchedChanged();
    }
}


bool NewsModel::remove(int index, const QModelIndex& parent) {
    //        qDebug() << "removing index number: " << index;
    beginRemoveRows(parent, index, index);
        vlist.removeAt(index);
        endRemoveRows();
        return true;
}


void NewsModel::fetchNewsId()
{
    setIsidfetched(false);
    disconnect(&networkrequest, &Network::complete, this, &NewsModel::parsePostInfo);
    connect(&networkrequest, &Network::complete, this, &NewsModel::parsePostId);
    networkrequest.setUrl(topstoriesapi);
    networkrequest.get();
}

void NewsModel::getPostInfo()
{
//    qDebug() << "entered id to getPostInfo is: " << id;
//    qDebug() << "sending request to : " << postinfoapi.toString() + QString::number(id) + ".json";
    int vlistsize = vlist.size();
    if ((10 + vlistsize) >= postids.size()) {
        // this should change to handle fetching new id's and not just returning something.
        return;
    }
    setLoaded(false);
    for (int i{finalrequestnumber - 10}; i <= finalrequestnumber; ++i) {
        networkrequest.setUrl(QUrl(postinfoapi.toString() + QString::number(postids[vlistsize + i]) + ".json"));
        networkrequest.get();
    }
}

bool NewsModel::getLoaded() const
{
    return loaded;
}

void NewsModel::setLoaded(bool value)
{
    if (loaded == value) {
        return;
    }
    loaded = value;
    emit loadedChanged(value);
}
