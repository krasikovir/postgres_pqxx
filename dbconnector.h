#include <string>
#include <exception>
#include <pqxx/pqxx>
#include <pqxx/except>
#include <pqxx/stream_to>

class FileSystemRepository {
public:
    explicit FileSystemRepository() : c("user=alex "
                                        "hostaddr=172.17.0.1 "
                                        "password=Cc327020 "
                                        "dbname=yashkadb "
                                        "port=2345")
    {
        c.prepare(
                "get",
                "SELECT * FROM table5 WHERE key = $1");
        c.prepare(
                "del",
                "delete from table5 WHERE key = $1");
        c.prepare(
                "put",
                "insert into table5 (key, value) values ($1, $2)");
    }

    void put(int key, int value);

    void del(int key);

    int get(int key);

private:
    const static inline std::string s = "user=alex "
                                        "hostaddr=127.0.0.1 "
                                        "password=Cc327020 "
                                        "dbname=yashkadb "
                                        "port=5432";
    pqxx::connection c;
};
