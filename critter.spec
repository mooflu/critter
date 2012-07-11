%define distrib %(if [ -f /etc/fedora-release ]; then echo -n "_FC"; cat /etc/fedora-release | awk '{print $4}'; fi)

%define ver      __NO_VERSION__
%define rel      1%{distrib}
%define prefix   /usr

Summary: SDL/OpenGL space shoot'em up game
Name:		CriticalMass
Version:	%ver
Release:	%rel
Copyright:	GPL
Group:		Amusements/Games
URL:		http://criticalmass.sourceforge.net
BuildRoot:	/var/tmp/%{name}-%{version}-root
Prefix:         %{prefix}
Source:		http://prdownloads.sourceforge.net/criticalmass/CriticalMass-%{ver}.tar.bz2

Requires: libpng >= 1.0.8
Requires: zlib >= 1.1.3

%description
Critical Mass (aka Critter) is an SDL/OpenGL space shoot'em up game.

%prep
%setup

%build
if [ ! -f configure ]; then
  CFLAGS="$MYCFLAGS" ./autogen.sh $MYARCH_FLAGS --prefix=%prefix
fi
CFLAGS="$MYCFLAGS" ./configure $MYARCH_FLAGS --prefix=%prefix

make

%install
rm -rf $RPM_BUILD_ROOT

make prefix=$RPM_BUILD_ROOT%{prefix} install

mkdir -p $RPM_BUILD_ROOT/%{prefix}/share/applications
mkdir -p $RPM_BUILD_ROOT/%{prefix}/share/icons

install -m 744 critter.png $RPM_BUILD_ROOT/%{prefix}/share/icons/critter.png

cat > $RPM_BUILD_ROOT/%{prefix}/share/applications/critter.desktop << EOF
[Desktop Entry]
Encoding=UTF-8
Name=Critical Mass
Exec=%{prefix}/bin/critter
Icon=%{prefix}/share/icons/critter.png
Type=Application
Terminal=False
Categories=Application;Game;ArcadeGame;
EOF

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-, root, root)

%doc Readme.html COPYING TODO
%{prefix}/bin/critter
%{prefix}/bin/Packer
%{prefix}/share/Critical_Mass/*
%{prefix}/man/man6/critter.6.gz
%{prefix}/share/applications/critter.desktop
%{prefix}/share/icons/critter.png
