#include "components.h"

#include <userver/utils/daemon_run.hpp>

int main(int argc, char* argv[])
{
  return userver::utils::DaemonMain(argc, argv, wj_service_template::MakeComponents());
}