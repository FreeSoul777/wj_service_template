#include "request_add.h"

#include <userver/components/component.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/from_string.hpp>

namespace wj_service_template
{
  namespace
  {
    RequestAdd::RequestAdd(const userver::components::ComponentConfig& config,
                           const userver::components::ComponentContext& component_context)
      : HttpHandlerBase(config, component_context)
      , pg_cluster_(
          component_context
            .FindComponent<userver::components::Postgres>("postgres-db-1")
            .GetCluster())
    {
    }

    std::string RequestAdd::HandleRequestThrow(const userver::server::http::HttpRequest& request,
                                               userver::server::request::RequestContext&) const
    {
      if (!request.HasArg("x"))
        throw userver::server::handlers::ClientError(userver::server::handlers::ExternalBody{"No 'x' query argument.\n"});
      if (!request.HasArg("y"))
        throw userver::server::handlers::ClientError(userver::server::handlers::ExternalBody{"No 'y' query argument.\n"});

      int32_t x = 0;
      try
      {
        x = userver::utils::FromString<int32_t>(request.GetArg("x"));
      }
      catch (const std::exception& err)
      {
        throw userver::server::handlers::ClientError(userver::server::handlers::ExternalBody{"Invalid 'x' value.\n"});
      }

      int32_t y = 0;
      try
      {
        y = userver::utils::FromString<int32_t>(request.GetArg("y"));
      }
      catch (const std::exception& err)
      {
        throw userver::server::handlers::ClientError(userver::server::handlers::ExternalBody{"Invalid 'y' value.\n"});
      }

      userver::storages::postgres::Transaction transaction =
        pg_cluster_->Begin(userver::storages::postgres::ClusterHostType::kMaster, {});

      auto res = transaction.Execute("INSERT INTO points (x, y) "
                                     "VALUES ($1, $2) "
                                     "ON CONFLICT (x, y) DO NOTHING",
                                     x, y);
      if (res.RowsAffected())
      {
        transaction.Commit();
        request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
        return fmt::format("The point ({}, {}) has been created.\n", x, y);
      }
      res = transaction.Execute("SELECT x, y FROM points WHERE x=$1 AND y=$2", x, y);
      transaction.Rollback();
      if (!res.IsEmpty())
      {
        request.SetResponseStatus(userver::server::http::HttpStatus::kConflict);
      }
      
      return fmt::format("The point ({}, {}) already exists.\n", x, y);
    }
  }

  void AppendRequestAdd(userver::components::ComponentList& component_list)
  {
    component_list.Append<RequestAdd>();
  }
}