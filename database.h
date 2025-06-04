//database.h
#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QtSql>

class Database : public QObject
{
    Q_OBJECT

public:
    explicit Database(QObject *parent = nullptr);
    ~Database();

    bool openDatabase(const QString &dbName);

    void insertSI(QString &inv,
                  QString &naim, QString &mod, QString &reestr, QString &zav, QString &pom,
                  QString &sost, QString &PO, QString &izg, QString &pravo, QString &date,
                  QString &diap, QString &pov, QString &fgis, QString &temp, QString &vlag,
                  QString &davl, QString &napr, QString &chast, QString &tecn, QString &clean, QString &remont);
    void updateSI(int SIId, QString &inv,
                  QString &naim, QString &mod, QString &reestr, QString &zav, QString &pom,
                  QString &sost, QString &PO, QString &izg, QString &pravo, QString &date,
                  QString &diap, QString &pov, QString &fgis, QString &temp, QString &vlag,
                  QString &davl, QString &napr, QString &chast, QString &tecn, QString &clean, QString &remont);

    void insertIO(QString &inv, QString &naim, QString &krnaim, QString &zav, QString &pom,
                  QString &izg, QString &pravo, QString &date, QString &pasp, QString &att,
                  QString &obj, QString &type, QString &khar, QString &temp, QString &vlag,
                  QString &davl, QString &napr, QString &chast, QString &tecn, QString &clean, QString &remont);
    void updateIO(int IOId,
                  QString &inv, QString &naim, QString &krnaim, QString &zav, QString &pom,
                  QString &izg, QString &pravo, QString &date, QString &pasp, QString &att,
                  QString &obj, QString &type, QString &khar, QString &temp, QString &vlag,
                  QString &davl, QString &napr, QString &chast, QString &tecn, QString &clean, QString &remont);

    void insertVO(QString &inv,
                  QString &naim, QString &zav, QString &pom, QString &izg, QString &pravo,
                  QString &date, QString &pred, QString &PO, QString &temp, QString &vlag,
                  QString &davl, QString &napr, QString &chast, QString &tecn, QString &clean, QString &remont);
    void updateVO(int VOId, QString &inv,
                  QString &naim, QString &zav, QString &pom, QString &izg, QString &pravo,
                  QString &date, QString &pred, QString &PO, QString &temp, QString &vlag,
                  QString &davl, QString &napr, QString &chast, QString &tecn, QString &clean, QString &remont);

    void insertPrtkl(QString &num, QString &date, QString &date_isp, QString &metod,
                     QString &SI, QString &IO, QString &VO, QString &arenda);
    void updatePrtkl(int PrtklID, QString &num, QString &date, QString &date_isp, QString &metod,
                     QString &SI, QString &IO, QString &VO, QString &arenda);

    void insertArenda(QString &inv, QString &naim, QString &type, QString &zav, QString &param, QString &reg_num,
                      QString &izg, QString &sobst, QString &diap, QString &pov_att, QString &dates);
    void updateArenda(int ArendaID, QString &inv, QString &naim, QString &type, QString &zav, QString &param,
                      QString &reg_num, QString &izg, QString &sobst, QString &diap, QString &pov_att, QString &dates);

    void delete_SI(int SIId);
    void delete_IO(int IOId);
    void delete_VO(int VOId);
    void delete_Prtkl(int PrtklID);
    void delete_Arenda(int ArendaID);

    QVector<QStringList> get_data_SI();
    QVector<QStringList> get_data_IO();
    QVector<QStringList> get_data_VO();
    QVector<QStringList> get_data_Prtkl();
    QVector<QStringList> get_data_Arenda();

    void insert_Info(QString &name, QString &adress);
    void update_Info(int InfoId, QString &name, QString &adress);
    void insert_Staff(QString &post, QString &name);
    void update_Staff(int StaffId, QString &post, QString &name);
    void insert_Staff_ruk(QString &post, QString &name);
    void update_Staff_ruk(int StaffId, QString &post, QString &name);
    void insert_Staff_gen(QString &post, QString &name);
    void update_Staff_gen(int StaffId, QString &post, QString &name);
    void insert_BirksKoeft(QString &inv, QString &info);
    void update_BirksKoeft(QString &inv, QString &info);
    void insert_Key(QString &info);

    QSqlDatabase db_r();
    void close_conn();

    QVector<bool> getExistsSIFromDatabase();
    QVector<QDate> getDatesPovFromDatabase();
    QStringList getInvPovFromDatabase();
    QStringList getNamesPovFromDatabase();

    QVector<bool> getExistsIOFromDatabase();
    QVector<QDate> getDatesAttFromDatabase();
    QStringList getInvAttFromDatabase();
    QStringList getNamesAttFromDatabase();

    QStringList get_data_info();
    QString get_data_metr();
    QString get_data_ruk();
    QString get_data_gen();
    QString get_data_BirksKoeft(QString &inv);
    bool get_data_Key();

private:
    QSqlDatabase db;

};

#endif // DATABASE_H
