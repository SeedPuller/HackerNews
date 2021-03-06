#ifndef NewsModel_H
#define NewsModel_H

#include <QAbstractListModel>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDateTime>
#include "network.h"

class NewsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    /*
     * roles for each column. ROLE_END is a flag for iteration convenience
     */
    enum {
        ROLE_START = Qt::UserRole + 1,
        idRole,
        authorRole,
        urlRole,
        titleRole,
        dateRole,
        commentRole,
        pointRole,
        ROLE_END
    };

    Q_PROPERTY(bool loaded READ getLoaded WRITE setLoaded NOTIFY loadedChanged);
    Q_PROPERTY(bool isReady READ getIsidfetched WRITE setIsidfetched NOTIFY isidfetchedChanged)

    explicit NewsModel(QObject *parent = nullptr);
    // return number of data's
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    // return data by role and index
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    // set data by role and index
//    bool setData(const QModelIndex &index, const QVariant &value,
//                 int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QHash<int, QByteArray> roleNames() const override;

    bool getLoaded() const;
    void setLoaded(bool value);

    bool getIsidfetched() const;
    void setIsidfetched(bool value);

public slots:
    QStringList get(int index);
    bool insert(const int id, const QString& author,
                const QString& url, const QString& title,
                const QString& date, const int point,
                const int comment, const QModelIndex &parent = QModelIndex());

    bool remove(int index, const QModelIndex& parent = QModelIndex());
    void fetchNewsId();
    void getPostInfo();

private slots:
    void parsePostId(const QByteArray& datas);
    void parsePostInfo(const QByteArray& data);

signals:
    void loadedChanged(const bool status);
    void isidfetchedChanged() const;

private:
    void checkRequestJobDone();


    QList<QVariantList> vlist;
    // list of columns
    const QList<QByteArray> columns{"id", "author", "url", "title", "date", "comment", "point"};
    bool loaded;
    QUrl topstoriesapi{"https://hacker-news.firebaseio.com/v0/topstories.json"};
    QUrl postinfoapi{"https://hacker-news.firebaseio.com/v0/item/"};
    Network networkrequest;
    int currentrequestnumber;
    int finalrequestnumber;
    bool isidfetched;
    QList<int> postids;
};

#endif // NewsModel_H
