class PersonLoader {
public:
    PersonLoader(string_view db_name, int db_connection_timeout, bool db_allow_exceptions, DBLogLevel db_log_level)
        : connector_(db_allow_exceptions, db_log_level),
          db_name_(db_name),
          db_connection_timeout_(db_connection_timeout),
          db_allow_exceptions_(db_allow_exceptions),
          db_log_level_(db_log_level) {}

    vector<Person> LoadPersons(int min_age, int max_age, string_view name_filter) {
        if (!ConnectToDB()) {
            return {};
        }

        DBQuery query = BuildQuery(min_age, max_age, name_filter);
        return FetchPersons(query);
    }

private:
    DBConnector connector_;
    string_view db_name_;
    int db_connection_timeout_;
    bool db_allow_exceptions_;
    DBLogLevel db_log_level_;

    bool ConnectToDB() {
        DBHandler db;
        if (db_name_.starts_with("tmp."s)) {
            db = connector_.ConnectTmp(db_name_, db_connection_timeout_);
        } else {
            db = connector_.Connect(db_name_, db_connection_timeout_);
        }
        return !(!db_allow_exceptions_ && !db.IsOK());
    }

    DBQuery BuildQuery(int min_age, int max_age, string_view name_filter) {
        ostringstream query_str;
        query_str << "from Persons "s
                  << "select Name, Age "s
                  << "where Age between "s << min_age << " and "s << max_age << " "s
                  << "and Name like '%"s << connector_.Quote(name_filter) << "%'"s;
        return DBQuery(query_str.str());
    }

    vector<Person> FetchPersons(const DBQuery& query) {
        vector<Person> persons;
        for (auto [name, age] : connector_.LoadRows<string, int>(query)) {
            persons.push_back({move(name), age});
        }
        return persons;
    }
};

//Теперь можно использовать класс `PersonLoader` для загрузки персон из базы данных. Пример использования:

/*int main() {
    PersonLoader loader("example.db", 10, true, DBLogLevel::DEBUG);
    vector<Person> persons = loader.LoadPersons(18, 30, "John");

    // Дальнейшая обработка полученных персон

    return 0;
}*/

//Я считаю, такой рефакторинг делает код более модульным, удобным для использования и поддержки.
//Также он позволяет легко тестировать отдельные компоненты класса `PersonLoader` без необходимости создавать и настраивать все зависимые объекты.