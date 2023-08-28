#include "Plugin.h"

// Забей хуй не трогай
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/msvc_sink.h>

// Забей хуй тебе не надо, в этой функции настраивается логгер для плагина
void InitLogging() {
  auto path = logs::log_directory();
  if (!path)
    return;

  const auto plugin = SKSE::PluginDeclaration::GetSingleton();
  *path /= fmt::format("{}.log", plugin->GetName());

  std::vector<spdlog::sink_ptr> sinks{
      std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true),
      std::make_shared<spdlog::sinks::msvc_sink_mt>()};

  auto logger =
      std::make_shared<spdlog::logger>("global", sinks.begin(), sinks.end());
  logger->set_level(spdlog::level::info);
  logger->flush_on(spdlog::level::info);

  spdlog::set_default_logger(std::move(logger));
  spdlog::set_pattern("[%^%L%$] %v");
}

// Функция обработки сообщений которую вызывает СКСЕ при разных эвентах, тебя
// сейчас интересует только kDataLoaded, после этого эвента можно начинать
// работать с данным и тд
auto initialize_messaging() -> void {
  if (!SKSE::GetMessagingInterface()->RegisterListener(
          [](SKSE::MessagingInterface::Message *message) {
            switch (message->type) {
            // Skyrim lifecycle events.
            case SKSE::MessagingInterface::kPostLoad: // Called after all
                                                      // plugins have finished
                                                      // running
            // SKSEPlugin_Load. It is now safe to do
            // multithreaded operations, or operations against
            // other plugins.
            case SKSE::MessagingInterface::kPostPostLoad: // Called after all
                                                          // kPostLoad message
                                                          // handlers
              // have run.
            case SKSE::MessagingInterface::kInputLoaded: // Called when all game
                                                         // data has been found.
              break;
            case SKSE::MessagingInterface::kDataLoaded: // All ESM/ESL/ESP
                                                        // plugins have loaded,
              // main menu is now active.
              {
                // It is now safe to access form data.
                logs::info("Get data loaded message");

                // Наши хуки инсталятся в этот момент
                MZTemplate::install_hooks();
                break;
              }
            // Skyrim game events.
            case SKSE::MessagingInterface::kNewGame: // Player starts a new game
                                                     // from main menu.
            case SKSE::MessagingInterface::kPreLoadGame: // Player selected a
                                                         // game to load, but it
            // hasn't loaded yet. Data will be the name of
            // the loaded save.
            case SKSE::MessagingInterface::kPostLoadGame: // Player's selected
                                                          // save game has
                                                          // finished
            // loading. Data will be a boolean indicating
            // whether the load was successful.
            case SKSE::MessagingInterface::kSaveGame: // The player has saved a
                                                      // game.
            // Data will be the save name.
            case SKSE::MessagingInterface::kDeleteGame: // The player deleted a
                                                        // saved game from
                                                        // within
              // the load menu.
              break;
            default: {
              break;
            }
            }
          })) {
    stl::report_and_fail("Unable to register message listener."sv);
  }
}

// Входная точка ДЛЛ плагинов
SKSEPluginLoad(const SKSE::LoadInterface *a_skse) {
  // ^ Вызов инициализации логгера
  InitLogging();

  // Стандартная процедура регистрации плагина через сксе интерфейс, хуй знает
  // че там в подробностях, знать не нужно
  const auto plugin = SKSE::PluginDeclaration::GetSingleton();
  logs::info("{} v{} is loading...", plugin->GetName(), plugin->GetVersion());

  SKSE::Init(a_skse);

  // Регистрируем обработку сообщений
  initialize_messaging();

  logs::info("{} loaded.", plugin->GetName());
  // logs <- вызовы функций логирования, лог пишется в документы \ скуримСЕ
  // \ сксе, лог по названию плагина

  // Входная точка должна вернуть true, если ты вернешь false тогда СКСЕ при
  // запуске покажет ошибку что проблемы с загрузкой длл и тд, например можешь
  // что-то проверить и вернуть false если что-то не так
  return true;
}
