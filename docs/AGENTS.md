# Документация и схемы

Правила для агента при правках в `docs/`. Firmware, PWM и электрическую топологию **не менять** из документации: сначала hardware-решение, потом код.

## Файлы

| Файл | Роль |
|---|---|
| `wiring.md` | Контроллерный стенд C3 / legacy NodeMCU, pin map, кнопки |
| `load-control-plan.md` | Силовая часть, fail-safe, bring-up, safety |
| `schematics/controller_stand.py` | Текущий LED-стенд ESP32-C3 |
| `schematics/system_overview.py` | Архитектура питания/управления |
| `schematics/load_control_channel.py` | Принципиальная схема одного DIM-канала |
| `schematics/four_channel_overview.py` | Четыре независимых DIM-интерфейса |
| `assets/schematics/*.svg` | Сгенерированный артефакт для Markdown/GitHub |
| `requirements.txt` | Pinned `schemdraw==0.23` |

ASCII в Markdown оставлять как compact textual reference (pin map, fail-safe). Не дублировать им то, что уже ясно из SVG.

## Схемы: цикл правки

1. Править только `.py` в `docs/schematics/`.
2. Каждый модуль экспортирует `draw() -> schemdraw.Drawing`.
3. Рендер из корня репозитория:

```bash
python -m pip install -r docs/requirements.txt
python docs/schematics/render_all.py
```

4. Закоммитить **и** `.py`, **и** обновлённые `.svg`. CI job `schematics` делает `git diff --exit-code docs/assets/schematics/` — устаревший SVG ломает сборку.
5. Новую схему: модуль + строка в `SPECS` в `render_all.py` + SVG + вставка в Markdown.

Пути в `render_all.py` относительны к скрипту. Headless: `schemdraw.use("svg")`, `Drawing(show=False, bgcolor="white")`. Без matplotlib, KiCad, LaTeX.

Зависимость только через `docs/requirements.txt` / `pip install`. Версию Schemdraw не поднимать попутно.

## Вставка в Markdown

Из файлов в `docs/`:

```markdown
![Схема канала управления нагрузкой](assets/schematics/load-control-channel.svg)
```

Из `README.md`: `docs/assets/schematics/...`.

## Стиль схем

- SVG, белый фон, стандартные символы, чтение слева направо.
- Короткие labels. На принципиальной схеме: `R1 500 Ω`, `R2`/`R3` `10 kΩ`, `U1 PC817`, `Q1 BC548` с B/C/E.
- Архитектура (`system_overview`, `four_channel_overview`): `schemdraw.flow.Box`, **не** `elm.Ic` с именами выводов внутри — они наезжают на title.
- Принципиальная схема канала: `elm.Optocoupler`, `elm.BjtNpn`. Подписи ставить явными `elm.Label` с зазором, не `loc="right"` на круге транзистора.
- После рендера **смотреть SVG**: нет наложений текста, пунктир изоляции не режет labels.
- Опциональный `50 kΩ BASE → DIM−` не рисовать (DNP). Только текст в Markdown.
- `DIM−` разных LDH **не** объединять в общую шину.

## Что не трогать из docs-задачи

- `src/`, `include/`, `platformio.ini`, PWM frequency/logic, инверсия канала.
- Safety/bring-up инструкции и pin mapping — не выкидывать при замене ASCII на SVG.
- `release.yml` не смешивать со схемами. Firmware-job в `build.yml` не объединять с `schematics`.
