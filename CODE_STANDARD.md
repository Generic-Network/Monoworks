# Monoworks Code Standard (abgeleitet aus dem bestehenden Code)

Dieses Dokument beschreibt den **aktuellen, aus dem Repository extrahierten** Coding-Standard für Agenten und Reviewer (insbesondere CodeRabbit).  
Wenn bestehender Modulcode im Einzelfall abweicht, gilt: **lokale Modulkonvention vor globaler Regel**, aber neue Stellen sollen sich an diesem Standard orientieren.

## 1) Sprache, Build und Plattform

- Primärsprache: **C++23** (`CMAKE_CXX_STANDARD 23`).
- Build-System: **CMake**.
- Plattformflags werden per Compile-Definition gesetzt (`MW_PLATFORM_WINDOWS`, `MW_PLATFORM_LINUX`, `MW_PLATFORM_OSX`).
- Feature-/Build-Flags werden über Makros gesteuert (z. B. `MW_PROFILING`, `MW_DEBUG`, `MW_RELEASE`).

## 2) Projekt- und Dateistruktur

- Engine-Core liegt unter `MonoEngine/source/...`.
- Runtime-spezifischer Code liegt unter `MonoRuntime/source/...` und `MonoRuntime/specific/...`.
- Editor-spezifischer Code liegt unter `MonoEditor/source/...`.
- Header-Endungen im Codebestand:
  - Vorwiegend **`.hh`** für C++-Header.
  - **`.h`** wird für einzelne Komponenten/Makro-Header genutzt.
  - **Neue Header sollen die Endung des jeweiligen Moduls übernehmen**.
- Quelltexte liegen als **`.cc`** vor; Qt-UI-nahe Dateien können `.cpp` sein.

## 3) Includes und Abhängigkeiten

- Include-Reihenfolge im Bestand:
  1. eigenes zugehöriges Headerfile
  2. projektinterne Includes
  3. Standardbibliothek / Third-Party
- Projektinterne Includes werden mit Projektpfaden geschrieben (z. B. `<common/Base.hh>`, `<core/Application.hh>`).
- `#pragma once` wird in Headern durchgängig verwendet.

## 4) Namespaces

- Primärer Namespace ist **`Monoworks`**.
- Verschachtelte Domains nutzen Sub-Namespaces (z. B. `Monoworks::Events`, `Monoworks::RHI`).
- Für kurze Scope-Deklaration ist `namespace Monoworks::RHI { ... }` etabliert.

## 5) Namenskonventionen

- Klassen: Präfix **`C`** (z. B. `CApplication`, `CLogManager`).
- Structs: Präfix **`S`** (z. B. `SEvent`, `SWindowResize`).
- Enums: Präfix **`E`** (z. B. `EEventType`, `EGraphicsAPI`).
- Membervariablen: Präfix **`m_`**.
- Statische Klassenmember: Präfix **`s_`**.
- Globale/Engine-Makros: Präfix **`MW_`**.
- Typaliases für Primitive folgen Kurzschema (`u32`, `s32`, `f32`, `byte_t`, `uptr_t`).

## 6) API-Design und Klassenstil

- Manager-/Subsystemklassen sind häufig statisch orientiert (`Init`, `Shutdown`, ggf. `Process...`).
- Lifecycle-Namen sind konsistent:
  - `Init()`
  - `Run()` / `Frame()` / `ProcessEvents()`
  - `Shutdown()`
- Getter werden oft als kurze Inline-Methoden im Header definiert.
- Wo sinnvoll werden `[[nodiscard]]` und `noexcept` verwendet; neue öffentliche APIs sollen dies beibehalten.

## 7) Logging und Profiling

- Logging läuft über zentrale Makros in `common/Log.h`:
  - `MW_TRACE`, `MW_INFO`, `MW_WARN`, `MW_ERROR`, `MW_FATAL`.
- Direkte stdout-Ausgaben sind zu vermeiden; stattdessen Logging-Makros verwenden.
- Profiling erfolgt über `MW_PROFILE_FUNC()` und optional `MW_PROFILE_FRAME_MARK()`.
- Neue Hotpath-/Subsystem-Funktionen sollen denselben Profiling-Ansatz nutzen.

## 8) Event-System-Konventionen

- Eventtypen werden als Enumwerte (`MW_EVENT_...`) geführt.
- Event-Payloads sind triviale, kleine Structs (copyable, nicht-pointer, size-beschränkt).
- Events werden über `CEventManager::EmitEvent(...)` oder `EmitEventNonDeffered(...)` verteilt.
- Callback-Signatur: `std::function<bool(SEvent&)>`.

## 9) Speicher- und Ressourcenkonventionen

- Eigenes Handle-/Ref-System ist vorhanden (`SHandle`, `CRef<T>`, `Ref<T>`).
- Für Engine-interne Objekte bevorzugt vorhandene Speicherabstraktionen nutzen, statt neue Pointer-Konzepte einzuführen.
- Bei Besitz außerhalb des Ref-Systems sind Lebenszyklen explizit in `Init/Shutdown` zu behandeln.

## 10) Dokumentation im Code

- Öffentliche Kern-Header nutzen Doxygen-Stil:
  - `@file`, `@brief`, `@author`, `@date`, `@ingroup`.
- Für öffentliche Typen/Funktionen werden kurze `@brief`-Beschreibungen verwendet.
- Neue öffentliche APIs sollen denselben Doku-Stil beibehalten.

## 11) Formatierungsstil (aus bestehendem Code abgeleitet)

- Geschweifte Klammern stehen überwiegend im Allman-Stil (öffnende Klammer in neuer Zeile).
- Einrückung ist im Bestand überwiegend tab-orientiert; bestehende Dateikonvention beibehalten.
- Kurze Inline-Getter in einer Zeile sind zulässig.
- Leere Zeilen werden zur logischen Blocktrennung genutzt (insb. zwischen Include-Gruppen und Methodenblöcken).

## 12) Fehlerbehandlung und Defensive Patterns

- Vor Zugriffen auf externe Ressourcen/Zeiger werden Guard-Checks genutzt (z. B. Null-/Validitätsprüfungen).
- Frühzeitige Returns bei ungültigen Zuständen sind etabliert.
- Unerwartete Zustände sollen über `MW_WARN`/`MW_ERROR` sichtbar gemacht werden.

## 13) Modulgrenzen und Erweiterungen

- Plattform-/API-spezifische Implementierungen liegen in dedizierten Unterordnern (`specific/...`, `rhi/specific/...`).
- Abstraktionsinterfaces liegen in agnostischen Bereichen (`agnostic/...`).
- Neue Features sollen diese Trennung beibehalten (agnostische API + spezifische Backend-Implementierung).

## 14) Review-Regeln für Agenten und CodeRabbit

- Prüfe zuerst auf Konventionsbruch bei:
  - Namenspräfixen (`C/S/E`, `m_`, `s_`, `MW_`)
  - Lifecycle-Methoden (`Init/Shutdown/...`)
  - Logging/Profiling-Makros
  - Namespace- und Include-Struktur
- Bei Vorschlägen gilt:
  - Keine Stilvorgaben einführen, die dem bestehenden Modulstil widersprechen.
  - Bestehende Architektur (Manager-, Event-, Ref-/Handle-Ansatz) respektieren.
  - Änderungen klein und lokal halten; keine unnötigen Refactors.

