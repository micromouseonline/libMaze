
# Changelog

All notable changes to this project will be documented here. Perhaps.
The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

The current version number should be visible in the file VERSION


## [Unreleased]
- Nothing yet

## [3.1.0] - 2026-03-19

### Changed
- `readTextMaze()` now detects and sets maze width internally from the file
  content. Callers no longer need to call `maze->setWidth()` before
  `readMaze()` for text files.
- `readBinaryMaze()` now sets maze width (16 for 256-byte files, 32 for
  1024-byte files) before loading data. Callers no longer need to pre-set width
  for binary files either.

## [3.0.0] - 2026-03-18

## Breaking Changes
 - Supersedes previous versions. Some API calls may have changed. Please check

### Added
 - Google Test suite for the entire library
 - This release is the code used in Decimus 5A, spring 2026

[Unreleased]: https://github.com/micromouseonline/libMaze/compare/v3.1.0...HEAD
[3.1.0]: https://github.com/micromouseonline/libMaze/releases/tag/v3.1.0
[3.0.0]: https://github.com/micromouseonline/libMaze/releases/tag/v3.0.0