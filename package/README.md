# Packaging

Distribution packaging for the C translation. Both package types are built on
every push by `.github/workflows/c-cpp.yml`, and both run the test suite as
part of the build, so a package that builds is one whose contents were
exercised.

## What is in the package

The binary, its man page, and the design notes. **Not** `zil/`: that directory
holds Infocom's original ZIL sources and a Z-machine build of the game, and
`zil/README.md` states plainly that they are not under an open licence. They
are reference material for developing the translation, not something to
redistribute. `make dist` excludes them from the source tarball.

## Before you ship any of this

Neither package is fit to distribute yet, and the placeholders say so in the
files themselves rather than only here.

The repository carries no `LICENSE` file, so `debian/copyright` and the RPM
`License:` tag are both marked `Proprietary` as a placeholder. Two separate
questions have to be answered before either can be replaced with something
accurate:

1. Under what terms is the C translation itself offered? That is the
   maintainer's to choose.
2. The game's text, plot, characters and the name *Planetfall* are Steve
   Meretzky's work, published by Infocom in 1983, and are not the maintainer's
   to relicense. A translation is a derivative work, so whatever licence is
   chosen for the code does not by itself make the package redistributable.

## dpkg

Debian has no `.spec` file; the equivalent is a `debian/` directory, which is
what `dpkg/debian/` holds — `control`, `rules`, `changelog`, `copyright` and
`source/format`.

`debian/` has to sit at the source root for `dpkg-buildpackage` to find it, so
stage it first:

```sh
cp -r package/dpkg/debian debian
dpkg-buildpackage -us -uc -b
```

The package installs the binary to `/usr/games`, following Debian policy for
games, rather than the `/usr/bin` the other distributions use.

## rpm

One spec covers all three RPM distributions rather than three near-identical
files that would drift apart; they differ in small, known ways that
conditionals handle:

- SUSE still wants a `Group:` tag.
- `%license` is not understood by very old RHEL.

```sh
make dist
mkdir -p ~/rpmbuild/SOURCES
cp planetfall-*.tar.gz ~/rpmbuild/SOURCES/
rpmbuild -ba package/rpm/planetfall.spec
```

CI builds it on Fedora, on Rocky Linux 9 as a RHEL rebuild (the real thing
needs a subscription), and on openSUSE Tumbleweed.

## A note on the C standard

The Makefile asks the compiler which name it answers to rather than assuming.
GCC only learned `-std=c23` in 14 and clang in 18; before that the same
language is spelled `-std=c2x`. RHEL 9 ships GCC 11, so without the fallback
the package would simply not build there.
