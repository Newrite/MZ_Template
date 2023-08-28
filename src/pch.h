#pragma once

// PCH это люди так обызывают файл который компилируется самым первым и который
// инклюдится везде, поэтому все что объявлено тут, доступно во всем проекте,
// забей кароче можешь погуглить потом
#include <RE/Skyrim.h>
#include <SKSE/SKSE.h>

// алиасы (ссылки) на рызне нейм спейсы, что бы не писать SKSE::WinAPI, а сразу
// WinAPI
namespace WinAPI = SKSE::WinAPI;
namespace logs = SKSE::log;
namespace stl = SKSE::stl;

// Заюзать все что есть в пространсте имен std::literals, что бы писать например
// сразу оператор 'sv' (конвертация строки в std::string_view тип), а не
// std::literals::sv
using namespace std::literals;
