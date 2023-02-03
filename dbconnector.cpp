#include "dbconnector.h"

void FileSystemRepository::del(int key) {
    pqxx::work txn{c};
    txn.exec_prepared("del", key);
    txn.commit();
}

void FileSystemRepository::put(int key, int value) {
    pqxx::work txn{c};
    txn.exec_prepared("put", key, value);
    txn.commit();
}

int FileSystemRepository::get(int key) {
    pqxx::work txn{c};
    pqxx::result a = txn.exec_prepared("get", key);
    txn.commit();
    return std::stoi(a.begin() -> at("value").c_str());
}


