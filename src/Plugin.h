#pragma once

namespace MZTemplate {

// ИД адреса из Address Library мода, он ведет на адрес в памяти который через
// реверс достается, первая цифра это ИД для СЕ, вторая дли АЕ, тут есть адрес
// только для СЕ
static inline REL::Relocation<uintptr_t> on_weapon_hit_address{
    RELOCATION_ID(37673, 0)};

// Оффсет который указывает какое место мы хукаем относительно адреса выше, это
// должно быть место где идет вызов какой-то функции
static inline auto on_weapon_hit_offset =
    REL::VariantOffset(0x3C0, 0, 0).offset();

// ИД адреса одной из виртуальных таблиц класса RE::PlayerCharacter
static inline REL::Relocation<uintptr_t> on_actor_update_pc_address{
    RELOCATION_ID(RE::VTABLE_PlayerCharacter.at(0).address(), 0)};

// Оффсет в виртуальной таблице адрес которой выше, указывает на функцию Update
// в классе RE::Actor (RE::PlayerCharacter наследует RE::Character, который в
// свою очередь наследует RE::Actor, поэтому у RE::PlayerCharacter есть
// виртуальная таблица функций класса RE::Actor)
static inline auto on_actor_update_offset =
    REL::VariantOffset(0xAD, 0, 0).offset();

// Можно использовать подобные структуры для описания хука
// Это обычный хук
struct OnWeaponHit {

  // Функция которая заинсталит наш хук
  static void install_hook(SKSE::Trampoline &trampoline) {

    // Логируем всякое
    logs::info("start hook on_weapon_hit"sv);

    // В weapon_hit_origin мы запишем "адрес" оригинальной функции, которую мы
    // заменяем, что бы потом вызвать оригинал, здесь также происходит и сам хук
    // по адресу + от этого адреса
    weapon_hit_origin = trampoline.write_call<5>(
        on_weapon_hit_address.address() + on_weapon_hit_offset, weapon_hit);

    logs::info("on_weapon_hit hook install"sv);
  }

  // функция которую мы хукаем имеет точно такую же сигнатуру (такие же
  // аргументы, и такой же тип возвращаемого значения), тут у нас описана
  // функция, которая вызовется вместо функции в коде игры
  static void weapon_hit(RE::Actor *target, RE::HitData &hit_data) {

    // проверяем указатель на nullptr, и если он валиден, логируем имя цели
    if (target) {
      logs::info("Target name: {}", target->GetDisplayFullName());
    }

    // Не забываем вызвать оригинальную функцию, что бы могли отработать сначала
    // хуки других длл по этому адресу, а также в самом конце и сам код игры (но
    // если нам как раз не нужно что бы оригинальный код исполнился, не вызываем
    // оригинальную функцию)
    return weapon_hit_origin(target, hit_data);
  }

  // Определение оригинальной функции, decltype означает взять тип от экземпляра
  // weapon_hit (что бы тип функции weapon_hit_origin совпадал с типом функции
  // weapon_hit
  static inline REL::Relocation<decltype(weapon_hit)> weapon_hit_origin;
};

// Это хук в виртуальную функцию, ссылка на нее в реверс либе -
// https://github.com/CharmedBaryon/CommonLibSSE-NG/blob/main/include/RE/A/Actor.h#L367
// Там напротив нее как раз написан оффсет который мы используем - 0AD, у нас
// функция update имеет отличную сигнатуру от гитхаба, потому что у функций
// класса есть неявный первый аргумент this который указывает на экземпляр
// класса
struct OnPlayerCharacterUpdate {

  // Функция которая заинсталит наш виртуальный хук
  static void install_hook() {

    // Логируем всякое
    logs::info("start hook OnPlayerCharacterUpdate");

    // В update_pc_origin мы запишем "адрес" оригинальной функции, которую мы
    // заменяем, что бы потом вызвать оригинал, здесь также происходит и сам хук
    // от адреса виртуальной таблица + оффсет
    update_pc_origin = on_actor_update_pc_address.write_vfunc(
        on_actor_update_offset, update_pc);

    logs::info("finish hook OnPlayerCharacterUpdate");
  }

  // функция которую мы хукаем имеет точно такую же сигнатуру (такие же
  // аргументы, и такой же тип возвращаемого значения), тут у нас описана
  // функция, которая вызовется вместо функции в коде игры, первый аргумент
  // персонаж игрока, второй аргумент это времяк которое прошло с прошлого
  // кадра, update вызывается каждый кадр
  static void update_pc(RE::PlayerCharacter *this_, float delta) {

    // Логируем дельту и вызываем оригинальную функцию
    logs::info("Player delta: {}", delta);
    return update_pc_origin(this_, delta);
  }

  // Определение оригинальной функции, decltype означает взять тип от экземпляра
  // update_pc (что бы тип функции update_pc_origin совпадал с типом функции
  // update_pc
  static inline REL::Relocation<decltype(update_pc)> update_pc_origin;
};

// Определим функцию которая вызовет все инсталл хуки, опишием реализацию в
// файле реализации (cpp)
void install_hooks();

}