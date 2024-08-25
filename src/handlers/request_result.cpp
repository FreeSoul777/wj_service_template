#include "request_result.h"

#include <userver/components/component.hpp>
#include <userver/storages/postgres/component.hpp>

#include <fmt/format.h>
#include <vector>

namespace wj_service_template
{
  namespace
  {
    constexpr uint8_t MIN_COUNT_POINTS = 3;

    double Perimeter(const std::vector<int32_t>& x, const std::vector<int32_t>& y, const ssize_t n)
    {
      double p = 0;
      for (auto i = 0; i < n; ++i)
      {
        auto j = (i + 1) % n;
        p += sqrt((x[i] - x[j]) * (x[i] - x[j]) + (y[i] - y[j]) * (y[i] - y[j]));
      }
      return p;
    }

    RequestResult::RequestResult(const userver::components::ComponentConfig& config,
                                 const userver::components::ComponentContext& component_context)
      : HttpHandlerBase(config, component_context)
      , pg_cluster_(
          component_context
            .FindComponent<userver::components::Postgres>("postgres-db-1")
            .GetCluster())
    {
    }

    std::string RequestResult::HandleRequestThrow(const userver::server::http::HttpRequest& /*request*/,
                                                  userver::server::request::RequestContext&) const
    {
      userver::storages::postgres::ResultSet res =
        pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                             "SELECT x, y FROM points");
      if (res.Size() < MIN_COUNT_POINTS)
      {
        throw userver::server::handlers::ClientError(userver::server::handlers::ExternalBody{"The polygon must have at least three vertices.\n"});
      }

      std::vector<int32_t> x_values{}, y_values{};
      ssize_t count = 0;
      for (const auto& row: res)
      {
        x_values.push_back(row[0].As<int32_t>());
        y_values.push_back(row[1].As<int32_t>());
        ++count;
      }

      const auto perimeter = Perimeter(x_values, y_values, count);

      return fmt::format("Perimeter = {}.\n", perimeter);
    }
  }

  void AppendRequestResult(userver::components::ComponentList& component_list)
  {
    component_list.Append<RequestResult>();
  }
}