//database.cpp
#include "database.h"
#include <QtSql>

Database::Database(QObject *parent)
    : QObject{parent}
{}

Database::~Database()
{
    if (db.isOpen()) {
        db.close();
    }
}

QSqlDatabase Database::db_r(){
    return db;
}

void Database::close_conn(){
    db.close();
}

bool Database::openDatabase(const QString &dbName)
{
    QDateTime currenttime;
    QString time_string = currenttime.currentDateTime().toString("yyyy/MM/dd hh:mm:ss,zzz");
    db = QSqlDatabase::addDatabase("QSQLITE", time_string);
    db.setDatabaseName(dbName);

    if (!db.open()) {
        qDebug() << "Ошибка подключения к базе данных:" << db.lastError().text();
        return false;
    }
    QSqlQuery query(db);
    // 21
    QString createTableSI = "CREATE TABLE IF NOT EXISTS SI ("
                          "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                          "inv TEXT, "
                          "naim TEXT, mod TEXT, reestr TEXT, zav TEXT, pom TEXT,"
                          "sost TEXT, PO TEXT, izg TEXT, pravo TEXT, date TEXT,"
                          "diap TEXT, pov TEXT, fgis TEXT, temp TEXT, vlag TEXT,"
                          "davl TEXT, napr TEXT, chast TEXT, tecn TEXT, clean TEXT, remont TEXT, is_exp BOOL)";
    if (!query.exec(createTableSI)) {
        qDebug() << "Ошибка создания таблицы SI:" << query.lastError().text();
        return false;
    }
    // 20
    QString createTableIO = "CREATE TABLE IF NOT EXISTS IO ("
                          "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                          "inv TEXT, naim TEXT, krnaim TEXT, zav TEXT, pom TEXT,"
                          "izg TEXT, pravo TEXT, date TEXT, pasp TEXT, att TEXT,"
                          "obj TEXT, type TEXT, khar TEXT, temp TEXT, vlag TEXT,"
                          "davl TEXT, napr TEXT, chast TEXT, tecn TEXT, clean TEXT, remont TEXT, is_exp BOOL)";
    if (!query.exec(createTableIO)) {
        qDebug() << "Ошибка создания таблицы IO:" << query.lastError().text();
        return false;
    }
    // 16
    QString createTableVO = "CREATE TABLE IF NOT EXISTS VO ("
                          "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                          "inv TEXT,"
                          "naim TEXT, zav TEXT, pom TEXT, izg TEXT, pravo TEXT,"
                          "date TEXT, pred TEXT, PO TEXT, temp TEXT, vlag TEXT,"
                          "davl TEXT, napr TEXT, chast TEXT, tecn TEXT, clean TEXT, remont TEXT, is_exp BOOL)";
    if (!query.exec(createTableVO)) {
        qDebug() << "Ошибка создания таблицы VO:" << query.lastError().text();
        return false;
    }
    QString createTableStaff = "CREATE TABLE IF NOT EXISTS Staffmetr ("
                            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                            "post TEXT,"
                            "name TEXT)";
    if (!query.exec(createTableStaff)) {
        qDebug() << "Ошибка создания таблицы Staffmetr:" << query.lastError().text();
        return false;
    }
    QString createTableStaff_ruk = "CREATE TABLE IF NOT EXISTS Staffruk ("
                               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                               "post TEXT,"
                               "name TEXT)";
    if (!query.exec(createTableStaff_ruk)) {
        qDebug() << "Ошибка создания таблицы Staffruk:" << query.lastError().text();
        return false;
    }
    QString createTableStaff_gen = "CREATE TABLE IF NOT EXISTS Staffgen ("
                               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                               "post TEXT,"
                               "name TEXT)";
    if (!query.exec(createTableStaff_gen)) {
        qDebug() << "Ошибка создания таблицы Staffgen:" << query.lastError().text();
        return false;
    }
    QString createTablePrtkl = "CREATE TABLE IF NOT EXISTS Prtkl ("
                                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                   "num TEXT, date TEXT, date_isp TEXT, metod TEXT, "
                                   "SI TEXT, IO TEXT, VO TEXT, arenda TEXT)";
    if (!query.exec(createTablePrtkl)) {
        qDebug() << "Ошибка создания таблицы Prtkl:" << query.lastError().text();
        return false;
    }
    QString createTableArenda = "CREATE TABLE IF NOT EXISTS Arenda ("
                                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                   "inv TEXT, naim TEXT, type TEXT, zav TEXT, param TEXT, "
                                   "reg_num TEXT, izg TEXT, sobst TEXT, diap TEXT, pov_att TEXT, dates TEXT)";
    if (!query.exec(createTableArenda)) {
        qDebug() << "Ошибка создания таблицы Staffgen:" << query.lastError().text();
        return false;
    }
    QString createTableInfo = "CREATE TABLE IF NOT EXISTS Info ("
                                "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                "name TEXT, adress TEXT)";
    if (!query.exec(createTableInfo)) {
        qDebug() << "Ошибка создания таблицы Info:" << query.lastError().text();
        return false;
    }
    QString createTableBirksKoeft = "CREATE TABLE IF NOT EXISTS BirksKoeft ("
                              "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                              "inv TEXT, info TEXT)";
    if (!query.exec(createTableBirksKoeft)) {
        qDebug() << "Ошибка создания таблицы BirksKoeft:" << query.lastError().text();
        return false;
    }
    QString createTableKey = "CREATE TABLE IF NOT EXISTS Key ("
                                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                    "info TEXT)";
    if (!query.exec(createTableKey)) {
        qDebug() << "Ошибка создания таблицы Key:" << query.lastError().text();
        return false;
    }
    return true;
}

void Database::insertSI(QString &inv,
                        QString &naim, QString &mod, QString &reestr, QString &zav, QString &pom,
                        QString &sost, QString &PO, QString &izg, QString &pravo, QString &date,
                        QString &diap, QString &pov, QString &fgis, QString &temp, QString &vlag,
                        QString &davl, QString &napr, QString &chast, QString &tecn, QString &clean, QString &remont){
    QSqlQuery query(db);

    query.prepare("INSERT INTO SI (inv, naim, mod, reestr, zav, pom, sost, PO, izg, pravo, "
                  "date, diap, pov, fgis, temp, vlag, davl, napr, chast, tecn, clean, remont) "
                  ""
                  "VALUES (:inv, :naim, :mod, :reestr, :zav, :pom, :sost, :PO, :izg, :pravo, "
                  ":date, :diap, :pov, :fgis, :temp, :vlag, :davl, :napr, :chast, :tecn, :clean, :remont)");

    query.bindValue(":inv", inv);
    query.bindValue(":naim", naim);
    query.bindValue(":mod", mod);
    query.bindValue(":reestr", reestr);
    query.bindValue(":zav", zav);
    query.bindValue(":pom", pom);
    query.bindValue(":sost", sost);
    query.bindValue(":PO", PO);
    query.bindValue(":izg", izg);
    query.bindValue(":pravo", pravo);
    query.bindValue(":date", date);
    query.bindValue(":diap", diap);
    query.bindValue(":pov", pov);
    query.bindValue(":fgis", fgis);
    query.bindValue(":temp", temp);
    query.bindValue(":vlag", vlag);
    query.bindValue(":davl", davl);
    query.bindValue(":napr", napr);
    query.bindValue(":chast", chast);
    query.bindValue(":tecn", tecn);
    query.bindValue(":clean", clean);
    query.bindValue(":remont", remont);

    if (!query.exec()) {
        qDebug() << "Ошибка вставки данных:" << query.lastError().text();
    } else {
        qDebug() << "СИ успешно добавлено.";
    }
}

void Database::updateSI(int SIId, QString &inv,
                        QString &naim, QString &mod, QString &reestr, QString &zav, QString &pom,
                        QString &sost, QString &PO, QString &izg, QString &pravo, QString &date,
                        QString &diap, QString &pov, QString &fgis, QString &temp, QString &vlag,
                        QString &davl, QString &napr, QString &chast, QString &tecn, QString &clean, QString &remont){
    QSqlQuery query(db);
    query.prepare("UPDATE SI SET inv = :inv, naim = :naim, mod = :mod, reestr = :reestr, zav = :zav, "
                  "pom = :pom, sost = :sost, PO = :PO, izg = :izg, pravo = :pravo, date = :date, "
                  "diap = :diap, pov = :pov, fgis = :fgis, temp = :temp, vlag = :vlag, davl = :davl, "
                  "napr = :napr, chast = :chast, tecn = :tecn, clean = :clean, remont = :remont WHERE id = :id");
    query.bindValue(":id", SIId);
    query.bindValue(":inv", inv);
    query.bindValue(":naim", naim);
    query.bindValue(":mod", mod);
    query.bindValue(":reestr", reestr);
    query.bindValue(":zav", zav);
    query.bindValue(":pom", pom);
    query.bindValue(":sost", sost);
    query.bindValue(":PO", PO);
    query.bindValue(":izg", izg);
    query.bindValue(":pravo", pravo);
    query.bindValue(":date", date);
    query.bindValue(":diap", diap);
    query.bindValue(":pov", pov);
    query.bindValue(":fgis", fgis);
    query.bindValue(":temp", temp);
    query.bindValue(":vlag", vlag);
    query.bindValue(":davl", davl);
    query.bindValue(":napr", napr);
    query.bindValue(":chast", chast);
    query.bindValue(":tecn", tecn);
    query.bindValue(":clean", clean);
    query.bindValue(":remont", remont);

    if (!query.exec()) {
        qDebug() << "Ошибка обновления данных:" << query.lastError().text();
    } else {
        qDebug() << "СИ успешно обновлено.";
    }
}

void Database::insertIO(QString &inv, QString &naim, QString &krnaim, QString &zav, QString &pom,
                        QString &izg, QString &pravo, QString &date, QString &pasp, QString &att,
                        QString &obj, QString &type, QString &khar, QString &temp, QString &vlag,
                        QString &davl, QString &napr, QString &chast, QString &tecn, QString &clean, QString &remont){

    QSqlQuery query(db);

    query.prepare("INSERT INTO IO (inv, naim, krnaim, zav, pom, izg, pravo, date, pasp, att, "
                  "obj, type, khar, temp, vlag, davl, napr, chast, tecn, clean, remont) "
                  ""
                  "VALUES (:inv, :naim, :krnaim, :zav, :pom, :izg, :pravo, :date, :pasp, :att, "
                  ":obj, :type, :khar, :temp, :vlag, :davl, :napr, :chast, :tecn, :clean, :remont)");

    query.bindValue(":inv", inv);
    query.bindValue(":naim", naim);
    query.bindValue(":krnaim", krnaim);
    query.bindValue(":zav", zav);
    query.bindValue(":pom", pom);
    query.bindValue(":izg", izg);
    query.bindValue(":pravo", pravo);
    query.bindValue(":date", date);
    query.bindValue(":pasp", pasp);
    query.bindValue(":att", att);
    query.bindValue(":obj", obj);
    query.bindValue(":type", type);
    query.bindValue(":khar", khar);
    query.bindValue(":temp", temp);
    query.bindValue(":vlag", vlag);
    query.bindValue(":davl", davl);
    query.bindValue(":napr", napr);
    query.bindValue(":chast", chast);
    query.bindValue(":tecn", tecn);
    query.bindValue(":clean", clean);
    query.bindValue(":remont", remont);

    if (!query.exec()) {
        qDebug() << "Ошибка вставки данных:" << query.lastError().text();
    } else {
        qDebug() << "ИО успешно добавлено.";
    }
}

void Database::updateIO(int IOId,
                        QString &inv, QString &naim, QString &krnaim, QString &zav, QString &pom,
                        QString &izg, QString &pravo, QString &date, QString &pasp, QString &att,
                        QString &obj, QString &type, QString &khar, QString &temp, QString &vlag,
                        QString &davl, QString &napr, QString &chast, QString &tecn, QString &clean, QString &remont){

    QSqlQuery query(db);

    query.prepare("UPDATE IO SET inv = :inv, naim = :naim, krnaim = :krnaim, zav = :zav, pom = :pom, "
                  "izg = :izg, pravo = :pravo, date = :date, pasp = :pasp, att = :att, obj = :obj, "
                  "type = :type, khar = :khar, temp = :temp, vlag = :vlag, davl = :davl, "
                  "napr = :napr, chast = :chast, tecn = :tecn, clean = :clean, remont = :remont WHERE id = :id");

    query.bindValue(":id", IOId);
    query.bindValue(":inv", inv);
    query.bindValue(":naim", naim);
    query.bindValue(":krnaim", krnaim);
    query.bindValue(":zav", zav);
    query.bindValue(":pom", pom);
    query.bindValue(":izg", izg);
    query.bindValue(":pravo", pravo);
    query.bindValue(":date", date);
    query.bindValue(":pasp", pasp);
    query.bindValue(":att", att);
    query.bindValue(":obj", obj);
    query.bindValue(":type", type);
    query.bindValue(":khar", khar);
    query.bindValue(":temp", temp);
    query.bindValue(":vlag", vlag);
    query.bindValue(":davl", davl);
    query.bindValue(":napr", napr);
    query.bindValue(":chast", chast);
    query.bindValue(":tecn", tecn);
    query.bindValue(":clean", clean);
    query.bindValue(":remont", remont);

    if (!query.exec()) {
        qDebug() << "Ошибка обновления данных:" << query.lastError().text();
    } else {
        qDebug() << "ИО успешно обновлено.";
    }
}

void Database::insertVO(QString &inv,
                        QString &naim, QString &zav, QString &pom, QString &izg, QString &pravo,
                        QString &date, QString &pred, QString &PO, QString &temp, QString &vlag,
                        QString &davl, QString &napr, QString &chast, QString &tecn, QString &clean, QString &remont)
{
    QSqlQuery query(db);

    query.prepare("INSERT INTO VO (inv, naim, zav, pom, izg, pravo, date, pred, "
                  "PO, temp, vlag, davl, napr, chast, tecn, clean, remont) "
                  "VALUES (:inv, :naim, :zav, :pom, :izg, :pravo, :date, :pred, "
                  ":PO, :temp, :vlag, :davl, :napr, :chast, :tecn, :clean, :remont)");
    query.bindValue(":inv", inv);
    query.bindValue(":naim", naim);
    query.bindValue(":zav", zav);
    query.bindValue(":pom", pom);
    query.bindValue(":izg", izg);
    query.bindValue(":pravo", pravo);
    query.bindValue(":date", date);
    query.bindValue(":pred", pred);
    query.bindValue(":PO", PO);
    query.bindValue(":temp", temp);
    query.bindValue(":vlag", vlag);
    query.bindValue(":davl", davl);
    query.bindValue(":napr", napr);
    query.bindValue(":chast", chast);
    query.bindValue(":tecn", tecn);
    query.bindValue(":clean", clean);
    query.bindValue(":remont", remont);

    if (!query.exec()) {
        qDebug() << "Ошибка вставки данных:" << query.lastError().text();
    } else {
        qDebug() << "ВО успешно добавлено.";
    }
}

void Database::updateVO(int VOId, QString &inv,
                        QString &naim, QString &zav, QString &pom, QString &izg, QString &pravo,
                        QString &date, QString &pred, QString &PO, QString &temp, QString &vlag,
                        QString &davl, QString &napr, QString &chast, QString &tecn, QString &clean, QString &remont)
{
    QSqlQuery query(db);

    query.prepare("UPDATE VO SET inv = :inv, naim = :naim, zav = :zav, pom = :pom, "
                  "izg = :izg, pravo = :pravo, date = :date, pred = :pred, "
                  "PO = :PO, temp = :temp, vlag = :vlag, davl = :davl, napr = :napr, "
                  "chast = :chast, tecn = :tecn, clean = :clean, remont = :remont WHERE id = :id");
    query.bindValue(":id", VOId);
    query.bindValue(":inv", inv);
    query.bindValue(":naim", naim);
    query.bindValue(":zav", zav);
    query.bindValue(":pom", pom);
    query.bindValue(":izg", izg);
    query.bindValue(":pravo", pravo);
    query.bindValue(":date", date);
    query.bindValue(":pred", pred);
    query.bindValue(":PO", PO);
    query.bindValue(":temp", temp);
    query.bindValue(":vlag", vlag);
    query.bindValue(":davl", davl);
    query.bindValue(":napr", napr);
    query.bindValue(":chast", chast);
    query.bindValue(":tecn", tecn);
    query.bindValue(":clean", clean);
    query.bindValue(":remont", remont);

    if (!query.exec()) {
        qDebug() << "Ошибка обновления данных:" << query.lastError().text();
    } else {
        qDebug() << "ВО успешно обновлено.";
    }
}

void Database::insertPrtkl(QString &num, QString &date, QString &date_isp, QString &metod,
                           QString &SI, QString &IO, QString &VO, QString &arenda){
    QSqlQuery query(db);

    query.prepare("INSERT INTO Prtkl (num, date, date_isp, metod, SI, IO, VO, arenda) "
                  ""
                  "VALUES (:num, :date, :date_isp, :metod, :SI, :IO, :VO, :arenda)");

    query.bindValue(":num", num);
    query.bindValue(":date", date);
    query.bindValue(":date_isp", date_isp);
    query.bindValue(":metod", metod);
    query.bindValue(":SI", SI);
    query.bindValue(":IO", IO);
    query.bindValue(":VO", VO);
    query.bindValue(":arenda", arenda);

    if (!query.exec()) {
        qDebug() << "Ошибка вставки данных:" << query.lastError().text();
    } else {
        qDebug() << "Протокол успешно добавлен.";
    }
}

void Database::updatePrtkl(int PrtklID, QString &num, QString &date, QString &date_isp, QString &metod,
                           QString &SI, QString &IO, QString &VO, QString &arenda){
    QSqlQuery query(db);

    query.prepare("UPDATE Prtkl SET num = :num, date = :date, date_isp = :date_isp, metod = :metod, "
                  "SI = :SI, IO = :IO, VO = :VO, arenda = :arenda WHERE id = :id");

    query.bindValue(":id", PrtklID);
    query.bindValue(":num", num);
    query.bindValue(":date", date);
    query.bindValue(":date_isp", date_isp);
    query.bindValue(":metod", metod);
    query.bindValue(":SI", SI);
    query.bindValue(":IO", IO);
    query.bindValue(":VO", VO);
    query.bindValue(":arenda", arenda);

    if (!query.exec()) {
        qDebug() << "Ошибка вставки данных:" << query.lastError().text();
    } else {
        qDebug() << "Протокол успешно обновлён.";
    }
}

void Database::insertArenda(QString &inv, QString &naim, QString &type, QString &zav, QString &param, QString &reg_num,
                            QString &izg, QString &sobst, QString &diap, QString &pov_att, QString &dates){
    QSqlQuery query(db);

    query.prepare("INSERT INTO Arenda (inv, naim, type, zav, param, reg_num, izg, sobst, diap, pov_att, dates ) "
                  ""
                  "VALUES (:inv, :naim, :type, :zav, :param, :reg_num, :izg, :sobst, :diap, :pov_att, :dates)");

    query.bindValue(":inv", inv);
    query.bindValue(":naim", naim);
    query.bindValue(":type", type);
    query.bindValue(":zav", zav);
    query.bindValue(":param", param);
    query.bindValue(":reg_num", reg_num);
    query.bindValue(":izg", izg);
    query.bindValue(":sobst", sobst);
    query.bindValue(":diap", diap);
    query.bindValue(":pov_att", pov_att);
    query.bindValue(":dates", dates);

    if (!query.exec()) {
        qDebug() << "Ошибка вставки данных:" << query.lastError().text();
    } else {
        qDebug() << "Оборудование успешно добавлено.";
    }
}

void Database::updateArenda(int ArendaID, QString &inv, QString &naim, QString &type, QString &zav, QString &param,
                            QString &reg_num, QString &izg, QString &sobst, QString &diap, QString &pov_att, QString &dates){
    QSqlQuery query(db);

    query.prepare("UPDATE Arenda SET inv = :inv, naim = :naim, type = :type, zav = :zav, param = :param, "
                  "reg_num = :reg_num, izg = :izg, sobst = :sobst, diap = :diap, pov_att = :pov_att, dates = :dates WHERE id = :id");

    query.bindValue(":id", ArendaID);
    query.bindValue(":inv", inv);
    query.bindValue(":naim", naim);
    query.bindValue(":type", type);
    query.bindValue(":zav", zav);
    query.bindValue(":param", param);
    query.bindValue(":reg_num", reg_num);
    query.bindValue(":izg", izg);
    query.bindValue(":sobst", sobst);
    query.bindValue(":diap", diap);
    query.bindValue(":pov_att", pov_att);
    query.bindValue(":dates", dates);

    if (!query.exec()) {
        qDebug() << "Ошибка вставки данных:" << query.lastError().text();
    } else {
        qDebug() << "Оборудование успешно обновлён.";
    }
}


void Database::delete_SI(int SIId)
{
    QSqlQuery query(db);

    query.prepare("DELETE FROM SI WHERE id = :id");
    query.bindValue(":id", SIId);

    if (!query.exec()) {
        qDebug() << "Ошибка удаления данных:" << query.lastError().text();
    } else {
        qDebug() << "Оборудование успешно удалено.";
    }
}

void Database::delete_IO(int IOId)
{
    QSqlQuery query(db);

    query.prepare("DELETE FROM IO WHERE id = :id");
    query.bindValue(":id", IOId);

    if (!query.exec()) {
        qDebug() << "Ошибка удаления данных:" << query.lastError().text();
    } else {
        qDebug() << "Оборудование успешно удалено.";
    }
}

void Database::delete_VO(int VOId)
{
    QSqlQuery query(db);

    query.prepare("DELETE FROM VO WHERE id = :id");
    query.bindValue(":id", VOId);

    if (!query.exec()) {
        qDebug() << "Ошибка удаления данных:" << query.lastError().text();
    } else {
        qDebug() << "Оборудование успешно удалено.";
    }
}

void Database::delete_Prtkl(int PrtklID)
{
    QSqlQuery query(db);

    query.prepare("DELETE FROM Prtkl WHERE id = :id");
    query.bindValue(":id", PrtklID);

    if (!query.exec()) {
        qDebug() << "Ошибка удаления данных:" << query.lastError().text();
    } else {
        qDebug() << "Протокол успешно удален.";
    }
}

void Database::delete_Arenda(int ArendaID)
{
    QSqlQuery query(db);

    query.prepare("DELETE FROM Arenda WHERE id = :id");
    query.bindValue(":id", ArendaID);

    if (!query.exec()) {
        qDebug() << "Ошибка удаления данных:" << query.lastError().text();
    } else {
        qDebug() << "Оборудование успешно удалено.";
    }
}


QVector<QStringList> Database::get_data_SI(){
    QVector<QStringList> data;
    QSqlQuery query(db);

    // QString zapros = "SELECT inv, naim, mod, reestr, zav, pom, sost, PO, izg, pravo, "
    //                  "date, diap, pov, fgis, temp, vlag, davl, napr, chast, tecn, clean, remont FROM SI WHERE inv LIKE 'СИ-%' ORDER BY CAST(SUBSTR(inv, 4) AS INTEGER)";
    QString zapros = "SELECT inv, naim, mod, reestr, zav, pom, sost, PO, izg, pravo, "
                     "date, diap, pov, fgis, temp, vlag, davl, napr, chast, tecn, clean, remont FROM SI ORDER BY CAST(inv AS INTEGER)";

    if (!query.exec(zapros)) {
        qDebug() << "Ошибка Select SI:" << query.lastError().text();
    }

    while (query.next()) {
        QString inv_data = query.value(0).toString();
        QString naim_data = query.value(1).toString();
        QString mod_data = query.value(2).toString();
        QString reestr_data = query.value(3).toString();
        QString zav_data = query.value(4).toString();
        QString pom_data = query.value(5).toString();
        QString sost_data = query.value(6).toString();
        QString PO_data = query.value(7).toString();
        QString izg_data = query.value(8).toString();
        QString pravo_data = query.value(9).toString();
        QString date_data = query.value(10).toString();
        QString diap_data = query.value(11).toString();
        QString pov_data = query.value(12).toString();
        QString fgis_data = query.value(13).toString();
        QString temp_data = query.value(14).toString();
        QString vlag_data = query.value(15).toString();
        QString davl_data = query.value(16).toString();
        QString napr_data = query.value(17).toString();
        QString chast_data = query.value(18).toString();
        QString tecn_data = query.value(19).toString();
        QString clean_data = query.value(20).toString();
        QString remont_data = query.value(21).toString();

        QStringList data_list;
        data_list << inv_data << naim_data << mod_data << reestr_data << zav_data
                  << pom_data << sost_data << PO_data << izg_data << pravo_data
                  << date_data << diap_data << pov_data << fgis_data << temp_data
                  << vlag_data << davl_data << napr_data << chast_data
                  << tecn_data << clean_data << remont_data;

        data.append(data_list);
    }
    return data;
}

QVector<QStringList> Database::get_data_IO(){
    QVector<QStringList> data;
    QSqlQuery query(db);

    // QString zapros = "SELECT inv, naim, krnaim, zav, pom, izg, pravo, date, pasp, att, "
    //                  "obj, type, khar, temp, vlag, davl, napr, chast, tecn, clean, remont FROM IO WHERE inv LIKE 'ИО-%' ORDER BY CAST(SUBSTR(inv, 4) AS INTEGER)";
    QString zapros = "SELECT inv, naim, krnaim, zav, pom, izg, pravo, date, pasp, att, "
                     "obj, type, khar, temp, vlag, davl, napr, chast, tecn, clean, remont FROM IO ORDER BY CAST(inv AS INTEGER)";

    if (!query.exec(zapros)) {
        qDebug() << "Ошибка Select IO:" << query.lastError().text();
    }

    while (query.next()) {
        QString inv_data = query.value(0).toString();
        QString naim_data = query.value(1).toString();   // наоборот с 2
        QString krnaim_data = query.value(2).toString(); // наоборот с 1
        QString zav_data = query.value(3).toString();
        QString pom_data = query.value(4).toString();
        QString izg_data = query.value(5).toString();
        QString pravo_data = query.value(6).toString();
        QString date_data = query.value(7).toString();
        QString pasp_data = query.value(8).toString();
        QString att_data = query.value(9).toString();
        QString obj_data = query.value(10).toString();
        QString type_data = query.value(11).toString();
        QString khar_data = query.value(12).toString();
        QString temp_data = query.value(13).toString();
        QString vlag_data = query.value(14).toString();
        QString davl_data = query.value(15).toString();
        QString napr_data = query.value(16).toString();
        QString chast_data = query.value(17).toString();
        QString tecn_data = query.value(18).toString();
        QString clean_data = query.value(19).toString();
        QString remont_data = query.value(20).toString();

        QStringList data_list;
        data_list << inv_data << krnaim_data << naim_data << zav_data << pom_data << izg_data
                  << pravo_data << date_data << pasp_data << att_data << obj_data << type_data
                  << khar_data << temp_data << vlag_data << davl_data << napr_data << chast_data
                  << tecn_data << clean_data << remont_data;

        data.append(data_list);
    }
    return data;
}

QVector<QStringList> Database::get_data_VO(){
    QVector<QStringList> data;
    QSqlQuery query(db);

    // QString zapros = "SELECT inv, naim, zav, pom, izg, pravo, date, pred, "
    //                  "PO, temp, vlag, davl, napr, chast, tecn, clean, remont FROM VO WHERE inv LIKE 'ВО-%' ORDER BY CAST(SUBSTR(inv, 4) AS INTEGER)";
    QString zapros = "SELECT inv, naim, zav, pom, izg, pravo, date, pred, "
                     "PO, temp, vlag, davl, napr, chast, tecn, clean, remont FROM VO ORDER BY CAST(inv AS INTEGER)";

    if (!query.exec(zapros)) {
        qDebug() << "Ошибка Select VO:" << query.lastError().text();
    }

    while (query.next()) {
        QString inv_data = query.value(0).toString();
        QString naim_data = query.value(1).toString();
        QString zav_data = query.value(2).toString();
        QString pom_data = query.value(3).toString();
        QString izg_data = query.value(4).toString();
        QString pravo_data = query.value(5).toString();
        QString date_data = query.value(6).toString();
        QString pred_data = query.value(7).toString();
        QString PO_data = query.value(8).toString();
        QString temp_data = query.value(9).toString();
        QString vlag_data = query.value(10).toString();
        QString davl_data = query.value(11).toString();
        QString napr_data = query.value(12).toString();
        QString chast_data = query.value(13).toString();
        QString tecn_data = query.value(14).toString();
        QString clean_data = query.value(15).toString();
        QString remont_data = query.value(16).toString();

        QStringList data_list;
        data_list << inv_data <<  naim_data << zav_data << pom_data << izg_data
                  << pravo_data << date_data << pred_data << PO_data << temp_data
                  << vlag_data << davl_data << napr_data << chast_data
                  << tecn_data << clean_data << remont_data;

        data.append(data_list);
    }
    return data;
}

QVector<QStringList> Database::get_data_Prtkl(){
    QVector<QStringList> data;
    QSqlQuery query(db);

    QString zapros = "SELECT * FROM Prtkl";

    if (!query.exec(zapros)) {
        qDebug() << "Ошибка Select Prtkl:" << query.lastError().text();
    }

    while (query.next()) {
        QString naim_data = query.value(1).toString();
        QString zav_data = query.value(2).toString();
        QString pom_data = query.value(3).toString();
        QString izg_data = query.value(4).toString();
        QString pravo_data = query.value(5).toString();
        QString date_data = query.value(6).toString();
        QString pred_data = query.value(7).toString();
        QString PO_data = query.value(8).toString();

        QStringList data_list;
        data_list << naim_data << zav_data << pom_data << izg_data
                  << pravo_data << date_data << pred_data << PO_data;

        data.append(data_list);
    }
    return data;
}

QVector<QStringList> Database::get_data_Arenda(){
    QVector<QStringList> data;
    QSqlQuery query(db);

    QString zapros = "SELECT * FROM Arenda";

    if (!query.exec(zapros)) {
        qDebug() << "Ошибка Select Arenda:" << query.lastError().text();
    }

    while (query.next()) {
        QString naim_data = query.value(1).toString();
        QString zav_data = query.value(2).toString();
        QString pom_data = query.value(3).toString();
        QString izg_data = query.value(4).toString();
        QString pravo_data = query.value(5).toString();
        QString date_data = query.value(6).toString();
        QString pred_data = query.value(7).toString();
        QString PO_data = query.value(8).toString();
        QString dat_data = query.value(9).toString();
        QString pre_data = query.value(10).toString();
        QString P_data = query.value(11).toString();

        QStringList data_list;
        data_list << naim_data << zav_data << pom_data << izg_data
                  << pravo_data << date_data << pred_data << PO_data
                  << dat_data << pre_data << P_data;

        data.append(data_list);
    }
    return data;
}


void Database::insert_Info(QString &name, QString &adress){

    QSqlQuery query(db);

    query.prepare("INSERT INTO Info (name, adress) "
                  "VALUES (:name, :adress)");
    query.bindValue(":name", name);
    query.bindValue(":adress", adress);

    if (!query.exec()) {
        qDebug() << "Ошибка вставки данных:" << query.lastError().text();
    } else {
        qDebug() << "Информация успешно добавлена.";
    }
}

void Database::update_Info(int InfoId, QString &name, QString &adress){

    QSqlQuery query(db);

    query.prepare("UPDATE Info SET name = :name, adress = :adress WHERE id = :id");
    query.bindValue(":id", InfoId);
    query.bindValue(":name", name);
    query.bindValue(":adress", adress);

    if (!query.exec()) {
        qDebug() << "Ошибка обновления данных:" << query.lastError().text();
    } else {
        qDebug() << "Информация успешно обновлена.";
    }
}

void Database::insert_Staff(QString &post, QString &name){

    QSqlQuery query(db);

    query.prepare("INSERT INTO Staffmetr (post, name) "
                  "VALUES (:post, :name)");
    query.bindValue(":post", post);
    query.bindValue(":name", name);

    if (!query.exec()) {
        qDebug() << "Ошибка вставки данных:" << query.lastError().text();
    } else {
        qDebug() << "Сотрудник успешно добавлен.";
    }
}

void Database::update_Staff(int StaffId, QString &post, QString &name){

    QSqlQuery query(db);

    query.prepare("UPDATE Staffmetr SET post = :post, name = :name WHERE id = :id");
    query.bindValue(":id", StaffId);
    query.bindValue(":post", post);
    query.bindValue(":name", name);

    if (!query.exec()) {
        qDebug() << "Ошибка обновления данных:" << query.lastError().text();
    } else {
        qDebug() << "Сотрудник успешно обновлен.";
    }
}

void Database::insert_Staff_ruk(QString &post, QString &name){

    QSqlQuery query(db);

    query.prepare("INSERT INTO Staffruk (post, name) "
                  "VALUES (:post, :name)");
    query.bindValue(":post", post);
    query.bindValue(":name", name);

    if (!query.exec()) {
        qDebug() << "Ошибка вставки данных:" << query.lastError().text();
    } else {
        qDebug() << "Сотрудник успешно добавлен.";
    }
}

void Database::update_Staff_ruk(int StaffId, QString &post, QString &name){

    QSqlQuery query(db);

    query.prepare("UPDATE Staffruk SET post = :post, name = :name WHERE id = :id");
    query.bindValue(":id", StaffId);
    query.bindValue(":post", post);
    query.bindValue(":name", name);

    if (!query.exec()) {
        qDebug() << "Ошибка обновления данных:" << query.lastError().text();
    } else {
        qDebug() << "Сотрудник успешно обновлен.";
    }
}

void Database::insert_Staff_gen(QString &post, QString &name){

    QSqlQuery query(db);

    query.prepare("INSERT INTO Staffgen (post, name) "
                  "VALUES (:post, :name)");
    query.bindValue(":post", post);
    query.bindValue(":name", name);

    if (!query.exec()) {
        qDebug() << "Ошибка вставки данных:" << query.lastError().text();
    } else {
        qDebug() << "Сотрудник успешно добавлен.";
    }
}

void Database::update_Staff_gen(int StaffId, QString &post, QString &name){

    QSqlQuery query(db);

    query.prepare("UPDATE Staffgen SET post = :post, name = :name WHERE id = :id");
    query.bindValue(":id", StaffId);
    query.bindValue(":post", post);
    query.bindValue(":name", name);

    if (!query.exec()) {
        qDebug() << "Ошибка обновления данных:" << query.lastError().text();
    } else {
        qDebug() << "Сотрудник успешно обновлен.";
    }
}

void Database::insert_BirksKoeft(QString &inv, QString &info){

    QSqlQuery query(db);

    query.prepare("INSERT INTO BirksKoeft (inv, info) "
                  "VALUES (:inv, :info)");
    query.bindValue(":inv", inv);
    query.bindValue(":info", info);

    if (!query.exec()) {
        qDebug() << "Ошибка вставки данных:" << query.lastError().text();
    } else {
        qDebug() << "Коэффициент успешно добавлен.";
    }
}

void Database::update_BirksKoeft(QString &inv, QString &info){

    QSqlQuery query(db);

    query.prepare("UPDATE BirksKoeft SET info = :info WHERE inv = :inv");
    query.bindValue(":inv", inv);
    query.bindValue(":info", info);

    if (!query.exec()) {
        qDebug() << "Ошибка обновления данных:" << query.lastError().text();
    } else {
        qDebug() << "Коэффициент успешно обновлен.";
    }
}

void Database::insert_Key(QString &info){

    QSqlQuery query(db);

    query.prepare("INSERT INTO Key (info) "
                  "VALUES (:info)");
    query.bindValue(":info", info);

    if (!query.exec()) {
        qDebug() << "Ошибка вставки данных:" << query.lastError().text();
    } else {
        qDebug() << "Ключ успешно добавлен.";
    }
}


QVector<bool> Database::getExistsSIFromDatabase() {
    QVector<bool> dates;
    QSqlQuery query(db);

    QString zapros = "SELECT date, pov FROM SI";
    if (!query.exec(zapros)) {
        qDebug() << "Ошибка Select SI:" << query.lastError().text();
    }

    while (query.next()) {
        QString dateStr = query.value(0).toString();
        QString povStr = query.value(1).toString();
        if (povStr != "-"){
            if (dateStr.split("@").size() > 1){
                dates.append(false);
            } else {
                dates.append(true);
            }
        }
    }
    return dates;
}

QVector<QDate> Database::getDatesPovFromDatabase() {
    QVector<QDate> dates;
    QStringList inv_nums;
    QSqlQuery query(db);

    QString zapros = "SELECT inv, pov FROM SI";
    if (!query.exec(zapros)) {
        qDebug() << "Ошибка Select SI:" << query.lastError().text();
    }

    while (query.next()) {
        QString inv_num = query.value(0).toString();
        QString dateStr = query.value(1).toString();
        if (dateStr != "-"){

            QStringList pov_list = dateStr.split("@");
            QString one_pov = pov_list.last();
            QStringList one_param_pov = one_pov.split(";");

            QDate date = QDate::fromString(one_param_pov.at(3), "dd.MM.yyyy");
            if (date.isValid()) {
                dates.append(date);
                inv_nums.append(inv_num);
            }
        }
    }
    return dates;
}

QStringList Database::getInvPovFromDatabase() {
    QVector<QDate> dates;
    QStringList inv_nums;
    QSqlQuery query(db);

    QString zapros = "SELECT inv, pov FROM SI";
    if (!query.exec(zapros)) {
        qDebug() << "Ошибка Select SI:" << query.lastError().text();
    }

    while (query.next()) {
        QString inv_num = query.value(0).toString();
        QString dateStr = query.value(1).toString();
        if (dateStr != "-"){

            QStringList pov_list = dateStr.split("@");
            QString one_pov = pov_list.last();
            QStringList one_param_pov = one_pov.split(";");

            QDate date = QDate::fromString(one_param_pov.at(3), "dd.MM.yyyy");
            if (date.isValid()) {
                dates.append(date);
                inv_nums.append(inv_num);
            }
        }
    }
    return inv_nums;
}

QStringList Database::getNamesPovFromDatabase() {
    QVector<QDate> dates;
    QStringList names;
    QSqlQuery query(db);

    QString zapros = "SELECT naim, mod, pov FROM SI";
    if (!query.exec(zapros)) {
        qDebug() << "Ошибка Select SI:" << query.lastError().text();
    }

    while (query.next()) {
        QString naim_mod = query.value(0).toString() + " " + query.value(1).toString();
        QString dateStr = query.value(2).toString();
        if (dateStr != "-"){

            QStringList pov_list = dateStr.split("@");
            QString one_pov = pov_list.last();
            QStringList one_param_pov = one_pov.split(";");

            QDate date = QDate::fromString(one_param_pov.at(3), "dd.MM.yyyy");
            if (date.isValid()) {
                dates.append(date);
                names.append(naim_mod);
            }
        }
    }
    return names;
}

QVector<bool> Database::getExistsIOFromDatabase() {
    QVector<bool> dates;
    QSqlQuery query(db);

    QString zapros = "SELECT date, att FROM IO";
    if (!query.exec(zapros)) {
        qDebug() << "Ошибка Select IO:" << query.lastError().text();
    }

    while (query.next()) {
        QString dateStr = query.value(0).toString();
        QString povStr = query.value(1).toString();
        if (povStr != "-"){
            if (dateStr.split("@").size() > 1){
                dates.append(false);
            } else {
                dates.append(true);
            }
        }
    }
    return dates;
}

QVector<QDate> Database::getDatesAttFromDatabase() {
    QVector<QDate> dates;
    QStringList inv_nums;
    QSqlQuery query(db);

    QString zapros = "SELECT inv, att FROM IO";
    if (!query.exec(zapros)) {
        qDebug() << "Ошибка Select IO:" << query.lastError().text();
    }

    while (query.next()) {
        QString inv_num = query.value(0).toString();
        QString dateStr = query.value(1).toString();

        QStringList att_list = dateStr.split("@");
        QString one_att = att_list.last();
        QStringList one_param_att = one_att.split(";");

        QDate date = QDate::fromString(one_param_att.at(3), "dd.MM.yyyy");
        if (date.isValid()) {
            dates.append(date);
            inv_nums.append(inv_num);
        }
    }
    return dates;
}

QStringList Database::getInvAttFromDatabase() {
    QVector<QDate> dates;
    QStringList inv_nums;
    QSqlQuery query(db);

    QString zapros = "SELECT inv, att FROM IO";
    if (!query.exec(zapros)) {
        qDebug() << "Ошибка Select IO:" << query.lastError().text();
    }

    while (query.next()) {
        QString inv_num = query.value(0).toString();
        QString dateStr = query.value(1).toString();

        QStringList att_list = dateStr.split("@");
        QString one_att = att_list.last();
        QStringList one_param_att = one_att.split(";");

        QDate date = QDate::fromString(one_param_att.at(3), "dd.MM.yyyy");
        if (date.isValid()) {
            dates.append(date);
            inv_nums.append(inv_num);
        }
    }
    return inv_nums;
}

QStringList Database::getNamesAttFromDatabase() {
    QVector<QDate> dates;
    QStringList names;
    QSqlQuery query(db);

    QString zapros = "SELECT krnaim, att FROM IO";
    if (!query.exec(zapros)) {
        qDebug() << "Ошибка Select IO:" << query.lastError().text();
    }

    while (query.next()) {
        QString naim_mod = query.value(0).toString();
        QString dateStr = query.value(1).toString();
        if (dateStr != "-"){

            QStringList pov_list = dateStr.split("@");
            QString one_pov = pov_list.last();
            QStringList one_param_pov = one_pov.split(";");

            QDate date = QDate::fromString(one_param_pov.at(3), "dd.MM.yyyy");
            if (date.isValid()) {
                dates.append(date);
                names.append(naim_mod);
            }
        }
    }
    return names;
}

QStringList Database::get_data_info(){
    QSqlQuery query(db);
    QStringList info;
    QString zapros = "SELECT * FROM Info ORDER BY id DESC LIMIT 1";
    if (!query.exec(zapros)) {
        qDebug() << "Ошибка Info:" << query.lastError().text();
    }
    query.next();
    QString name = query.value(1).toString();
    QString adress = query.value(2).toString();
    info.append(name);
    info.append(adress);

    return info;
}

QString Database::get_data_metr(){
    QSqlQuery query(db);
    QString zapros = "SELECT * FROM Staffmetr ORDER BY id DESC LIMIT 1";
    if (!query.exec(zapros)) {
        qDebug() << "Ошибка Staffmetr:" << query.lastError().text();
    }
    query.next();
    QString metr = query.value(2).toString();
    return metr;
}

QString Database::get_data_ruk(){
    QSqlQuery query(db);
    QString zapros = "SELECT * FROM Staffruk ORDER BY id DESC LIMIT 1";
    if (!query.exec(zapros)) {
        qDebug() << "Ошибка Staffruk:" << query.lastError().text();
    }
    query.next();
    QString ruk = query.value(2).toString();
    return ruk;
}

QString Database::get_data_gen(){
    QSqlQuery query(db);
    QString zapros = "SELECT * FROM Staffgen ORDER BY id DESC LIMIT 1";
    if (!query.exec(zapros)) {
        qDebug() << "Ошибка Staffgen:" << query.lastError().text();
    }
    query.next();
    QString gen = query.value(2).toString();
    return gen;
}

QString Database::get_data_BirksKoeft(QString &inv){
    QSqlQuery query(db);
    query.prepare(
        "SELECT info FROM BirksKoeft "
        "WHERE inv = :inv "
        "ORDER BY "
        "  CASE WHEN info IS NOT NULL AND info != '' THEN 0 ELSE 1 END, "
        "  LENGTH(info) DESC "
        "LIMIT 1"
        );
    query.bindValue(":inv", inv);

    if (!query.exec()) {
        qDebug() << "Ошибка BirksKoeft:" << query.lastError().text();
    }
    if (query.next()) {
        return query.value(0).toString();
    }
    return QString();
}

bool Database::get_data_Key(){
    QSqlQuery query(db);
    query.prepare("SELECT * FROM Key ORDER BY id DESC LIMIT 1");

    if (!query.exec()) {
        qDebug() << "Ошибка Key:" << query.lastError().text();
    }
    if (query.next()) {
        if (query.value(1).toString() == "54uijefg8OIJ$TOfipj94foi3fwmZ") {
            return true;
        }
    }
    return false;
}
