#
# One spec for RHEL (and its rebuilds), Fedora and SUSE. The three differ in
# small, well-known ways rather than fundamentally, so they are handled with
# conditionals instead of three near-identical files that would drift apart:
#
#   * SUSE still wants a Group: tag and prefers %doc/%license placement rules
#     that predate Fedora's.
#   * %license is not understood by very old RHEL, where it has to be %doc.
#   * Fedora and RHEL want the C compiler pulled in by BuildRequires: gcc;
#     SUSE spells the same dependency the same way, but wants make explicitly.
#
# Build with:  rpmbuild -ba package/rpm/planetfall.spec
# after putting the tarball from `make dist` in ~/rpmbuild/SOURCES.
#

Name:           planetfall
Version:        0.1.0
Release:        1%{?dist}
Summary:        Translation of Infocom's Planetfall into C

# NOT A CONSIDERED LICENSING STATEMENT -- PLACEHOLDER.
#
# The repository carries no LICENSE file. Two questions have to be answered
# before this tag means anything:
#
#   1. Under what terms is the C translation offered? The maintainer's choice.
#   2. The game's text, plot, characters and the name "Planetfall" are Steve
#      Meretzky's work, published by Infocom in 1983, and are not the
#      maintainer's to relicense. A translation is a derivative work.
#
# The original ZIL sources are excluded from the tarball on purpose:
# zil/README.md states they are not under an open licence.
License:        Proprietary

URL:            https://github.com/segin/planetfall
Source0:        %{name}-%{version}.tar.gz

%if 0%{?suse_version}
Group:          Amusements/Games/Adventure/Text
%endif

BuildRequires:  gcc
BuildRequires:  make
# The test suite drives the built binary from shell and Python.
BuildRequires:  python3

%description
A reimplementation, in ISO C23, of the 1983 Infocom interactive fiction game
Planetfall, written by Steve Meretzky. It is translated from the original ZIL
(Zork Implementation Language) sources rather than interpreted from a
Z-machine story file, so it runs as a native program with no interpreter
required.

You play an Ensign Seventh Class aboard the Stellar Patrol Ship Feinstein.
Commands are typed in plain English in the usual interactive-fiction style.

The translation is incomplete; consult the project's notes for what is and is
not yet implemented.

%prep
%autosetup

%build
# The upstream Makefile pins -std=c23 and builds with -Werror; let the distro
# add its own hardening and optimisation on top rather than replacing them.
make %{?_smp_mflags} CFLAGS="%{optflags} -std=c23 -D_DEFAULT_SOURCE -Wall -Werror -Iinclude"

%install
make install DESTDIR=%{buildroot} PREFIX=%{_prefix} BINDIR=%{_bindir} MANDIR=%{_mandir}

%check
make test

%files
%{_bindir}/%{name}
%{_mandir}/man6/%{name}.6*
%doc AGENTS.md docs/ENGINE_MECHANICS.md

%changelog
* Mon Aug 31 2026 Kirn Gill II <segin2005@gmail.com> - 0.1.0-1
- Initial packaging of the C translation.
