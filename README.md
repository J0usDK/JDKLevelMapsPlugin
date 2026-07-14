# JDKLevelMaps

CryEngine editor extension for generating optimized runtime data maps from level information.

The goal of this project is to provide a flexible pipeline for converting expensive editor-only data into lightweight runtime representations that can be efficiently queried during gameplay.

Currently the main use case is vegetation data baking for environmental audio systems.

## Overview

Modern games often contain large amounts of world data that is expensive or impossible to query directly at runtime.

Instead of performing complex searches during gameplay, JDKLevelMaps processes level data in the editor and generates compact runtime maps.

Example workflow:

```
CryEngine Sandbox
        |
        v
 JDKLevelMaps Editor Tool
        |
        v
 Runtime Data Maps
        |
        v
 Gameplay Systems
```

## Current Features

### Vegetation Coverage Maps

Generates spatial maps containing vegetation information extracted from CryEngine levels.

Example use cases:

* Footstep sound variation.
* Grass interaction effects.
* Environmental audio.
* Gameplay interaction with vegetation.

Instead of checking individual vegetation instances at runtime, gameplay systems can perform a fast lookup using world coordinates.

Example:

```
World Position
      |
      v
Map Lookup
      |
      v
Vegetation Density
```

## Planned Features

The architecture is designed to support multiple map types.

Possible future implementations:

* Lighting maps.

  * Stealth gameplay.
  * AI visibility calculations.
  * Dynamic difficulty systems.

* Surface information maps.

  * Audio variations.
  * Gameplay effects.

* Environmental data maps.

  * Wind exposure.
  * Weather influence.
  * World interaction data.

* Custom gameplay metadata.

## Runtime Data Format

Generated maps use a custom binary format optimized for fast loading and lookup.

The format stores:

* Map dimensions.
* World origin.
* Cell size.
* Layer-specific data.

Example vegetation density map:

```
0    0    10   50
0    30   80   100
5    70   100  100
```

Values represent density/intensity depending on the map type.

Example:

```
0       No vegetation
1-100   Vegetation density
```

## Architecture

```
JDKLevelMaps
|
├── Editor
│   └── CryEngine Sandbox integration
│
├── Baking
│   └── Data generation pipeline
│
├── Runtime
│   └── Fast data access API
│
└── Formats
    └── Binary map serialization
```

## Engine Compatibility

Currently developed for:

```
CryEngine 5.7.1 LTS
```

Base engine revision:

```
cd017c4f782aaa03806dc73370ea91ad86147a72
```

Branch:

```
release
```

## Requirements

* CryEngine 5.7.1 source build.
* Visual Studio 2022.
* C++17 compiler.
* CMake.

## Build

1. Copy the `JDKLevelMaps` plugin into your CryEngine source tree:

```
Code/
└── Sandbox/
    └── Plugins/
        └── JDKLevelMapsPlugin/
```

2. Regenerate CryEngine solution files.

3. Build the engine.


## Project Status

Early development.

Implemented:

* [x] Project structure.
* [ ] Vegetation extraction.
* [ ] Binary map serialization.
* [ ] Runtime API.
* [ ] Editor visualization.
* [ ] Additional map layers.

## Design Goals

JDKLevelMaps aims to provide:

* Fast runtime queries.
* Minimal memory overhead.
* Editor-driven data generation.
* Extensible map layer system.
* Simple integration with gameplay systems.

## License

This repository contains only JDKLevelMaps source code.

CryEngine source code is not included.

CryEngine is property of Crytek.