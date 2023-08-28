#include "Plugin.h"

namespace MZTemplate {

void install_hooks() {

  // Просто пишем в лог всякое
  logs::info("start install hooks");

  // Кароче хуйня из сксе которая выделяет место для записи твоих хуков, Феникс говорит 1024 хватает за глаза, если слишком мало выделил то сксе предупредит на старте игры
  auto& trampoline = SKSE::GetTrampoline();
  trampoline.create(1024);

  // Вызываем инсталл хуков
  // Для хуков виртуальной функции trampoline не нужно
  OnPlayerCharacterUpdate::install_hook();
  // А вот для обычных нужно
  OnWeaponHit::install_hook(trampoline);
  
  logs::info("finish install hooks");
}

}