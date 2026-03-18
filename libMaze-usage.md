# Library Usage Guide

This document describes how this library is versioned, how to include it in your projects, and how to contribute changes back.

---

## Overview

This is a pure C/C++ library designed for use in both native (Linux/Windows) and embedded/cross-compiled projects. It is distributed as source code and is intended to be embedded directly into consuming projects using `git subtree`. No package manager is required.

The library has a stable public API. Breaking changes are signalled by a major version bump and documented in [CHANGELOG.md](CHANGELOG.md).

---

## Versioning

This library uses [Semantic Versioning](https://semver.org/):

- **MAJOR** — breaking API changes
- **MINOR** — new backwards-compatible features
- **PATCH** — bug fixes

Every release is tagged in git (`v1.0.0`, `v1.1.0`, `v2.0.0` etc.). The current version is recorded in the [VERSION](VERSION) file at the root of the library.

Consult [CHANGELOG.md](CHANGELOG.md) before updating across a major version boundary.

---

## Including the Library in Your Project

The recommended method is `git subtree`. This embeds the library source directly into your project repository as plain files. No special git knowledge is required to build the project — it looks like any other subdirectory. No package manager is needed, and it works identically on Linux and Windows with any IDE or toolchain.

### First-time setup

Add the library remote once per clone of your project:

```bash
git remote add mylib https://github.com/you/mylib.git
```

Then add the library at a specific version tag:

```bash
git subtree add --prefix=libs/mylib mylib v1.2.0 --squash
```

This creates a single squash commit in your project recording the library version. The library source will be present under `libs/mylib/`.

### Checking the current version

Two ways to check which version of the library a project is using:

1. Inspect the `VERSION` file directly:
   ```
   libs/mylib/VERSION
   ```

2. Check the git log for the subtree:
   ```bash
   git log --oneline libs/mylib
   ```
   The squash commit message records the library commit hash it was pulled from.

### Updating to a newer version

```bash
git subtree pull --prefix=libs/mylib mylib v1.3.0 --squash
```

### Switching to a different version (including downgrading)

```bash
git subtree pull --prefix=libs/mylib mylib v1.1.0 --squash
```

Git subtree does not distinguish between upgrading and downgrading — you can move to any tagged version freely.

---

## CMake Integration

When consumed as a subtree the library integrates via CMake's `add_subdirectory`:

```cmake
add_subdirectory(libs/mylib)
target_link_libraries(your_target PRIVATE mylib)
```

The library's `CMakeLists.txt` exposes a `mylib` target with its include paths set correctly. You do not need to manage include directories manually.

The test suite is excluded automatically when the library is consumed this way — see [Tests](#tests) below.

---

## Contributing Changes Back

One advantage of `git subtree` over submodules is that you can make changes to the library while working inside a consuming project, then push those changes back to the library repository.

### Preferred workflow

Where possible, make changes in the library repository directly, tag a release, then pull the new tag into consuming projects. This keeps the library history clean.

### If you make a fix inside a project first

Push the change back to the library's `main` branch:

```bash
git subtree push --prefix=libs/mylib mylib main
```

Then, in the library repository, review the change, tag a new release, and pull the updated tag into any other projects that need the fix.

**Important:** avoid making the same fix in multiple projects without first consolidating it back into the library. Divergent fixes across projects are difficult to reconcile.

---

## Tests

The library includes a [Google Test](https://github.com/google/googletest) suite under `tests/`. These are built automatically when the library is the top-level CMake project (i.e. when developing the library standalone), and are excluded automatically when the library is consumed via `add_subdirectory` in a parent project.

### Building and running tests (library development)

```bash
cmake -B build
cmake --build build
ctest --test-dir build
```

Google Test must be available on the build machine. On Linux install it via your package manager; on Windows use vcpkg or a local build. This dependency is only required when developing the library itself — consuming projects do not need it.

---

## Maintenance Notes (for library authors)

- Bump the `VERSION` file and update `CHANGELOG.md` with every release
- Tag every release in git before pushing the tag to the remote:
  ```bash
  git tag -a v1.2.0 -m "Release v1.2.0"
  git push origin v1.2.0
  ```
- Document breaking changes prominently in `CHANGELOG.md` under a `### Breaking Changes` heading
- Maintain a `v1.x` (or equivalent) branch if bug fixes need to be backported to an older major version while development continues on `main`
