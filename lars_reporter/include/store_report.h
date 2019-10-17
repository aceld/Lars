#pragma once

#include "mysql.h"
#include "lars.pb.h"

class StoreReport
{
public:
    StoreReport();

    void store(lars::ReportStatusRequest req);

private:
    MYSQL _db_conn;
};

