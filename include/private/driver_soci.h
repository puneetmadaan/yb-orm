// -*- Mode: C++; c-basic-offset: 4; tab-width: 4; indent-tabs-mode: nil; -*-
#ifndef YB__ORM__DRIVER_SOCI__INCLUDED
#define YB__ORM__DRIVER_SOCI__INCLUDED

#include <memory>
#include <vector>
#include "util/thread.h"
#include "orm/sql_driver.h"
#include <soci/soci.h>

namespace Yb {

class SOCICursorBackend: public SqlCursorBackend
{
    soci::session *conn_;
    soci::statement *stmt_;
    std::string sql_;
    bool is_select_, bound_first_, executed_;
    TypeCodes param_types_;
    soci::row row_;
    std::vector<std::string> in_params_;
    std::vector<soci::indicator> in_flags_;
public:
    SOCICursorBackend(soci::session *conn);
    ~SOCICursorBackend();
    void close();
    void exec_direct(const String &sql);
    void prepare(const String &sql);
    void bind_params(const TypeCodes &types);
    void exec(const Values &params);
    RowPtr fetch_row();
};

class SOCIDriver;

class SOCIConnectionBackend: public SqlConnectionBackend
{
    soci::session *conn_;
    SOCIDriver *drv_;
    bool own_handle_;
public:
    SOCIConnectionBackend(SOCIDriver *drv);
    ~SOCIConnectionBackend();
    void open(SqlDialect *dialect, const SqlSource &source);
    void use_raw(SqlDialect *dialect, void *raw_connection);
    void *get_raw();
    std::auto_ptr<SqlCursorBackend> new_cursor();
    void close();
    void begin_trans();
    void commit();
    void rollback();
};

class SOCIDriver: public SqlDriver
{
    friend class SOCIConnectionBackend;
    Mutex conn_mux_;
public:
    SOCIDriver(bool use_odbc);
    std::auto_ptr<SqlConnectionBackend> create_backend();
    void parse_url_tail(const String &dialect_name,
            const String &url_tail, StringDict &source);
    bool numbered_params();
};

} //namespace Yb

// vim:ts=4:sts=4:sw=4:et:
#endif // YB__ORM__DRIVER_SOCI__INCLUDED
